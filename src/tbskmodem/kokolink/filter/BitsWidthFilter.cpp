#include "BitsWidthFilter.h"
namespace TBSKmodemCPP
{
    using namespace std;
    // """ 任意ビット幅のintストリームを任意ビット幅のint値にエンコードします。
    // """
    BitsWidthFilter::BitsWidthFilter(int input_bits, int output_bits) :BasicRoStream(),
        _input_bits{ input_bits },
        _output_bits{ output_bits },
        _iter{move(unique_ptr<BitsWidthConvertIterator>())},
        _pos{0}
    {
    }
    BitsWidthFilter::~BitsWidthFilter() {
    }

    BitsWidthFilter& BitsWidthFilter::SetInput(shared_ptr<IRoStream<int>>&& src)
    {
        this->_pos = 0;
        this->_iter = move(make_unique<BitsWidthConvertIterator>(move(src), this->_input_bits, this->_output_bits));
        return *this;
    }
    int BitsWidthFilter::Next()
    {
        if (this->_iter == NULL) {
            throw PyStopIteration();
        }
        auto r = this->_iter->Next();
        this->_pos = this->_pos + 1;
        return r;
    }
    // @property
    TBSK_INT64 BitsWidthFilter::GetPos()const
    {
        return this->_pos;
    }
}
