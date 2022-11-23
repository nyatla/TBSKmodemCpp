#pragma once
#include "../../../interfaces.h"
#include "../../../streams/rostreams.h"
namespace TBSKmodemCPP
{
    using namespace std;
    class Preamble {
    public:
        virtual shared_ptr<IRoStream<double>> GetPreamble()const = 0;
        virtual NullableResult<TBSK_INT64> WaitForSymbol(shared_ptr<IRoStream<double>>&& src) = 0;
    };
}