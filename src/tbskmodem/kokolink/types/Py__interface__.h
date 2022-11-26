
#pragma once
#include <exception>
#include <vector>
#include <memory>

namespace TBSKmodemCPP
{
    /*
    IIteratorはPythonのIteratorのエミュレーションインタフェイスです。
    */
    template <typename T> class IPyIterator
    {
    public:
        virtual T Next() = 0;
    };
}



