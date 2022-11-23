#include "compatibility.h"
#include <algorithm>
#include <memory>

namespace TBSKmodemCPP
{
    template <typename T> T vector_sum(const std::vector<T>& src, int idx, int len) {
        T w = 0;
        for (int i = 0;i < len;i++) {
            w = w + src.at((size_t)idx + i);
        }
        return w;
    }
    template <typename T> T vector_sort(std::vector<T>& src, bool reverse)
    {
        if (!reverse) {
            std::sort(src.begin(), src.end());
        }
        else {
            std::sort(src.begin(), src.end(), [](T a, T b) {return a > b;});
        }
    }

}


namespace TBSKmodemCPP
{
    using namespace std;
    //template <typename T> PyIterator<T>::PyIterator(const vector<T>* src) :PyIterator(make_shared<const vector<T>>(src))
    //{}
    template <typename T> PyIterator<T>::PyIterator(const unique_ptr<const vector<T>>&& src) :_ptr(src)
    {}
    template <typename T> PyIterator<T>::PyIterator(const shared_ptr<const vector<T>>&& src):_ptr(src)
    {}
    template <typename T> T PyIterator<T>::PyIterator::Next()
    {
        if (this->_ptr >= this->_src.size()) {
            throw PyStopIteration();
        }
        auto r = this->_src[this->_ptr];
        this->_ptr++;
        return r;
    }


    template <typename T> static std::vector<T>* Functions::Flatten(const std::vector<std::vector<T>>& s)
    {
        std::vector<T> d = new std::vector<T>();
        for (auto i = 0;i < s.size();i++) {
            for (auto j = 0;j < s[i].size();j++) {
                d.push_back(s[i][j]);
            }
        }
        return d;
    }

    template <typename T> static std::vector<T>* Functions::Repeat(int n, T pad)
    {
        auto r = new std::vector<T>();
        for (auto i = 0;i < n;i++) {
            r->push_back(pad);
        }
        return r;
    }

    //Float2bytesの8bit版
    class Double2Byte8Iter :public virtual IPyIterator<TBSK_BYTE>
    {
    private:
        IPyIterator<double>& _src;
    public:
        Double2Byte8Iter(IPyIterator<double>& src) :_src{ src } {
        }
        TBSK_BYTE Next()
        {
            double d = this->_src.Next();
            return (TBSK_BYTE)(d * 127 + 128);
        }
    };
    //Float2bytesの16bit版
    class Double2Byte16Iter :public virtual IPyIterator<TBSK_BYTE>
    {
    private:
        IPyIterator<double>& _src;
        const int DISK = (int)((pow(2, 16) - 1) / 2); //#Daisukeパッチ
        int _bitnum = 0;
        TBSK_BYTE tmp=0;
    public:
        Double2Byte16Iter(IPyIterator<double>& src) :_src{ src } {
        }
        TBSK_BYTE Next() {
            if (this->_bitnum == 1) {
                this->_bitnum = 0;
                return tmp;
            }
            else {
                double d = this->_src.Next();
                auto f = d * this->DISK;
                this->_bitnum = 1;
                if (f >= 0)
                {
                    TBSK_UINT16 v = (TBSK_UINT16)(std::min((double)INT16_MAX, f));
                    this->tmp = (TBSK_BYTE)((v >> 8) & 0xff);
                    return (TBSK_BYTE)(v & 0xff);
                }
                else
                {
                    TBSK_UINT16 v = (TBSK_UINT16)(0xffff + (int)(std::max(f, (double)INT16_MIN)) + 1);
                    this->tmp = (TBSK_BYTE)((v >> 8) & 0xff);
                    return (TBSK_BYTE)(v & 0xff);
                }
            }
        }
    };
}



namespace TBSKmodemCPP
{
    size_t MemBuffer::_WriteBytes(const void* v, size_t l) {
        for (auto i = 0;i < l;i++) {
            this->_buf.push_back(*((const TBSK_BYTE*)v + i));
        }
        return this->_buf.size() - l;//先頭位置
    }

    const void* MemBuffer::GetPtr() {
        return this->_buf.data() + this->_buf.size();
    };

    size_t MemBuffer::WriteBytes(IBinaryReader& v, size_t size, int padding)
    {
        std::unique_ptr<TBSK_BYTE[]> p = std::make_unique<TBSK_BYTE[]>(size);
        try {
            v.ReadBytes(size, p.get());
        }
        catch (BinaryReaderException& e) {
            throw e;
        }
        auto r = this->WriteBytes(p.get(), size, padding);
        return r;
    }
    size_t MemBuffer::WriteBytes(const void* v, size_t size, int padding)
    {
        auto r = this->_WriteBytes(v, size);
        if (padding > 0) {
            const char w = 0;
            for (auto i = 0;i < padding;i++) {
                this->_WriteBytes(&w, 1);
            }
        }
        return r;
    }
    size_t MemBuffer::WriteInt16LE(const size_t v) {
        TBSK_BYTE w[] = {
        ((v >> 0) & 0xff),
        ((v >> 8) & 0xff)
        };
        return this->_WriteBytes(w, 2);
    }
    size_t MemBuffer::WriteInt32LE(const size_t v) {
        TBSK_BYTE w[] = {
        ((v >> 0) & 0xff),
        ((v >> 8) & 0xff),
        ((v >> 16) & 0xff),
        ((v >> 24) & 0xff)
        };
        return this->_WriteBytes(w, 4);
    };
    TBSK_INT32 MemBuffer::AsInt32LE(const size_t idx)const {
        const TBSK_BYTE* w = (this->_buf.data() + idx);
        return ((TBSK_INT32)(w[3]) << 24) | ((TBSK_INT32)(w[2]) << 16) | ((TBSK_INT32)(w[1]) << 8) | w[0];
    }
    TBSK_INT16 MemBuffer::AsInt16LE(const size_t idx)const {
        const TBSK_BYTE* w = (this->_buf.data() + idx);
        return ((TBSK_INT16)((TBSK_INT16)(w[1]) << 8) | w[0]);
    }
    const TBSK_BYTE* MemBuffer::AsBytesPtr(const size_t idx)const {
        return (this->_buf.data() + idx);
    }
    const char* MemBuffer::AsCharPtr(const size_t idx)const {
        return (const char*)(this->_buf.data() + idx);
    }
    const TBSK_BYTE MemBuffer::AsByte(const size_t idx)const {
        return *(this->_buf.data() + idx);
    }
    size_t MemBuffer::Dump(IBinaryWriter& dest)const {
        dest.WriteBytes(this->_buf.size(), this->_buf.data());
        return this->_buf.size();
    }
}


