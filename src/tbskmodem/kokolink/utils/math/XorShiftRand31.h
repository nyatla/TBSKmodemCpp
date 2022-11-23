#pragma once
#include "../recoverable.h"
#include "../../types/types.h"

namespace TBSKmodemCPP
{
    // """ https://ja.wikipedia.org/wiki/Xorshift
    // """
    class XorShiftRand31 :IPyIterator<TBSK_INT32>
    {
    private:
        TBSK_UINT64 _seed;
    public:
        XorShiftRand31(TBSK_UINT32 seed, int skip = 0);
        // def randRange(self,limit:int):
        //     """ 0<=n<limit-1の値を返します。
        //     """
        //     return next(self) % limit
        TBSK_INT32 Next();
        int RandRange(TBSK_INT32 limit);
    };

}


