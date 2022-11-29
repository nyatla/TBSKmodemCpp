#include "./CoffPreamble.h"
#include "../traitblockcoder.h"
#include "../../../utils/AsyncMethod.h"
#include "../../../interfaces.h"
#include "../../../streams/BitStream.h"
#include "../../../compatibility.h"
#include "../../../utils/recoverable.h"
#include "../../../utils/AverageInterator.h"
#include "../../../utils/BufferedIterator.h"
#include "../../../utils/RingBuffer.h"

namespace TBSKmodemCPP
{
    using std::make_shared;
}

namespace TBSKmodemCPP
{

#pragma warning( disable : 4250 )

    class PreambleBits :public TraitBlockEncoder {
    public:
        PreambleBits(const shared_ptr<const TraitTone>& symbol, int cycle) :TraitBlockEncoder(symbol)
        {
            auto bs = make_shared<vector<int>>();
            //const std::vector<int> a{ 0, 1 };
            bs->push_back(0);
            bs->push_back(1);
            //std::vector<int> b(cycle, 1);//Functions::Repeat(cycle, 1);// [1]*self._cycle;
            for (auto i = 0;i < cycle;i++) {
                bs->push_back(1);
            }
            //auto b2 = std::vector<int>{ 1 };
            bs->push_back(1);
            //auto c = std::vector<int>();
            //for (auto i = 0; i < cycle; i++)
            //{
            //    c.push_back(i % 2);
            //}
            for (auto i = 0; i < cycle; i++)
            {
                bs->push_back(i % 2);
            }
            int c = (*bs)[bs->size() - 1];
            //auto d = std::vector<int>{ (1 + *c.end()) % 2, (1 + *c.end()) % 2, *c.end() };
            bs->push_back((1 + c) % 2);
            bs->push_back((1 + c) % 2);
            bs->push_back(c);

            //auto pa = new PyIteratorAttached<int>(bs);
            //this->_stream = new BitStream(*pa, 1);
            //this->_pa = pa;
            auto w = make_shared<PyIterator<int>>(bs);
            auto ptr = make_unique<BitStream>(w, 1);
            this->SetInput(move(ptr));
            //this->_bitstream = bs;
            //this->SetInput(new BitStream(Functions.Flatten(new int[] { 0, 1 }, b, new int[] { 1 }, c, d), bitwidth:1));
        }

        virtual ~PreambleBits() {
        }
    };
#pragma warning( default : 4250 )

}
namespace TBSKmodemCPP
{

    class ASwaitForSymbol : public AsyncMethod<NullableResult<TBSK_INT64>> {
    private:
        const int _symbol_ticks;
        const int _sample_width;
        int _co_step;
        CoffPreamble& _parent;
        //SelfCorrcoefIterator _scofiter;
        shared_ptr<BufferedIterator<double>> _cof;
        unique_ptr<AverageInterator> _avi;
        //AverageInterator _avi;
        int _cofbuf_len;
        unique_ptr<RingBuffer<double>> _rb;
        double _gap;
        int _nor;
        double _pmax;
        bool _closed;
        NullableResult<TBSK_INT64> _result;

