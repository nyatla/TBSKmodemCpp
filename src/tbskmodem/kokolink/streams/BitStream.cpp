#include "BitStream.h"


namespace TBSKmodemCPP
{
    using namespace std;
    BitStream::BitStream(shared_ptr<IPyIterator<int>>&& src, int bitwidth) :
        _bw{BitsWidthConvertIterator(dynamic_pointer_cast<IPyIterator<int>>(src), bitwidth, 1) }
    {
        this->_pos = 0;
    }
    BitStream::~BitStream() {
    }
    int BitStream::Next()
    {
        int r;
        try {
            r = this->_bw.Next();
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


