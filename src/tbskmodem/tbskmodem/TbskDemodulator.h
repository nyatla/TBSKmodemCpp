#pragma once
#include "../kokolink/protocol/tbsk/tbskmodem.h"
#include <memory>




namespace TBSKmodemCPP
{
    class AsyncDemodulateAsInt :public virtual AsyncMethod<shared_ptr<IPyIterator<int>>>
    {
    private:
        const shared_ptr<AsyncDemodulateX> _inst;
        int _bitwidth;
    public:
        AsyncDemodulateAsInt(const shared_ptr<AsyncDemodulateX>& inst, int bitwidth);
        shared_ptr<IPyIterator<int>> GetResult()override;
        void Close()override;
        bool Run()override;
    };
    
    class AsyncDemodulateAsChar :public virtual AsyncMethod<shared_ptr<IPyIterator<char>>>
    {
    private:
        const shared_ptr<AsyncDemodulateX> _inst;
    public:
        AsyncDemodulateAsChar(const shared_ptr<AsyncDemodulateX>& inst);
        shared_ptr<IPyIterator<char>> GetResult()override;
        void Close()override;
        bool Run()override;
    };


    class TbskDemodulator:public TbskDemodulator_impl
    {

    public:
        TbskDemodulator(const std::shared_ptr<const TraitTone>& tone, double preamble_th=CoffPreamble::DEFAULT_TH, int preamble_cycle=CoffPreamble::DEFAULT_CYCLE);
        TbskDemodulator(const std::shared_ptr<const TraitTone>& tone, const std::shared_ptr<Preamble>& preamble);

    public:
        std::shared_ptr<IPyIterator<int>> DemodulateAsBit(const std::shared_ptr<IPyIterator<double>>& src);



        //    """ TBSK信号からnビットのint値配列を復元します。
        //        関数は信号を検知する迄制御を返しません。信号を検知せずにストリームが終了した場合は空のシェアドポインタを返します。
        //    """
        std::shared_ptr<IPyIterator<int>> DemodulateAsInt(const std::shared_ptr<IPyIterator<double>>& src, int bitwidth = 8);
        std::shared_ptr<IPyIterator<char>> DemodulateAsChar(const std::shared_ptr<IPyIterator<double>>& src);
    };


}