    public:
        ASwaitForSymbol(CoffPreamble& parent,const shared_ptr<IRoStream<double>>& src) :AsyncMethod(),
            _symbol_ticks{ (int)parent._symbol->size() },
            _sample_width{ parent._cycle + 1 },
            _co_step{ 0 },
            _parent{ parent },
            //_scofiter{ SelfCorrcoefIterator(this->_symbol_ticks, src, this->_symbol_ticks) },
            _cof{make_shared<BufferedIterator<double>>(
                make_shared<SelfCorrcoefIterator>(this->_symbol_ticks, src, this->_symbol_ticks), this->_symbol_ticks * (6 + parent._cycle * 2), 0)
            },
            _avi{make_unique<AverageInterator>(this->_cof, this->_symbol_ticks)},
            _rb{make_unique<RingBuffer<double>>(this->_symbol_ticks * this->_sample_width, 0) },
            _result{ NullableResult<TBSK_INT64>() },
            _pmax{ 0 }
        {

            auto symbol_ticks = (int)parent._symbol->size();
            //#後で見直すから10シンボル位記録しておく。
            auto cofbuf_len = symbol_ticks * (6 + parent._cycle * 2);
            //# cofbuf_len=symbol_ticks*10
            //this->_parent = parent;
            //this->_cof = new BufferedIterator<double>(this->_scofiter, cofbuf_len, 0);
            //this->_avi = new AverageInterator(this._cof, symbol_ticks);
            //auto sample_width = parent._cycle + 1;
            //# rb=RingBuffer(symbol_ticks*3,0)
            //this->_sample_width = sample_width;
            this->_cofbuf_len = cofbuf_len;
            //this->_symbol_ticks = symbol_ticks;
            //this->_rb = new RingBuffer<double>(symbol_ticks * sample_width, 0);
            this->_gap = 0; //#gap
            this->_nor = 0; //#ストリームから読みだしたTick数
            //this._pmax;
            this->_co_step = 0;
            this->_closed = false;

        }
        NullableResult<TBSK_INT64> GetResult()
        {
            return this->_result; //コピー可能な値なので特に何もすることなく。
            //TBSK_ASSERT(this->_co_step >= 4);
            //return this->_result;
        }
    public:
        void Close()override
        {
            if (!this->_closed)
            {
                this->_parent._asmethtod_lock = false;
                this->_closed = true;
            }
        }
        bool Run()override
        {
            TBSK_ASSERT(!this->_closed);
            //# print("wait",self._co_step)
            if (this->_closed)
            {
                return true;
            }
            //# ローカル変数の生成
            auto avi = this->_avi.get();
            auto cof = this->_cof.get();
            auto rb = this->_rb.get();
            try
            {
                while (true)
                {
                    //# ギャップ探索
                    if (this->_co_step == 0)
                    {
                        this->_gap = 0;
                        this->_co_step = 1;
                    }
                    //# ASync #1
                    if (this->_co_step == 1)
                    {
                        while (true)
                        {
                            try
                            {
                                rb->Append(avi->Next());
                                //# print(rb.tail)
                                this->_nor = this->_nor + 1;
                                this->_gap = rb->GetTop() - rb->GetTail();
                                if (this->_gap < 0.5)
                                {
                                    continue;
                                }
                                if (rb->GetTop() < 0.1)
                                {
                                    continue;
                                }
                                if (rb->GetTail() > -0.1)
                                {
                                    continue;
                                }
                                break;
                            }
                            catch (RecoverableStopIteration)
                            {
                                return false;
                            }
                        }
                        this->_co_step = 2; //#Co進行
                    }
                    if (this->_co_step == 2)
                    {
                        //# print(1,self._nor,rb.tail,rb.top,self._gap)
                        //# ギャップ最大化
                        while (true)
                        {
                            try
                            {
                                rb->Append(avi->Next());
                                this->_nor = this->_nor + 1;
                                auto w = rb->GetTop() - rb->GetTail();
                                if (w >= this->_gap)
                                {
                                    //# print(w,self._gap)
                                    this->_gap = w;
                                    continue;
                                }
                                break;
                            }
                            catch (RecoverableStopIteration)
                            {
                                return false;
                            }
                        }
                        //# print(2,nor,rb.tail,rb.top,self._gap)
                        if (this->_gap < this->_parent._threshold)
                        {
                            this->_co_step = 0;// #コルーチンをリセット
                            continue;
                        }
                        //# print(3,nor,rb.tail,rb.top,self._gap)
                        //# print(2,nor,self._gap)
                        this->_pmax = rb->GetTail();
                        this->_co_step = 3;
                    }
                    if (this->_co_step == 3)
                    {
                        //#同期シンボルピーク検出
                        while (true)
                        {
                            try
                            {
                                auto n = avi->Next();
                                this->_nor = this->_nor + 1;
                                if (n > this->_pmax)
                                {
                                    this->_pmax = n;
                                    continue;
                                }
                                if (this->_pmax > 0.1)
                                {
                                    break;
                                }
                            }
                            catch (RecoverableStopIteration)
                            {
                                return false;
                            }
                        }
                        this->_co_step = 4; //#end
                        auto symbol_ticks = this->_symbol_ticks;
                        auto sample_width = this->_sample_width;
                        auto cofbuf_len = this->_cofbuf_len;
                        auto cycle = this->_parent._cycle;

                        //# print(4,self._nor,rb.tail,rb.top,self._gap)
                        //# print(3,self._nor)
                        //# #ピーク周辺の読出し
                        //# [next(cof) for _ in range(symbol_ticks//4)]
                        //# バッファリングしておいた相関値に3値平均フィルタ
                        auto& buf_s = cof->GetBuf().Sublist((int)(cof->GetBuf().GetLength() - symbol_ticks), symbol_ticks);//buf = cof.buf[-symbol_ticks:]
                        auto buf = buf_s.get();
                        //var b =[(i + self._nor - symbol_ticks + 1, buf[i] + buf[i + 1] + buf[2]) for i in range(len(buf) - 2)];// #位置,相関値
                        struct TPcTuple {
                            int pos;
                            double cof;
                            bool operator<(const struct TPcTuple& right) const {
                                return cof <= right.cof;
                            }
                        };
                        vector<struct TPcTuple> b(buf->size() - 2);
                        for (auto i = 0; i < buf->size() - 2; i++)
                        {
                            b[i].pos = i + this->_nor - symbol_ticks + 1;
                            b[i].cof = buf->at(i) + buf->at((size_t)i + 1) + buf->at(2);
                            //b.Add((i + this._nor - symbol_ticks + 1, buf[i] + buf[i + 1] + buf[2]));
                        }
                        //var b.sort(key = lambda x: x[1], reverse = True);
                        vector_sort(b, true);

                        //#ピークを基準に詳しく様子を見る。
                        auto peak_pos = b[0].pos;//b[0][0];
                        //# print(peak_pos-symbol_ticks*3,(self._nor-(peak_pos+symbol_ticks*3)))
                        //# Lレベルシンボルの範囲を得る
                        //# s=peak_pos-symbol_ticks*3-(self._nor-cofbuf_len)
                        auto s = peak_pos - symbol_ticks * sample_width - (this->_nor - cofbuf_len);
                        auto& lw_s = cof->GetBuf().Sublist(s, cycle * symbol_ticks);
                        auto lw = lw_s.get();

                        vector_sort(lw);
                        auto lwlen = (int)(lw->size()) * 3 / 2 + 1;
                        auto lwsum = vector_sum(lw, 0, lwlen);
                        auto lw0 = lw->at(0);

                        //lw = lw.Take(lw.Length * 3 / 2 + 1).ToArray(); //lw[:len(lw) * 3 / 2 + 1];
                        if (lwsum / lwlen > lw0 * 0.66)
                        {
                            //# print("ERR(L",peak_pos+src.pos,sum(lw)/len(lw),min(lw))
                            this->_co_step = 0;//#co_step0からやり直す。
                            continue;// #バラツキ大きい
                        }


                        //#Hレベルシンボルの範囲を得る
                        //# s=peak_pos-symbol_ticks*6-(self._nor-cofbuf_len)
                        s = peak_pos - symbol_ticks * sample_width * 2 - (this->_nor - cofbuf_len);
                        auto& lh_s = cof->GetBuf().Sublist(s, cycle * symbol_ticks);
                        auto lh = lh_s.get();
                        vector_sort(lh, true);
                        auto lhlen = (int)(lh->size()) * 3 / 2 + 1;
                        auto lhsum = vector_sum(lh, 0, lhlen);
                        auto lh0 = lh->at(0);
                        //lh = lh.Take(lh.Length * 3 / 2 + 1).ToArray(); //lh = lh[:len(lh) * 3 / 2 + 1]

                        if (lhsum / lhlen < lh0 * 0.66)
                        {
                            //# print("ERR(H",peak_pos+src.pos,sum(lh)/len(lh),max(lh))
                            this->_co_step = 0;// #co_step0からやり直す。
                            continue; //#バラツキ大きい
                        }
                        //#値の高いのを抽出してピークとする。
                        //# print(peak_pos)
                        this->_result.set((TBSK_INT64)peak_pos - this->_nor);//#現在値からの相対位置
                        this->Close();
                        return true;
                    }
                    throw std::runtime_error("Invalid co_step");
                }
            }
            catch (PyStopIteration) {
                this->_co_step = 4; //#end
                this->Close();
                this->_result.set();
                return true;
                //# print("END")
            }
            catch (std::exception) {
                this->_co_step = 4; //#end
                this->Close();
                throw;
            }
        }
    };

    CoffPreamble::CoffPreamble(const shared_ptr<const TraitTone>& tone, double threshold, int cycle) :
        _symbol{ tone },
        _threshold{threshold},
        _cycle{cycle}
    {
        this->_asmethtod_lock = false;
    }

    shared_ptr<IRoStream<double>> CoffPreamble::GetPreamble()const {

        return make_shared<PreambleBits>(this->_symbol, this->_cycle);
        // # return enc.setInput(BitStream([0,1]+[1,1]+[1]+[0,1]+[0,0,1],1))
        // # return enc.setInput(BitStream([0,1,1,1,1,0,1,0,0,1],1))
    }
    NullableResult<TBSK_INT64> CoffPreamble::WaitForSymbol(const shared_ptr<IRoStream<double>>&& src)
    {
        TBSK_ASSERT(!this->_asmethtod_lock);
        auto asmethtod = make_shared<ASwaitForSymbol>(*this, src);
        if (asmethtod->Run())
        {
            return asmethtod->GetResult();
        }
        else
        {
            //# ロックする（解放はASwaitForSymbolのclose内で。）
            this->_asmethtod_lock = true;
            throw RecoverableException<NullableResult<TBSK_INT64>>(asmethtod);
        }
    }


}