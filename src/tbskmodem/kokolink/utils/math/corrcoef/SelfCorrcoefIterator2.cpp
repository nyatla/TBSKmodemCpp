#include "./SelfCorrcoefIterator2.h"
#include <cmath>

namespace TBSKmodemCPP
{
    using std::shared_ptr;




    // from ....types import Deque, Iterable, Iterator
    // from ...recoverable import RecoverableIterator, RecoverableStopIteration

    // """ src[:]とsrc[shift:]の相関を返すストリームです。
    //     n番目に区間[n,n+window]と[n+shift,n+shift+window]の相関値を返します。
    //     開始からwindow-shift個の要素は0になります。
    // """

    SelfCorrcoefIterator2::SelfCorrcoefIterator2(int window, const shared_ptr<IPyIterator<double>>& src, int shift):
        xyi_len{ window },
        xyi{ make_unique<int[]>(this->xyi_len * 2) },
        _srcx{ src }

    {
        //for (auto i = 0; i < window; i++)
        //{
        //    this->xyi[i] = NULL;
        //}

        this->c = 0;//#エントリポイント
        this->n = 0;//#有効なデータ数
        this->sumxi = 0;
        this->sumxi2 = 0;
        this->sumyi = 0;
        this->sumyi2 = 0;
        this->sumxiyi = 0;
        // """初期化
        // """
        this->fill_c = 0;
        for (auto i = 0; i < shift; i++)
        {
            this->_srcy.push(0);//extend([0]);
        }
    }
    SelfCorrcoefIterator2::~SelfCorrcoefIterator2()
    {
    }
    static const int FP = 30;
    double SelfCorrcoefIterator2::Next()
    {
        auto c = this->c;
        auto l = this->xyi_len;
        auto m = c % l;
        TBSK_INT32 vxs;
        try
        {
            double dv = this->_srcx->Next();
            assert(-1 <= dv && dv <= 1);
            vxs = (int)(dv * (1 << FP));
        }
        catch (RecoverableStopIteration&)
        {
            throw;
        }


        this->_srcy.push(vxs);
        TBSK_INT32 vys = this->_srcy.front();
        this->_srcy.pop();
        TBSK_INT64 vx = vxs;//キャスト
        TBSK_INT64 vy = vys;//キャスト
        if (this->fill_c<this->xyi_len)
        {
            this->fill_c++;
            // #値追加
            this->n += 1 - 0;
            this->sumxi += vx - 0;
            this->sumxi2 += ((vx * vx)>>FP) - 0;//vx**2-0;
            this->sumyi += vy - 0;
            this->sumyi2 += ((vy * vy)>>FP) - 0;// vy**2-0;
            this->sumxiyi += ((vx * vy)>>FP) - 0;
            //this->xyi[m] = new double[] { vx, vy };
            this->xyi[m * 2+0] = vxs;
            this->xyi[m * 2+1] = vys;
        }
        else
        {
            // #値削除
            TBSK_INT64 lxi = this->xyi[m*2+0];
            TBSK_INT64 lyi = this->xyi[m*2+1];
            this->n += 1 - 1;
            this->sumxi += vx - lxi;
            this->sumxi2 += ((vx * vx) >> FP) - ((lxi * lxi)>>FP);
            this->sumyi += vy - lyi;
            this->sumyi2 += ((vy * vy) >> FP) - ((lyi * lyi)>>FP);
            this->sumxiyi += ((vx * vy) >> FP) - ((lxi * lyi)>>FP);
            this->xyi[m*2+0] = vxs;
            this->xyi[m*2+1] = vys;
        }

        this->c += 1;
        TBSK_ASSERT(this->n > 0);
        // if(this.n==0){
        //     return null;
        // }
        if (this->n == 1)
        {
            return (double)1;
        }

        auto sumxi_ = ((double)this->sumxi)/(1 << FP);
        auto meanx_ = sumxi_ / (this->n);
        auto sumxi2_ = ((double)this->sumxi2) / (1 << FP);
        auto v = (sumxi2_ + (meanx_ * meanx_) * this->n - 2 * meanx_ * sumxi_);
        if (v <= 0)
        {
            return 0;
        }
        auto stdx = sqrt(v / (this->n - 1));
        if (stdx < (1.0 / (1 << FP)))
        {
            return 0.;
        }
        auto sumyi_ = ((double)this->sumyi) / (1 << FP);
        auto meany_ = sumyi_ / (this->n);
        auto sumyi2_ = ((double)this->sumyi2) / (1 << FP);
        v = (sumyi2_ + (meany_ * meany_) * this->n - 2 * meany_ * sumyi_);
        if (v <= 0)
        {
            return 0;
        }
        auto stdy = sqrt(v / (this->n - 1));
        if (stdy < (1.0 / (1 << FP)))
        {
            return 0.;
        }
        auto sumxiyi_ = ((double)this->sumxiyi) / (1 << FP);
        v = sumxiyi_ + this->n * meanx_ * meany_ - meany_ * sumxi_ - meanx_ * sumyi_;
        auto covxy = v / (this->n - 1);
        auto r = covxy / (stdx * stdy);

        return r > 1 ? 1.f : (r < -1 ? -1 : r);
    }

}


