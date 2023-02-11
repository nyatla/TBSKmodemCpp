#pragma once
#include "./Preamble.h"
#include "../toneblock.h"
#include "../../../streams/rostreams.h"
#include <exception>
#include <stdexcept>
#include <memory>

namespace TBSKmodemCPP
{

    class WaitForSymbolAS : public AsyncMethod<NullableResult<TBSK_INT64>> {
    };
    class WaitForSymbolAS_impl;

    // """ 台形反転信号プリアンブルです。
    // """
    class CoffPreamble :public Preamble
    {
    public:
        static const int DEFAULT_CYCLE;
        static const double DEFAULT_TH;
    private:
        friend WaitForSymbolAS_impl;
        const double _threshold;
        const shared_ptr<const TraitTone> _symbol;
        const int _cycle; //#平坦部分のTick数
        bool _asmethtod_lock;
    public:
        CoffPreamble(const shared_ptr<const TraitTone>& tone, double threshold = CoffPreamble::DEFAULT_TH, int cycle = CoffPreamble::DEFAULT_CYCLE);
    public:
        shared_ptr<IRoStream<double>> GetPreamble()const override;
    public:
        NullableResult<TBSK_INT64> WaitForSymbol(const shared_ptr<IRoStream<double>>& src) override;
    };

}

