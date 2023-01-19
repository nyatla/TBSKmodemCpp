#pragma once
#include "./riffio.h"
#include <vector>
#include <memory>
namespace TBSKmodemCPP
{
    using std::vector;
    using std::shared_ptr;

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
        PcmData(const TBSK_BYTE* src, size_t size, int sample_bits, int frame_rate, const vector<shared_ptr<const Chunk>>* chunks=NULL);

        PcmData(IPyIterator<double>& src, int sample_bits, int frame_rate, const vector<shared_ptr<const Chunk>>* chunks=NULL);

        PcmData(const vector<double>& src, int sample_bits, int frame_rate, const vector<shared_ptr<const Chunk>>* chunks=NULL);

        //PcmData(const shared_ptr<const vector<double>>& src, int sample_bits, int frame_rate, const vector<shared_ptr<const Chunk>>* chunks) :
        //    PcmData(*src.get(), sample_bits, frame_rate, chunks) {};
        PcmData(const shared_ptr<const vector<double>>& src, int sample_bits, int frame_rate, const vector<shared_ptr<const Chunk>>* chunks=NULL) :
            PcmData(*src.get(), sample_bits, frame_rate, chunks) {};



        int GetSampleBits()const;
        int GetFramerate()const;
        int GetByteslen()const;
        unique_ptr<vector<double>> DataAsFloat();

    };





}
