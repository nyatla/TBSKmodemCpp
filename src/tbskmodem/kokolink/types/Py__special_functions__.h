#pragma once
#include <string>

namespace TBSKmodemCPP
{
    template <typename T> class Py__getitem__{
    public:
        // __getitem__関数は this[int i]にマップします。
        virtual T operator[](int i)const =0;
    };
    class Py__len__{
    public:
        // __len__関数は Lengthにマップします。
        virtual int GetLength()const =0;
    };
    class Py__repr__{
    public:
        virtual std::string __repr__()const=0;
    };
    class Py__str__{
    public:
        // __str__関数は ToStringにマップします。
        //string? ToString{get;}
    };
    template <typename T> class Py__next__{
    public:
        // __next__関数は Nextにマップします。
        virtual T Next()=0;
    };      
}