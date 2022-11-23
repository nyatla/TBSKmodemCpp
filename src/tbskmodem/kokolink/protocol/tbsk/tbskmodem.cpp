#pragma once
#include "../../interfaces.h"
#include "./toneblock.h"
#include "../../streams/rostreams.h"
#include "./preamble/Preamble.h"
#include "./preamble/CoffPreamble.h"
#include "./traitblockcoder.h"
#include "../../compatibility.h"
#include "../../streams/BitStream.h"
#include "../../filter/BitsWidthFilter.h"
#include "../../utils/AsyncMethod.h"
#include <math.h>
#include <memory>

namespace TBSKmodemCPP
{
    using namespace std;

    class DiffBitEncoder : public BasicRoStream<int>, virtual public IBitStream
    {
    private:
        shared_ptr<IRoStream<int>> _src;
        int _last_bit;
        bool _is_eos;
        int _pos;
    public:
        DiffBitEncoder(int firstbit, shared_ptr<IRoStream<int>>& src) :BasicRoStream(),
            _src{ src }
        {
            this->_last_bit = firstbit;
            this->_is_eos = false;
            this->_pos = 0;
        }
        int Next()override
        {
            if (this->_is_eos)
            {
                throw PyStopIteration();
            }
            if (this->_pos == 0)
            {
                this->_pos = this->_pos + 1;
                return this->_last_bit; //#1st基準シンボル
            }
            int n;
            try
            {
                n = this->_src->Next();
            }
            catch (PyStopIteration e)
            {
                this->_is_eos = true;
                throw PyStopIteration(e);
            }
            if (n == 1)
            {
                //pass
            }
            else
            {
                this->_last_bit = (this->_last_bit + 1) % 2;
            }
            return this->_last_bit;
        }
        // @property
    public:
        TBSK_INT64 GetPos()const
        {
            return  this->_pos;
        }
        unique_ptr<vector<int>> Gets(int maxsize, bool fillup)override {
            return BasicRoStream<int>::Gets(maxsize, fillup);
        }
        void Seek(int size)override {
            return BasicRoStream<int>::Seek(size);
        }
    };

}

namespace TBSKmodemCPP
{
    using namespace std;

    // """ TBSKの変調クラスです。
    //     プリアンブルを前置した後にビットパターンを置きます。
    // """
    class TbskModulator
    {
    private:
        const shared_ptr<const TraitTone> _tone;
        const shared_ptr<const Preamble> _preamble;

        TraitBlockEncoder _enc;
    public:
        TbskModulator(const shared_ptr<const TraitTone>& tone) :
            _tone{ tone },
            _preamble{ make_shared<const CoffPreamble>(tone) },
            _enc{ TraitBlockEncoder(tone) }
        {
        }

        TbskModulator(const shared_ptr<const TraitTone>& tone, shared_ptr<const Preamble>& preamble) :
            _tone{ tone },
            _preamble{ preamble },
            _enc{ TraitBlockEncoder(tone) }
        {
        }
        virtual ~TbskModulator() {
        }

    private:
        IPyIterator<double>* ModulateAsBit(shared_ptr<IRoStream<int>>&& src)
        {
            auto ave_window_shift = max((int)(this->_tone->size() * 0.1), 2) / 2; //#検出用の平均フィルタは0.1*len(tone)//2だけずれてる。ここを直したらTraitBlockDecoderも直せ

            auto tbe = make_shared<TraitBlockEncoder>(this->_tone);
            tbe->SetInput(make_shared<DiffBitEncoder>(0, src));
            auto d = make_shared<vector<shared_ptr<IPyIterator<double>>>>();
            d->push_back(this->_preamble->GetPreamble());
            d->push_back(tbe);
            d->push_back(make_shared<Repeater<double>>(0, ave_window_shift));

            return new IterChain<double>(d);
        }
    public:
        IPyIterator<double>* ModulateAsBit(shared_ptr<IPyIterator<int>>& src)
        {
            return this->ModulateAsBit(make_shared<RoStream<int>>(src));
        }


        IPyIterator<double>* Modulate(shared_ptr<IPyIterator<int>>&& src, int bitwidth = 8)
        {
            auto s = make_shared<BitsWidthFilter>(bitwidth);
            s->SetInput(make_shared<RoStream<int>>(src));
            return this->ModulateAsBit(s);
        }


        IPyIterator<double>* Modulate(const char* src, int length = -1)
        {
            if (length < 0) {
                length = (int)strnlen_s(src, (size_t)16 * 1024);
            }
            auto d = make_shared<vector<int>>();
            for (auto i = 0;i < length;i++) {
                d->push_back((int)*(src + i));
            }
            return this->Modulate(make_shared<PyIterator<int>>(d), 8);
        }
    };
}

