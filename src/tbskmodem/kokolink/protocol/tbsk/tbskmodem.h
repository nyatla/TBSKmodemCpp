#pragma once
#include "./preamble/Preamble.h"
#include "./toneblock.h"
#include "../../interfaces.h"
#include "../../streams/rostreams.h"
#include "../../compatibility.h"
#include "./traitblockcoder.h"
#include "./preamble/CoffPreamble.h"

#include <memory>
#include <vector>



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
            const std::shared_ptr<IRoStream<int>> _src;
            int _last_bit;
            bool _is_eos;
            int _pos;
        public:
            DiffBitEncoder(int firstbit, const std::shared_ptr<IRoStream<int>>& src) :BasicRoStream(),
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
            unique_ptr<std::vector<int>> Gets(int maxsize, bool fillup)override {
                return BasicRoStream<int>::Gets(maxsize, fillup);
            }
            void Seek(int size)override {
                return BasicRoStream<int>::Seek(size);
            }
        };
#pragma warning( default : 4250 )
    protected:
        const std::shared_ptr<const TraitTone> _tone;
        const std::shared_ptr<const Preamble> _preamble;
        const std::shared_ptr<TraitBlockEncoder> _enc;

    public:
        /**
        * suffixはemptyを設定することでnullと同じになります。
        */
        TbskModulator_impl(const std::shared_ptr<const TraitTone>& tone, const std::shared_ptr<const Preamble>& preamble);
        virtual ~TbskModulator_impl();
    public:
        shared_ptr<IPyIterator<double>> ModulateAsBit(const std::shared_ptr<IRoStream<int>>& src, const std::shared_ptr<IPyIterator<double>>& suffix, bool suffix_pad);
    };
}

namespace TBSKmodemCPP
{
    class TbskDemodulator_impl;

    class AsyncDemodulateX :public virtual AsyncMethod<std::shared_ptr<TraitBlockDecoder>>
    {
    private:
        TbskDemodulator_impl& _parent;
        const int _tone_ticks;
        std::shared_ptr<RoStream<double>> _stream;
        bool _closed;
        NullableResult<TBSK_INT64> _peak_offset;
        int _co_step;
        std::shared_ptr<TraitBlockDecoder> _result;
        std::shared_ptr<WaitForSymbolAS> _wsrex;
    public:
        AsyncDemodulateX(TbskDemodulator_impl& parent, const std::shared_ptr<IPyIterator<double>>& src);
        virtual ~AsyncDemodulateX();
        std::shared_ptr<TraitBlockDecoder> GetResult() override;
        void Close()override;
        bool Run()override;
    };

    class TbskDemodulator_impl
    {
    protected:

        friend AsyncDemodulateX;
        const std::shared_ptr<const TraitTone> _tone;
        const std::shared_ptr<Preamble> _pa_detector;
        bool _asmethod_lock;
        //""" TBSK信号からビットを復元します。
        //    関数は信号を検知する迄制御を返しません。信号を検知せずにストリームが終了した場合はNoneを返します。
        //"""
    public:
        TbskDemodulator_impl(const std::shared_ptr<const TraitTone>& tone, const std::shared_ptr<Preamble>& preamble);

    public:
        shared_ptr<IPyIterator<int>> DemodulateAsBit(const std::shared_ptr<IPyIterator<double>>& src);
    };





}