#pragma once
#include "../interfaces.h"
#include "../utils/recoverable.h"

#include <vector>
#include <queue>
#include <memory>
namespace TBSKmodemCPP
{
    using namespace std;
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
#pragma warning( default : 4250 )




}














