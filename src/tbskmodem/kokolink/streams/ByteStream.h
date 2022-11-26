#pragma once
#include "../interfaces.h"
#include "./bytestreams.h"
#include <memory>

namespace TBSKmodemCPP
{


#pragma warning( disable : 4250 )
    class ByteStream :public BasicByteStream {

    private:
        TBSK_INT64 _pos;
        shared_ptr<IPyIterator<int>> _iter;
        // """ iterをラップするByteStreamストリームを生成します。
        //     bytesの場合は1バイトづつ返します。
        //     strの場合はbytesに変換してから1バイトづつ返します。
        // """
    private:
        ByteStream(shared_ptr<IPyIterator<int>>&& src, int inital_pos = 0);
    public:
        //ByteStream(IPyIterator<TBSK_BYTE>& ref_src, int inital_pos = 0);
        //ByteStream(IPyIterator<char>& ref_src, int inital_pos = 0);
        //ByteStream(IPyIterator<int>& ref_src, int inital_pos = 0);
        virtual ~ByteStream();
        int Next()override;
        TBSK_INT64 GetPos()const override;
    };
#pragma warning( default : 4250 )

}