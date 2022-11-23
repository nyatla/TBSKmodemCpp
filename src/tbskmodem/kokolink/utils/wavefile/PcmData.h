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
    class PcmData{
    private:
        //const int _sample_bits;
        //const int _frame_rate;
        //std::vector<TBSK_BYTE> _frames;
        const WaveFile _wavfile;

    public:
        static std::unique_ptr<const PcmData> Load(IBinaryReader& fp);
        static void Dump(PcmData& src, IBinaryWriter& dest);
        PcmData(IBinaryReader& fp);
        PcmData(const TBSK_BYTE* src, int size, int sample_bits, int frame_rate, std::vector<std::shared_ptr<const Chunk>>& chunks);
        PcmData(IPyIterator<double>& src, int sample_bits, int frame_rate, std::vector<std::shared_ptr<const Chunk>>& chunks);
        int GetSampleBits()const;
        int GetFramerate()const;
        int GetByteslen()const;
        std::unique_ptr<std::vector<double>> DataAsFloat();

    };





}
