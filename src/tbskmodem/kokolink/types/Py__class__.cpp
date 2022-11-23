
#include "Py__class__.h"
#include "types.h"
#include "../compatibility.h"

namespace TBSKmodemCPP
{
    using namespace std;

    PyStopIteration::PyStopIteration() :exception()
    {

    }
    PyStopIteration::PyStopIteration(exception const& innerException):exception(innerException)
    {

    }
    template <typename T> IterChain<T>::IterChain(shared_ptr<vector<shared_ptr<IPyIterator<T>>>> &src)
    {
        this->_src = src;
        this->_current = NULL;
    }
    template <typename T> IterChain<T>::~IterChain()
    {
    }
    template <typename T> T IterChain<T>::Next()
    {
        while (true)
        {
            if (this->_current == NULL)
            {
                try
                {
                    this->_current = this->_src->Next();
                }
                catch (PyStopIteration& e)
                {   //イテレーション配列の終端なら終了。
                    throw e;
                }
            }
            try
            {
                return this->_current->Next();
            }
            catch (PyStopIteration& e)
            {   //値取得で失敗したらイテレーションの差し替え。
                continue;
            }

        }

    }



    template <typename T> Repeater<T>::Repeater(T v, int count)
    {
        this->_v = v;
        this->_count = count;
    }
    template <typename T> T Repeater<T>::Next()
    {
        if (this->_count == 0)
        {
            throw PyStopIteration();
        }
        this->_count--;
        return this->_v;
    }
    //  定数個の値を返すイテレータ

}