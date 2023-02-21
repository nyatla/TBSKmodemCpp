#include "SumIterator.h"

namespace TBSKmodemCPP
{
    using std::shared_ptr;
    using std::make_unique;


    SumIterator::SumIterator(const shared_ptr<IPyIterator<double>>& src, int length) :
        _src{src},
        _buf{make_unique<RingBuffer<double>>(length,0.) }
    {
        this->_sum=0;
        // # self._length=length
        // # self._num_of_input=0
        // self._gen=None
        return;
    }
    SumIterator::~SumIterator() {
    }





    double SumIterator::Next()
    {
        auto s = this->_src->Next();
        auto d = this->_buf->Append(s);
        this->_sum = this->_sum + s - d;
        // # self._num_of_input=self._num_of_input+1
        return this->_sum;
    }
    // @property
    // def buf(self)->RingBuffer[T]:
    //     return self._buf
    const RingBuffer<double>& SumIterator::GetBuf() const{
        return *this->_buf.get();
    }
}



