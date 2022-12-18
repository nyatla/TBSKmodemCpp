#pragma once
#include "../../../src/tbskmodem/tbskmodem.h"
#include "./PointerHolder.h"
#include <memory>
#include <vector>


using namespace TBSKmodemCPP;


static PointerHolder _instances;

#ifdef EMSCRIPTEN_ON
#include <emscripten/bind.h>
#include <emscripten.h>
using namespace emscripten;
#else

#define EMSCRIPTEN_KEEPALIVE
#define EM_VAL void*
#define EM_JS(a,b,c,d)
#define EM_ASM(a)
#define EM_ASM_(a)
#define val int
#endif

template<typename T> struct PyIteratorWrapper {
    int last_status;
    T last_value;
    shared_ptr<IPyIterator<T>> ptr;
};

#ifdef __cplusplus
extern "C" {
#endif





    //ポインタテーブルのサイズを返す。
    int EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_PointerHolder_Size()
    {
        return _instances.Size();
    }

    

    void EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_Dispose(void* ptr)
    {
        _instances.Remove(ptr);
    }





    void* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TraitTone(const double* data, int length) {
        auto r = std::make_shared<TraitTone>();
        for (auto i = 0;i < length;i++) {
            r->push_back(*(data + i));
        }
        return _instances.Add(r);
    }
    void* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_SinTone(int points,int cycle) {
        auto r = std::make_shared<SinTone>(points,cycle);
        return _instances.Add(r);
    }
    void* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_XPskSinTone(int points,int cycle,int div) {
        auto r = std::make_shared<XPskSinTone>(points, cycle,div);
        return _instances.Add(r);
    }


    void* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_CoffPreamble(void* tone_ptr) {
        const shared_ptr<TraitTone>* tone = (const shared_ptr<TraitTone>*)(tone_ptr);
        auto r = std::make_shared<CoffPreamble>(*tone);
        return _instances.Add(r);
    }

    //
    //  TbskModulator
    //

    shared_ptr<TbskModulator>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TbskModulator(const shared_ptr<TraitTone>* tone_ptr, const shared_ptr<Preamble>* preamble_ptr)
    {
        //tone,preambleを生成

        auto r = std::make_shared<TbskModulator>(*tone_ptr, *preamble_ptr);
        return (shared_ptr<TbskModulator>*)_instances.Add(r);
    }




    shared_ptr<struct PyIteratorWrapper<double>>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TbskModulator_Modulate(shared_ptr<TbskModulator>* ptr, shared_ptr<IPyIterator<int>>* src_ptr) {
        shared_ptr<TbskModulator>& ref = *ptr;
        shared_ptr<IPyIterator<int>>&  src_ref= *src_ptr;
        
        auto r=make_shared<struct PyIteratorWrapper<double>>();
        r->ptr= ref->Modulate(std::move(src_ref), 0);
        r->last_value = 0;
        return (shared_ptr<struct PyIteratorWrapper<double>>*)_instances.Add(r);
    }


    shared_ptr<TbskDemodulator>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TbskDemodulator(const shared_ptr<TraitTone>* tone_ptr, const shared_ptr<Preamble>* preamble_ptr)
    {
        //tone,preambleを生成

        auto r = std::make_shared<TbskDemodulator>(*tone_ptr, *preamble_ptr);
        return (shared_ptr<TbskDemodulator>*)_instances.Add(r);
    }

    void wasm_tbskmodem_TbskDemodulator_DemodulateAsInt(shared_ptr<IPyIterator<double>>* src_ptr){
    }
    /*************************************************************
        wasm_tbskmodem_IPyIterator
     *************************************************************/

    //  戻り値はイテレータのステータス。
    //  0:
    //  1:
    //  2:
    //
    int EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_IPyIterator_double_next(shared_ptr<struct PyIteratorWrapper<double>>* ptr)
    {
        try {
            auto v = (*ptr)->ptr->Next();
            (*ptr)->last_value = v;
            return 1;
        }
        catch (RecoverableStopIteration) {
            (*ptr)->last_value = 0;
            return 2;   //一時中断
        }
        catch (PyStopIteration) {
            (*ptr)->last_value = 0;
            return 3;   //終端
        }
    }
    double EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_IPyIterator_double_value(shared_ptr<struct PyIteratorWrapper<double>>* ptr)
    {
        return (*ptr)->last_value;
    }


    int EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_IPyIterator_int_next(shared_ptr<struct PyIteratorWrapper<int>>* ptr)
    {
        try {
            auto v = (*ptr)->ptr->Next();
            (*ptr)->last_value = v;
            return 1;
        }
        catch (RecoverableStopIteration) {
            (*ptr)->last_value = 0;
            return 2;   //一時中断
        }
        catch (PyStopIteration) {
            (*ptr)->last_value = 0;
            return 3;   //終端
        }
    }

    int EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_IPyIterator_int_value(shared_ptr<struct PyIteratorWrapper<double>>* ptr)
    {
        return (*ptr)->last_value;
    }






    shared_ptr<PcmData>* wasm_tbskmodem_PcmData_1(const TBSK_BYTE* src, size_t size, int sample_bits, int frame_rate) {
        return (shared_ptr<PcmData>*)_instances.Add(make_shared<PcmData>(src, size, sample_bits, frame_rate));
    }



    int wasm_tbskmodem_PcmData_GetSampleBits(const shared_ptr<PcmData>* ptr)
    {
        return (*ptr)->GetSampleBits();
    };
    int wasm_tbskmodem_GetFramerate(const shared_ptr<PcmData>* ptr)
    {
        return (*ptr)->GetFramerate();
    }
    int wasm_tbskmodem_GetByteslen(const shared_ptr<PcmData>* ptr) {
        return (*ptr)->GetByteslen();
    }
    const int wasm_tbskmodem_DataAsFloat(const shared_ptr<PcmData>* ptr, double* buf, int buf_len_in_bytes) {
        auto& r = (*ptr)->DataAsFloat();
        auto tick_count = (*ptr)->GetByteslen() / ((*ptr)->GetSampleBits() / 8);
        if (buf_len_in_bytes < r->size() * sizeof(double) * tick_count) {
            return 0;
        }
        memcpy(buf, r.get(), r->size() * sizeof(double));
        return r->size();
    }


#ifdef __cplusplus
}
#endif

EM_JS(int, JsIntProvider_next_handler, (int handle), {
let r = _TBSKmodemJS._callback_handler(handle);
return r;
});



class JsIntProvider :public IPyIterator<int> {
private:
    int _handle=0;
    int count = 3;
public:
    JsIntProvider(){}
    void setHandle(int h) {
        this->_handle = h;
    }
    int Next(){
        int p1 = JsIntProvider_next_handler(this->_handle);
        if (this->count > 0) {
            return 1;
        }
        throw PyStopIteration();
    }
};


#ifdef __cplusplus
extern "C" {
#endif


    shared_ptr<JsIntProvider>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_JsIntProvider() {
        return (shared_ptr<JsIntProvider>*)_instances.Add(make_shared<JsIntProvider>());
    }
    void EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_JsIntProvider_setHandle(shared_ptr<JsIntProvider>* ptr,int handle) {
        (*ptr)->setHandle(handle);
    }

#ifdef __cplusplus
}
#endif