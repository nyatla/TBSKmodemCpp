#include "RingBuffer.h"

namespace TBSKmodemCPP{





    //template <typename T> RingBuffer<T>::RingBuffer(size_t length, const T pad):
    //    _buf{vector<T>()}
    //{
    //    for (int i = 0;i < length;i++) {
    //        this->_buf.push_back(T);
    //    }
    //    this->_p = 0;
    //}
    //template <typename T> RingBuffer<T>::~RingBuffer() {
    //}

    //template <typename T> T RingBuffer<T>::Append(T v){
    //    const T* b = this->_buf.data();
    //    const auto length = this->_buf.size();
    //    auto ret=b[this->_p];
    //    b[this->_p]=v;
    //    this->_p=(this->_p+1)%length;
    //    return ret;

    //}
    //template <typename T> void RingBuffer<T>::Extend(const T* buf, int len){
    //    for (int i = 0;i < len;i++) {
    //        this->Append(*(buf + i));
    //    }
    //}
    /////** リストの一部を切り取って返します。
    ////    この関数はバッファの再配置を行いません。
    ////*/    
    //template <typename T> const unique_ptr<vector<T>> RingBuffer<T>::Sublist(int pos, int size)const {
    //    const auto l = this->_buf->size();
    //    auto buf = this->_buf->data();
    //    if (pos >= 0) {
    //        const auto p = this->_p + pos;
    //        if (size >= 0) {
    //            TBSK_ASSERT(pos + size <= l);
    //            auto ret = make_unique<vector<T>>(size);
    //            for (auto i = 0;i < size;i++) {
    //                ret[i] = buf[(p + i) % l];
    //            }
    //            return ret;
    //        }
    //        else {
    //            TBSK_ASSERT(pos + size + 1 >= 0);
    //            auto ret = make_unique<vector<T>>(- size);
    //            // return tuple([self._buf[(p+size+i+1)%l] for i in range(-size)])
    //            for (auto i = 0;i < -size;i++) {
    //                ret[i] = buf[(p + size + i + 1) % l];
    //            }
    //            return ret;
    //        }
    //    }
    //    else {
    //        auto p = this->_p + l + pos;
    //        if (size >= 0) {
    //            TBSK_ASSERT(l + pos + size <= l);
    //            // return tuple([self._buf[(p+i)%l] for i in range(size)])
    //            auto ret = make_unique<vector<T>>(size);
    //            for (auto i = 0;i < size;i++) {
    //                ret[i] = buf[(p + i) % l];
    //            }
    //            return ret;
    //        }
    //        else {
    //            TBSK_ASSERT(l + pos + size + 1 >= 0);
    //            // return tuple([self._buf[(p-i+l)%l] for i in range(-size)])
    //            auto ret = make_unique<vector<T>>(size);
    //            for (auto i = 0;i < -size;i++) {
    //                ret[i] = buf[(p - i + l) % l];
    //            }
    //            return ret;
    //        }
    //    }
    //};

}
//TBSKmodemCPP::RingBuffer<int> a();