namespace TBSKmodemCPP
{
    using namespace std;





    class TbskDemodulator
    {
        // """ nBit intイテレータから1バイト単位のhex stringを返すフィルタです。
        // """
    private:
        template <typename T> class AsyncDemodulateX : public AsyncMethod<unique_ptr<IPyIterator<T>>>
        {
        private:
            TbskDemodulator& _parent;
            const int _tone_ticks;
            shared_ptr<RoStream<double>> _stream;
            bool _closed;
            NullableResult<TBSK_INT64> _peak_offset;
            int _co_step;
            unique_ptr<IPyIterator<T>> _result;
            shared_ptr<AsyncMethod<NullableResult<TBSK_INT64>>> _wsrex;
        protected:
            virtual unique_ptr<IPyIterator<T>> _CreateResult(const shared_ptr<TraitBlockDecoder>& src)const = 0;
        public:
            AsyncDemodulateX(TbskDemodulator& parent,const shared_ptr<IPyIterator<double>>& src) : AsyncMethod<unique_ptr<IPyIterator<T>>>(),
                _parent{ parent },
                _tone_ticks{ (int)parent._tone.size() },
                _stream{make_shared<RoStream<double>>(src)},
                _peak_offset{ NullableResult<TBSK_INT64>() }
            {
                //this->_wsrex = NULL;
                this->_co_step = 0;
                this->_closed = false;
                //this->_result = NULL;
            }
            virtual ~AsyncDemodulateX() {
                this->Close();
            }
            unique_ptr<IPyIterator<T>> GetResult() override
            {   //コピー不能値なのでここでは取り外す。
                TBSK_ASSERT(this->_co_step >= 4);
                auto a = move(this->_result);
                return a;

            }
            void Close()
            {
                if (!this->_closed)
                {
                    this->_wsrex.reset();
                    this->_parent._asmethod_lock = false;
                    this->_closed = true;

                }

            }

            bool Run()
            {
                //# print("run",self._co_step)
                TBSK_ASSERT(!this->_closed);

                if (this->_co_step == 0)
                {
                    try
                    {
                        this->_peak_offset = this->_parent._pa_detector.WaitForSymbol(this->_stream); //#現在地から同期ポイントまでの相対位置
                        TBSK_ASSERT(this->_wsrex == NULL);
                        this->_co_step = 2;
                    }
                    catch (RecoverableException<AsyncMethod<NullableResult<TBSK_INT64>>> &rexp)
                    {
                        this->_wsrex = rexp.Detach();
                        this->_co_step = 1;
                        TBSK_ASSERT(this->_result==NULL);
                        return false;
                    }
                }
                if (this->_co_step == 1)
                {
                    if (!this->_wsrex->Run()) {
                        return false;
                    } else {
                        this->_peak_offset = this->_wsrex->GetResult();
                        this->_wsrex.reset();
                        this->_co_step = 2;
                    }
                }
                if (this->_co_step == 2)
                {
                    if (!this->_peak_offset.success)
                    {
                        TBSK_ASSERT(this->_result == NULL);
                        this->Close();
                        this->_co_step = 4;
                        return true;
                    }
                    //# print(self._peak_offset)
                    this->_co_step = 3;
                }
                if (this->_co_step == 3)
                {
                    try
                    {
                        TBSK_ASSERT(this->_peak_offset.success);
                        //# print(">>",self._peak_offset+self._stream.pos)
                        this->_stream->Seek(this->_tone_ticks + (int)this->_peak_offset.value);// #同期シンボル末尾に移動
                        //# print(">>",stream.pos)
                        auto tbd = make_shared<TraitBlockDecoder>(this->_tone_ticks);
                        tbd->SetInput(this->_stream);
                        this->_stream.reset();
                        this->_result = this->_CreateResult(tbd);
                        TBSK_ASSERT(this->_result != NULL);
                        this->Close();
                        this->_co_step = 4;
                        return true;

                    }
                    catch (RecoverableStopIteration)
                    {
                        return false;

                    }
                    catch (PyStopIteration)
                    {
                        TBSK_ASSERT(this->_result == NULL);
                        this->Close();
                        this->_co_step = 4;
                        return true;

                    }
                }
                throw exception();

            }
        };
    private:
        const TraitTone& _tone;
        Preamble& _pa_detector;
        bool _asmethod_lock;

        TbskDemodulator(const TraitTone& tone, Preamble& preamble) :
            _pa_detector{preamble},
            _tone{tone}
        {
            this->_asmethod_lock = false;
        }




    //""" TBSK信号からビットを復元します。
    //    関数は信号を検知する迄制御を返しません。信号を検知せずにストリームが終了した場合はNoneを返します。
    //"""
    private:


