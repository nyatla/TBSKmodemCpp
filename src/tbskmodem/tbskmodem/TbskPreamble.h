#pragma once
#include "../kokolink/protocol/tbsk/toneblock.h"
#include "../kokolink/protocol/tbsk/preamble/Preamble.h"
#include "../kokolink/protocol/tbsk/preamble/CoffPreamble.h"

#include <memory>


namespace TBSKmodemCPP
{
    class TbskPreamble
    {
    public:
        static std::shared_ptr<Preamble> createCoff(const std::shared_ptr<const TraitTone>& tone)
        {
            return createCoff(tone, CoffPreamble::DEFAULT_TH, CoffPreamble::DEFAULT_CYCLE);
        }
        static std::shared_ptr<Preamble> createCoff(const std::shared_ptr<const TraitTone>& tone, double threshold, int cycle)
        {
            return std::make_shared<CoffPreamble>(tone, threshold, cycle);
        }
    };
}

