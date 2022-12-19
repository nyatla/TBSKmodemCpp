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
    template <typename ASYNC_METHOD> class RecoverableException :public exception
    {
        // """ 関数を再試行します。再試行可能な状態で失敗した場合は、自分自信を返します。
        // """
    private:
        const shared_ptr<ASYNC_METHOD> _recover_instance;
        RecoverableException() = delete;
    public:
        RecoverableException(const shared_ptr<ASYNC_METHOD>& recover_instance) : _recover_instance{ recover_instance }{}
        virtual ~RecoverableException() {
            this->Close();
        }
        virtual void Close() {
            this->_recover_instance->Close();
        }
        shared_ptr<ASYNC_METHOD> Detach() {
            return std::move(this->_recover_instance);
        }
    };

    //template <typename T> RecoverableException<T>::RecoverableException(const shared_ptr<AsyncMethod<T>>& recover_instance) : _recover_instance{ recover_instance }
    //{
    //}
}
