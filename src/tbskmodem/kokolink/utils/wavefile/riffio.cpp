#include "riffio.h"

namespace TBSKmodemCPP
{

    //};

    // """チャンクのベースクラス。
    // """

    Chunk::Chunk(IBinaryReader& fp) :
        _buf{make_unique<MemBuffer>(1)},
        _name{ this->_buf->WriteBytes(fp, 4) },
        _size{ this->_buf->WriteBytes(fp, 4) }
    {
    }

    Chunk::Chunk(const char* name, size_t size) :
        _buf{ make_unique<MemBuffer>(1) },
        _name{ this->_buf->WriteBytes(name, 4) },
        _size{ this->_buf->WriteInt32LE(size) }

    {
        TBSK_ASSERT(strnlen_s(name, 5) <= 4);
    }
    Chunk::~Chunk(){}

    // @property
    // def name(self)->bytes:
    //     return self._name
    const char* Chunk::GetName()const
    {
        return this->_buf->AsCharPtr(this->_name);
    }
    int Chunk::GetSize()const
    {
        return this->_buf->AsInt32LE(this->_size);
    }
    size_t Chunk::Dump(IBinaryWriter& writer)const {
        return this->_buf->Dump(writer);
    }

}
namespace TBSKmodemCPP
{


    RawChunk::RawChunk(const char* name, size_t size, IBinaryReader& fp) :Chunk(name,size),
        _data{ this->_buf->WriteBytes(fp,this->GetSize() + this->GetSize() % 2) }
    {
    }

    RawChunk::RawChunk(const char* name, const TBSK_BYTE* data, size_t data_len) :Chunk(name, data_len),
        _data{ this->_buf->WriteBytes(data,data_len,data_len % 2) }    //data+padding
    {
    }


    const void* RawChunk::GetData()const
    {
        return this->_buf->AsBytesPtr(this->_data);
    }


}
namespace TBSKmodemCPP
{
    FmtChunk::FmtChunk(int size, IBinaryReader& fp) :Chunk("fmt ",size)
    {
        this->_buf->WriteBytes(fp, 2);
        this->_buf->WriteBytes(fp, 2);
        this->_buf->WriteBytes(fp, 4);
        this->_buf->WriteBytes(fp, 4);
        this->_buf->WriteBytes(fp, 2);
        this->_buf->WriteBytes(fp, 2);
    }

    FmtChunk::FmtChunk(int framerate, int samplewidth, int nchannels) :Chunk("fmt ", FmtChunk::CHUNK_SIZE)
    {
        this->_buf->WriteInt16LE(WAVE_FORMAT_PCM); //+0
        this->_buf->WriteInt16LE(nchannels);//+2
        this->_buf->WriteInt32LE(framerate);//+4
        this->_buf->WriteInt32LE((std::size_t)nchannels * framerate * samplewidth);//+8
        this->_buf->WriteInt16LE((std::size_t)nchannels * samplewidth);//+12
        this->_buf->WriteInt16LE((std::size_t)samplewidth * 8);//+14
    }
    int FmtChunk::GetNchannels()const
    {
        return this->_buf->AsInt16LE(2);
    }

    unsigned int FmtChunk::GetFramerate()const
    {
        return (unsigned int)this->_buf->AsInt32LE(4);
    }

    int FmtChunk::GetSamplewidth()const
    {
        return this->_buf->AsInt16LE(14);
    }

}
namespace TBSKmodemCPP
{


    DataChunk::DataChunk(size_t size, IBinaryReader& fp) :RawChunk("data", size, fp)
    {
    }
    DataChunk::DataChunk(const TBSK_BYTE* data, size_t data_len) :RawChunk("data", data, data_len)
    {
    }

}
namespace TBSKmodemCPP
{
    ChunkHeader::ChunkHeader(IBinaryReader& fp) :Chunk(fp),
        _form{ this->_buf->WriteBytes(fp, 4) }
    {
    }
    ChunkHeader::ChunkHeader(const char* name, size_t size, IBinaryReader& fp) :Chunk(name, size),
        _form{this->_buf->WriteBytes(fp, 4) }
    {
    }

    ChunkHeader::ChunkHeader(const char* name, size_t size, const char* form) :Chunk(name, size),
        _form{ this->_buf->WriteBytes(form, 4) }
    {
    }
    const char* ChunkHeader::GetForm()
    {
        return this->_buf->AsCharPtr(this->_form);
    }

}
namespace TBSKmodemCPP
{


    RiffHeader::RiffHeader(IBinaryReader& fp) :ChunkHeader(fp)
    {
        TBSK_ASSERT(strncmp(this->GetName(), "RIFF", 4) == 0);
    }
    RiffHeader::RiffHeader(size_t size, const char* form) :ChunkHeader("RIFF", size, form)
    {
    }

