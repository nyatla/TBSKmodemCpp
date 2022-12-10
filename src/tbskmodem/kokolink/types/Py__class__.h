
#pragma once
#include "./Py__interface__.h"
#include "../NoneCopyConstructor_class.h"
#include <exception>
#include <vector>
#include <memory>
#include <queue>

namespace TBSKmodemCPP
{
    using std::exception;
    using std::shared_ptr;
    using std::unique_ptr;
    using std::vector;
    using std::make_shared;
    using std::make_unique;
    using std::move;
}


namespace TBSKmodemCPP
{
    class PyStopIteration : public exception
    {
    public:
        PyStopIteration();
        PyStopIteration(exception const& innerException);
    };
}

namespace TBSKmodemCPP
{

    template <typename T> class PyIterator :public NoneCopyConstructor_class, public virtual IPyIterator<T>
    {
    private:
        class VectorWrapper :public NoneCopyConstructor_class
        {
        public:
            const vector<T>* _buf;
        public:
            VectorWrapper(const vector<T>* src) :_buf{ src } {}
        };
        class PtrWrapperCU :public VectorWrapper
        {
        private:
            const unique_ptr<const vector<T>>& _src;
        public:
            PtrWrapperCU(const unique_ptr< const vector<T>>& src) :_src{ src }, VectorWrapper(src.get()) {}
        };
        class PtrWrapperCS :public VectorWrapper
        {
        private:
            shared_ptr<const vector<T>> _src;
        public:
            PtrWrapperCS(const shared_ptr<const vector<T>>& src) :_src{ src }, VectorWrapper(src.get()) {}
        };

    private:
        size_t _ptr = 0;
        const unique_ptr<VectorWrapper> _src;
    public:
        PyIterator(const vector<T>* src) :
            _src{ make_unique<VectorWrapper>(src) } {}
        PyIterator(const unique_ptr<const vector<T>>&& src) :
            _src{make_unique<PtrWrapperCU>(src) } {};//参照
        PyIterator(const shared_ptr<const vector<T>>&& src) :
            _src{ make_unique<PtrWrapperCS>(src) } {};//共有
        PyIterator(const vector<T>& src) :
            _src{ make_unique<VectorWrapper>(&src) } {};//参照
        T Next()override
        {
            if (this->_ptr >= this->_src->_buf->size()) {
                throw PyStopIteration();
            }
            auto r = this->_src->_buf->at(this->_ptr);
            this->_ptr++;
            return r;
        }
    };


}


namespace TBSKmodemCPP
{
    // IPyIteratorを連結するイテレータ
    template <typename T> class IterChain : public NoneCopyConstructor_class,public virtual IPyIterator<T>
    {
    private:
        std::deque<shared_ptr<IPyIterator<T>>> _src;
        //IPyIterator<IPyIterator<T>*>* _src;
        // IPyIterator<T>? _current;
        shared_ptr<IPyIterator<T>> _current;
    public:
        IterChain(const vector<shared_ptr<IPyIterator<T>>>& src){
            for (auto i = 0;i < src.size();i++) {
                this->_src.push_back(src[i]);
            }
            this->_current.reset();
        }
        virtual ~IterChain() {};
        T Next()override
        {
            while (true)
            {
                if (!this->_current)
                {
                    if (!this->_src.empty()) {
                        this->_current = this->_src.front();
                        this->_src.pop_front();
                    }
                    else {
                        throw PyStopIteration();
                    }
                }
                try
                {
                    return this->_current->Next();
                }
                catch (PyStopIteration)
                {   //値取得で失敗したらイテレーションの差し替え。
                    this->_current.reset();
                    continue;
                }

            }

        }
    };



}

namespace TBSKmodemCPP
{
    //  定数個の値を返すイテレータ
    template <typename T> class  Repeater : public NoneCopyConstructor_class,public virtual IPyIterator<T>
    {
    private:
        T _v;
        int _count;
    public:
        Repeater(T v, int count)
        {
            this->_v = v;
            this->_count = count;
        };
    public:
        T Next()override
        {
            if (this->_count == 0)
            {
                throw PyStopIteration();
            }
            this->_count--;
            return this->_v;
        }
    };
}


namespace TBSKmodemCPP
{

    template <typename T> class BasicIterator : public NoneCopyConstructor_class,public virtual IPyIterator<T> {
        // public:
        //     T Next();
    };
}
