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



//  JSからWASMに転送するためのIter. Recoverable選択可能。
//
template<typename T> class InputIterator :public NoneCopyConstructor_class, public virtual IPyIterator <T> {
private:
    bool _recoverable;
    T _last_next;
    std::queue<T> _q;
    bool _closed;
public:
    InputIterator(bool recoveable = false) :_closed{ false }, _recoverable{ recoveable }
    {
    }
    int Put(T v)
    {
        this->_q.push(v);
        return this->_q.size();
    }
    void Close() {
        this->_closed = true;
    }
    //C++向けAPI
    T Next()
    {
        if (this->_closed) {
            throw PyStopIteration();
        }
        if (this->_q.empty()) {
            if (this->_recoverable) {
                throw RecoverableStopIteration();
            }
            this->_closed = true;
            throw PyStopIteration();
        }
        auto r=this->_q.front();
        this->_q.pop();
        return r;
    }
};


//  JSからWASMのイテレータを参照するためのもの。
//
template<typename T> class OutputIterator :public NoneCopyConstructor_class
{
private:
    const shared_ptr<IPyIterator<T>> _src;
    T _last_next;
public:
    OutputIterator(const shared_ptr<IPyIterator<T>>& src) :_src{ src }, _last_next{0}
    {
    }
    //Nextの実行結果を返します。
    //Nextの返す値はlast_nextで取得できます。
    int hasNext() {
        try {
            auto r = this->_src->Next();
            this->_last_next = r;
            return 0;
        }
        catch (RecoverableStopIteration) {
            return 1;
        }
        catch (PyStopIteration) {
            return 2;
        }
    }
    T lastNext() {
        return this->_last_next;
    }
};

template class OutputIterator<int>;
template class OutputIterator<double>;


#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif





