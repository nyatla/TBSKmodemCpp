#pragma once
#include "RingBuffer.h"
#include "recoverable.h"

namespace TBSKmodemCPP
{
    using namespace std;
    // """ 任意範囲のログを取りながら返すイテレータ
    //     このイテレータはRecoverableStopInterationを利用できます。
    // """
    template <typename T> class BufferedIterator:public BasicRecoverableIterator<T>
    {
    private:
        RingBuffer<T> _buf;
        shared_ptr<IPyIterator<T>> _src;

        // def __init__(self,src:Iterator[T],size:int):
        //     self._src=src
        //     self._buf=RingBuffer(size,0)
    public:
        BufferedIterator(shared_ptr<IPyIterator<T>>&& src, int size, const T pad);
        virtual ~BufferedIterator();

        T Next();

        // @property
        // def buf(self)->RingBuffer:
        //     return self._buf
        const RingBuffer<T>& GetBuf()const;
    };
}

