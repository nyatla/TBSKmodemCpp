#include "../../types/types.h"
#include "./XorShiftRand31.h"

namespace TBSKmodemCPP
{

    XorShiftRand31::XorShiftRand31(TBSK_UINT32 seed, int skip) {
        this->_seed = seed;

        for (auto i = 0;i < skip;i++) {
            this->Next();
        }
    }
    TBSK_INT32 XorShiftRand31::Next() {
        auto y = this->_seed;
        y = y ^ (y << 13);
        y = y ^ (y >> 17);
        y = y ^ (y << 5);
        y = y & 0x7fffffff;
        this->_seed = y;
        return (TBSK_INT32)y;
    }
    // def randRange(self,limit:int):
    //     """ 0<=n<limit-1の値を返します。
    //     """
    //     return next(self) % limit
    int XorShiftRand31::RandRange(TBSK_INT32 limit) {
        return this->Next() % limit;
    }

}


