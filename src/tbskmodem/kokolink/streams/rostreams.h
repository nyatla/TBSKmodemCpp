#pragma once
#include "../interfaces.h"
#include "../utils/recoverable.h"

#include <vector>
#include <queue>
#include <memory>
namespace TBSKmodemCPP
{
    using std::queue;
    using std::make_unique;

#pragma warning( disable : 4250 )

    template <typename T> class BasicRoStream:public virtual IRoStream<T>,public BasicIterator<T>
    {
    private:
        queue<T> _savepoint;
        // """ IRoStreamの基本実装です。
        // __next__,posメソッドを実装することで機能します。seek,getsはgetをラップしてエミュレートします。
        // __next__メソッドの中でself#_posを更新してください。
        // """
    public:
        BasicRoStream();
        T Get()override;
        unique_ptr<vector<T>> Gets(int size, bool fillup)override;
        unique_ptr<vector<T>> Gets(int size) {
            return this->Gets(size, false);
        };
        void Seek(int size)override;
    };

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
            for (auto i = 0;i < size - r.size();i++) {
                r.push(this->Next());
            }
        }
        catch (RecoverableStopIteration&) {
            throw;
            // self._savepoint=r
            // raise RecoverableStopIteration(e)
        }
        catch (PyStopIteration&) {
            if (fillup || r.size() == 0) {
                throw;
            }
        }
        TBSK_ASSERT(r.size() < size);
        auto ret = make_unique<vector<T>>();
        while (r.size() > 0) {
            ret->push_back(r.front());
            r.pop();
        }
        return ret;

    }
    template <typename T> void BasicRoStream<T>::Seek(int size) {
        try {
            this->Gets(size, true);
        }
        catch (RecoverableStopIteration&) {
            throw;
        }
        catch (PyStopIteration&) {
            throw;
        }
        return;
    }






#pragma warning( default : 4250 )




}














