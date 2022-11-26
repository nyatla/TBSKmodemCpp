#pragma once
#include "../../types/types.h"
#include "../../types/Py__class__.h"
#include "../../compatibility.h"
#include <vector>
#include <memory>

namespace TBSKmodemCPP
{
    // """チャンクのベースクラス。
    // """
    class Chunk :public NoneCopyConstructor_class {
    protected:
        // def __init__(self,name:bytes,size:int):
        //     assert(len(name)==4)
        //     self._name=name
        //     self._size=size
        const unique_ptr<MemBuffer> _buf;
        const size_t _name;
        const size_t _size;
    public:
        Chunk(IBinaryReader& fp);
        virtual ~Chunk();

        Chunk(const char* name, size_t size);
        const char* GetName()const;
        int GetSize()const;
        virtual size_t Dump(IBinaryWriter& writer)const;
    };
}
namespace TBSKmodemCPP
{
    // """ペイロードをそのまま格納するチャンクです.    
    // """
    class RawChunk :public Chunk {
    protected:
        const size_t _data;
    public:
        RawChunk(const char* name, size_t size, IBinaryReader& fp);
        RawChunk(const char* name, const TBSK_BYTE* data, size_t data_len);
        virtual ~RawChunk() {}
    public:
        const void* GetData()const;
    };

}
namespace TBSKmodemCPP
{

    // """fmtチャンクを格納するクラス.
    // """
    class FmtChunk : public Chunk {
    public:
        const static int CHUNK_SIZE = 2 + 2 + 4 + 4 + 2 + 2;
        const static TBSK_INT16 WAVE_FORMAT_PCM = 0x0001;
        //FmtChunk(BinaryReader& fp, MemStreamBuilder& stream) :Chunk(fp, stream)
        //{
        //    this->ptr = (const char*)stream.GetPtr();
        //    stream.WriteBytes(fp, 2);
        //    stream.WriteBytes(fp, 2);
        //    stream.WriteBytes(fp, 4);
        //    stream.WriteBytes(fp, 4);
        //    stream.WriteBytes(fp, 2);
        //    stream.WriteBytes(fp, 2);
        //}
        FmtChunk(int size, IBinaryReader& fp);


        FmtChunk(int framerate, int samplewidth, int nchannels);
        int GetNchannels()const;
        unsigned int GetFramerate()const;
        int GetSamplewidth()const;

    };
}
namespace TBSKmodemCPP
{

    // """dataチャンクを格納するクラス
    // """
    class DataChunk :public RawChunk
    {
    public:
        //DataChunk(BinaryReader& fp, MemStreamBuilder& stream) :RawChunk(fp, stream)
        //{
        //}
        DataChunk(size_t size, IBinaryReader& fp);
        DataChunk(const TBSK_BYTE* data, size_t data_len);

    };

}
namespace TBSKmodemCPP
{
    // """Formatフィールドを含むChunk構造を格納するクラス
    // """
    class ChunkHeader :public Chunk {
    protected:
        const size_t _form; //4byte
        //std::vector<TBSK_BYTE> _data;

    public:
        ChunkHeader(IBinaryReader& fp);
        ChunkHeader(const char* name, size_t size, IBinaryReader& fp);
        ChunkHeader(const char* name, size_t size, const char* form);
        virtual ~ChunkHeader() {};
        const char* GetForm();
    };

}
namespace TBSKmodemCPP
{
    class RiffHeader :public ChunkHeader
    {
        // def __init__2(self,size:int,form:bytes):
        //     super().__init__(b"RIFF",size,form)
    public:
        RiffHeader(IBinaryReader& fp);
        RiffHeader(size_t size, const char* form);
    };
    // """下位構造をそのまま格納するLIST
    // """
    class RawListChunk :public ChunkHeader {
    private:
        const size_t _payload;
        // def __init__3(self,size:int,form:bytes,fp:RawIOBase):
        //     super().__init__(b"LIST",size,form)
        //     self._payload=fp.read(size)
    public:
        RawListChunk(IBinaryReader& fp);
        RawListChunk(int size, IBinaryReader& fp);
        RawListChunk(const char* form, const TBSK_BYTE* payload, int payload_len);
        const void* GetPayload();
    };

}
namespace TBSKmodemCPP
{


