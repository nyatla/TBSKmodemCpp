#include "./PcmData.h"
#include <math.h>
#include <stdexcept>
#include <exception>

namespace TBSKmodemCPP {
    using std::invalid_argument;
    using std::min;
    using std::max;

}

namespace TBSKmodemCPP
{
    static unique_ptr<vector<TBSK_BYTE>> Float2bytes(IPyIterator<double>& fdata, int bits)
    {
        auto ret = make_unique<vector<TBSK_BYTE>>();
        if (bits == 8)
        {
            try {
                for (;;) {
                    auto d = fdata.Next();
                    ret->push_back((TBSK_BYTE)(d * 127 + 128));
                }
            }
            catch (PyStopIteration) {
                //nothing to do
            }
            return ret;
        }
        else if (bits == 16)
        {
            int r = (int)((pow(2, 16) - 1) / 2); //#Daisukeパッチ
            try {
                for (;;) {
                    auto d = fdata.Next();
                    auto f = d * r;
                    if (f >= 0)
                    {
                        TBSK_UINT16 v = (TBSK_UINT16)(min((double)INT16_MAX, f));
                        ret->push_back((TBSK_BYTE)(v & 0xff));
                        ret->push_back((TBSK_BYTE)((v >> 8) & 0xff));
                    }
                    else
                    {
                        TBSK_UINT16 v = (TBSK_UINT16)(0xffff + (int)(max(f, (double)INT16_MIN)) + 1);
                        ret->push_back((TBSK_BYTE)(v & 0xff));
                        ret->push_back((TBSK_BYTE)((v >> 8) & 0xff));
                    }
                }
            }catch (PyStopIteration) {
                    //nothing to do
            }
            return ret;
        }
        throw std::invalid_argument("Invalid bits");
    }
    static unique_ptr<vector<TBSK_BYTE>> Float2bytes(const vector<double>& fdata, int bits) {
        PyIterator<double> a(fdata);
        return Float2bytes(a, bits);
    }




    unique_ptr<const PcmData> PcmData::Load(IBinaryReader& fp)
    {
        return std::make_unique<const PcmData>(fp);
    }


    void PcmData::Dump(PcmData& src, IBinaryWriter& dest)
    {
        src._wavfile->Dump(dest);
    }


    PcmData::PcmData(IBinaryReader& fp) :_wavfile(make_unique<WaveFile>(fp)) {
    }

    PcmData::PcmData(const TBSK_BYTE* src, size_t size, int sample_bits, int frame_rate, const vector<shared_ptr<const Chunk>>* chunks) :
        _wavfile{make_unique<WaveFile>(frame_rate, sample_bits/8, 1, src, size, chunks) }
    {
        //TBSK_ASSERT((this->_frames.size()) % (this->_sample_bits / 8) == 0);//   #srcの境界チェック
    }

    PcmData::PcmData(IPyIterator<double>& src, int sample_bits, int frame_rate, const vector<shared_ptr<const Chunk>>* chunks) :
        _wavfile{make_unique<WaveFile>(frame_rate, sample_bits / 8, 1,*Float2bytes(src,sample_bits), chunks) }
    {
    }
    PcmData::PcmData(const vector<double>& src, int sample_bits, int frame_rate, const vector<shared_ptr<const Chunk>>* chunks) :
        _wavfile{make_unique<WaveFile>(frame_rate, sample_bits / 8, 1,*Float2bytes(src,sample_bits), chunks) }
    {
    }


    // """サンプリングビット数
    // """

    int PcmData::GetSampleBits()const
    {
        return this->_wavfile->GetFmt()->GetSamplewidth();
    }
    // @property
    // def frame_rate(self)->int:
    //     """サンプリングのフレームレート
    //     """
    //     return self._frame_rate
    int PcmData::GetFramerate()const
    {
        return this->_wavfile->GetFmt()->GetFramerate();
    }
    // @property
    // def timelen(self):
    //     """データの記録時間
    //     """
    //     return len(self._frames)/(self._sample_bits//8*self._frame_rate)
    //float GetTimelen()const {
    //    return this->_wavfile._frames.size() / (this->_sample_bits / 8 * this->_frame_rate);
    //}




    // @property
    // def byteslen(self)->int:
    //     """Waveファイルのデータサイズ
    //     Waveファイルのdataセクションに格納されるサイズです。
    //     """
    //     return len(self._frames)
    int PcmData::GetByteslen()const {
        return this->_wavfile->GetData()->GetSize();
    }
    // @property
    // def data(self)->bytes:
    //     """ 振幅データ
    //     """
    //     return self._frames
    //const TBSK_BYTE* GetData()
    //{
    //    return this->_wavfile.GetData()->.data();
    //}


    // def dataAsFloat(self)->Sequence[float]:

    //     data=self._frames
    //     bits=self._sample_bits
    //     if bits==8:
    //         # a=[struct.unpack_from("B",data,i)[0]/256-0.5 for i in range(len(data))]
    //         # b=list(np.frombuffer(data, dtype="uint8")/256-0.5)
    //         # print(a==b)
    //         # return list(np.frombuffer(data, dtype="uint8")/256-0.5)
    //         return [struct.unpack_from("B",data,i)[0]/255-0.5 for i in range(len(data))]
    //     elif bits==16:
    //         assert(len(data)%2==0)
    //         r=(2**16-1)//2 #Daisukeパッチ
    //         # a=[struct.unpack_from("<h",data,i*2)[0]/r for i in range(len(data)//2)]
    //         # b=list(np.frombuffer(data, dtype="int16")/r)
    //         # print(a==b)
    //         # return list(np.frombuffer(data, dtype="int16")/r)
    //         return [struct.unpack_from("<h",data,i*2)[0]/r for i in range(len(data)//2)]
    //     raise ValueError()






    const unique_ptr<vector<double>> PcmData::DataAsFloat()
    {
        const TBSK_BYTE* data = (const TBSK_BYTE*)this->_wavfile->GetData()->GetData();
        auto data_size = this->_wavfile->GetData()->GetSize();
        auto bits = this->GetSampleBits();

        auto ret = unique_ptr<vector<double>>();
        if (bits == 8) {
            for (auto i = 0;i < data_size;i++) {
                ret->push_back(*(data + i) / 255. - 0.5);
            }
            return ret;
        }
        else if (bits == 16) {
            if (data_size % 2 != 0) {
                data_size = data_size - 1;
            }
            //TBSK_ASSERT(data_size%2==0);
            double r = (pow(2, 16) - 1) / 2;//(2 * *16 - 1)//2 #Daisukeパッチ
            int c = 0;
            TBSK_UINT16 b = 0;
            for (auto i = 0;i < data_size;i++)
            {
                b = (TBSK_UINT16)(b >> 8 | (i << 8));
                c = (c + 1) % 2;
                if (c == 0) {
                    if ((0x8000 & b) == 0) {
                        ret->push_back(b / r);
                    }
                    else
                    {
                        ret->push_back((((TBSK_UINT32)b - 0x0000ffff) - 1) / r);
                    }
                    b = 0;
                }
            }
            return ret;
        }
        throw invalid_argument("invalid bits");
    }




}
