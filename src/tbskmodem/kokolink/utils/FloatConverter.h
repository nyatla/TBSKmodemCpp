#pragma once
#include "../types/types.h"

namespace TBSKmodemCPP
{

    /**
     * 正規化された浮動小数点値と固定小数点値を相互変換します。
     */
    class FloatConverter
    {
    public:
        static double ByteToDouble(TBSK_BYTE b);
        static double Int16ToDouble(TBSK_INT16 b);
        static TBSK_BYTE DoubleToByte(double b);
        static TBSK_INT16 DoubleToInt16(double b);
    };
}



