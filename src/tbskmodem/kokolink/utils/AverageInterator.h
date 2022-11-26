#pragma once

#include "./SumIterator.h"
#include <memory>
namespace TBSKmodemCPP
{

    class AverageInterator :public SumIterator
    {
    private:
        const int _length;
    public:
        AverageInterator(const shared_ptr<IPyIterator<double>>&& src, int ticks);

        // """ 末尾からticksまでの平均値を連続して返却します。
        //     このイテレータはRecoverableStopInterationを利用できます。
        // """
        double Next()override;
    };
}

