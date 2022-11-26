#include "toneblock.h"
#include <math.h>



namespace TBSKmodemCPP
{
    TraitTone::TraitTone() :vector<double>()
    {
    }
    // """ 信号強度をv倍します。
    // """
    TraitTone& TraitTone::Mul(double v) {
        for (auto i = 0; i < this->size(); i++)
        {
            (*this)[i] = (*this)[i] * v;
        }
        return *this;
    }




    // """ Sin波形のトーン信号です。
    //     このトーン信号を使用したTBSKはDPSKと同じです。
    // """
    SinTone::SinTone(int points, int cycle) :TraitTone()
    {
        auto s = TBSK_PI * 2 / points * 0.5;
        auto d1 = vector<double>();
        for (auto i = 0; i < points; i++)
        {
            d1.push_back((double)sin(s + i * TBSK_PI * 2 / points));
        }
        for (auto i = 0; i < cycle; i++)
        {
            for (auto i2 = 0;i2 < d1.size();i2++) {
                this->push_back(d1[i2]);
            }
        }
    }

    // """ トーン信号を巡回符号でBPSK変調した信号です。
    //     2^bits-1*len(base_tone)の長さです。
    // """
    MSeqTone::MSeqTone(int bits, int tap) :MSeqTone(bits, tap, SinTone(20, 1))
    {
    }
    MSeqTone::MSeqTone(int bits, int tap, const TraitTone& base_tone) :TraitTone()
    {
        auto mseq = MSequence(bits, tap);
        auto sequence = mseq.GenOneCycle();

        for (int i = 0;i < sequence->size();i++) {
            for (int j = 0;j < base_tone.size();j++)
            {
                this->push_back(j * (i * 2 - 1));
            }
        }
        this->_sequence = move(sequence);
    }
    MSeqTone::~MSeqTone() {
    }
    const vector<int>& MSeqTone::GetSequence()
    {
        return *this->_sequence.get();
    }

    // """ トーン信号をPN符号でBPSK変調した信号です。
    //     intervalティック単位で変調します。
    // """
    PnTone::PnTone(int seed, int interval) : PnTone(seed, interval, SinTone(20, 8)) {}
    PnTone::PnTone(int seed, int interval, const TraitTone& base_tone) : TraitTone()
    {
        //auto tone = base_tone != null ? base_tone : new SinTone(20, 8);

        auto pn = XorShiftRand31(seed, /*skip*/29);
        auto c = 0;
        int f = 0;
        for (auto i = 0;i < base_tone.size();i++)
        {
            if (c % interval == 0)
            {
                f = (pn.Next() & 0x02) - 1;
            }
            c = c + 1;
            this->push_back(i * f);
        }
    }

    class DefaultIter :public  IPyIterator<int>
    {
    private:
        XorShiftRand31 _pn;
    public:
        DefaultIter() :_pn{ XorShiftRand31(999, 299) }
        {
        }
        virtual ~DefaultIter() {
        }
        int Next()
        {
            return ((this->_pn.Next() & 0x01) * 2 - 1);
        }
    };


    XPskSinTone::XPskSinTone(int points, int cycle, int div) :XPskSinTone(points, cycle, div, NULL) {}

    XPskSinTone::XPskSinTone(int points, int cycle, int div, IPyIterator<int>& shift) :XPskSinTone(points, cycle, div, &shift) {}

    XPskSinTone::XPskSinTone(int points, int cycle, int div, IPyIterator<int>* shift) :TraitTone()
    {
        unique_ptr<DefaultIter> d = shift==NULL?make_unique<DefaultIter>():NULL;


        auto lshift = d != NULL ? d.get() : shift;
        auto delta = TBSK_PI * 2 / points;
        auto s = delta * 0.5;
        for (auto i = 0; i < points * cycle; i++)
        {
            s = s + delta + lshift->Next() * (TBSK_PI * 2 / div);
            this->push_back(sin(s));
        }

    }
}

