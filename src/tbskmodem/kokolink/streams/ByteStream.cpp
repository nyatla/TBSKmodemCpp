#include "ByteStream.h"

namespace TBSKmodemCPP
{
    using namespace std;

    class ByteCastIter :public virtual IPyIterator<int>
    {
    private:
        IPyIterator<TBSK_BYTE>* _src;
    public:
        ByteCastIter(IPyIterator<TBSK_BYTE>& ref_src)
        {
            this->_src = &ref_src;
        }
        int Next()override
        {
            return this->_src->Next();
        }
    };
    class CharCastIter : public virtual IPyIterator<int>
    {
    private:
        IPyIterator<char>* _src;
    public:
        CharCastIter(IPyIterator<char>& ref_src)
        {
            this->_src = &ref_src;
        }
        int Next()override
        {
            return (int)this->_src->Next();
        }
    };



    ByteStream::ByteStream(shared_ptr<IPyIterator<int>>&& src, int inital_pos):
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