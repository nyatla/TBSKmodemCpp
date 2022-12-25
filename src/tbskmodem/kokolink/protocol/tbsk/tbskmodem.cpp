#include "./tbskmodem.h"
#include "../../utils/AsyncMethod.h"
#include "../../streams/BitStream.h"
#include "../../filter/BitsWidthFilter.h"
#include "./preamble/CoffPreamble.h"
#include <string.h>

namespace TBSKmodemCPP
{
#pragma warning( disable : 4250 )

    class DiffBitEncoder : public BasicRoStream<int>, virtual public IBitStream
    {
    private:
        const shared_ptr<IRoStream<int>> _src;
        int _last_bit;
        bool _is_eos;
        int _pos;
    public:
        DiffBitEncoder(int firstbit, const shared_ptr<IRoStream<int>>& src) :BasicRoStream(),
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
            catch (PyStopIteration&)
            {
                this->_is_eos = true;
                throw;
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
        TBSK_INT64 GetPos()const override
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
#pragma warning( default : 4250 )
}


namespace TBSKmodemCPP
{
    using std::make_shared;
    using std::max;
}
namespace TBSKmodemCPP
{
    TbskModulator::TbskModulator(const shared_ptr<const TraitTone>& tone) :
        _tone{ tone },
        _preamble{ make_shared<const CoffPreamble>(tone) },
        _enc{ make_unique<TraitBlockEncoder>(tone) }
    {
    }

    TbskModulator::TbskModulator(const shared_ptr<const TraitTone>& tone, const shared_ptr<const Preamble>& preamble) :
        _tone{ tone },
        _preamble{ preamble },
        _enc{ make_shared<TraitBlockEncoder>(tone) }
    {
    }
    TbskModulator::~TbskModulator() {
    }

    shared_ptr<IPyIterator<double>> TbskModulator::_ModulateAsBit(const shared_ptr<IRoStream<int>>& src)
    {
        auto ave_window_shift = max((int)(this->_tone->size() * 0.1), 2) / 2; //#検出用の平均フィルタは0.1*len(tone)//2だけずれてる。ここを直したらTraitBlockDecoderも直せ

        auto& tbe = this->_enc;
        tbe->SetInput(make_shared<DiffBitEncoder>(0, src));
        vector <shared_ptr<IPyIterator<double>>> d{
            this->_preamble->GetPreamble(),
            tbe,
            make_shared<Repeater<double>>(0, ave_window_shift)
        };
        return make_shared<IterChain<double>>(d);
    }
    shared_ptr<IPyIterator<double>> TbskModulator::ModulateAsBit(const shared_ptr<IRoStream<int>>& src) {
        return this->_ModulateAsBit(src);
    }
    shared_ptr<IPyIterator<double>> TbskModulator::ModulateAsBit(const shared_ptr<IPyIterator<int>>& src, int bitwidth) {
        auto s = make_shared<BitsWidthFilter>(bitwidth);
        s->SetInput(make_shared<RoStream<int>>(src));
        return this->ModulateAsBit(s);
    }

    shared_ptr<IPyIterator<double>> TbskModulator::Modulate(const shared_ptr<IPyIterator<int>>& src, int bitwidth)
    {
        auto s = make_shared<BitsWidthFilter>(bitwidth);
        s->SetInput(make_shared<RoStream<int>>(src));
        return this->ModulateAsBit(s);
    }


    shared_ptr<IPyIterator<double>> TbskModulator::Modulate(const char* src, int length)
    {
        if (length < 0) {
            length = (int)strnlen(src, (size_t)16 * 1024);
        }
        auto d = make_shared<vector<int>>();
        for (auto i = 0;i < length;i++) {
            d->push_back((int)*(src + i));
        }
        return this->Modulate(make_shared<PyIterator<int>>(d), 8);
    }
}

namespace TBSKmodemCPP
{