    public:
        unique_ptr<IPyIterator<int>> DemodulateAsBit(const shared_ptr<IPyIterator<double>>& src)
        {
            TBSK_ASSERT(!this->_asmethod_lock);
            class Asdem :public AsyncDemodulateX<int> {
            public:
                Asdem(TbskDemodulator& parent,const shared_ptr<IPyIterator<double>>& src):AsyncDemodulateX<int>(parent,src) {}
            protected:
                unique_ptr<IPyIterator<int>> _CreateResult(const shared_ptr<TraitBlockDecoder>& src)const
                {
                    class IterWrapper :public virtual IPyIterator<int>
                    {
                    private:
                        const shared_ptr<IPyIterator<int>> _src;
                        IPyIterator<int>* _ptr;
                    public:
                        //PyIterator(const vector<T>* src);
                        IterWrapper(const shared_ptr<IPyIterator<int>>& src) :_src{ src }, _ptr{src.get()} {};
                        int Next()override { return this->_ptr->Next(); };
                    };
                    return make_unique<IterWrapper>(src);
                };
            };
            auto asmethod = make_shared<Asdem>(*this, src);
            if (asmethod->Run())
            {   
                auto ret= asmethod->GetResult();
                return ret;
            }
            else
            {
                this->_asmethod_lock = true;// #解放はAsyncDemodulateXのcloseで
                throw RecoverableException<Asdem>(asmethod);
            }
        }



        //    """ TBSK信号からnビットのint値配列を復元します。
        //        関数は信号を検知する迄制御を返しません。信号を検知せずにストリームが終了した場合はNoneを返します。
        //    """
        shared_ptr<IPyIterator<int>> DemodulateAsInt(const shared_ptr<IPyIterator<double>>& src, int bitwidth = 8)
        {
            TBSK_ASSERT(!this->_asmethod_lock);
            class Asdem :public AsyncDemodulateX<int> {
            private:
                const int _bitwidth;
            public:
                Asdem(TbskDemodulator& parent,const shared_ptr<IPyIterator<double>>& src,int bitwidth) :AsyncDemodulateX<int>(parent, src),_bitwidth(bitwidth) {}
            protected:
                unique_ptr<IPyIterator<int>> _CreateResult(const shared_ptr<TraitBlockDecoder>& src)const override
                {
                    auto bwf= make_unique<BitsWidthFilter>(1, this->_bitwidth);
                    bwf->SetInput(src);
                    return bwf;
                };
            };
            
            auto asmethod = make_shared<Asdem>(*this, src,bitwidth);
            if (asmethod->Run())
            {
                auto ret = asmethod->GetResult();
                return ret;
            }
            else
            {
                this->_asmethod_lock = true;// #解放はAsyncDemodulateXのcloseで
                throw RecoverableException<Asdem>(asmethod);
            }
        }
        unique_ptr<IPyIterator<char>> DemodulateAsChar(const shared_ptr<IPyIterator<double>>& src)
        {
            TBSK_ASSERT(!this->_asmethod_lock);

            class CharFilter :public virtual BasicRoStream<char>, public virtual IFilter<CharFilter, IRoStream<int>, char>
            {
            private:
                TBSK_INT64 _pos;
                unique_ptr<BitsWidthConvertIterator> _iter;
            public:
                CharFilter() :BasicRoStream() {
                    this->_pos = 0;
                }
                virtual ~CharFilter() {
                }
                CharFilter& SetInput(shared_ptr<IRoStream<int>>&& src)override
                {
                    this->_pos = 0;
                    this->_iter = make_unique<BitsWidthConvertIterator>(src, 1, 8);
                    return *this;
                }
                char Next()override
                {
                    if (this->_iter == NULL) {
                        throw PyStopIteration();
                    }
                    auto r = this->_iter->Next();
                    this->_pos = this->_pos + 1;
                    return (char)r;
                }
                // @property
                TBSK_INT64 GetPos()const override
                {
                    return this->_pos;
                }
            };

            class Asdem :public AsyncDemodulateX<char> {
            public:
                Asdem(TbskDemodulator& parent,const shared_ptr<IPyIterator<double>>& src) :AsyncDemodulateX<char>(parent, src) {}
            protected:
                unique_ptr<IPyIterator<char>> _CreateResult(const shared_ptr<TraitBlockDecoder>& src)const {
                    auto bwf = make_unique<CharFilter>();
                    bwf->SetInput(src);
                    return bwf;
                };
            };







            auto asmethod = make_shared<Asdem>(*this, src);
            if (asmethod->Run())
            {
                auto ret = asmethod->GetResult();
                return ret;
            }
            else
            {
                this->_asmethod_lock = true;// #解放はAsyncDemodulateXのcloseで
                throw RecoverableException<Asdem>(asmethod);
            }
        }
    };


}
