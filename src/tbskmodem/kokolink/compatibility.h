#pragma once
#include "./interfaces.h"
#include <vector>
#include <iterator>
#include <memory>
#include <fstream>
#include <iostream>

namespace TBSKmodemCPP
{
    class NoneCopyConstructor_class
    {
    public:
        NoneCopyConstructor_class();
        virtual ~NoneCopyConstructor_class();
    public:
        // コピー禁止 (C++11)
        NoneCopyConstructor_class(const NoneCopyConstructor_class&) = delete;
        NoneCopyConstructor_class& operator=(const NoneCopyConstructor_class&) = delete;
    };
}


namespace TBSKmodemCPP
{

    template <typename T> T vector_sum(const std::vector<T>& src, int idx, int len);
    template <typename T> T vector_sum(const std::vector<T>* src, int idx, int len) {
        return vector_sum(*src, idx, len);
    }
    template <typename T> void vector_sort(std::vector<T>& src, bool reverse = false);
    template <typename T> void vector_sort(std::vector<T>* src, bool reverse = false) {
        return vector_sort(*src, reverse);
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
    template <typename T> class VectorWrapper :public NoneCopyConstructor_class
    {
    public:
        const vector<T>* _buf;
    public:
        VectorWrapper(const vector<T>* src) :_buf{ src } {}
    };


    template <typename T> class PyIterator :public NoneCopyConstructor_class, public virtual IPyIterator<T>
    {
    private:
        size_t _ptr = 0;
        const unique_ptr<VectorWrapper<T>> _src;
    public:
        //PyIterator(const vector<T>* src);
        PyIterator(const unique_ptr<const vector<T>>&& src);//参照
        PyIterator(const shared_ptr<const vector<T>>&& src);//共有
        PyIterator(const vector<T>& src);//参照
        T Next()override;
    };

}
    
namespace TBSKmodemCPP
{

    using std::make_unique;

    class Functions {
    public:
        //template <typename T> IPyIterator<T> ToPyIterator(shared_ptr<const vector<T>>&& src) {
        //    return new PyIterator<T>>(src);
        //}
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




    public:
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