    RawListChunk::RawListChunk(IBinaryReader& fp) :ChunkHeader(fp),
        _payload{ this->_buf->WriteBytes(fp,this->GetSize() - 4) }//fmtの文だけ引く
    {
        TBSK_ASSERT(strncmp(this->GetName(), "LIST", 4) == 0);
    }
    RawListChunk::RawListChunk(int size, IBinaryReader& fp) :ChunkHeader("LIST", size, fp),
        _payload{ this->_buf->WriteBytes(fp, this->GetSize() - 4) }//fmtの文だけ引く
    {
        TBSK_ASSERT(strncmp(this->GetName(), "LIST", 4) == 0);
    }
    RawListChunk::RawListChunk(const char* form, const TBSK_BYTE* payload, int payload_len) :ChunkHeader("LIST", payload_len + 4, form),
        _payload{ this->_buf->WriteBytes(payload,payload_len) }//fmtの文だけ引く
    {
    }
    const void* RawListChunk::GetPayload() {
        return this->_buf->AsBytesPtr(this->_payload);
    }

   //// """Info配下のチャンクを格納するクラス
   // // """
   // public class InfoItemChunk:RawChunk{
   //     // def __init__2(self,name:bytes,data:bytes):
   //     //     assert(len(name)==4)
   //     //     super().__init__(name,data)
   //     public InfoItemChunk(byte[] name,byte[] data):
   //         base(name,data)
   //     {
   //         Debug.Assert(name.Length==4);
   //     }
   //     public InfoItemChunk(string name,byte[] data):
   //         this(BinUtils.Ascii2byte(name),data)
   //     {
   //     }
   //     // def __init__3(self,name:bytes,size:int,fp:RawIOBase):
   //     //     super().__init__(name,size,fp)
   //     public InfoItemChunk(byte[] name,int size,Stream fp):
   //         base(name,size,fp){
   //         Debug.Assert(name.Length==4);
   //     }
   //     public InfoItemChunk(string name,int size,Stream fp):
   //         this(BinUtils.Ascii2byte(name),size,fp){
   //         Debug.Assert(name.Length==4);
   //     }
   //     //    def _summary_dict(self)->dict:
   //     //         return dict(super()._summary_dict(),**{"value":self.data})
   //     //public Dictionary<string,object> _Summary_dict(){
   //     //    var r=base._Summary_dict();
   //     //    r.Add("value",this.Data);
   //     //    return r;
   //     //}        
   // }

    //// """
    //// Args:
    //// items
    ////     (タグ名,値)のタプルか、InfoItemChunkオブジェクトの混在シーケンスが指定できます。
    ////     タプルの場合は[0]のフィールドは4バイトである必要があります。
    //// """
    //public class InfoListChunk:ChunkHeader{
    //    readonly private IEnumerable<InfoItemChunk> _items;


    //    // def __init__2(self,size:int,fp:RawIOBase):
    //    //     super().__init__(b"LIST",size,b"INFO")
    //    //     #Infoパーサ
    //    //     read_size=4
    //    //     items=[]
    //    //     while read_size<self._size:
    //    //         name,size=struct.unpack_from('<4sL',fp.read(8))
    //    //         item=InfoItemChunk(name,size,fp)
    //    //         read_size+=size+size%2+8
    //    //         items.append(item)
    //    //     self._items=items
    //    public InfoListChunk(int size,Stream fp):        
    //        base("LIST",size,"INFO")
    //    {
    //        // #Infoパーサ
    //        var read_size=4;
    //        var items=new List<InfoItemChunk>();
    //        while(read_size<this._size){
    //            byte[] name=BinUtils.ReadBytes(fp,4);
    //            int rsize=(int)BinUtils.ReadUint32LE(fp);
    //            var item=new InfoItemChunk(name, rsize, fp);
    //            items.Add(item);
    //            read_size+= rsize + rsize % 2+8;
    //            items.Add(item);
    //        }
    //        this._items=items;    
    //    }

