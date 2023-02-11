#include "FloatConverter.h"
#include <climits>

namespace TBSKmodemCPP
{
    double FloatConverter::ByteToDouble(TBSK_BYTE b)
    {
        return (double)b / 255 - 0.5;
    }
    double FloatConverter::Int16ToDouble(TBSK_INT16 b)
    {
        if (b >= 0)
        {
            return ((double)b) / SHRT_MAX;
        }
        else
        {
            return -(((double)b) / SHRT_MIN);
        }
    }

    TBSK_BYTE FloatConverter::DoubleToByte(double b)
    {
        return (TBSK_BYTE)(b * 127 + 128);
    }
    TBSK_INT16 FloatConverter::DoubleToInt16(double b)
    {
        assert(1 >= b && b >= -1);
        if (b >= 0)
        {
            return (TBSK_INT16)(SHRT_MAX * b);
        }
        else
        {
            return (TBSK_INT16)(-SHRT_MIN * b);
        }
    }

}