#pragma once
#include "../../types/types.h"
#include "../../types/Py__class__.h"
#include "./riffio.h"
#include "../../compatibility.h"
#include <vector>
#include <memory>
namespace TBSKmodemCPP
{
    // """ wavファイルのラッパーです。1chモノラルpcmのみ対応します。
    // """
    class PcmData :public NoneCopyConstructor_class
    {
    private:
        //const int _sample_bits;
        //const int _frame_rate;
        //std::vector<TBSK_BYTE> _frames;
        const unique_ptr<WaveFile> _wavfile;

    public:
        static unique_ptr<const PcmData> Load(IBinaryReader& fp);
        static void Dump(PcmData& src, IBinaryWriter& dest);
        PcmData(IBinaryReader& fp);
        PcmData(const TBSK_BYTE* src, size_t size, int sample_bits, int frame_rate, const vector<shared_ptr<const Chunk>>& chunks);
        PcmData(const TBSK_BYTE* src, size_t size, int sample_bits, int frame_rate);
        PcmData(IPyIterator<double>& src, int sample_bits, int frame_rate, const vector<shared_ptr<const Chunk>>& chunks);
        PcmData(IPyIterator<double>& src, int sample_bits, int frame_rate) :PcmData(src, sample_bits, frame_rate, vector<shared_ptr<const Chunk>>()) {};

        //PcmData(const vector<TBSK_BYTE>& src, int sample_bits, int frame_rate) :PcmData((const TBSK_BYTE*)(src.data()), src.size(), sample_bits, frame_rate) {};



        int GetSampleBits()const;
        int GetFramerate()const;
        int GetByteslen()const;
        const unique_ptr<vector<double>> DataAsFloat();

    };





}
