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
        RoStream(const shared_ptr<IPyIterator<T>>& src) :BasicRoStream<T>(),
            _src{ src }
        {
            this->_pos = 0;
        }
        virtual ~RoStream() {}
        T Next()override
        {
            T r;
            try {
                r = this->_src->Next(); //#RecoverableStopInterationを受け取っても問題ない。
            }
            catch (RecoverableStopIteration e) {
                throw e;
            }
            this->_pos += 1;
            return r;
        };
        TBSK_INT64 GetPos()const override {
            return this->_pos;
        }
    };





}


