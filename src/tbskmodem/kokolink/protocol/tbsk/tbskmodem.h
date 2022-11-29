#pragma once
#include "tbskmodem.h"
#include "./preamble/Preamble.h"
#include "./toneblock.h"
#include "../../interfaces.h"
#include "../../streams/rostreams.h"
#include "../../compatibility.h"
#include "./traitblockcoder.h"


#include <math.h>
#include <memory>





namespace TBSKmodemCPP
{
    using std::make_shared;
    using std::max;
}
namespace TBSKmodemCPP
{

    // """ TBSKの変調クラスです。
    //     プリアンブルを前置した後にビットパターンを置きます。
    // """
    class TbskModulator
    {
    private:
        const shared_ptr<const TraitTone> _tone;
        const shared_ptr<const Preamble> _preamble;

        const unique_ptr<TraitBlockEncoder> _enc;
    public:
        TbskModulator(const shared_ptr<const TraitTone>& tone);

        TbskModulator(const shared_ptr<const TraitTone>& tone, const shared_ptr<const Preamble>& preamble);
        virtual ~TbskModulator();
    private:
    public:
        unique_ptr<IPyIterator<double>> ModulateAsBit();

        unique_ptr<IPyIterator<double>> ModulateAsBit(const shared_ptr<IRoStream<int>>&& src);
        unique_ptr<IPyIterator<double>> ModulateAsBit(const shared_ptr<IPyIterator<int>>& src);
        unique_ptr<IPyIterator<double>> Modulate(const shared_ptr<IPyIterator<int>>&& src, int bitwidth = 8);
        unique_ptr<IPyIterator<double>> Modulate(const char* src, int length = -1);
    };
}

namespace TBSKmodemCPP
{
    template <typename T> class AsyncDemodulateX;
    class TbskDemodulator
    {
        // """ nBit intイテレータから1バイト単位のhex stringを返すフィルタです。
        // """
    private:
        friend AsyncDemodulateX<int>;
        friend AsyncDemodulateX<char>;//美しくない実装
        const TraitTone& _tone;
        Preamble& _pa_detector;
        bool _asmethod_lock;

        TbskDemodulator(const TraitTone& tone, Preamble& preamble);




    //""" TBSK信号からビットを復元します。
    //    関数は信号を検知する迄制御を返しません。信号を検知せずにストリームが終了した場合はNoneを返します。
    //"""
    private:


    public:
        unique_ptr<IPyIterator<int>> DemodulateAsBit(const shared_ptr<IPyIterator<double>>& src);



        //    """ TBSK信号からnビットのint値配列を復元します。
        //        関数は信号を検知する迄制御を返しません。信号を検知せずにストリームが終了した場合はNoneを返します。
        //    """
        shared_ptr<IPyIterator<int>> DemodulateAsInt(const shared_ptr<IPyIterator<double>>& src, int bitwidth = 8);

        unique_ptr<IPyIterator<char>> DemodulateAsChar(const shared_ptr<IPyIterator<double>>& src);

    };


}
