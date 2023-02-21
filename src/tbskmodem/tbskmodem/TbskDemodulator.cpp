#include "../kokolink/filter/BitsWidthFilter.h"
#include "TbskDemodulator.h"
#include "TbskTone.h"
#include "TbskPreamble.h"
#include <string.h>




namespace TBSKmodemCPP
{
    using std::make_shared;
    using std::dynamic_pointer_cast;

    TbskDemodulator::TbskDemodulator(const shared_ptr<const TraitTone>& tone, double preamble_th, int preamble_cycle):
        TbskDemodulator(tone, make_shared<CoffPreamble>(tone,preamble_th,preamble_cycle))
    {
    }


    TbskDemodulator::TbskDemodulator(const shared_ptr<const TraitTone>& tone, const shared_ptr<Preamble>& preamble) :TbskDemodulator_impl(tone, preamble) {
    }


    shared_ptr<IPyIterator<int>> TbskDemodulator::DemodulateAsBit(const shared_ptr<IPyIterator<double>>& src)
    {
        TBSK_ASSERT(!this->_asmethod_lock);
        return TbskDemodulator_impl::DemodulateAsBit(src);
    }
    

    class AsyncDemodulateAsInt :public virtual AsyncMethod<shared_ptr<IPyIterator<int>>>
    {
    private:
        const shared_ptr<AsyncDemodulateX> _inst;
        int _bitwidth;
    public:
        AsyncDemodulateAsInt(const shared_ptr<AsyncDemodulateX>& inst, int bitwidth) :_inst(inst), _bitwidth(bitwidth) {}
        shared_ptr<IPyIterator<int>> GetResult()override{
            auto bwf=make_shared<BitsWidthFilter>(1, this->_bitwidth);
            bwf->SetInput(this->_inst->GetResult());
            return bwf;
        };
        void Close()override {
            this->_inst->Close();
        };
        bool Run()override {
            return this->_inst->Run();
        };
    };

    //    """ TBSK信号からnビットのint値配列を復元します。
    //        関数は信号を検知する迄制御を返しません。信号を検知せずにストリームが終了した場合はemptyのshared_ptrを返します。
    //    """
    shared_ptr<IPyIterator<int>> TbskDemodulator::DemodulateAsInt(const shared_ptr<IPyIterator<double>>& src, int bitwidth)
    {
        TBSK_ASSERT(!this->_asmethod_lock);
        try {
            auto r=TbskDemodulator_impl::DemodulateAsBit(src);
            if (r) {
                return make_shared<BitsWidthConvertIterator>(r, 1, bitwidth);
            }
            else {
                return shared_ptr<IPyIterator<int>>();
            }
        }
        catch (RecoverableException<AsyncDemodulateX>& e) {
            auto a = make_shared<AsyncDemodulateAsInt>(e.Detach(), bitwidth);
            throw RecoverableException<AsyncDemodulateAsInt>(a);
        }
    }




#pragma warning(disable : 4250 )
    class CharIterator :public IPyIterator<char>
    {
    private:
        unique_ptr<BitsWidthConvertIterator> _iter;
    public:
        CharIterator(const shared_ptr<IPyIterator<int>>& src){
            this->_iter = make_unique<BitsWidthConvertIterator>(src, 1, 8);
        }
        char Next()override
        {
            if (this->_iter == NULL) {
                throw PyStopIteration();
            }
            auto r = this->_iter->Next();
            return (char)r;
        }
    };
#pragma warning(default : 4250 )



    class AsyncDemodulateAsChar :public virtual AsyncMethod<shared_ptr<IPyIterator<char>>>
    {
    private:
        const shared_ptr<AsyncDemodulateX> _inst;
    public:
        AsyncDemodulateAsChar(const shared_ptr<AsyncDemodulateX>& inst) :_inst(inst){}
        shared_ptr<IPyIterator<char>> GetResult()override {
            return make_shared<CharIterator>(this->_inst->GetResult());
        };
        void Close()override {
            this->_inst->Close();
        };
        bool Run()override {
            return this->_inst->Run();
        };
    };

    shared_ptr<IPyIterator<char>> TbskDemodulator::DemodulateAsChar(const shared_ptr<IPyIterator<double>>& src)
    {
        TBSK_ASSERT(!this->_asmethod_lock);
        try {
            auto r = TbskDemodulator_impl::DemodulateAsBit(src);
            if (r) {
                return make_shared<CharIterator>(r);
            }
            else {
                return shared_ptr<IPyIterator<char>>();
            }
        }
        catch (RecoverableException<AsyncDemodulateX>& e) {
            auto a = make_shared<AsyncDemodulateAsChar>(e.Detach());
            throw RecoverableException<AsyncDemodulateAsChar>(a);

        }
    }
    


};


