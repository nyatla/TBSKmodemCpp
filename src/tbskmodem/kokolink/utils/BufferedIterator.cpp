#include "BufferedIterator.h"
namespace TBSKmodemCPP
{


    template <typename T> BufferedIterator<T> ::BufferedIterator(shared_ptr<IPyIterator<T>>&& src, int size, const T pad) :
        _src{ src },
        _buf {RingBuffer<T>(size, pad)}
    {
    }
    template <typename T> BufferedIterator<T> ::~BufferedIterator() {
    }

    template <typename T> T BufferedIterator<T>::Next()
    {
        T d;
        try {
            d = this->_src->Next();
        }
        catch (RecoverableStopIteration e) {
            throw e;
        }
        this->_buf.Append(d);
        return d;
    }
    // @property
    // def buf(self)->RingBuffer:
    //     return self._buf
    template <typename T> const RingBuffer<T>& BufferedIterator<T> :: GetBuf()const
    {
        return this->_buf;
    }

}

