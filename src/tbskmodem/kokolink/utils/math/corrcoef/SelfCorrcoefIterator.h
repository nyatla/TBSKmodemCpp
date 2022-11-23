#pragma once
#include "../../../interfaces.h"
#include "../../recoverable.h"
#include <queue>
#include <memory>
namespace TBSKmodemCPP
{
    using namespace std;
    // from math import sqrt
    // from typing import Union



    // from ....types import Deque, Iterable, Iterator
    // from ...recoverable import RecoverableIterator, RecoverableStopIteration

    // """ src[:]とsrc[shift:]の相関を返すストリームです。
    //     n番目に区間[n,n+window]と[n+shift,n+shift+window]の相関値を返します。
    //     開始からwindow-shift個の要素は0になります。
    // """

    class SelfCorrcoefIterator : public IRecoverableIterator<double>
    {
    private:
        double* xyi;
        const int xyi_len;
        //std::vector<double[]>* xyi;
        int c;
        int n;
        double sumxi;
        double sumxi2;
        double sumyi;
        double sumyi2;
        double sumxiyi;
        const shared_ptr<IPyIterator<double>> _srcx;
        std::queue<double> _srcy;
        int fill_c; //バッファ埋めのカウンタ

    public:
        SelfCorrcoefIterator(int window, const shared_ptr<IPyIterator<double>>& src, int shift = 0);
        virtual ~SelfCorrcoefIterator();
        double Next() override;
    };
}


