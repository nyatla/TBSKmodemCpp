#pragma once
#include "../types/types.h"
#include "../compatibility.h"
#include <vector>
#include <string>
#include <memory>
namespace TBSKmodemCPP
{



    template <typename T> class RingBuffer:public NoneCopyConstructor_class
    {
    private:
        vector<T> _buf;


        int _p;


        //static T* _genIEnumerable(int length, T pad)
        //{
        //    auto r = new T[length];
        //    for (auto i = 0;i < length;i++) {
        //        *(r + i) = pad;
        //    }
        //    return r;
        //};
    public:
        RingBuffer(size_t length, const T pad);
        virtual ~RingBuffer();
        T Append(T v);
        void Extend(const T* buf, int len);
        /** リストの一部を切り取って返します。
            この関数はバッファの再配置を行いません。
            C++拡張仕様 戻り値は解放して。
        */
        const unique_ptr<vector<T>> Sublist(int pos, int size)const;
        // @property
        // def tail(self)->T:
        //     """ バッファの末尾 もっとも新しい要素"""
        //     length=len(self._buf)
        //     return self._buf[(self._p-1+length)%length]
        T& GetTail()const;
        // @property
        // def top(self)->T:
        //     """ バッファの先頭 最も古い要素"""
        //     return self._buf[self._p]
        T& GetTop()const;
        // def __getitem__(self,s)->List[T]:
        //     """ 通常のリストにして返します。
        //         必要に応じて再配置します。再配置せずに取り出す場合はsublistを使用します。
        //     """
        //     b=self._buf
        //     if self._p!=0:
        //         self._buf= b[self._p:]+b[:self._p]
        //     self._p=0
        //     return self._buf[s]
        T operator[](int s);
        // def __len__(self)->int:
        //     return len(self._buf)
        int GetLength()const;
    };
}
