#include "../FloatConverter.h"
#include "./PcmData.h"
#include <stdexcept>
#include <exception>
#include <cmath>

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
                    ret->push_back(FloatConverter::DoubleToByte(fdata.Next()));
                }
            }
            catch (PyStopIteration&) {
                //nothing to do
            }
            return ret;
        }
        else if (bits == 16)
        {
            int r = (int)((pow(2, 16) - 1) / 2); //#Daisukeパッチ
            try {
                for (;;) {
                    auto v = FloatConverter::DoubleToInt16(fdata.Next());
                    ret->push_back((TBSK_BYTE)(((TBSK_UINT16)v) & 0xff));
                    ret->push_back((TBSK_BYTE)((v >> 8) & 0xff));
                }
            }catch (PyStopIteration&) {
                    //nothing to do
            }
            return ret;
        }
        throw std::invalid_argument("Invalid bits");
    }
    static unique_ptr<vector<TBSK_BYTE>> Float2bytes(const vector<double>& fdata, int bits) {
        PyIterator<double> a(fdata);//一次利用限定
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






    int PcmData::GetByteslen()const {
        return this->_wavfile->GetData()->GetSize();
    }
    /// <summary>
    /// バイナリデータをそのまま返す。
    /// </summary>
    /// <returns></returns>
    const TBSK_BYTE* PcmData::GetData()const {
        return (const TBSK_BYTE*)this->_wavfile->GetData()->GetData();
    }




    unique_ptr<const vector<double>> PcmData::DataAsFloat()const
    {
        const TBSK_BYTE* data = (const TBSK_BYTE*)this->_wavfile->GetData()->GetData();
        auto data_size = this->_wavfile->GetData()->GetSize();
        auto bits = this->GetSampleBits();

        auto ret = make_unique<vector<double>>();
        if (bits == 8) {
            for (auto i = 0;i < data_size;i++) {
                ret->push_back(FloatConverter::ByteToDouble(*(data + i)));
            }
            return ret;
        }
        else if (bits == 16) {
            if (data_size % 2 != 0) {
                data_size = data_size - 1;
            }
            //TBSK_ASSERT(data_size%2==0);
            double r = (std::pow(2, 16) - 1) / 2;//(2 * *16 - 1)//2 #Daisukeパッチ
            for (auto i = 0;i < data_size;i += 2)
            {
                TBSK_INT16 v = (TBSK_INT16)((TBSK_UINT16) (* (data + i)) | (((TBSK_UINT16) * (data + i+1)) << 8));
                ret->push_back(FloatConverter::Int16ToDouble(v));
            }
            return ret;
        }
        throw invalid_argument("invalid bits");
    }




}