#ifdef __cplusplus
extern "C" {
#endif

    EM_JS(void, dbgprint2, (int num), {
        console.log("dbgprint:"+num);
    });
    void dbgprint(int num) {
        dbgprint2(num);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //  RawMemory走査関数
    //////////////////////////////////////////////////////////////////////////////////////////////////////


    void* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_malloc(int size)
    {
        return malloc(size);
    }
    void EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_free(void* ptr)
    {
        free(ptr);
    }
    void EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_putInt(void* ptr, int pos, int v)
    {
        *((int*)ptr + pos) = v;
    }
    int EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_getInt(void* ptr, int pos)
    {
        return *((int*)ptr + pos);
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //  SharedMemoryオブジェクト管理関数
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    //ポインタテーブルのサイズを返す。
    int EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_PointerHolder_Size()
    {
        return _instances.Size();
    }

    //ポインタテーブルからインスタンスを削除。エラーの場合-1
    void EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_Dispose(void* ptr)
    {
        _instances.Remove(ptr);
    }
#ifdef __cplusplus
}
#endif
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //  入出力補助関数
    //////////////////////////////////////////////////////////////////////////////////////////////////////


    EXTERN_C shared_ptr<InputIterator<int>>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_IntInputIterator() {
        auto r = std::make_shared<InputIterator<int>>();
        return (shared_ptr<InputIterator<int>>*)_instances.Add(r);
    }
    EXTERN_C void EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_IntInputIterator_put(const shared_ptr<InputIterator<int>>* ptr, int v)
    {
        (*ptr)->Put(v);
    }

    EXTERN_C shared_ptr<InputIterator<double>>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_DoubleInputIterator() {
        auto r = std::make_shared<InputIterator<double>>();
        return (shared_ptr<InputIterator<double>>*)_instances.Add(r);
    }
    EXTERN_C void EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_DoubleInputIterator_put(const shared_ptr<InputIterator<double>>* ptr, double v)
    {
        (*ptr)->Put(v);
    }

    //EXTERN_C shared_ptr<OutputIterator<int>>* new_IntOutputIterator(const shared_ptr<IPyIterator<int>>& src) {
    //    auto r = std::make_shared<OutputIterator<int>>(src);
    //    return (shared_ptr<OutputIterator<int>>*)_instances.Add(r);
    //}
    EXTERN_C int EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_IntOutputIterator_hasNext(const shared_ptr<OutputIterator<int>>* ptr)
    {
        return (*ptr)->hasNext();
    }
    EXTERN_C int EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_IntOutputIterator_lastNext(const shared_ptr<OutputIterator<int>>* ptr)
    {
        return (*ptr)->lastNext();
    }

    //EXTERN_C shared_ptr<OutputIterator<double>>* new_DoubleOutputIterator(const shared_ptr<IPyIterator<double>>& src) {
    //    auto r = std::make_shared<OutputIterator<double>>(src);
    //    return (shared_ptr<OutputIterator<double>>*)_instances.Add(r);
    //}
    EXTERN_C int EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_DoubleOutputIterator_hasNext(const shared_ptr<OutputIterator<double>>* ptr)
    {
        return (*ptr)->hasNext();
    }
    EXTERN_C double EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_DoubleOutputIterator_lastNext(const shared_ptr<OutputIterator<double>>* ptr)
    {
        return (*ptr)->lastNext();
    }


    //////////////////////////////////////////////////////////////////////////////////
    //  TBSKmodemクラス
    //////////////////////////////////////////////////////////////////////////////////




    EXTERN_C void* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TraitTone(const double* data, int length) {
        auto r = std::make_shared<TraitTone>();
        for (auto i = 0;i < length;i++) {
            r->push_back(*(data + i));
        }
        return _instances.Add(r);
    }
    EXTERN_C void* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_SinTone(int points, int cycle) {
        auto r = std::make_shared<SinTone>(points, cycle);
        return _instances.Add(r);
    }
    EXTERN_C void* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_XPskSinTone(int points, int cycle, int div) {
        auto r = std::make_shared<XPskSinTone>(points, cycle, div);
        return _instances.Add(r);
    }

    EXTERN_C shared_ptr<CoffPreamble>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_CoffPreamble(const shared_ptr<TraitTone>* tone, double threshold, int cycle) {
        auto r = std::make_shared<CoffPreamble>(*tone,threshold,cycle);
        return (shared_ptr<CoffPreamble>*)_instances.Add(r);
    }




    //
    //  TbskModulator
    //

    EXTERN_C shared_ptr<TbskModulator>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TbskModulator(const shared_ptr<TraitTone>* tone, const shared_ptr<Preamble>* preamble)
    {
        //tone,preambleを生成
        auto r = std::make_shared<TbskModulator>(*tone, *preamble);
        return (shared_ptr<TbskModulator>*)_instances.Add(r);
    }

    EXTERN_C shared_ptr<OutputIterator<double>>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TbskModulator_Modulate_A(const shared_ptr<TbskModulator>* ptr, const shared_ptr<InputIterator<int>>* src)
    {
        const shared_ptr<TbskModulator>& ref = *ptr;
        auto m = ref->Modulate((*src));//例外を吐くなら注意!
        auto r = std::make_shared<OutputIterator<double>>(m);
        return (shared_ptr<OutputIterator<double>>*)_instances.Add(r);
    }



    //
    //  TbskDemodulator
    //



    EXTERN_C shared_ptr<TbskDemodulator>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TbskDemodulator(const shared_ptr<TraitTone>* tone_ptr, const shared_ptr<Preamble>* preamble_ptr)
    {
        //tone,preambleを生成

        auto r = std::make_shared<TbskDemodulator>(*tone_ptr, *preamble_ptr);
        return (shared_ptr<TbskDemodulator>*)_instances.Add(r);
    }





    EXTERN_C shared_ptr<OutputIterator<int>>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TbskDemodulator_DemodulateAsInt(const shared_ptr<TbskDemodulator>* ptr, const shared_ptr<InputIterator<double>>* src) {
        const shared_ptr<TbskDemodulator>& ref = *ptr;
        try {
            shared_ptr<IPyIterator<int>> m = (*ptr)->DemodulateAsInt(*src);//例外トラップ機構が変
            if (!m) {
                //未割当ならnull
                return NULL;
            }
            auto r = std::make_shared<OutputIterator<int>>(m);
            return (shared_ptr<OutputIterator<int>>*)_instances.Add(r);
        }
        catch (RecoverableException<DemodulateAsBitAS>& e) {
            e.Close();
            return NULL;
        }
        catch (...) {
            return NULL;
        }

    }






    EXTERN_C shared_ptr<PcmData>* wasm_tbskmodem_PcmData_1(const TBSK_BYTE* src, size_t size, int sample_bits, int frame_rate) {
        return (shared_ptr<PcmData>*)_instances.Add(make_shared<PcmData>(src, size, sample_bits, frame_rate));
    }



    EXTERN_C int wasm_tbskmodem_PcmData_GetSampleBits(const shared_ptr<PcmData>* ptr)
    {
        return (*ptr)->GetSampleBits();
    };
    EXTERN_C int wasm_tbskmodem_GetFramerate(const shared_ptr<PcmData>* ptr)
    {
        return (*ptr)->GetFramerate();
    }
    EXTERN_C int wasm_tbskmodem_GetByteslen(const shared_ptr<PcmData>* ptr) {
        return (*ptr)->GetByteslen();
    }
    EXTERN_C const int wasm_tbskmodem_DataAsFloat(const shared_ptr<PcmData>* ptr, double* buf, int buf_len_in_bytes) {
        auto& r = (*ptr)->DataAsFloat();
        auto tick_count = (*ptr)->GetByteslen() / ((*ptr)->GetSampleBits() / 8);
        if (buf_len_in_bytes < r->size() * sizeof(double) * tick_count) {
            return 0;
        }
        memcpy(buf, r.get(), r->size() * sizeof(double));
        return r->size();
    }






#ifdef __cplusplus
extern "C" {
#endif



    extern void TBSKmodemJS_init();
    void EMSCRIPTEN_KEEPALIVE TBSKmodemJS() {
        TBSKmodemJS_init();
    }

#ifdef __cplusplus
}
#endif


//int main()
//{
//    TBSKmodemJS_init();
//}