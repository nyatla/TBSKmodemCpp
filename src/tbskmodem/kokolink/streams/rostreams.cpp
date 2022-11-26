#include "rostreams.h"
#include <vector>
#include <queue>
namespace TBSKmodemCPP
{

    template <typename T> BasicRoStream<T>::BasicRoStream()
    {
        //this->_savepoint=new Queue<T>();
    }
    template <typename T> T BasicRoStream<T>::Get() {
        if (this->_savepoint.size() > 0) {
            // #読出し済みのものがあったらそれを返す。
            T r = this->_savepoint.front();
            // this._savepoint=self._savepoint[1:]
            // if(this._savepoint.Length==0){
            //     self._savepoint=null;
            // }
            return r;
        }
        return this->Next();
    }
    template <typename T> unique_ptr<vector<T>> BasicRoStream<T>::Gets(int size, bool fillup)
    {
        queue<T>& r = this->_savepoint;
        try {
            for (auto i = 0;i < size - r.Count;i++) {
                r->Enqueue(this.Next());
            }
        }
        catch (RecoverableStopIteration e) {
            throw e;
            // self._savepoint=r
            // raise RecoverableStopIteration(e)
        }
        catch (PyStopIteration e) {
            if (fillup || r.Count == 0) {
                throw e;
            }
        }
        TBSK_ASSERT(r.Count < size);
        auto ret = make_unique<vector<T>>();
        while (r.Count > 0) {
            ret->push_back(r.front());
        }
        return ret;

    }
    template <typename T> void BasicRoStream<T>::Seek(int size) {
        try {
            this->Gets(size, true);
        }
        catch (RecoverableStopIteration e) {
            throw e;
        }
        catch (PyStopIteration e) {
            throw e;
        }
        return;
    }

}














