#include "Bits2StrFilter.h"
namespace TBSKmodemCPP
{
    using namespace std;

    Bits2StrFilter::Bits2StrFilter(int input_bits, const char* encoding) :BasicRoStream(),
        _encoding{ encoding },
        _input_bits{ input_bits },
        _savedata{ vector<TBSK_BYTE>() },
        _iter{move(unique_ptr<BitsWidthConvertIterator>())},
        _pos{0}
    {
        TBSK_ASSERT(encoding == "ascii");
    }
    Bits2StrFilter::~Bits2StrFilter() {
    }
    Bits2StrFilter& Bits2StrFilter::SetInput(shared_ptr<IRoStream<int>>&& src)
    {
        this->_pos = 0;
        this->_savedata.clear();
        this->_iter = move(make_unique<BitsWidthConvertIterator>(move(src), this->_input_bits, 8));
        return *this;
    }
    char Bits2StrFilter::Next()
    {
        if (this->_iter == NULL)
        {
            throw PyStopIteration();
        }
        while (true)
        {
            int d;
            try
            {
                d = this->_iter->Next();
            }
            catch (RecoverableStopIteration e)
            {
                throw e;
            }
            this->_savedata.push_back((TBSK_BYTE)d);
            //try
            //{
            //    var r = System.Text.Encoding.GetEncoding(this._encoding,new EncoderExceptionFallback(),new DecoderExceptionFallback()).GetChars(this._savedata.ToArray());
            //    this._savedata.Clear();
            //    return r[0];
            //}
            //catch (DecoderFallbackException)
            //{
            //    continue;
            //}
            return this->_savedata.front();
        }
    }
    TBSK_INT64 Bits2StrFilter::GetPos()const
    {
        return this->_pos;
    }


}