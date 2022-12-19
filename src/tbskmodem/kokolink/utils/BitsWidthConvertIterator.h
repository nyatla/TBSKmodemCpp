#pragma once
#include "./recoverable.h"
#include "../interfaces.h"
#include "../types/types.h"
#include "../compatibility.h"
#include <memory>
namespace TBSKmodemCPP
{
    class StopIteration_BitsWidthConvertIterator_FractionalBitsLeft :public PyStopIteration
    {
    public:
        StopIteration_BitsWidthConvertIterator_FractionalBitsLeft() : PyStopIteration() { }
        StopIteration_BitsWidthConvertIterator_FractionalBitsLeft(exception const& innerException) :PyStopIteration(innerException) {}

    };

    // """ 任意ビット幅のintストリームを任意ビット幅のint値に変換するイテレータです。
    // """
    class BitsWidthConvertIterator :public NoneCopyConstructor_class,virtual public IRecoverableIterator<int> {
    private:
        const shared_ptr<IPyIterator<int>> _src;
        bool _is_eos;
        const int _input_bits;
        const int _output_bits;
        TBSK_UINT32 _bits;
        int _n_bits;
        // def __init__(self,src:Iterator[int],input_bits:int=8,output_bits:int=1):
        //     """
        //     """
        //     super().__init__()
        //     self._src=src
        //     self._is_eos=False
        //     self._input_bits=input_bits
        //     self._output_bits=output_bits
        //     self._bits  =0#byte値
        //     self._n_bits=0 #読み出し可能ビット数
    public:
        BitsWidthConvertIterator(const shared_ptr<IPyIterator<int>>& src, int input_bits = 8, int output_bits = 1);
        virtual ~BitsWidthConvertIterator();

        int Next();
    };

}
