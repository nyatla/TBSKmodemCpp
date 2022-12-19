#include "BitStream.h"
#include <memory>

namespace TBSKmodemCPP
{
    using std::make_unique;
    using std::dynamic_pointer_cast;
}
namespace TBSKmodemCPP
{
    BitStream::BitStream(const shared_ptr<IPyIterator<int>>& src, int bitwidth) :
        _bw{make_unique<BitsWidthConvertIterator>(src, bitwidth, 1) }
    {
        this->_pos = 0;
    }
    BitStream::~BitStream() {
    }
    int BitStream::Next()
    {
        int r;
        try {
            r = this->_bw->Next();
        }
        catch (RecoverableStopIteration e) {
            throw e;
        }
        this->_pos = this->_pos + 1;
        return r;
    }
    // @property
    TBSK_INT64 BitStream::GetPos()const
    {
        return this->_pos;
    }


}


