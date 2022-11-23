#pragma once
#include "./Preamble.h"
#include "../toneblock.h"
#include "../../../streams/rostreams.h"
#include <exception>
#include <stdexcept>
#include <memory>

namespace TBSKmodemCPP
{
    using namespace std;

    class ASwaitForSymbol;

    // """ 台形反転信号プリアンブルです。
    // """
    class CoffPreamble :public Preamble
    {
    public:
    private:
        friend ASwaitForSymbol; 
        const double _threshold;
        const shared_ptr<const TraitTone> _symbol;
        const int _cycle; //#平坦部分のTick数
        bool _asmethtod_lock;
    public:
        CoffPreamble(const shared_ptr<const TraitTone>& tone, double threshold = 1.0, int cycle = 4);
    public:
        shared_ptr<IRoStream<double>> GetPreamble()const override;
    public:
        NullableResult<TBSK_INT64> WaitForSymbol(shared_ptr<IRoStream<double>>&& src) override;
    };
}

