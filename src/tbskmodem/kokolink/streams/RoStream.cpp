#include <vector>
#include "RoStream.h"
#include "../utils/recoverable.h"
namespace TBSKmodemCPP
{

    //template <typename T> RoStream<T>::RoStream(const shared_ptr<IPyIterator<T>>& src) :BasicRoStream<T>(),
    //    _src{src}
    //{
    //    this->_pos = 0;
    //}
    //template <typename T> T RoStream<T>::Next()
    //{
    //    T r;
    //    try {
    //        r = this->_src.Next(); //#RecoverableStopInterationを受け取っても問題ない。
    //    }
    //    catch (RecoverableStopIteration e) {
    //        throw e;
    //    }
    //    this->_pos += 1;
    //    return r;
    //}
    //template <typename T> TBSK_INT64 RoStream<T>::GetPos()const
    //{
    //    return this->_pos;
    //}



}


