#pragma once
#include "../../interfaces.h"
#include "../../utils/math/MSequence.h"
#include "../../utils/math/XorShiftRand31.h"
#include <vector>
#include <memory>

namespace TBSKmodemCPP
{
    class TraitTone :public vector<double> ,public NoneCopyConstructor_class{
    public:
        TraitTone();
        // """ 信号強度をv倍します。
        // """
        TraitTone& Mul(double v);
    };
}
namespace TBSKmodemCPP
{

    // """ Sin波形のトーン信号です。
    //     このトーン信号を使用したTBSKはDPSKと同じです。
    // """
    class SinTone :public TraitTone {
    private:
    public:
        SinTone(int points, int cycle = 1);
    };
}
namespace TBSKmodemCPP
{

    // """ トーン信号を巡回符号でBPSK変調した信号です。
    //     2^bits-1*len(base_tone)の長さです。
    // """
    class MSeqTone :public TraitTone
    {
    private:
        unique_ptr<vector<int>> _sequence;
    public:
        MSeqTone(int bits, int tap);
        MSeqTone(int bits, int tap, const TraitTone& base_tone);

        virtual ~MSeqTone();
        const std::vector<int>& GetSequence();
    };

    // """ トーン信号をPN符号でBPSK変調した信号です。
    //     intervalティック単位で変調します。
    // """
    class PnTone : public TraitTone
    {
    public:
        PnTone(int seed, int interval = 2);
        PnTone(int seed, int interval, const TraitTone& base_tone);
    };




    // """ Sin波形をXPSK変調したトーン信号です。
    //     1Tick単位でshiftイテレータの返す値×2pi/divの値だけ位相をずらします。
    // """
    class XPskSinTone :public TraitTone {

        // """
        //     Args:
        //     shift   -1,0,1の3値を返すイテレータです。省略時は乱数値です。
        // """



    public:
        XPskSinTone(int points, int cycle = 1, int div = 8);

        XPskSinTone(int points, int cycle, int div, IPyIterator<int>& shift);
    private:
        XPskSinTone(int points, int cycle, int div, IPyIterator<int>* shift);
    };
}