    //    static private int InfoListChunk_init(IEnumerable<InfoItemChunk> items)
    //    {
    //        var s = 0;
    //        foreach (var i in items)
    //        {
    //            s += i.Size + i.Size % 2 + 8;
    //        }
    //        return s;
    //    }
    //    public InfoListChunk(IEnumerable<InfoItemChunk> items):
    //        base("LIST", InfoListChunk_init(items),"INFO")
    //    {
    //        this._items=items;
    //    }
    //    //public InfoListChunk(IEnumerable<ValueTuple<byte[],byte[]>> items):
    //    //    this(()=>{
    //    //        var d=new List<InfoItemChunk>();
    //    //        foreach(var i in items){
    //    //            d.Add(new InfoItemChunk(i[0],i[1]));
    //    //        }
    //    //    }())
    //    //{
    //    //}
    //    // @property
    //    // def data(self)->bytes:
    //    //     payload=b"".join([i.toChunkBytes() for i in self._items])
    //    //     return super().data+payload
    //    override public byte[] Data{
    //        get{
    //            var b=new List<byte>();
    //            b.AddRange(base.Data);
    //            foreach (var i in this._items){
    //                b.AddRange(i.ToChunkBytes());
    //            }
    //            return b.ToArray();
    //        }
    //    }
    //    // @property
    //    // def items(self)->Sequence[InfoItemChunk]:
    //    //     return self._items
    //    public IEnumerable<InfoItemChunk> Items{
    //        get{
    //            return this._items;
    //        }
    //    }
    //    // def _summary_dict(self)->dict:
    //    //     return dict(super()._summary_dict(),**{"info":[i._summary_dict() for i in self.items]})        
    //    //public Dictionary<string,object> _Summary_dict(){
    //    //    var r=base._Summary_dict();
    //    //    var sub=new List<Dictionary<string, object>>;
    //    //    foreach(var i in this._items){
    //    //        sub.Add(i);
    //    //    }
    //    //    r.Add("info",sub);
    //    //    return r;
    //    //}
    //}

}
namespace TBSKmodemCPP
{
    WaveFile::WaveFile(IBinaryReader& fp) :RiffHeader(fp),
        _chunks{ std::vector<std::shared_ptr<const Chunk>>() }
    {
        TBSK_ASSERT(strncmp(this->GetForm(), "WAVE", 4) == 0);
        auto chunk_size = this->GetSize();
        chunk_size -= 4;//fmt分
        while (chunk_size > 8)
        {
            char name[4];
            try {
                fp.ReadBytes(4, name);
            }
            catch (const BinaryReaderException& e) {
                break;
            }
            int size;
            fp.ReadInt32LE(&size);
            chunk_size -= 8;
            if (memcmp(name, "fmt ", 4) == 0) {
                this->_chunks.push_back(std::make_shared<FmtChunk>(size, fp));
            }
            else if (memcmp(name, "data", 4) == 0) {
                this->_chunks.push_back(std::make_shared<DataChunk>(size, fp));
            }
            else if (memcmp(name, "LIST", 4) == 0) {
                this->_chunks.push_back(std::make_shared<RawListChunk>(size, fp));
            }
            else {
                this->_chunks.push_back(std::make_shared<RawChunk>(name, size, fp));
            }
        }
    }
    static size_t toSize(size_t frames_len,const vector<shared_ptr<const Chunk>>* extchunks=NULL) {
        size_t s = 4;//form
        s = s + FmtChunk::CHUNK_SIZE + 8;
        s = s + frames_len + 8;
        if (extchunks != NULL) {
            for (int i = 0;i < extchunks->size();i++) {
                auto cs = extchunks->at(i)->GetSize();
                s = s + cs + cs % 2 + 8;
            }
        }
        return s;
    }
    WaveFile::WaveFile(int samplerate, int samplewidth, int nchannel, const TBSK_BYTE* frames, size_t frames_len, const vector<shared_ptr<const Chunk>>& extchunks) :
        RiffHeader(toSize(frames_len,&extchunks), "WAVE"), _chunks{ std::vector<shared_ptr<const Chunk>>() }
    {
        this->_chunks.push_back(std::make_shared<FmtChunk>(samplerate, samplewidth, nchannel));
        this->_chunks.push_back(std::make_shared<DataChunk>(frames, frames_len));
        //if (extchunks != NULL) {
        for (auto i = 0;i < extchunks.size();i++) {
            this->_chunks.push_back(extchunks.at(i));
        }
    }

    WaveFile::WaveFile(int samplerate, int samplewidth, int nchannel, const TBSK_BYTE* frames, std::size_t frames_len) :
        RiffHeader(toSize(frames_len,NULL), "WAVE"), _chunks{vector<shared_ptr<const Chunk>>() }
    {
        this->_chunks.push_back(std::make_shared<FmtChunk>(samplerate, samplewidth, nchannel));
        this->_chunks.push_back(std::make_shared<DataChunk>(frames, frames_len));
    }



    const DataChunk* WaveFile::GetData()const {
        auto ret=this->GetChunk("data");
        if (ret == NULL) {
            return NULL;
        }
        return (DataChunk*)ret;
    }
    const FmtChunk* WaveFile::GetFmt()const {
        auto ret = this->GetChunk("fmt ");
        if (ret == NULL) {
            return NULL;
        }
        return (FmtChunk*)ret;

    }

    //""" nameに一致するchunkを返します。
    //    Return:
    //        チャンクが見つかればチャンクオブジェクトを返します。なければNoneです。
    //"""
    const Chunk* WaveFile::GetChunk(const char* name)const
    {
        for (auto i = 0;i < this->_chunks.size();i++) {
            if (memcmp(this->_chunks.at(i)->GetName(), name, 4) == 0) {
                return this->_chunks.at(i).get();
            }
        }
        return NULL;
    }
    size_t WaveFile::Dump(IBinaryWriter& writer)const{
        size_t ret=0;
        ret += Chunk::Dump(writer);
        for (auto i = 0;i < this->_chunks.size();i++) {
            ret += this->_chunks.at(i)->Dump(writer);
        }
        return ret;
    }

    // def __str__(self):
    //     return str([str(i) for i in self._chunks])
    //public string ToString(){
    //    return this._chunks.ToString();
    //}
};

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



