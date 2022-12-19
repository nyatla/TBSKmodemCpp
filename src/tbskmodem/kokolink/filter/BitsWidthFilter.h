#pragma once
#include "../interfaces.h"
#include "../streams/rostream.h"
#include "../utils/BitsWidthConvertIterator.h"
#include <memory>
namespace TBSKmodemCPP
{

#pragma warning( disable : 4250 )

    // """ 任意ビット幅のintストリームを任意ビット幅のint値にエンコードします。
    // """
    class BitsWidthFilter :public virtual BasicRoStream<int>, public virtual IFilter<BitsWidthFilter, IRoStream<int>, int>
    {
    private:
        const int _input_bits;
        const int _output_bits;
        TBSK_INT64 _pos;
        unique_ptr<BitsWidthConvertIterator> _iter;
    public:
        BitsWidthFilter(int input_bits = 8, int output_bits = 1);

        BitsWidthFilter& SetInput(const shared_ptr<IRoStream<int>>& src)override;
        virtual ~BitsWidthFilter();
        int Next()override;
        TBSK_INT64 GetPos()const override;
    };
#pragma warning( default : 4250 )

}