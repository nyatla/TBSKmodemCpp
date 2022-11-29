#pragma once
#include "../types/Py__class__.h"
#include "./AsyncMethod.h"
#include<memory>

namespace TBSKmodemCPP
{
    // """ リカバリー可能なStopIterationです。
    //     イテレータチェーンのスループット調停などで、イテレータが再実行の機会を与えるために使います。
    //     この例外を受け取ったハンドラは、__next__メソッドを実行することで前回失敗した処理を再実行することができます。
    //     再実行を行わない場合はStopIterationと同じ振る舞いをしますが、異なる処理系ではセッションのファイナライズ処理が必要かもしれません。
    // """
    class RecoverableStopIteration : public PyStopIteration
    {
    public:
        RecoverableStopIteration() {};
        RecoverableStopIteration(exception innerException) :PyStopIteration(innerException) {};
    };




    template <typename T> class BasicRecoverableIterator :public BasicIterator<T> {
    };



}
namespace TBSKmodemCPP
{
    // リカバリー手段を送信する例外です。
    // T型のリカバリ手段のインスタンスはDetach関数で取得できます。
    // リカバリを取得しなかった場合は例外破棄と同時にその手段も破棄します。
    // 
    template <typename T> class RecoverableException :exception
    {
        // """ 関数を再試行します。再試行可能な状態で失敗した場合は、自分自信を返します。
        // """
    private:
        const shared_ptr<AsyncMethod<T>> _recover_instance;
    public:
        RecoverableException(const shared_ptr<AsyncMethod<T>>& recover_instance);
        virtual ~RecoverableException();
        virtual void Close();
        const shared_ptr<AsyncMethod<T>> Detach();
    };

    template <typename T> RecoverableException<T>::RecoverableException(const shared_ptr<AsyncMethod<T>>& recover_instance) : _recover_instance{ recover_instance }
    {
    }
    template <typename T> RecoverableException<T>::~RecoverableException() {
        this->Close();
    }
    template <typename T> const shared_ptr<AsyncMethod<T>> RecoverableException<T>::Detach()
    {
        return move(this->_recover_instance);
    }
    template <typename T> void RecoverableException<T>::Close()
    {
        this->_recover_instance->Close();
    }
}
