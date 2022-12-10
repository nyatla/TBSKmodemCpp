#pragma once
#include "./interfaces.h"
#include "./NoneCopyConstructor_class.h"
#include "types/Py__class__.h"
#include <algorithm>
#include <functional>

namespace TBSKmodemCPP
{
    using std::make_shared;
    using std::make_unique;
    using std::vector;
}



namespace TBSKmodemCPP
{
    template <typename T> T vector_sum(const vector<T>& src, int idx, int len) {
        T w = 0;
        for (int i = 0;i < len;i++) {
            w = w + src.at((size_t)idx + i);
        }
        return w;
    }
    template <typename T> T vector_sum(const vector<T>* src, int idx, int len) {
        return vector_sum(*src, idx, len);
    }

    template <typename T> void vector_sort(vector<T>& src, bool reverse=false)
    {
        if (!reverse) {
            std::sort(src.begin(), src.end());
        }
        else {
            std::sort(src.begin(), src.end(), std::greater<T>());
        }
    }
    template <typename T> void vector_sort(vector<T>* src, bool reverse = false) {
        vector_sort(*src, reverse);
    }
}


namespace TBSKmodemCPP
{
    class BinaryReaderException :std::exception
    {
    public:
        BinaryReaderException() :std::exception() {};
        virtual ~BinaryReaderException() {};
    };

    class IBinaryReader {
    public:
        //  必要なサイズを読みだせない場合BinaryReaderException
        virtual void ReadBytes(size_t size, void* buf) = 0;
        virtual void ReadInt32LE(int* buf) = 0;
        virtual void ReadInt16LE(short* buf) = 0;
    };
    class IBinaryWriter {
    public:
        //  必要なサイズを読みだせない場合BinaryReaderException
        virtual size_t WriteBytes(size_t size, const void* buf) = 0;

    };
}
namespace TBSKmodemCPP
{

    class FileReader :public IBinaryReader {
    private:
        unique_ptr<std::ifstream> _ifs;
    public:  
        FileReader(const char* path);
        virtual ~FileReader();
        void ReadBytes(size_t size, void* buf);
        void ReadInt32LE(int* buf);
        void ReadInt16LE(short* buf);
    };

    class FileWriter:public IBinaryWriter {
    private:
        unique_ptr<std::ofstream> _ofs;
    public:
        FileWriter(const char* path);
        virtual ~FileWriter();
        //  必要なサイズを読みだせない場合BinaryReaderException
        size_t WriteBytes(size_t size, const void* buf)override;

    };

}



    
namespace TBSKmodemCPP
{

    using std::make_unique;
    using std::make_shared;

    namespace Functions {
        template <typename T> shared_ptr<vector<T>> ToVector(IPyIterator<T>* src) {
            auto r = make_shared<vector<T>>();
            try {
                for (;;) {
                    r->push_back(src->Next());
                }
            }
            catch(PyStopIteration) {
                return r;
            }
            throw std::exception();
        }
        template <typename T> shared_ptr<vector<T>> ToVector(const shared_ptr<IPyIterator<T>>& src)
        {
            return ToVector(src.get());
        }

        //template <typename T> IPyIterator < T ToPyIterator(shared_ptr<vector<T>>& src) {
        //    return make_unique<PyIterator<T>>(src);
        //}
        //template <typename T> static unique_ptr <IPyIterator<T>> ToPyIterator(shared_ptr<vector<T>>&& src) {
        //    return make_unique<PyIterator<T>>(src);
        //}
        //template <typename T> static unique_ptr <IPyIterator<T>> ToPyIterator(unique_ptr<const vector<T>>&& src) {
        //    return make_unique<PyIterator<T>>(src);
        //}
        //template <typename T> static unique_ptr <IPyIterator<T>> ToPyIterator(unique_ptr<vector<T>>&& src) {
        //    return make_unique<PyIterator<T>>(src);
        //}




        //vctorを接続した新しいvectorを割り当てて返す。
        template <typename T> static vector<T>* Flatten(const std::vector<std::vector<T>>& s);
    };
}

namespace TBSKmodemCPP
{

    class MemBuffer:public NoneCopyConstructor_class
    {
    private:
        unique_ptr<vector<TBSK_BYTE>> _buf;
        size_t _WriteBytes(const TBSK_BYTE* v, size_t l);
    public:
        MemBuffer(int i);
        const void* GetPtr();
        size_t WriteBytes(IBinaryReader& v, size_t size, int padding = 0);
        size_t WriteBytes(const TBSK_BYTE* v, size_t size, int padding = 0);
        size_t WriteBytes(const char* v, size_t size, int padding = 0) {return this->WriteBytes((const TBSK_BYTE*)v, size, padding); };
        size_t WriteInt16LE(const size_t v);
        size_t WriteInt32LE(const size_t v);
        TBSK_INT32 AsInt32LE(const size_t idx)const;
        TBSK_INT16 AsInt16LE(const size_t idx)const;
        const TBSK_BYTE* AsBytesPtr(const size_t idx)const;
        const char* AsCharPtr(const size_t idx)const;
        const TBSK_BYTE AsByte(const size_t idx)const;
        size_t Dump(IBinaryWriter& dest)const;
    };

}





