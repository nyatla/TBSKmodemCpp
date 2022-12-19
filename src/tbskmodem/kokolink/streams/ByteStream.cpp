#include "ByteStream.h"

namespace TBSKmodemCPP
{

    class ByteCastIter :public virtual IPyIterator<int>
    {
    private:
        const shared_ptr<IPyIterator<TBSK_BYTE>> _src;
    public:
        ByteCastIter(const shared_ptr<IPyIterator<TBSK_BYTE>>& ref_src) :_src{ref_src}
        {
        }
        int Next()override
        {
            return this->_src->Next();
        }
    };
    class CharCastIter : public virtual IPyIterator<int>
    {
    private:
        const shared_ptr<IPyIterator<char>> _src;
    public:
        CharCastIter(const shared_ptr<IPyIterator<char>>& ref_src) :_src{ ref_src }
        {
        }
        int Next()override
        {
            return (int)this->_src->Next();
        }
    };



    ByteStream::ByteStream(const shared_ptr<IPyIterator<int>>& src, int inital_pos):
        _pos{ inital_pos },
        _iter{ src }
    {
    }
    //ByteStream::ByteStream(IPyIterator<TBSK_BYTE>& ref_src, int inital_pos) :ByteStream(new ByteCastIter(ref_src), inital_pos, true)
    //{
    //}
    //ByteStream::ByteStream(IPyIterator<char>& ref_src, int inital_pos) :ByteStream(new CharCastIter(ref_src), inital_pos, true)
    //{
    //}
    //ByteStream::ByteStream(IPyIterator<int>& ref_src, int inital_pos) :ByteStream(&ref_src, inital_pos, false)
    //{
    //}
    ByteStream::~ByteStream() {
    }
    int ByteStream::Next() {
        auto r = this->_iter->Next();
        this->_pos = this->_pos + 1;
        return r;
    }
    TBSK_INT64 ByteStream::GetPos()const
    {
        return this->_pos;
    }
}