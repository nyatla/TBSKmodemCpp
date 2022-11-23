
#pragma once
#include <exception>
#include <vector>
#include <memory>
#include "Py__special_functions__.h"

namespace TBSKmodemCPP
{
    class PyStopIteration : public std::exception
    {
    public:
        PyStopIteration();
        PyStopIteration(exception const& innerException);
    };
}

namespace TBSKmodemCPP
{
    /*
    IIteratorはPythonのIteratorのエミュレーションインタフェイスです。
    */
    template <typename T> class IPyIterator :public virtual Py__next__<T> {

    };
    template <typename T> class  IPyIterator_sptr :public std::shared_ptr<IPyIterator<T>>
    {
    };
}


namespace TBSKmodemCPP
{
    using namespace std;
    // IPyIteratorを連結するイテレータ
    template <typename T> class IterChain : public virtual IPyIterator<T>
    {
    private:
        shared_ptr<vector<shared_ptr<IPyIterator<T>>>> _src;
        //IPyIterator<IPyIterator<T>*>* _src;
        // IPyIterator<T>? _current;
        IPyIterator<T>* _current;
    public:
        IterChain(shared_ptr<vector<shared_ptr<IPyIterator<T>>>>& src);
        virtual ~IterChain();
        T Next()override;
    };
}

namespace TBSKmodemCPP
{
    //  定数個の値を返すイテレータ
    template <typename T> class  Repeater:public virtual IPyIterator<T>
    {
    private:
        T _v;
        int _count;
    public:
        Repeater(T v, int count);
    public:
        T Next() override;
    };

    template <typename T> class  Repeater_sptr :public std::shared_ptr<Repeater<T>>, public virtual IPyIterator_sptr<T>
    {
    public:
        Repeater_sptr(T v, int count) :std::shared_ptr<Repeater<T>>(new Repeater<T>(v, count)) {}
    };





    template <typename T> class BasicIterator:public virtual IPyIterator<T>{
        // public:
        //     T Next();
    };
}