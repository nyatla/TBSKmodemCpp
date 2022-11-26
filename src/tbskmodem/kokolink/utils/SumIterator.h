#pragma once
#include "recoverable.h"
#include "RingBuffer.h"


namespace TBSKmodemCPP
{
    // """ ストリームの読み出し位置から過去N個の合計を返すイテレータです。
    //     このイテレータはRecoverableStopInterationを利用できます。
    // """
    class SumIterator :public BasicRecoverableIterator<double>
    {
    private:
        const shared_ptr<IPyIterator<double>> _src;
        double _sum;
        const unique_ptr<RingBuffer<double>> _buf;

        // def __init__(self,src:Iterator[T],length:int):
        //     self._src=src
        //     self._buf=RingBuffer[T](length,0)
        //     self._sum=0
        //     # self._length=length
        //     # self._num_of_input=0
        //     self._gen=None
        //     return
    public:
        SumIterator(const shared_ptr<IPyIterator<double>>& src, int length);
        virtual ~SumIterator();
        // def __next__(self) -> T:
        //     try:
        //         s=next(self._src)
        //     except RecoverableStopIteration as e:
        //         raise RecoverableStopIteration(e)
        //     d=self._buf.append(s)
        //     self._sum=self._sum+s-d
        //     # self._num_of_input=self._num_of_input+1
        //     return self._sum
        double Next()override;
        // @property
        // def buf(self)->RingBuffer[T]:
        //     return self._buf
        const RingBuffer<double>& GetBuf() const;
    };

}




