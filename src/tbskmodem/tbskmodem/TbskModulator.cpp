#include "../kokolink/filter/BitsWidthFilter.h"
#include "TbskModulator.h"
#include "TbskTone.h"
#include "TbskPreamble.h"


namespace TBSKmodemCPP
{
    using std::make_shared;
    using std::shared_ptr;
    using std::vector;

    TbskModulator::TbskModulator(const shared_ptr<const TraitTone>& tone) :
        TbskModulator_impl(tone, make_shared<CoffPreamble>(tone))
    {
    }

    TbskModulator::TbskModulator(const shared_ptr<const TraitTone>& tone, const shared_ptr<const Preamble>& preamble) 
        :TbskModulator_impl(tone, preamble) {

    }


    shared_ptr<IPyIterator<double>> TbskModulator::ModulateAsBit(const shared_ptr<IRoStream<int>>& src) {
        return TbskModulator_impl::ModulateAsBit(src);
    }
    shared_ptr<IPyIterator<double>> TbskModulator::Modulate(const shared_ptr<IPyIterator<int>>& src, int bitwidth) {
        auto s = make_shared<BitsWidthFilter>(bitwidth);
        s->SetInput(make_shared<RoStream<int>>(src));
        return this->ModulateAsBit(s);
    }




    shared_ptr<IPyIterator<double>> TbskModulator::Modulate(const char* src, int length) {
        if (length < 0) {
            length = (int)strnlen(src, (size_t)16 * 1024);
        }
        auto d = make_shared<vector<int>>();
        for (auto i = 0;i < length;i++) {
            d->push_back((int)*(src + i));
        }
        return this->Modulate(make_shared<PyIterator<int>>(d), 8);
    }



}