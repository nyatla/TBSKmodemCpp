#include "RingBuffer.h"

namespace TBSKmodemCPP{





    template <typename T> RingBuffer<T>::RingBuffer(size_t length, const T pad):
        _buf{vector<T>()}
    {
        for (int i = 0;i < length;i++) {
            this->_buf.push_back(T);
        }
        this->_p = 0;
    }
    template <typename T> RingBuffer<T>::~RingBuffer() {
    }

    template <typename T> T RingBuffer<T>::Append(T v){
        const T* b = this->_buf.data();
        const auto length = this->_buf.size();
        auto ret=b[this->_p];
        b[this->_p]=v;
        this->_p=(this->_p+1)%length;
        return ret;

    }
    template <typename T> void RingBuffer<T>::Extend(const T* buf, int len){
        for (int i = 0;i < len;i++) {
            this->Append(*(buf + i));
        }
    }
    ///** リストの一部を切り取って返します。
    //    この関数はバッファの再配置を行いません。
    //*/    
    template <typename T> const unique_ptr<vector<T>> RingBuffer<T>::Sublist(int pos, int size)const {
        const auto l = this->_buf->size();
        auto buf = this->_buf->data();
        if (pos >= 0) {
            const auto p = this->_p + pos;
            if (size >= 0) {
                TBSK_ASSERT(pos + size <= l);
                auto ret = make_unique<vector<T>>(size);
                for (auto i = 0;i < size;i++) {
                    ret[i] = buf[(p + i) % l];
                }
                return ret;
            }
            else {
                TBSK_ASSERT(pos + size + 1 >= 0);
                auto ret = make_unique<vector<T>>(- size);
                // return tuple([self._buf[(p+size+i+1)%l] for i in range(-size)])
                for (auto i = 0;i < -size;i++) {
                    ret[i] = buf[(p + size + i + 1) % l];
                }
                return ret;
            }
        }
        else {
            auto p = this->_p + l + pos;
            if (size >= 0) {
                TBSK_ASSERT(l + pos + size <= l);
                // return tuple([self._buf[(p+i)%l] for i in range(size)])
                auto ret = make_unique<vector<T>>(size);
                for (auto i = 0;i < size;i++) {
                    ret[i] = buf[(p + i) % l];
                }
                return ret;
            }
            else {
                TBSK_ASSERT(l + pos + size + 1 >= 0);
                // return tuple([self._buf[(p-i+l)%l] for i in range(-size)])
                auto ret = make_unique<vector<T>>(size);
                for (auto i = 0;i < -size;i++) {
                    ret[i] = buf[(p - i + l) % l];
                }
                return ret;
            }
        }
    };
    //// @property
    //// def tail(self)->T:
    ////     """ バッファの末尾 もっとも新しい要素"""
    ////     length=len(self._buf)
    ////     return self._buf[(self._p-1+length)%length]
    //template <typename T> T& RingBuffer<T>::GetTail()const {
    //    // """ バッファの末尾 もっとも新しい要素"""
    //    auto length=this->_buf_len;
    //    return this->_buf[(this->_p-1+length)%length];
    //};
    //// @property
    //// def top(self)->T:
    ////     """ バッファの先頭 最も古い要素"""
    ////     return self._buf[self._p]
    //template <typename T> T& RingBuffer<T>::GetTop()const{
    //    // """ バッファの先頭 最も古い要素"""
    //    return this->_buf[this->_p];
    //};

    //// def __getitem__(self,s)->List[T]:
    ////     """ 通常のリストにして返します。
    ////         必要に応じて再配置します。再配置せずに取り出す場合はsublistを使用します。
    ////     """
    ////     b=self._buf
    ////     if self._p!=0:
    ////         self._buf= b[self._p:]+b[:self._p]
    ////     self._p=0
    ////     return self._buf[s]
    //template <typename T> T RingBuffer<T>::operator[](int s)
    //{
    //    auto b=this->_buf;
    //    if(this->_p!=0){
    //        auto l=b.Length;
    //        auto b2=new T[l];
    //        for(auto i=0;i<l;i++){
    //            b2[i]=b[(i+this->_p)%l];
    //        }
    //        this->_buf=b2;
    //    }
    //    this->_p=0;
    //    return this->_buf[s];
    //};

    //// def __len__(self)->int:
    ////     return len(self._buf)
    //template <typename T> int RingBuffer<T>::GetLength()const
    //{
    //    return this->_buf.Length;
    //};
    //// def __repr__(self)->str:
    ////     return str(self._buf)
    //template <typename T> std::string RingBuffer<T>::__repr__()const
    //{
    //    return "this->_buf";
    //};
    //

}
//TBSKmodemCPP::RingBuffer<int> a();