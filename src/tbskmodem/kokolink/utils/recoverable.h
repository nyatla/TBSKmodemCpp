#pragma once
#include "../types/Py__class__.h"
#include "../types/types.h"
#include<memory>

namespace TBSKmodemCPP
{
    using namespace std;
    // """ リカバリー可能なStopIterationです。
    //     イテレータチェーンのスループット調停などで、イテレータが再実行の機会を与えるために使います。
    //     この例外を受け取ったハンドラは、__next__メソッドを実行することで前回失敗した処理を再実行することができます。
    //     再実行を行わない場合はStopIterationと同じ振る舞いをしますが、異なる処理系ではセッションのファイナライズ処理が必要かもしれません。
    // """
    class RecoverableStopIteration : public PyStopIteration
    {
    public:
        RecoverableStopIteration();
        RecoverableStopIteration(exception innerException);
    };

    template <typename T> class BasicRecoverableIterator :public BasicIterator<T> {
    };


    // リカバリー手段を送信する例外です。
    // T型のリカバリ手段のインスタンスはDetach関数で取得できます。
    // リカバリを取得しなかった場合は例外破棄と同時にその手段も破棄します。
    // 
    template <typename T> class RecoverableException :exception
    {
        // """ 関数を再試行します。再試行可能な状態で失敗した場合は、自分自信を返します。
        // """
    private:
        const shared_ptr<T> _recover_instance;
    public:
        RecoverableException(const shared_ptr<T>& recover_instance);
        virtual ~RecoverableException();
        virtual void Close();
        const shared_ptr<T> Detach();
    };



    //template <typename ASMETHOD> class AsyncMethodRecoverException : RecoverableException<ASMETHOD>
    //{
    //private:
    //    ASMETHOD* _asmethod;

    //    //""" AsyncMethodをラップするRecoverExceptionです。
    //    //    Recoverableをgeneratorで実装するときに使います。

    //    //    このクラスは、(exception, T) を返すgeneratorをラップして、recoverで再実行可能な状態にします。
    //    //    generatorはnextで再実行可能な状態でなければなりません。
    //    //"""
    //public:
    //    AsyncMethodRecoverException(ASMETHOD* asmethod);

    //    virtual ~AsyncMethodRecoverException();
    //    //""" 例外発生元のrunを再実行します。
    //    //    例外が発生した場合、closeを実行してそのまま例外を再生成します。
    //    //"""
    //    void Close() override;
    //    const ASMETHOD* Detach();
    //};


    // class NoneRestrictIteraor(IRecoverableIterator[T]):
    //     """ Noneの混在したストリームで、Noneを検出するたびにRecoverableStopInterationを発生します。
    //         None混在の一般IteratorをIRecoverableIteratorに変換します。
    //     """
    //     def __init__(self,iter:Iterator[T]):
    //         self._iter=iter
    //     def __next__(self) ->T:
    //         r=next(self._iter)
    //         if r is None:
    //             raise RecoverableStopIteration()
    //         return r
    // class SkipRecoverIteraor(Iterator[T]):
    //     """ IRecoverableIteratorを一般Iteratorをに変換します。
    //     """
    //     def __init__(self,iter:Iterator):
    //         self._iter=iter
    //     def __next__(self) ->T:
    //         while True:
    //             try:
    //                 return next(self._iter)
    //             except RecoverableStopIteration:
    //                 # print("skip")
    //                 continue
}
