#include "MSequence.h"
#include <math.h>
#include <vector>
#include <iterator>
namespace TBSKmodemCPP {
    using std::make_unique;
}
namespace TBSKmodemCPP
{

    MSequence::MSequence(int bits, int tap, TBSK_UINT32 sr):
        _bits{bits},
        _tap{tap},
        _mask{ (TBSK_UINT32)(pow(2, bits) - 1) }//(2**bits-1);
    {
        TBSK_ASSERT(bits < 64);
        TBSK_ASSERT(bits >= 2);
        TBSK_ASSERT(tap < bits);
        TBSK_ASSERT(sr != 0);
        this->_sr = sr & this->_mask;
    }
    MSequence::MSequence(int bits, TBSK_UINT32 sr) :MSequence(bits, bits - 2, sr) {
    }


    int MSequence::Next() {
        auto b = this->_bits;
        auto t = this->_tap;
        auto sr = this->_sr;
        // # type(sr)

        auto m = (sr >> b);
        auto n = (sr >> t);
        // # print(sr,(sr<<1) , (n^m),(sr<<1)+ (n^m))
        auto bit = (n ^ m) & 1;
        sr = (sr << 1) | bit;
        this->_sr = sr & this->_mask;
        return (int)bit;
    }
    // def maxcycle(self):
    //     """最大周期を返します。
    //     """
    //     return 2**(self._bits+1)-1
    int MSequence::GetMaxcycle()const {
        return (int)(pow(2, this->_bits) + 1) - 1;
    }
    // @property
    // def gets(self,n:int):
    //     """n個の成分を返します。
    //     """
    //     return tuple([next(self) for i in range(n)])
    unique_ptr<vector<int>> MSequence::Gets(int n) {
        auto r = make_unique<vector<int>>();
        for (auto i = 0;i < n;i++) {
            r->push_back(this->Next());
        }
        return r;
    }
    // def getOneCycle(self):
    //     """1サイクル分のシーケンスを得ます
    //     """
    //     return self.gets(self.cycles())
    unique_ptr<vector<int>> MSequence::GenOneCycle() {
        return this->Gets(this->Cycles());
    }
    // def cycles(self)->int:
    //     """M系列の周期を計測します。
    //     """
    //     old_sr=self._sr
    //     l=self.maxcycle
    //     b=[]
    //     b.append(next(self))
    //     mv=0
    //     for i in range(l+1):
    //         b.append(next(self))
    //         #チェック
    //         if b[0:i]==b[i:i*2]:                
    //             mv=i
    //         b.append(next(self))
    //     # print("".join([str(i)for i in b]))
    //     self._sr=old_sr
    //     return mv
    int MSequence::Cycles() {
        auto old_sr = this->_sr;
        auto l = this->GetMaxcycle();
        auto b = std::vector<int>();
        b.push_back(this->Next());
        auto mv = 0;
        for (int i = 0;i < l + 1;i++) {
            b.push_back(this->Next());
            // #チェック
            // if b[0:i]==b[i:i*2]:                
            //     mv=i
            int i2 = i * 2;
            std::vector<int> lv;
            std::vector<int> rv;
            std::copy(b.begin(),b.begin()+i, std::back_inserter(lv));
            std::copy(b.begin() + i, b.begin() + i2, std::back_inserter(rv));
            if (lv == rv) {
                mv = i;
            }
            b.push_back(this->Next());
        }
        // # print("".join([str(i)for i in b]))
        this->_sr = old_sr;
        return mv;
    }
    //static public Dictionary<int,int> GetCycleMap(int bits){
    //    var r=new Dictionary<int,int>();
    //    for(var i=0;i<bits-1;i++){
    //        var m=new MSequence(bits,i);
    //        r.Add(i,m.Cycles());
    //    }
    //    return r;

}