    class WaveFile :public RiffHeader
    {
    public:
        vector<shared_ptr<const Chunk>> _chunks;


    public:
        WaveFile(IBinaryReader& fp);
        //
        // extchunksの内容はこのインスタンスに移譲する。
        WaveFile(int samplerate, int samplewidth, int nchannel, const TBSK_BYTE* frames, size_t frames_len, const vector<shared_ptr<const Chunk>>& extchunks);
        
        WaveFile(int samplerate, int samplewidth, int nchannel, const vector<TBSK_BYTE>& frames, const vector<shared_ptr<const Chunk>>& extchunks) :
            WaveFile(samplerate, samplewidth, nchannel, (const TBSK_BYTE*)frames.data(), frames.size(), extchunks) {}

        WaveFile(int samplerate, int samplewidth, int nchannel, const TBSK_BYTE* frames, size_t frames_len);
        WaveFile(int samplerate, int samplewidth, int nchannel, const vector<TBSK_BYTE>& frames) :
            WaveFile(samplerate, samplewidth, nchannel, (const TBSK_BYTE*)frames.data(),frames.size()) {}



        const DataChunk* GetData()const;
        const FmtChunk* GetFmt()const;
        //""" nameに一致するchunkを返します。
        //    Return:
        //        チャンクが見つかればチャンクオブジェクトを返します。なければNoneです。
        //"""
        const Chunk* GetChunk(const char* name)const;
        size_t Dump(IBinaryWriter& writer)const override;

        // def __str__(self):
        //     return str([str(i) for i in self._chunks])
        //public string ToString(){
        //    return this._chunks.ToString();
        //}
    };
}
// if __name__ == '__main__':
//     with open("cat1.wav","rb") as f:
//         src=f.read()
//         r=WaveFile(BytesIO(src))
//         print(r)
//         dest=r.toChunkBytes()
//         print(src==dest)
//         for i in range(len(src)):
//             if src[i]!=dest[i]:
//                 print(i)
//         with open("ssss.wav","wb") as g:
//             g.write(dest)
//         n=WaveFile(44100,2,2,r.chunk(b"data").data)
//         with open("ssss2.wav","wb") as g:
//             g.write(n.toChunkBytes())

//         n=WaveFile(44100,2,2,r.chunk(b"data").data,[
//             InfoListChunk([
//                     (b"IARL",b"The location where the subject of the file is archived"),
//                     (b"IART",b"The artist of the original subject of the file"),
//                     (b"ICMS",b"The name of the person or organization that commissioned the original subject of the file"),
//                     (b"ICMT",b"General comments about the file or its subject"),
//                     (b"ICOP",b"Copyright information about the file (e.g., 'Copyright Some Company 2011')"),
//                     (b"ICRD",b"The date the subject of the file was created (creation date)"),
//                     (b"ICRP",b"Whether and how an image was cropped"),
//                     (b"IDIM",b"The dimensions of the original subject of the file"),
//                     (b"IDPI",b"Dots per inch settings used to digitize the file"),
//                     (b"IENG",b"The name of the engineer who worked on the file"),
//                     (b"IGNR",b"The genre of the subject"),
//                     (b"IKEY",b"A list of keywords for the file or its subject"),
//                     (b"ILGT",b"Lightness settings used to digitize the file"),
//                     (b"IMED",b"Medium for the original subject of the file"),
//                     (b"INAM",b"Title of the subject of the file (name)"),
//                     (b"IPLT",b"The number of colors in the color palette used to digitize the file"),
//                     (b"IPRD",b"Name of the title the subject was originally intended for"),
//                     (b"ISBJ",b"Description of the contents of the file (subject)"),
//                     (b"ISFT",b"Name of the software package used to create the file"),
//                     (b"ISRC",b"The name of the person or organization that supplied the original subject of the file"),
//                     (b"ISRF",b"The original form of the material that was digitized (source form)"),
//                     (b"ITCH",b"The name of the technician who digitized the subject file"),]
//                     )])
//         with open("ssss3.wav","wb") as g:
//             g.write(n.toChunkBytes())
//         with open("ssss3.wav","rb") as g:
//             r=WaveFile(g)
//             print(r)

//     with open("ssss2.wav","rb") as f:
//         src=f.read()
//         r=WaveFile(BytesIO(src))
//         print(r)




