#pragma once
#include "../types/types.h"
#include "../compatibility.h"
#include <vector>
#include <string>
#include <memory>
namespace TBSKmodemCPP
{



    template <typename T> class RingBuffer :public NoneCopyConstructor_class
    {
    private:
        std::vector<T> _buf;
        int _p;
    private:
        class SubIterator :public IPyIterator<T>
        {
        private:
            int _size;
            int _pos;
            const std::vector<T>& _buf;

        public:
            SubIterator(const std::vector<T>& buf, int pos, int size) :_pos{ pos }, _size{ size }, _buf{ buf } {
                assert(size < buf.size());
            }
            T Next()override
            {
                if (this->_size < 1) {
                    throw PyStopIteration();
                }
                this->_size = this->_size - 1;
                int p = this->_pos;
                this->_pos = (this->_pos + 1) % this->_buf.size();
                return this->_buf.at(p);
            }
        };

    public:
        RingBuffer(size_t length, const T pad)
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

        /// <summary>
        /// この関数は、posからsizeの要素を参照するイテレータを返します。
        /// イテレータの生存期間はこのインスタンスと同じです。
        /// バッファ操作を行う関数を使用すると、イテレータの返り値は影響を受けます。
        /// </summary>
        /// <param name="pos"></param>
        /// <param name="size"></param>
        /// <returns></returns>
        std::unique_ptr<IPyIterator<T>> SubIter(int pos, int size)const
        {
            return std::make_unique<SubIterator>(this->_buf, (pos+this->_p)%((int)this->_buf.size()), size);
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
