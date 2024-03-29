#pragma once
#include "../../../interfaces.h"
#include "../../recoverable.h"
#include "./ISelfCorrcoefIterator.h"
#include <queue>
#include <memory>
namespace TBSKmodemCPP
{
    // """ src[:]とsrc[shift:]の相関を返すストリームです。
    //     n番目に区間[n,n+window]と[n+shift,n+shift+window]の相関値を返します。
    //     開始からwindow-shift個の要素は0になります。
    // """

    class SelfCorrcoefIterator : public ISelfCorrcoefIterator
    {
    private:
        const int xyi_len;
        const std::unique_ptr<double[]> xyi;
        //std::vector<double[]>* xyi;
        int c;
        int n;
        double sumxi;
        double sumxi2;
        double sumyi;
        double sumyi2;
        double sumxiyi;
        const std::shared_ptr<IPyIterator<double>> _srcx;
        std::queue<double> _srcy;
        int fill_c; //バッファ埋めのカウンタ

    public:
        SelfCorrcoefIterator(int window, const std::shared_ptr<IPyIterator<double>>& src, int shift = 0);
        virtual ~SelfCorrcoefIterator();
        double Next() override;
    };
}


