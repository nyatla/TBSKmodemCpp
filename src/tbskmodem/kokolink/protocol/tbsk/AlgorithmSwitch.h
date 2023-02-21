#pragma once
#include "../../../kokolink/utils/math/corrcoef/ISelfCorrcoefIterator.h" 
#include "../../../kokolink/utils/math/corrcoef/SelfCorrcoefIterator2.h" 
#include <memory>



namespace TBSKmodemCPP
{
    class AlgorithmSwitch {
    public:
        static std::shared_ptr<ISelfCorrcoefIterator> createSelfCorrcoefIterator(int window,const std::shared_ptr<IPyIterator<double>>& src, int shift = 0)
        {
            return std::make_shared<SelfCorrcoefIterator2>(window, src, shift = shift);
        }

    };





}