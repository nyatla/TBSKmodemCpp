#pragma once

#include "../interfaces.h"
#include "../streams/rostream.h"
#include "../utils/BitsWidthConvertIterator.h"
#include<vector>
#include<memory>
namespace TBSKmodemCPP
{
#pragma warning( disable : 4250 )


    // """ nBit intイテレータから1バイト単位のbytesを返すフィルタです。
    // """
    class Bits2BytesFilter : public BasicRoStream<TBSK_BYTE>, public virtual IFilter<Bits2BytesFilter, IRoStream<int>, TBSK_BYTE>
    {
    private:
        const int _input_bits;
        TBSK_INT64 _pos=0;
        unique_ptr<BitsWidthConvertIterator> _iter;

    public:
        Bits2BytesFilter(int input_bits = 1);
        virtual ~Bits2BytesFilter();
        Bits2BytesFilter& SetInput(const std::shared_ptr<IRoStream<int>>& src)override;
        TBSK_BYTE Next()override;
        TBSK_INT64 GetPos()const override;
    };
#pragma warning( default : 4250 )
}