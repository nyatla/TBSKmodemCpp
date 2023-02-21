#include "./tbskmodem.h"
#include "../../utils/AsyncMethod.h"
#include "../../streams/BitStream.h"
#include "../../filter/BitsWidthFilter.h"

#include <string.h>
#include "../../cpp_debug.h"



namespace TBSKmodemCPP
{
    using std::shared_ptr;
    using std::make_shared;
    using std::max;

    TbskModulator_impl::TbskModulator_impl(const shared_ptr<const TraitTone>& tone, const shared_ptr<const Preamble>& preamble):
        _tone{ tone },
        _preamble{ preamble },
        _enc{ make_shared<TraitBlockEncoder>(tone) }
    {
    }
    TbskModulator_impl::~TbskModulator_impl() {
    }

    shared_ptr<IPyIterator<double>> TbskModulator_impl::ModulateAsBit(const shared_ptr<IRoStream<int>>& src,const shared_ptr<IPyIterator<double>>& suffix, bool suffix_pad)
    {
        auto ave_window_shift = max((int)(this->_tone->size() * 0.1), 2) / 2; //#検出用の平均フィルタは0.1*len(tone)//2だけずれてる。ここを直したらTraitBlockDecoderも直せ

        auto& tbe = this->_enc;
        tbe->SetInput(make_shared<DiffBitEncoder>(0, src));
        vector <shared_ptr<IPyIterator<double>>> d;
        d.push_back(this->_preamble->GetPreamble());
        d.push_back(tbe);

        if (suffix) {
            d.push_back(suffix);

        }
        if (suffix_pad) {
            d.push_back(make_shared<Repeater<double>>(0, ave_window_shift));
        }
        return make_shared<IterChain<double>>(d);
    }
}





namespace TBSKmodemCPP
{
#pragma warning( disable : 4250 )
    using std::shared_ptr;
    using std::make_shared;



    AsyncDemodulateX::AsyncDemodulateX(TbskDemodulator_impl& parent,const shared_ptr<IPyIterator<double>>& src) :
        AsyncMethod<shared_ptr<TraitBlockDecoder>>(),
        _parent{ parent },
        _tone_ticks{ (int)parent._tone->size() },
        _stream{ make_shared<RoStream<double>>(src) },
        _peak_offset{ NullableResult<TBSK_INT64>() }
        {
            //this->_wsrex = NULL;
            this->_co_step = 0;
            this->_closed = false;
            //this->_result = NULL;
        }
    AsyncDemodulateX::~AsyncDemodulateX() {
        this->Close();
    }
    shared_ptr<TraitBlockDecoder> AsyncDemodulateX::GetResult()
    {   //コピー不能値なのでここでは取り外す。
        TBSK_ASSERT(this->_co_step >= 4);
        return this->_result;

    }
    void AsyncDemodulateX::Close()
    {
        if (!this->_closed)
        {
            this->_wsrex.reset();
            this->_parent._asmethod_lock = false;
            this->_closed = true;

        }
    }
    bool AsyncDemodulateX::Run()
    {
        //# print("run",self._co_step)
        TBSK_ASSERT(!this->_closed);

        if (this->_co_step == 0)
        {
            try
            {
                this->_peak_offset = this->_parent._pa_detector->WaitForSymbol(this->_stream); //#現在地から同期ポイントまでの相対位置
                TBSK_ASSERT(this->_wsrex == NULL);
                this->_co_step = 2;
            }
            catch (RecoverableException<WaitForSymbolAS>& rexp)
            {
                this->_wsrex =  rexp.Detach();;//rexp.Detach();
                this->_co_step = 1;
                TBSK_ASSERT(this->_result == NULL);
                return false;
            }
        }

        if (this->_co_step == 1)
        {
            if (!this->_wsrex->Run()) {
                return false;
            }
            else {
                this->_peak_offset = this->_wsrex->GetResult();
                this->_wsrex.reset();
                this->_co_step = 2;
            }
        }
        if (this->_co_step == 2)
        {
            if (!this->_peak_offset.success)
            {
                TBSK_ASSERT(this->_result == NULL);
                this->Close();
                this->_co_step = 4;
                return true;
            }
            //# print(self._peak_offset)
            this->_co_step = 3;
        }
        if (this->_co_step == 3)
        {
            try
            {
                TBSK_ASSERT(this->_peak_offset.success);
                //# print(">>",self._peak_offset+self._stream.pos)
                this->_stream->Seek(this->_tone_ticks + (int)this->_peak_offset.value);// #同期シンボル末尾に移動
                //# print(">>",stream.pos)
                auto tbd = make_shared<TraitBlockDecoder>(this->_tone_ticks);
                tbd->SetInput(this->_stream);
                this->_stream.reset();
                this->_result = tbd;
                this->Close();
                this->_co_step = 4;
                return true;

            }
            catch (RecoverableStopIteration&)
            {
                return false;

            }
            catch (PyStopIteration&)
            {
                TBSK_ASSERT(this->_result == NULL);
                this->Close();
                this->_co_step = 4;
                return true;

            }
        }
        throw exception();

    }


