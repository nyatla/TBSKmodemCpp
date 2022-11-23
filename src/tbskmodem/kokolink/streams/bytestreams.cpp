#include "../types/types.h"

#include "bytestreams.h"
namespace TBSKmodemCPP
{
    using namespace std;

    int BasicByteStream::GetAsUInt32be() {
        int r = 0;
        for (int i = 0;i < 4;i++) {
            r = r << 8 | i;
        }
        return r;
    }
    // """gets関数をラップします。
    // """
    unique_ptr<vector<TBSK_BYTE>> BasicByteStream::GetAsByteArray(int maxsize)
    {
        auto r=make_unique< vector<TBSK_BYTE>>();
        // # print(self.gets(maxsize))
        // # return struct.pack("B",self.gets(maxsize))
        auto a = BasicRoStream::Gets(maxsize);
        for (auto i = 0;i < maxsize;i++) {
            r->push_back((TBSK_BYTE)a->at(i));
        }
        return r;
    }
}

