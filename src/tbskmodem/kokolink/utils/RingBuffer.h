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
    public:
        RingBuffer(size_t length, const T pad) :
            _buf{ vector<T>() }
        {
            for (int i = 0;i < length;i++) {
                this->_buf.push_back(pad);
            }
            this->_p = 0;
        }
        ~RingBuffer() {
        }
    public:

        T Append(T v) {
            auto& b = this->_buf;
            const auto length = b.size();
            auto ret = b[this->_p];
            b[this->_p] = v;
            this->_p = (this->_p + 1) % length;
            return ret;

        }
        void Extend(const T* buf, int len) {
            for (int i = 0;i < len;i++) {
                this->Append(*(buf + i));
            }
        }
        ///** リストの一部を切り取って返します。
        //    この関数はバッファの再配置を行いません。
        //*/    
        const unique_ptr<vector<T>> Sublist(int pos, int size)const
        {
            auto& buf = this->_buf;
            const auto l = this->_buf.size();
            if (pos >= 0) {
                const auto p = this->_p + pos;
                if (size >= 0) {
                    TBSK_ASSERT(pos + size <= l);
                    auto ret = make_unique<vector<T>>(size);
                    auto ret_ptr = ret.get();
                    
                    for (auto i = 0;i < size;i++) {
                        ret_ptr->at(i) = buf[(p + i) % l];
                    }
                    return ret;
                }
                else {
                    TBSK_ASSERT(pos + size + 1 >= 0);
                    auto ret = make_unique<vector<T>>(-size);
                    auto ret_ptr = ret.get();
                    // return tuple([self._buf[(p+size+i+1)%l] for i in range(-size)])
                    for (auto i = 0;i < -size;i++) {
                        ret_ptr->at(i) = buf[(p + size + i + 1) % l];
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
                    auto ret_ptr = ret.get();
                    for (auto i = 0;i < size;i++) {
                        ret_ptr->at(i) = buf[(p + i) % l];
                    }
                    return ret;
                }
                else {
                    TBSK_ASSERT(l + pos + size + 1 >= 0);
                    // return tuple([self._buf[(p-i+l)%l] for i in range(-size)])
                    auto ret = make_unique<vector<T>>(size);
                    auto ret_ptr = ret.get();
                    for (auto i = 0;i < -size;i++) {
                        ret_ptr->at(i) = buf[(p - i + l) % l];
                    }
                    return ret;
                }
            }
        }
        T GetTail()const {
            auto length = this->_buf.size();
            return this->_buf[(this->_p - 1 + length) % length];

        }
        T GetTop()const {
            return this->_buf[this->_p];
        }
        size_t GetLength()const {
            return this->_buf.size();
        }

    };
}