    TbskDemodulator_impl::TbskDemodulator_impl(const shared_ptr<const TraitTone>& tone, const shared_ptr<Preamble>& preamble) :
        _pa_detector{ preamble },
        _tone{ tone }
    {
        this->_asmethod_lock = false;
    }



    shared_ptr<IPyIterator<int>> TbskDemodulator_impl::DemodulateAsBit(const shared_ptr<IPyIterator<double>>& src)
    {
        TBSK_ASSERT(!this->_asmethod_lock);
        auto asmethod = make_shared<AsyncDemodulateX>(*this,src);
        if (asmethod->Run())
        {   
            auto ret= asmethod->GetResult();
            return ret;
        }
        else
        {
            this->_asmethod_lock = true;// #解放はAsyncDemodulateXのcloseで
            throw RecoverableException<AsyncDemodulateX>(asmethod);
        }
    };
#pragma warning(default : 4250 )
};

////////////////////////////////////////////////////////////////////////////////
// フィルタのバリエーションを分離するのが難しかったのでこのまま
////////////////////////////////////////////////////////////////////////////////
/*

namespace TBSKmodemCPP
{
    using std::shared_ptr;

    DemodulateAsBitAS::DemodulateAsBitAS(TbskDemodulator_impl& parent, const shared_ptr<IPyIterator<double>>& src) :AsyncDemodulateX(parent, src) {};

    shared_ptr<IPyIterator<int>> DemodulateAsBitAS::ConvertResult(shared_ptr<TraitBlockDecoder>& r)const {
        return r;//そのまま返す。
    }

}

namespace TBSKmodemCPP
{

    DemodulateAsIntAS::DemodulateAsIntAS(TbskDemodulator_impl& parent, const shared_ptr<IPyIterator<double>>& src, int bitwidth) :AsyncDemodulateX(parent, src), _bitwidth(bitwidth) {};
    shared_ptr<IPyIterator<int>> DemodulateAsIntAS::ConvertResult(shared_ptr<TraitBlockDecoder>& r)const {
        auto bwf = make_shared<BitsWidthFilter>(1, this->_bitwidth);
        bwf->SetInput(r);
        return bwf;
    }
}
namespace TBSKmodemCPP
{
#pragma warning(disable : 4250 )
    class CharFilter :public virtual BasicRoStream<char>, public virtual IFilter<CharFilter, IRoStream<int>, char>
    {
    private:
        TBSK_INT64 _pos;
        unique_ptr<BitsWidthConvertIterator> _iter;
    public:
        CharFilter() :BasicRoStream() {
            this->_pos = 0;
        }
        virtual ~CharFilter() {
        }
        CharFilter& SetInput(const shared_ptr<IRoStream<int>>& src)override
        {
            this->_pos = 0;
            this->_iter = make_unique<BitsWidthConvertIterator>(src, 1, 8);
            return *this;
        }
        char Next()override
        {
            if (this->_iter == NULL) {
                throw PyStopIteration();
            }
            auto r = this->_iter->Next();
            this->_pos = this->_pos + 1;
            return (char)r;
        }
        // @property
        TBSK_INT64 GetPos()const override
        {
            return this->_pos;
        }
    };
#pragma warning(default : 4250 )

    DemodulateAsCharAS::DemodulateAsCharAS(TbskDemodulator_impl& parent, const shared_ptr<IPyIterator<double>>& src) :AsyncDemodulateX(parent, src) {};
    shared_ptr<IPyIterator<char>> DemodulateAsCharAS::ConvertResult(shared_ptr<TraitBlockDecoder>& r)const {
        auto bwf = make_shared<CharFilter>();
        bwf->SetInput(r);
        return bwf;
    }
}*/