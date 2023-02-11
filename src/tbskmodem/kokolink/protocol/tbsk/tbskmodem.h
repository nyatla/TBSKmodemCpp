#pragma once
#include "./preamble/Preamble.h"
#include "./toneblock.h"
#include "../../interfaces.h"
#include "../../streams/rostreams.h"
#include "../../compatibility.h"
#include "./traitblockcoder.h"
#include "./preamble/CoffPreamble.h"

#include <memory>





namespace TBSKmodemCPP
{

    // """ TBSKの変調クラスです。
    //     プリアンブルを前置した後にビットパターンを置きます。
    // """
    class TbskModulator_impl
    {
    protected:
#pragma warning( disable : 4250 )
        class DiffBitEncoder : public BasicRoStream<int>, virtual public IBitStream
        {
        private:
            const shared_ptr<IRoStream<int>> _src;
            int _last_bit;
            bool _is_eos;
            int _pos;
        public:
            DiffBitEncoder(int firstbit, const shared_ptr<IRoStream<int>>& src) :BasicRoStream(),
                _src{ src }
            {
                this->_last_bit = firstbit;
                this->_is_eos = false;
                this->_pos = 0;
            }
            int Next()override
            {
                if (this->_is_eos)
                {
                    throw PyStopIteration();
                }
                if (this->_pos == 0)
                {
                    this->_pos = this->_pos + 1;
                    return this->_last_bit; //#1st基準シンボル
                }
                int n;
                try
                {
                    n = this->_src->Next();
                }
                catch (PyStopIteration&)
                {
                    this->_is_eos = true;
                    throw;
                }
                if (n == 1)
                {
                    //pass
                }
                else
                {
                    this->_last_bit = (this->_last_bit + 1) % 2;
                }
                return this->_last_bit;
            }
            // @property
        public:
            TBSK_INT64 GetPos()const override
            {
                return  this->_pos;
            }
            unique_ptr<vector<int>> Gets(int maxsize, bool fillup)override {
                return BasicRoStream<int>::Gets(maxsize, fillup);
            }
            void Seek(int size)override {
                return BasicRoStream<int>::Seek(size);
            }
        };
#pragma warning( default : 4250 )
    protected:
        const shared_ptr<const TraitTone> _tone;
        const shared_ptr<const Preamble> _preamble;
        const shared_ptr<TraitBlockEncoder> _enc;

        static shared_ptr<IBitStream> createDiffBitEncoder(int firstbit, const shared_ptr<IRoStream<int>>& src);
    public:
        TbskModulator_impl(const shared_ptr<const TraitTone>& tone, const shared_ptr<const Preamble>& preamble);
        virtual ~TbskModulator_impl();
    public:
        shared_ptr<IPyIterator<double>> ModulateAsBit(const shared_ptr<IRoStream<int>>& src);
    };
}

namespace TBSKmodemCPP
{
    class TbskDemodulator_impl;

    class AsyncDemodulateX :public virtual AsyncMethod<shared_ptr<TraitBlockDecoder>>
    {
    private:
        TbskDemodulator_impl& _parent;
        const int _tone_ticks;
        shared_ptr<RoStream<double>> _stream;
        bool _closed;
        NullableResult<TBSK_INT64> _peak_offset;
        int _co_step;
        shared_ptr<TraitBlockDecoder> _result;
        shared_ptr<WaitForSymbolAS> _wsrex;
    public:
        AsyncDemodulateX(TbskDemodulator_impl& parent, const shared_ptr<IPyIterator<double>>& src);
        virtual ~AsyncDemodulateX();
        shared_ptr<TraitBlockDecoder> GetResult() override;
        void Close()override;
        bool Run()override;
    };

    class TbskDemodulator_impl
    {
    protected:

        friend AsyncDemodulateX;
        const shared_ptr<const TraitTone> _tone;
        const shared_ptr<Preamble> _pa_detector;
        bool _asmethod_lock;
        //""" TBSK信号からビットを復元します。
        //    関数は信号を検知する迄制御を返しません。信号を検知せずにストリームが終了した場合はNoneを返します。
        //"""
    public:
        TbskDemodulator_impl(const shared_ptr<TraitTone>& tone, const shared_ptr<Preamble>& preamble);

    public:
        shared_ptr<IPyIterator<int>> DemodulateAsBit(const shared_ptr<IPyIterator<double>>& src);
    };

    /*
    class DemodulateAsBitAS :public AsyncDemodulateX<std::shared_ptr<IPyIterator<int>>> {
    public:
        DemodulateAsBitAS(TbskDemodulator_impl& parent, const std::shared_ptr<IPyIterator<double>>& src);
        std::shared_ptr<IPyIterator<int>> ConvertResult(std::shared_ptr<TraitBlockDecoder>& r)const override;
    };

    class DemodulateAsIntAS :public AsyncDemodulateX<std::shared_ptr<IPyIterator<int>>> {
    private:
        int _bitwidth;
    public:
        DemodulateAsIntAS(TbskDemodulator_impl& parent, const std::shared_ptr<IPyIterator<double>>& src, int bitwidth);
        std::shared_ptr<IPyIterator<int>> ConvertResult(std::shared_ptr<TraitBlockDecoder>& r)const override;
    };

    class DemodulateAsCharAS :public AsyncDemodulateX<std::shared_ptr<IPyIterator<char>>>
    {
    public:
        DemodulateAsCharAS(TbskDemodulator_impl& parent, const std::shared_ptr<IPyIterator<double>>& src);
        std::shared_ptr<IPyIterator<char>> ConvertResult(std::shared_ptr<TraitBlockDecoder>& r)const override;
    };
    */


}