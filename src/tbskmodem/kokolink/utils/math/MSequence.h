#pragma once
#include "../../types/types.h"
#include "../../interfaces.h"
#include <vector>
namespace TBSKmodemCPP
{
    using namespace std;
    class MSequence:IPyIterator<int> {
    private:
        const int _bits;
        const int _tap;
        const TBSK_UINT32 _mask;
        TBSK_UINT32 _sr;
    public:
        MSequence(int bits, TBSK_UINT32 sr = 1);
        MSequence(int bits, int tap, TBSK_UINT32 sr = 1);
        int Next();
        int GetMaxcycle()const;
        unique_ptr<vector<int>> Gets(int n);
        unique_ptr<vector<int>> GenOneCycle();
        int Cycles();
        //static public Dictionary<int,int> GetCycleMap(int bits){
        //    var r=new Dictionary<int,int>();
        //    for(var i=0;i<bits-1;i++){
        //        var m=new MSequence(bits,i);
        //        r.Add(i,m.Cycles());
        //    }
        //    return r;
    };




}


