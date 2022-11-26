
#pragma once
#include "./Py__interface__.h"
#include "../compatibility.h"
#include <exception>
#include <vector>
#include <memory>

namespace TBSKmodemCPP
{
    using std::exception;
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
    // IPyIteratorを連結するイテレータ
    template <typename T> class IterChain : public NoneCopyConstructor_class,public virtual IPyIterator<T>
    {
    private:
        shared_ptr<vector<shared_ptr<IPyIterator<T>>>> _src;
        //IPyIterator<IPyIterator<T>*>* _src;
        // IPyIterator<T>? _current;
        IPyIterator<T>* _current;
    public:
        IterChain(const shared_ptr<vector<shared_ptr<IPyIterator<T>>>>& src);
        virtual ~IterChain();
        T Next()override;
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
        Repeater(T v, int count);
    public:
        T Next() override;
    };
}

namespace TBSKmodemCPP
{

    template <typename T> class BasicIterator : public NoneCopyConstructor_class,public virtual IPyIterator<T> {
        // public:
        //     T Next();
    };
}
