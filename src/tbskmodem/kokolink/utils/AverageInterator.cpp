#include "AverageInterator.h"
namespace TBSKmodemCPP
{
    using namespace std;
    AverageInterator::AverageInterator(const shared_ptr<IPyIterator<double>>&& src, int ticks) : SumIterator(src, ticks),
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
            r = this->Next();
        }
        catch (RecoverableStopIteration e) {
            throw e;
        }
        return r / this->_length;
    }

}