    template <typename T> class ICreateResult{
    public:
        virtual shared_ptr<IPyIterator<T>> _CreateResult(const shared_ptr<TraitBlockDecoder>& src)const = 0;
    };
#pragma warning( disable : 4250 )
    template <typename T> class AsyncDemodulateX :public virtual AsyncMethod<shared_ptr<IPyIterator<T>>>
    {
    private:
        TbskDemodulator& _parent;
        const int _tone_ticks;
        shared_ptr<RoStream<double>> _stream;
        bool _closed;
        NullableResult<TBSK_INT64> _peak_offset;
        int _co_step;
        shared_ptr<IPyIterator<T>> _result;
        shared_ptr<WaitForSymbolAS> _wsrex;
    protected:
        const shared_ptr<ICreateResult<T>> _create_result;
    public:
        AsyncDemodulateX(TbskDemodulator& parent,const shared_ptr<ICreateResult<T>>& create_result,const shared_ptr<IPyIterator<double>>& src) : AsyncMethod<shared_ptr<IPyIterator<T>>>(),
            _parent{ parent },
            _tone_ticks{ (int)parent._tone->size() },
            _stream{ make_shared<RoStream<double>>(src) },
            _peak_offset{ NullableResult<TBSK_INT64>() },
            _create_result{create_result}
        {
            //this->_wsrex = NULL;
            this->_co_step = 0;
            this->_closed = false;
            //this->_result = NULL;
        }
        virtual ~AsyncDemodulateX() {
            this->Close();
        }
        shared_ptr<IPyIterator<T>> GetResult() override
        {   //コピー不能値なのでここでは取り外す。
            TBSK_ASSERT(this->_co_step >= 4);
            return this->_result;

        }
        void Close()override
        {
            if (!this->_closed)
            {
                this->_wsrex.reset();
                this->_parent._asmethod_lock = false;
                this->_closed = true;

            }
        }
        bool Run()override
        {
            //# print("run",self._co_step)
            TBSK_ASSERT(!this->_closed);

            if (this->_co_step == 0)
            {
                try
                {
                    this->_peak_offset = this->_parent._pa_detector->WaitForSymbol(this->_stream); //#現在地から同期ポイントまでの相対位置
                    TBSK_ASSERT(this->_wsrex == NULL);
                    this->_co_step = 2;
                }
                catch (RecoverableException<WaitForSymbolAS>& rexp)
                {
                    this->_wsrex =  rexp.Detach();;//rexp.Detach();
                    this->_co_step = 1;
                    TBSK_ASSERT(this->_result == NULL);
                    return false;
                }
            }

            if (this->_co_step == 1)
            {
                if (!this->_wsrex->Run()) {
                    return false;
                }
                else {
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
                    this->_result = this->_create_result->_CreateResult(tbd);
                    TBSK_ASSERT(this->_result != NULL);
                    this->Close();
                    this->_co_step = 4;
                    return true;

                }
                catch (RecoverableStopIteration&)
                {
                    return false;

                }
                catch (PyStopIteration&)
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
#pragma warning(default : 4250 )


#pragma warning(disable : 4250 )

}
namespace TBSKmodemCPP
{
    TbskDemodulator::TbskDemodulator(const shared_ptr<TraitTone>& tone) :
        _pa_detector{ make_shared<CoffPreamble>(tone) },
        _tone{ tone }
    {
        this->_asmethod_lock = false;
    }

    TbskDemodulator::TbskDemodulator(const shared_ptr<TraitTone>& tone, const shared_ptr<Preamble>& preamble) :
        _pa_detector{ preamble },
        _tone{ tone }
    {
        this->_asmethod_lock = false;
    }




    class DemodulateAsBitAS_impl :public virtual DemodulateAsBitAS, public AsyncDemodulateX<int> {
        class CreateResult :public ICreateResult<int> {
        public:
            shared_ptr<IPyIterator<int>> _CreateResult(const shared_ptr<TraitBlockDecoder>& src)const override
            {
                class IterWrapper :public virtual IPyIterator<int>
                {
                private:
                    const shared_ptr<IPyIterator<int>> _src;
                    IPyIterator<int>* _ptr;
                public:
                    //PyIterator(const vector<T>* src);
                    IterWrapper(const shared_ptr<IPyIterator<int>>& src) :_src{ src }, _ptr{ src.get() } {};
                    int Next()override { return this->_ptr->Next(); };
                };
                return make_shared<IterWrapper>(src);
            };
        };
    public:
        DemodulateAsBitAS_impl(TbskDemodulator& parent, const shared_ptr<IPyIterator<double>>& src) :
            AsyncDemodulateX<int>(parent, make_shared<CreateResult>(), src) {
        }
    };


    shared_ptr<IPyIterator<int>> TbskDemodulator::DemodulateAsBit(const shared_ptr<IPyIterator<double>>& src)
    {
        TBSK_ASSERT(!this->_asmethod_lock);
        auto asmethod = make_shared<DemodulateAsBitAS_impl>(*this,src);
        if (asmethod->Run())
        {   
            auto ret= asmethod->GetResult();
            return ret;
        }
        else
        {
            this->_asmethod_lock = true;// #解放はAsyncDemodulateXのcloseで
            throw RecoverableException<DemodulateAsBitAS>(std::dynamic_pointer_cast<DemodulateAsBitAS>(asmethod));
        }
    }


    class DemodulateAsIntAS_impl :public virtual DemodulateAsIntAS, public AsyncDemodulateX<int> {
        class CreateResult :public ICreateResult<int> {
            const int _bitwidth;
        public:
            CreateResult(int bitwidth) : _bitwidth(bitwidth) {}
            shared_ptr<IPyIterator<int>> _CreateResult(const shared_ptr<TraitBlockDecoder>& src)const override
            {
                auto bwf = make_shared<BitsWidthFilter>(1, this->_bitwidth);
                bwf->SetInput(src);
                return bwf;
            };
        };
    public:
        DemodulateAsIntAS_impl(TbskDemodulator& parent, const shared_ptr<IPyIterator<double>>& src,int bitwidth) :
            AsyncDemodulateX<int>(parent, make_shared<CreateResult>(bitwidth), src) {
        }
    };




    //    """ TBSK信号からnビットのint値配列を復元します。
    //        関数は信号を検知する迄制御を返しません。信号を検知せずにストリームが終了した場合はemptyのshared_ptrを返します。
    //    """
    shared_ptr<IPyIterator<int>> TbskDemodulator::DemodulateAsInt(const shared_ptr<IPyIterator<double>>& src, int bitwidth)
    {
        TBSK_ASSERT(!this->_asmethod_lock);
        //class CreateResult :public ICreateResult<int> {
        //    const int _bitwidth;
        //public:
        //    CreateResult(int bitwidth) : _bitwidth(bitwidth) {}
        //    shared_ptr<IPyIterator<int>> _CreateResult(const shared_ptr<TraitBlockDecoder>& src)const override
        //    {
        //        auto bwf = make_shared<BitsWidthFilter>(1, this->_bitwidth);
        //        bwf->SetInput(src);
        //        return bwf;
        //    };
        //};
            
        auto asmethod = make_shared<DemodulateAsIntAS_impl>(*this, src,bitwidth);
        if (asmethod->Run())
        {
            const auto ret = asmethod->GetResult();
            return ret;
        }
        else
        {
            this->_asmethod_lock = true;// #解放はAsyncDemodulateXのcloseで
            //throw RecoverableException<DemodulateAsIntAS>(std::dynamic_pointer_cast<DemodulateAsIntAS>(asmethod));
            throw RecoverableException<DemodulateAsIntAS>(asmethod);
        }
    }


    class DemodulateAsCharAS_impl :public virtual DemodulateAsCharAS, public AsyncDemodulateX<char> {
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
            CharFilter& SetInput(const shared_ptr<IRoStream<int>>& src)override
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
        class CreateResult :public ICreateResult<char> {
        public:
            shared_ptr<IPyIterator<char>> _CreateResult(const shared_ptr<TraitBlockDecoder>& src)const {
                auto bwf = make_shared<CharFilter>();
                bwf->SetInput(src);
                return bwf;
            };
        };
    public:
        DemodulateAsCharAS_impl(TbskDemodulator& parent, const shared_ptr<IPyIterator<double>>& src) :
            AsyncDemodulateX<char>(parent, make_shared<CreateResult>(), src) {
        }
    };
#pragma warning(default : 4250 )



//    class DemodulateAsCharAS :public virtual DemodulateAsCharAS, public AsyncDemodulateX<char> {
//#pragma warning(disable : 4250 )
//        class CharFilter :public virtual BasicRoStream<char>, public virtual IFilter<CharFilter, IRoStream<int>, char>
//        {
//        private:
//            TBSK_INT64 _pos;
//            unique_ptr<BitsWidthConvertIterator> _iter;
//        public:
//            CharFilter() :BasicRoStream() {
//                this->_pos = 0;
//            }
//            virtual ~CharFilter() {
//            }
//            CharFilter& SetInput(const shared_ptr<IRoStream<int>>& src)override
//            {
//                this->_pos = 0;
//                this->_iter = make_unique<BitsWidthConvertIterator>(src, 1, 8);
//                return *this;
//            }
//            char Next()override
//            {
//                if (this->_iter == NULL) {
//                    throw PyStopIteration();
//                }
//                auto r = this->_iter->Next();
//                this->_pos = this->_pos + 1;
//                return (char)r;
//            }
//            // @property
//            TBSK_INT64 GetPos()const override
//            {
//                return this->_pos;
//            }
//        };
//#pragma warning(default : 4250 )
//    public:
//        DemodulateAsCharAS(TbskDemodulator& parent, const shared_ptr<IPyIterator<double>>& src) :AsyncDemodulateX<char>(parent, src) {}
//    protected:
//        shared_ptr<IPyIterator<char>> _CreateResult(const shared_ptr<TraitBlockDecoder>& src)const {
//            auto bwf = make_shared<CharFilter>();
//            bwf->SetInput(src);
//            return bwf;
//        };
//    };


    shared_ptr<IPyIterator<char>> TbskDemodulator::DemodulateAsChar(const shared_ptr<IPyIterator<double>>& src)
    {
        TBSK_ASSERT(!this->_asmethod_lock);

        auto asmethod = make_shared<DemodulateAsCharAS_impl>(*this, src);
        if (asmethod->Run())
        {
            auto ret = asmethod->GetResult();
            return ret;
        }
        else
        {
            this->_asmethod_lock = true;// #解放はAsyncDemodulateXのcloseで
            throw RecoverableException<DemodulateAsCharAS>(std::dynamic_pointer_cast<DemodulateAsCharAS>(asmethod));
        }
    }
};


