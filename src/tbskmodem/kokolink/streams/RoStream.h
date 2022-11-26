#pragma once
#include <vector>
#include <queue>
#include "../interfaces.h"
#include "./rostreams.h"
namespace TBSKmodemCPP
{

    // """T型のRoStreamです。
    // """
    template <typename T> class RoStream:public BasicRoStream<T>
    {
    private:
        const shared_ptr<IPyIterator<T>> _src;
        TBSK_INT64 _pos;
    public:
        RoStream(const shared_ptr<IPyIterator<T>>& src);
        virtual ~RoStream() {}
        T Next()override;
        TBSK_INT64 GetPos()const override;
    };







}


