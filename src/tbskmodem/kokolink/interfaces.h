#pragma once
#include "types/Py__special_functions__.h"
#include "types/Py__class__.h"

#include "types/types.h"
#include <vector>
#include <memory>

namespace TBSKmodemCPP
{
    using namespace std;

    template <typename T> class IRecoverableIterator : public virtual IPyIterator<T>
    {
    };

    template <typename T> class IRoStream : public virtual IRecoverableIterator<T>
    {
    public:
        // @abstractmethod
        // def seek(self,size:int):
        //     """ストリームの先頭を移動します。
        //     終端に到達するとStopIterationをスローします。
        //     """
        //     pass
        virtual void Seek(int size) = 0;

        virtual T Get()=0;
        // @abstractmethod
        // def get(self)->T:
        //     """ストリームの先頭から1要素を要素を取り出します。
        //     終端に到達するとStopIterationをスローします。
        //     """
        //     pass
        virtual unique_ptr<vector<T>> Gets(int size, bool fillup) = 0;


        // @abstractproperty
        // def pos(self)->int:
        //     """ストリーム上の現在の読み出し位置です。get/getsで読み出した要素数+seekで読み出した要素数の合計です。
        //     """
        //     pass
        virtual TBSK_INT64 GetPos()const = 0;
    };



    // """Byte値のストリームです。返却型はbyteの値範囲を持つintです。
    // """
    class IByteStream :public virtual IRoStream<int> {
        // def getAsUInt32be(self)->int:
        //     """byteStreamから32bit unsigned int BEを読み出します。
        //     """
        //     pass
        virtual int GetAsUInt32be() = 0;
        // def getAsByteArray(self,size)->bytearray:
        //     """byteStreamから指定サイズのbytearrayを読み出します。
        //     """
        //     pass
        virtual unique_ptr<vector<TBSK_BYTE>> GetAsByteArray(int maxsize)=0;
    };

    // """ Bit値を返すストリームです。返却型は0,1の値範囲を持つintです。
    // """
    class IBitStream :public virtual IRoStream<int> {};

    // class IReader(Generic[T],ABC):
    //     """Tから値を読み出します。このクラスは分類であり関数を持ちません。
    //     """

    // class IScanner(Generic[T],ABC):
    //     """Tの読み出し位置を変更せずに値を抽出します。このクラスは分類であり関数を持ちません。
    //     """






    //     """入力と出力の型が異なるストリームです。
    //     ソース値のプロバイダSからD型の値を生成します。
    //     """
    template <typename IMPL, typename S, typename D> class IFilter :public virtual IRoStream<D>
    {
        //     @abstractmethod
        //     def setInput(self,src:S)->"IFilter[S,D]":
        //         """新しいソースをインスタンスにセットします。
        //         関数が終了した時点で、インスタンスの状態は初期状態に戻ります。
        //         内部キューがある場合、既に読み出された入力側の内部キューと、未出力の出力側のキューがある場合は値を破棄します。 

        //         ・望ましい実装

        //             Noneがセットされた場合は、インスタンスは終端状態になり、ブロックしているget(s)/peek(s)関数があればStopInterationを発生させて終了するべきです。
        //         """
        //         pass
    public:
        virtual IMPL& SetInput(shared_ptr<S>&& src) = 0;

        virtual ~IFilter() {}
    };


    // class IEncoder(IFilter[S,D],Generic[S,D],ABC):
    //     """より物理値に近い情報を出力するストリームです。
    //     """
    template <typename IMPL, typename S, typename D> class IEncoder :public virtual IFilter<IMPL, S, D> { };


    // class IDecoder(IFilter[S,D],Generic[S,D],ABC):
    //     """より抽象値に近い情報を出力するストリームです。
    //     """
    template <typename IMPL, typename S, typename D> class IDecoder :public virtual IFilter<IMPL, S, D> { };

    // class IGenerator(IRoStream[T],Generic[T],ABC):
    //     """T型のストリームを生成するクラスです。
    //     ストリームクラスの亜種ですが、主にパラメーターから数列を生成するクラスの分類です。
    //     """
    //     pass
    template <typename T> class IGenerator :public virtual IRoStream<T> {};


}

