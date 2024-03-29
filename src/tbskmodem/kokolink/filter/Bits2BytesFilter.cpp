#include "Bits2BytesFilter.h"

namespace TBSKmodemCPP
{

    Bits2BytesFilter::Bits2BytesFilter(int input_bits) :
        _input_bits{input_bits},
        _iter{ std::move(unique_ptr<BitsWidthConvertIterator>())}
    {

    }
    Bits2BytesFilter::~Bits2BytesFilter() {
    }

    Bits2BytesFilter& Bits2BytesFilter::SetInput(const shared_ptr<IRoStream<int>>& src)
    {
        this->_pos = 0;
        this->_iter =make_unique<BitsWidthConvertIterator>(src, this->_input_bits, 8);
        return *this;
    }
    TBSK_BYTE Bits2BytesFilter::Next()
    {
        if (this->_iter.get() == NULL) {
            throw PyStopIteration();
        }
        auto r = this->_iter->Next();
        this->_pos = this->_pos + 1;
        TBSK_ASSERT(0 <= r && r <= 255);
        return (TBSK_BYTE)r;

    }
    TBSK_INT64 Bits2BytesFilter::GetPos()const
    {
        return this->_pos;
    }
}