#include "./traitblockcoder.h"
#include "../../utils/SumIterator.h"
#include "../../utils/AverageInterator.h"


namespace TBSKmodemCPP
{
    using std::make_shared;
}

namespace TBSKmodemCPP
{

    // """ ビット列をTBSK変調した振幅信号に変換します。出力レートは入力のN倍になります。
    //     N=len(trait_block)*2*len(tone)
    //     このクラスは、toneを一単位とした信号とtrail_blockとの積を返します。
    // """

    TraitBlockEncoder::TraitBlockEncoder(const shared_ptr<const TraitTone>& tone) :
        _btone{ tone },
        _is_eos{ true },
        _src{ shared_ptr<IBitStream>()},
        _pos{ 0 }
    {
        this->_sblock.push_back(-1);
    }
    TraitBlockEncoder::~TraitBlockEncoder() {

    }
    TraitBlockEncoder& TraitBlockEncoder::SetInput(const shared_ptr<IBitStream>&& src)
    {
        this->_is_eos = false;//True if src is None else False
        //this->_tone_q->Clear();//=new Queue<float>();
        queue<double> empty;   this->_tone_q.swap(empty);//キューのクリア
        // # print(len(self._tone_q))
        this->_pos = 0;
        this->_src = src;
        return *this;
    }

    double TraitBlockEncoder::Next()
    {
        if (this->_tone_q.size() == 0) {
            if (this->_is_eos) {
                throw PyStopIteration();
            }
            try {
                auto sign = this->_src->Next() != 0 ? 1 : -1;//1 if next(self._src)!=0 else -1
                const TraitTone* btone = this->_btone.get();
                for (auto i : this->_sblock) {
                    for (auto j : *btone) {
                        this->_tone_q.push(sign * i * j);
                    }
                }
            }
            catch (PyStopIteration e) {
                this->_is_eos = true;
                throw e;
            }
        }
        auto r = this->_tone_q.front();
        this->_tone_q.pop();
        this->_pos += 1; //#posのインクリメント
        return r;
    }

    // @property
    TBSK_INT64 TraitBlockEncoder::GetPos()const
    {
        return this->_pos;
    }
}

namespace TBSKmodemCPP
{
    // """ シンボル幅がNのTBSK相関信号からビットを復調します。

    // """
    TraitBlockDecoder::TraitBlockDecoder(int trait_block_ticks, double threshold) :
        _trait_block_ticks{ trait_block_ticks },
        _threshold{ threshold },
        _avefilter{ move(unique_ptr<AverageInterator>()) }
    {
        this->_is_eos = true;
        //this->_samples = new List<double>();// #観測値
    }
    TraitBlockDecoder::~TraitBlockDecoder() {
    }

        // """ 
        //     Args:
        //         src
        //             TBSK信号の開始エッジにポインタのあるストリームをセットします。
        // """
    TraitBlockDecoder& TraitBlockDecoder::SetInput(const shared_ptr<IRoStream<double>>&& src)
    {
        this->_is_eos = false;
        auto cof = make_shared<SelfCorrcoefIterator>(this->_trait_block_ticks, src, this->_trait_block_ticks);
        auto ave_window = std::max((int)(this->_trait_block_ticks * 0.1), 2);// #検出用の平均フィルタは0.1*len(tone)//2だけずれてる。個々を直したらtbskmodem#TbskModulatorも直せ
        this->_avefilter = make_unique<AverageInterator>(cof, ave_window);
        this->_last_data = 0;

        this->_preload_size = this->_trait_block_ticks + ave_window / 2 - 1;    //#平均値フィルタの初期化サイズ。ave_window/2足してるのは、平均値の遅延分.
        this->_block_skip_size = this->_trait_block_ticks - 1 - 2; //#スキップ数
        this->_block_skip_counter = this->_block_skip_size; //#スキップ数
        this->_samples.clear();// = new List<double>();// #観測値
        this->_shift = 0;

        // # try:
        // #     [next(self._avefilter) for i in range(self._trait_block_ticks+ave_window//2)]
        // # except StopIteration:
        // #     self._is_eos=True
        this->_pos = 0;
        return *this;
    }
    int TraitBlockDecoder::Next()
    {
        if (this->_is_eos) {
            throw PyStopIteration();
        }
        try {
            // #この辺の妙なカウンターはRecoverableStopInterationのため
            auto lavefilter = this->_avefilter.get();
            assert(lavefilter != NULL);

            // #平均イテレータの初期化(初めの一回目だけ)
            while (this->_preload_size > 0) {
                lavefilter->Next();
                this->_preload_size = this->_preload_size - 1;
            }
            // #ブロックヘッダの読み出し(1ブロック読出しごとにリセット)
            while (this->_block_skip_counter > 0) {
                lavefilter->Next();
                this->_block_skip_counter = this->_block_skip_counter - 1;
            }
            while (this->_samples.size() < 3) {
                this->_samples.push_back(lavefilter->Next());
            }




            std::vector<double>& samples = this->_samples;
            auto r = samples[1];
            if (samples[0] * samples[1] < 0 || samples[1] * samples[2] < 0) {
                // #全ての相関値が同じ符号でなければ何もしない
                this->_block_skip_counter = this->_block_skip_size; //#リセット
            }
            else {
                const size_t sz = samples.size();

                std::vector<double> asample(sz);
                // #全ての相関値が同じ符号
                for (auto i = 0; i < sz; i++) {
                    asample[i] = abs(samples[i]);
                }
                // #一番大きかったインデクスを探す
                if (asample[1] > asample[0] && asample[1] > asample[2]) {
                    // #遅れも進みもしてない
                    // pass
                }
                else if (asample[0] > asample[2]) {
                    // #探索場所が先行してる
                    this->_shift = this->_shift - 1;
                }
                else if (asample[0] < asample[2]) {
                    // #探索場所が遅行してる
                    this->_shift = this->_shift + 1;
                }
                else {
                    // #不明
                    // pass
                }

                if (this->_shift > 10) {
                    this->_shift = 0;
                    // # print(1)
                    this->_block_skip_counter = this->_block_skip_size + 1;
                }
                else if (this->_shift < -10) {
                    this->_shift = 0;
                    // # print(-1)
                    this->_block_skip_counter = this->_block_skip_size - 1;
                }
                else {
                    this->_block_skip_counter = this->_block_skip_size;
                }
            }


            this->_samples.clear();

            // # print(self._src.pos,r)
            auto th = this->_threshold;
            this->_pos = this->_pos + 1;
            if (r > th) {
                // # print(1,1)
                this->_last_data = r;
                return 1;
            }
            else if (r < -th) {
                // # print(2,0)
                this->_last_data = r;
                return 0;
            }
            else if (this->_last_data - r > th) {
                // # print(3,1)
                this->_last_data = r;
                return 1;
            }
            else if (r - this->_last_data > th) {
                // # print(4,0)
                this->_last_data = r;
                return 0;
            }
            else {
                this->_is_eos = true;
                throw PyStopIteration();
            }
        }
        catch (RecoverableStopIteration e) {
            throw e;
        }
        catch (PyStopIteration e) {
            this->_is_eos = true;
            throw e;
        }
    }
    // @property
    TBSK_INT64 TraitBlockDecoder::GetPos()const {
        return this->_pos;
    }
}