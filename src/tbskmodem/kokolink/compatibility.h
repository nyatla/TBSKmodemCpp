#pragma once
#include "./interfaces.h"
#include <vector>
#include <iterator>
#include <memory>


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
        virtual ~BinaryReaderException();
    };

    class IBinaryReader {
    public:
        //  必要なサイズを読みだせない場合BinaryReaderException
        virtual size_t ReadBytes(size_t size, void* buf) = 0;
        virtual void ReadInt32LE(int* buf) = 0;
        virtual void ReadInt16LE(short* buf) = 0;
    };
    class IBinaryWriter {
    public:
        //  必要なサイズを読みだせない場合BinaryReaderException
        virtual size_t WriteBytes(size_t size, const void* buf) = 0;
        //virtual void ReadInt32LE(int* buf) = 0;
        //virtual void ReadInt16LE(short* buf) = 0;

    };
}


namespace TBSKmodemCPP
{
    using namespace std;
    template <typename T> class PyIterator :public virtual IPyIterator<T>
    {

    private:
        shared_ptr<const vector<T>> _src;
        size_t _ptr;
    public:
        //PyIterator(const vector<T>* src);
        PyIterator(const unique_ptr<const vector<T>>&& src);
        PyIterator(const shared_ptr<const vector<T>>&& src);

        T Next()override;
    };



    //// srcポインタをインスタンスにアタッチするPyIterator
    //// srcポインタの寿命はこのインスタンスに同期します。
    ////
    //template <typename T> class AttachedPyIterator :public PyIterator<T>
    //{

    //private:
    //    std::vector<T>* _attached;
    //public:
    //    AttachedPyIterator(std::vector<T>* src) :PyIterator<T>(*src), _attached{ src } {
    //    }
    //    virtual ~AttachedPyIterator() {
    //        TBSK_SAFE_DELETE(this->_attached);
    //    }
    //};

}
    
namespace TBSKmodemCPP
{
    using namespace std;


    class Functions {
    public:
        template <typename T> static unique_ptr <IPyIterator<T>> ToPyIterator(shared_ptr<const vector<T>>&& src) {
            return make_unique<PyIterator<T>>(src);
        }
        template <typename T> static unique_ptr <IPyIterator<T>> ToPyIterator(shared_ptr<vector<T>>& src) {
            return make_unique<PyIterator<T>>(src);
        }
        template <typename T> static unique_ptr <IPyIterator<T>> ToPyIterator(shared_ptr<vector<T>>&& src) {
            return make_unique<PyIterator<T>>(src);
        }
        template <typename T> static unique_ptr <IPyIterator<T>> ToPyIterator(unique_ptr<const vector<T>>&& src) {
            return make_unique<PyIterator<T>>(src);
        }
        template <typename T> static unique_ptr <IPyIterator<T>> ToPyIterator(unique_ptr<vector<T>>&& src) {
            return make_unique<PyIterator<T>>(src);
        }




    public:
        //vctorを接続した新しいvectorを割り当てて返す。
        template <typename T> static std::vector<T>* Flatten(const std::vector<std::vector<T>>& s);

        template <typename T> static std::vector<T>* Repeat(int n, T pad);
    };
}

namespace TBSKmodemCPP
{

    class MemBuffer {
    private:
        std::vector<TBSK_BYTE> _buf;
        size_t _WriteBytes(const void* v, size_t l);
    public:
        const void* GetPtr();
        size_t WriteBytes(IBinaryReader& v, size_t size, int padding = 0);
        size_t WriteBytes(const void* v, size_t size, int padding = 0);
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


    //}

    //class BinUtils
    //{
    //    static public byte[] Ascii2byte(string s)
    //    {
    //        return System.Text.Encoding.ASCII.GetBytes(s);
    //    }
    //    static public UInt16 Bytes2Uint16LE(byte[] b, int s)
    //    {
    //        return (UInt16)((b[s + 1] << 8) | b[s + 0]);
    //    }
    //    static public UInt16 Bytes2Uint16LE(byte[] b)
    //    {
    //        return Bytes2Uint16LE(b, 0);
    //    }

    //    static public UInt32 Bytes2Uint32LE(byte[] b, int s)
    //    {
    //        return (UInt32)((b[s + 3] << 24) | (b[s + 2] << 16) | (b[s + 1] << 8) | b[s + 0]);
    //    }
    //    static public UInt32 Bytes2Uint32LE(byte[] b)
    //    {
    //        return Bytes2Uint32LE(b, 0);
    //    }
    //    static public UInt32 ReadUint32LE(Stream fp)
    //    {
    //        return BinUtils.Bytes2Uint32LE(ReadBytes(fp,4)); //LE
    //    }
    //    static public byte[] Uint16LE2Bytes(int s)
    //    {
    //        return new byte[] { (byte)((s >> 0) & 0xff), (byte)((s >> 8) & 0xff) };
    //    }
    //    static public byte[] Uint32LE2Bytes(int s)
    //    {
    //        return new byte[] { (byte)((s >> 0) & 0xff), (byte)((s >> 8) & 0xff), (byte)((s >> 16) & 0xff), (byte)((s >> 24) & 0xff) };
    //    }
    //    static public bool IsEqualAsByte(byte[] a, string b)
    //    {
    //        return a.SequenceEqual(BinUtils.Ascii2byte(b));
    //    }
    //    static public byte[] ReadBytes(Stream fp, int size)
    //    {
    //        var ret = new byte[size];
    //        for(var i = 0; i < size; i++)
    //        {
    //            var w=fp.ReadByte();
    //            if (w < 0)
    //            {
    //                throw new EndOfStreamException();
    //            }
    //            ret[i] = (byte)w;
    //        }
    //        return ret;
    //    }


    //    static public byte[] ToByteArray(IEnumerable<int> s)
    //    {
    //        using (var ms = new MemoryStream())
    //        {
    //            foreach (var i in s)
    //            {
    //                Debug.Assert(i >= 0 && i <= 255);
    //                ms.WriteByte((byte)i);
    //            }
    //            ms.Flush();
    //            return ms.ToArray();
    //        }
    //    }
    //    static public byte[] ToByteArray(IEnumerable<uint> s)
    //    {
    //        using (var ms = new MemoryStream())
    //        {
    //            foreach (var i in s)
    //            {
    //                Debug.Assert(i >= 0 && i <= 255);
    //                ms.WriteByte((byte)i);
    //            }
    //            ms.Flush();
    //            return ms.ToArray();
    //        }
    //    }


    //}



