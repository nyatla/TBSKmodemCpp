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

    class SelfCorrcoefIterator2 : public ISelfCorrcoefIterator
    {
    private:
        const int xyi_len;
        const std::unique_ptr<TBSK_INT32[]> xyi;
        int c;
        int n;
        TBSK_INT64 sumxi;
        TBSK_INT64 sumxi2;
        TBSK_INT64 sumyi;
        TBSK_INT64 sumyi2;
        TBSK_INT64 sumxiyi;
        const std::shared_ptr<IPyIterator<double>> _srcx;
        std::queue<int> _srcy;
        int fill_c; //バッファ埋めのカウンタ

    public:
        SelfCorrcoefIterator2(int window, const std::shared_ptr<IPyIterator<double>>& src, int shift = 0);
        virtual ~SelfCorrcoefIterator2();
        double Next() override;
    };
}


