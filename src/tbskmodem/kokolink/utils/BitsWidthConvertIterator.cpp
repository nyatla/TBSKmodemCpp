#include "./BitsWidthConvertIterator.h"
namespace TBSKmodemCPP
{


    BitsWidthConvertIterator::BitsWidthConvertIterator(shared_ptr<IPyIterator<int>>&& src, int input_bits, int output_bits) :
        _src{ src },
        _input_bits{ input_bits },
        _output_bits{ output_bits }
    {
        this->_is_eos = false;
        this->_bits = 0;//#byte値
        this->_n_bits = 0;//#読み出し可能ビット数
    }
    BitsWidthConvertIterator::~BitsWidthConvertIterator() {
    }

    int BitsWidthConvertIterator::Next()
    {
        if (this->_is_eos) {
            throw PyStopIteration();
        }
        auto n_bits = this->_n_bits;
        auto bits = this->_bits;
        while (n_bits < this->_output_bits) {
            TBSK_UINT32 d;
            try {
                d = (unsigned)this->_src->Next();
            }
            catch (RecoverableStopIteration e) {
                this->_bits = bits;
                this->_n_bits = n_bits;
                throw e;
            }
            catch (PyStopIteration e) {
                this->_is_eos = true;
                if (n_bits != 0) {
                    // # print("Fraction")
                    throw StopIteration_BitsWidthConvertIterator_FractionalBitsLeft(e);
                }
                throw PyStopIteration(e);
            }
            bits = (bits << this->_input_bits) | d;
            n_bits = n_bits + this->_input_bits;
        }
        TBSK_UINT32 r = 0;
        for (auto i = 0;i < this->_output_bits;i++) {
            r = (r << 1) | ((bits >> (n_bits - 1)) & 0x01);
            n_bits = n_bits - 1;
        }
        this->_n_bits = n_bits;
        this->_bits = bits;
        return (int)r;
    }


}
