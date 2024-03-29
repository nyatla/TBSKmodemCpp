#pragma once
#include "../interfaces.h"
#include "./rostreams.h"
#include <vector>
namespace TBSKmodemCPP
{

#pragma warning( disable : 4250 )
    class BasicByteStream :public virtual IByteStream,public BasicRoStream<int> {

        // """BigEndianのUint32を読み出す
        // """
    public:
        int GetAsUInt32be()override;
        // """gets関数をラップします。
        // """
        unique_ptr<vector<TBSK_BYTE>> GetAsByteArray(int maxsize)override;
    };
#pragma warning( default : 4250 )
}


