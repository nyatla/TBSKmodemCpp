#pragma once
#include "../recoverable.h"
#include "../RingBuffer.h"


namespace TBSKmodemCPP
{
    // """ ストリームの読み出し位置から過去N個の合計を返すイテレータです。
    //     このイテレータはRecoverableStopInterationを利用できます。
    // """
    class SumIterator :public BasicRecoverableIterator<double>
    {
    private:
        const std::shared_ptr<IPyIterator<double>> _src;
        double _sum;
        const std::unique_ptr<RingBuffer<double>> _buf;

    public:
        SumIterator(const std::shared_ptr<IPyIterator<double>>& src, int length);
        virtual ~SumIterator();

        double Next()override;
        const RingBuffer<double>& GetBuf() const;
    };

}




