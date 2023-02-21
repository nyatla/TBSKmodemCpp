#pragma once
#include "../kokolink/protocol/tbsk/tbskmodem.h"
#include <memory>





namespace TBSKmodemCPP
{

    // """ TBSKの変調クラスです。
    //     プリアンブルを前置した後にビットパターンを置きます。
    // """
    class TbskModulator:TbskModulator_impl
    {
    public:
        TbskModulator(const std::shared_ptr<const TraitTone>& tone);
        TbskModulator(const std::shared_ptr<const TraitTone>& tone, int preamble_cycle);

        TbskModulator(const std::shared_ptr<const TraitTone>& tone, const std::shared_ptr<const Preamble>& preamble);
        virtual ~TbskModulator() {};
    public:
        std::shared_ptr<IPyIterator<double>> ModulateAsBit(const std::shared_ptr<IRoStream<int>>& src, bool stopsymbol=true);
        std::shared_ptr<IPyIterator<double>> Modulate(const std::shared_ptr<IPyIterator<int>>& src, int bitwidth = 8, bool stopsymbol = true);
        std::shared_ptr<IPyIterator<double>> Modulate(const char* src, int length = -1, bool stopsymbol = true);
    };
}

