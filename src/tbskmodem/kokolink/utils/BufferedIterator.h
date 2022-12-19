#pragma once
#include "RingBuffer.h"
#include "recoverable.h"

namespace TBSKmodemCPP
{
    // """ 任意範囲のログを取りながら返すイテレータ
    //     このイテレータはRecoverableStopInterationを利用できます。
    // """
    template <typename T> class BufferedIterator:public BasicRecoverableIterator<T>
    {
    private:
        unique_ptr<RingBuffer<T>> _buf;
        const shared_ptr<IPyIterator<T>> _src;

        // def __init__(self,src:Iterator[T],size:int):
        //     self._src=src
        //     self._buf=RingBuffer(size,0)
    public:
        BufferedIterator(const shared_ptr<IPyIterator<T>>& src, int size, const T pad) :
            _src{ src },
            _buf{make_unique<RingBuffer<T>>(size, pad) }
        {
        }
        virtual ~BufferedIterator(){};

        T Next()
        {
            T d;
            try {
                d = this->_src->Next();
            }
            catch (RecoverableStopIteration e) {
                throw e;
            }
            this->_buf->Append(d);
            return d;
        };

        // @property
        // def buf(self)->RingBuffer:
        //     return self._buf
        const RingBuffer<T>& GetBuf()const
        {
            return *this->_buf.get();
        }
    };

}

