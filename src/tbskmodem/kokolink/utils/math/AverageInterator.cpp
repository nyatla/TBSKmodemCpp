#include "AverageInterator.h"
namespace TBSKmodemCPP
{
    using std::shared_ptr;

    AverageInterator::AverageInterator(const shared_ptr<IPyIterator<double>>& src, int ticks) : SumIterator(src, ticks),
        _length(ticks)
    {
    }
    // """ 末尾からticksまでの平均値を連続して返却します。
    //     このイテレータはRecoverableStopInterationを利用できます。
    // """
    double AverageInterator::Next()
    {
        double r;
        try {
            r = SumIterator::Next();
        }
        catch (RecoverableStopIteration&) {
            throw;
        }
        return r / this->_length;
    }

}

