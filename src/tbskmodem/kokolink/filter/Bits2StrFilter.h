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
    class Bits2StrFilter : public virtual BasicRoStream<char>, public virtual IFilter<Bits2StrFilter, IRoStream<int>, char>
    {
    private:
        const int _input_bits;
        const char* _encoding;
        TBSK_INT64 _pos;
        vector<TBSK_BYTE> _savedata;
        unique_ptr<BitsWidthConvertIterator> _iter;
    public:
        Bits2StrFilter(int input_bits = 1, const char* encoding = "ascii");
        virtual ~Bits2StrFilter();
        Bits2StrFilter& SetInput(const shared_ptr<IRoStream<int>>& src)override;
        char Next()override;
        TBSK_INT64 GetPos()const override;
    };
#pragma warning( default : 4250 )

}
