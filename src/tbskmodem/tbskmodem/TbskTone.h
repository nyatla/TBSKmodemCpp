#pragma once
#include "../kokolink/protocol/tbsk/toneblock.h"

#include <memory>


namespace TBSKmodemCPP
{

    class TbskTone
    {
    public:
        static std::shared_ptr<SinTone> CreateSin()
        {
            return CreateSin(10, 10);
        }
        static std::shared_ptr<SinTone> CreateSin(int points)
        {
            return CreateSin(points, 1);
        }
        static std::shared_ptr<SinTone> CreateSin(int points, int cycle)
        {
            return std::make_shared<SinTone>(points, cycle);
        }

    public:
        static std::shared_ptr<XPskSinTone> CreateXPskSin()
        {
            return CreateXPskSin(10, 10);
        }
        static std::shared_ptr<XPskSinTone> CreateXPskSin(int points)
        {
            return CreateXPskSin(points, 1);
        }
        static std::shared_ptr<XPskSinTone> CreateXPskSin(int points, int cycle)
        {
            return std::make_shared<XPskSinTone>(points, cycle, 8);
        }

    public:
        static std::shared_ptr<PnTone> CreatePn(int seed, int interval,const std::shared_ptr<const TraitTone>& base_tone)
        {
            const TraitTone* t = base_tone.get();//一時的な借用
            return std::make_shared<PnTone>(seed, interval,*t);
        }
        static std::shared_ptr<PnTone> CreatePn(int seed)
        {
            return std::make_shared<PnTone>(seed, 2);
        }

    public:
        static std::shared_ptr<MSeqTone> CreateMSeq(const std::shared_ptr<MSequence>& mseq)
        {
            auto s=std::make_shared<SinTone>(20, 1);
            return CreateMSeq(mseq,s);
        }
        static std::shared_ptr<MSeqTone> CreateMSeq(int bits, int tap)
        {
            auto w = std::make_shared<MSequence>(bits, tap);
            return CreateMSeq(w);
        }
        static std::shared_ptr<MSeqTone> CreateMSeq(int bits, int tap, const std::shared_ptr<const TraitTone> base_tone)
        {
            auto mseq=make_shared<MSequence>(bits, tap);
            return CreateMSeq(mseq, base_tone);
        }
        static std::shared_ptr<MSeqTone> CreateMSeq(const std::shared_ptr<MSequence>& mseq, const std::shared_ptr<const TraitTone> base_tone)
        {
            return std::make_shared<MSeqTone>(*mseq, *base_tone);
        }

    public:
        static std::shared_ptr<TraitTone> CreateCostom(const std::vector<double> d)
        {
            auto t = std::make_shared<TraitTone>();
            for (auto i : d) {
                t->push_back(i);
            }
            return t;
        }

    };
}
        

