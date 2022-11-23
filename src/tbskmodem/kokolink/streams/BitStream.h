#pragma once
#include "../interfaces.h"
#include "./rostream.h"
#include "../utils/BitsWidthConvertIterator.h"
#include <memory>

namespace TBSKmodemCPP
{
    using namespace std;

#pragma warning( disable : 4250 )
    class BitStream :public virtual IBitStream,public BasicRoStream<int>
    {
    private:
        TBSK_INT64 _pos;
        BitsWidthConvertIterator _bw;

        // """ 任意ビット幅のintストリームを1ビット単位のビットストリームに展開します。
        // """
    public:
        BitStream(shared_ptr<IPyIterator<int>>&& src, int bitwidth = 8);
        virtual ~BitStream();
        int Next()override;
        TBSK_INT64 GetPos()const override;
    //public:
    //    //diamond extends
    //    void Seek(int size)override { BasicRoStream::Seek(size);};
    //    unique_ptr<vector<int>> Gets(int maxsize, bool fillup)override {return BasicRoStream::Gets(maxsize, fillup); };

    };
#pragma warning( default : 4250 )
}


