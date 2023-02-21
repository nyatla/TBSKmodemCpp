#include "../kokolink/filter/BitsWidthFilter.h"
#include "TbskModulator.h"
#include "TbskTone.h"
#include "TbskPreamble.h"
#include <string.h>

namespace TBSKmodemCPP
{
    using std::make_shared;
    using std::shared_ptr;
    using std::vector;

    class SuffixTone : public IPyIterator<double>
    {
    private:
        shared_ptr<const TraitTone> _tone;
        int _c;
    public:
        SuffixTone(const shared_ptr<const TraitTone>& tone) :_tone{ tone }, _c{ 0 }
        {
        }
        double Next()
        {
            auto c = this->_c;
            auto& tone = this->_tone;
            if (c >= tone->size())
            {
                throw PyStopIteration();
            }
            
            auto r = c % 2 == 0 ? tone->at(c) * 0.5 : -tone->at(c) * 0.5;
            this->_c++;
            return r;
        }
    };




    TbskModulator::TbskModulator(const shared_ptr<const TraitTone>& tone) :
        TbskModulator(tone, make_shared<CoffPreamble>(tone))
    {   //inherit constructor
    }
    TbskModulator::TbskModulator(const shared_ptr<const TraitTone>& tone, int preamble_cycle) :
        TbskModulator(tone, make_shared<CoffPreamble>(tone,CoffPreamble::DEFAULT_TH,preamble_cycle))
    {
    }

    TbskModulator::TbskModulator(const shared_ptr<const TraitTone>& tone, const shared_ptr<const Preamble>& preamble) 
        :TbskModulator_impl(tone, preamble) {

    }


    shared_ptr<IPyIterator<double>> TbskModulator::ModulateAsBit(const shared_ptr<IRoStream<int>>& src, bool stopsymbol) {
        if (stopsymbol) {
            auto v=make_shared<SuffixTone>(this->_tone);
            return TbskModulator_impl::ModulateAsBit(src, v, true);
        }
        else {
            shared_ptr<IPyIterator<double>> tmp;//null
            return TbskModulator_impl::ModulateAsBit(src, tmp,true);
        }
    }
    shared_ptr<IPyIterator<double>> TbskModulator::Modulate(const shared_ptr<IPyIterator<int>>& src, int bitwidth, bool stopsymbol) {
        auto s = make_shared<BitsWidthFilter>(bitwidth);
        s->SetInput(make_shared<RoStream<int>>(src));
        return this->ModulateAsBit(s, stopsymbol);
    }




    shared_ptr<IPyIterator<double>> TbskModulator::Modulate(const char* src, int length, bool stopsymbol) {
        if (length < 0) {
            length = (int)strnlen(src, (size_t)16 * 1024);
        }
        auto d = make_shared<vector<int>>();
        for (auto i = 0;i < length;i++) {
            d->push_back((int)*(src + i));
        }
        return this->Modulate(make_shared<PyIterator<int>>(d), 8, stopsymbol);
    }



}