#include "./SelfCorrcoefIterator.h"
#include <math.h>

namespace TBSKmodemCPP
{

    // from math import sqrt
    // from typing import Union



    // from ....types import Deque, Iterable, Iterator
    // from ...recoverable import RecoverableIterator, RecoverableStopIteration

    // """ src[:]とsrc[shift:]の相関を返すストリームです。
    //     n番目に区間[n,n+window]と[n+shift,n+shift+window]の相関値を返します。
    //     開始からwindow-shift個の要素は0になります。
    // """

    SelfCorrcoefIterator::SelfCorrcoefIterator(int window, const shared_ptr<IPyIterator<double>>& src, int shift):
        xyi_len{ window * 2 }, _srcx{src}
    {
        this->xyi = new double[this->xyi_len];//std::vector<double[]>(window);// Functions.Create2dArray<double?>(window,2,null); //#Xi
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
    SelfCorrcoefIterator::~SelfCorrcoefIterator()
    {
        TBSK_SAFE_DELETE(this->xyi);
    }

    double SelfCorrcoefIterator::Next()
    {
        auto c = this->c;
        auto l = this->xyi_len;
        auto m = c % l;
        double vx;
        try
        {
            vx = this->_srcx->Next();
        }
        catch (RecoverableStopIteration e)
        {
            throw e;
        }


        this->_srcy.push(vx);
        auto vy = this->_srcy.front();
        this->_srcy.pop();

        if (this->fill_c<this->xyi_len)
        {
            this->fill_c++;
            // #値追加
            this->n += 1 - 0;
            this->sumxi += vx - 0;
            this->sumxi2 += vx * vx - 0;//vx**2-0;
            this->sumyi += vy - 0;
            this->sumyi2 += vy * vy - 0;// vy**2-0;
            this->sumxiyi += vx * vy - 0;
            //this->xyi[m] = new double[] { vx, vy };
            this->xyi[m * 2+0] = vx;
            this->xyi[m * 2+1] = vy;
        }
        else
        {
            // #値削除
            auto lxi = this->xyi[m*2+0];
            auto lyi = this->xyi[m*2+1];
            this->n += 1 - 1;
            this->sumxi += vx - lxi;
            this->sumxi2 += vx * vx - lxi * lxi;
            this->sumyi += vy - lyi;
            this->sumyi2 += vy * vy - lyi * lyi;
            this->sumxiyi += vx * vy - lxi * lyi;
            this->xyi[m*2+0] = vx;
            this->xyi[m*2+1] = vy;
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

        auto sumxi = this->sumxi;
        auto meanx_ = sumxi / (this->n);
        auto sumxi2 = this->sumxi2;
        auto v = (sumxi2 + (meanx_ * meanx_) * this->n - 2 * meanx_ * sumxi);
        if (v < 0)
        {
            v = 0;
        }
        auto stdx = sqrt(v / (this->n - 1));

        auto sumyi = this->sumyi;
        auto meany_ = sumyi / (this->n);
        auto sumyi2 = this->sumyi2;
        v = (sumyi2 + (meany_ * meany_) * this->n - 2 * meany_ * sumyi);
        if (v < 0)
        {
            v = 0;
        }
        auto stdy = sqrt(v / (this->n - 1));

        v = this->sumxiyi + this->n * meanx_ * meany_ - meany_ * sumxi - meanx_ * sumyi;
        auto covxy = v / (this->n - 1);
        auto r = stdx * stdy == 0 ? 0 : covxy / (stdx * stdy);
        return r > 1 ? 1.f : (r < -1 ? -1 : r);
    }

}


