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
        if (this->_q.size()==0) {
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
    std::shared_ptr<std::vector<T>> ToVector() {
        auto ret=std::make_shared<std::vector<T>>();
        try {
            for (;;) {
                ret->push_back(this->Next());
            }
        }
        catch (PyStopIteration&) {
            //nothsing to do
        }
        return ret;
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
            if (!this->_src) {
                //emptyならstopiteration扱い
//                EM_ASM({ console.log("RECNULL") });
                return 1;
            }
            auto r = this->_src->Next();
            this->_last_next = r;
            return 0;
        }
        catch (RecoverableStopIteration&) {
//            EM_ASM({console.log("RECOVER")});
            return 1;
        }
        catch (PyStopIteration&) {
//            EM_ASM({ console.log("STOP") });
            return 2;
        }
    }
    T lastNext() {
        return this->_last_next;
    }
};


#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif





#ifdef __cplusplus
extern "C" {
#endif

    EXTERN_C int EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_VERSION(int mode)
    {
        switch (mode) {
        case 0:return TBSKmodemCPP::VERSION_MAJOR;
        case 1:return TBSKmodemCPP::VERSION_MINER;
        case 2:return TBSKmodemCPP::VERSION_PATCH;
        default:
            return -1;
        }
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

    EXTERN_C shared_ptr<InputIterator<double>>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_DoubleInputIterator(bool is_recoveable=false) {
        auto r = std::make_shared<InputIterator<double>>(is_recoveable);
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

    //will be deplicate!
    EXTERN_C shared_ptr<TbskModulator>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TbskModulator_A(const shared_ptr<TraitTone>* tone,int preamble_cycle)
    {
        //tone,preambleを生成
        auto r = std::make_shared<TbskModulator>(*tone, preamble_cycle);
        return (shared_ptr<TbskModulator>*)_instances.Add(r);
    }
    EXTERN_C shared_ptr<TbskModulator>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TbskModulator_B(const shared_ptr<TraitTone>* tone, const shared_ptr<Preamble>* preamble)
    {
        //tone,preambleを生成
        auto r = std::make_shared<TbskModulator>(*tone, *preamble);
        return (shared_ptr<TbskModulator>*)_instances.Add(r);
    }

    /**
    *   戻り値はdisposeする必要があります。
    */
    EXTERN_C shared_ptr<OutputIterator<double>>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TbskModulator_Modulate(const shared_ptr<TbskModulator>* ptr, const shared_ptr<InputIterator<int>>* src,bool stopsymbol)
    {
        const shared_ptr<TbskModulator>& ref = *ptr;
        auto m = ref->Modulate((*src), 8,stopsymbol);//例外を吐くなら注意!
        auto r = std::make_shared<OutputIterator<double>>(m);
        return (shared_ptr<OutputIterator<double>>*)_instances.Add(r);
    }



    //
    //  TbskDemodulator
    //
    EXTERN_C shared_ptr<TbskDemodulator>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TbskDemodulator_A(const shared_ptr<TraitTone>* tone_ptr,double preambe_th,int preamble_cycle)
    {
        //tone,preambleを生成

        auto r = std::make_shared<TbskDemodulator>(*tone_ptr, preambe_th, preamble_cycle);
        return (shared_ptr<TbskDemodulator>*)_instances.Add(r);
    }


    EXTERN_C shared_ptr<TbskDemodulator>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TbskDemodulator_B(const shared_ptr<TraitTone>* tone_ptr, const shared_ptr<Preamble>* preamble_ptr)
    {
        //tone,preambleを生成

        auto r = std::make_shared<TbskDemodulator>(*tone_ptr, *preamble_ptr);
        return (shared_ptr<TbskDemodulator>*)_instances.Add(r);
    }




    /** 
    *   非NULLの場合、有効な戻り値イテレータ。
    *   NULLの場合、終端までに信号を検出できなかった。
    * 
    *   戻り値はdisposeする必要があります。
    */
    EXTERN_C shared_ptr<OutputIterator<int>>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TbskDemodulator_DemodulateAsInt(const shared_ptr<TbskDemodulator>* ptr, const shared_ptr<InputIterator<double>>* src) {
        const shared_ptr<TbskDemodulator>& ref = *ptr;
        try {
            shared_ptr<IPyIterator<int>> m = (*ptr)->DemodulateAsInt(*src);
            //mは未割当の場合もある。(終端到達で未検出)OutputIterator側でチェックする事！
            auto r = std::make_shared<OutputIterator<int>>(m);
            return (shared_ptr<OutputIterator<int>>*)_instances.Add(r);
        }
        catch (RecoverableException<AsyncDemodulateX>& e) {
            e.Close();
            return NULL;
        }
        catch (...) {
            return NULL;
        }
    }



    struct DemodulateResult{
        int type;
        shared_ptr<IPyIterator<int>> iter;
        shared_ptr<AsyncDemodulateAsInt> recover;
        void setIPyIterator(const shared_ptr<IPyIterator<int>>& v) {
            this->type = 1;
            this->iter= v;
        }
        void setDemodulateResult(const shared_ptr<AsyncDemodulateAsInt>& v) {
            this->type = 2;
            this->recover = v;
        }
    };

   /**
    *   特殊戻り値で複数の値を返します。
    *   戻り値はdisposeする必要があります。
    */
    EXTERN_C shared_ptr<struct DemodulateResult>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TbskDemodulator_DemodulateAsInt_B(const shared_ptr<TbskDemodulator>* ptr, const shared_ptr<InputIterator<double>>* src) {
        const shared_ptr<TbskDemodulator>& ref = *ptr;
        try {
            //未検出、または検出の場合
            shared_ptr<IPyIterator<int>> m = (*ptr)->DemodulateAsInt(*src);//例外トラップ
            //mは未割当の場合もある。(終端到達で未検出)OutputIterator側でチェックする事！

            auto r = std::make_shared<struct DemodulateResult>();
            r->setIPyIterator(m);
            return (shared_ptr<struct DemodulateResult>*)_instances.Add(r);
        }
        catch (RecoverableException<AsyncDemodulateAsInt>& e) {
            //ストリームの中断を検知した場合
            auto r = std::make_shared<struct DemodulateResult>();
            r->setDemodulateResult(e.Detach());
            return (shared_ptr<struct DemodulateResult>*)_instances.Add(r);
        }
        catch (...) {
            return NULL;
        }
    }
    EXTERN_C int EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TbskDemodulator_DemodulateResult_GetType(const shared_ptr<struct DemodulateResult>* ptr) {
        return (*ptr)->type;
    }

    /**
    *   Resultがoutputであればその値を返します。
    *   戻り値はdisposeする必要があります。
    */
    EXTERN_C shared_ptr<OutputIterator<int>>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TbskDemodulator_DemodulateResult_GetOutput(const shared_ptr<struct DemodulateResult>* ptr, const shared_ptr<InputIterator<double>>* src) {
        assert((*ptr)->type==1);
        const auto& p = (*ptr)->iter;
        auto r = std::make_shared<OutputIterator<int>>(p);
        return (shared_ptr<OutputIterator<int>>*)_instances.Add(r);
    }
    /**
    *   Resultがrecoverbleであれば実行してその結果を返します。
    *   戻り値はdisposeする必要があります。
    */
    EXTERN_C shared_ptr<OutputIterator<int>>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_TbskDemodulator_DemodulateResult_Recover(const shared_ptr<struct DemodulateResult>* ptr)
    {
        assert((*ptr)->type == 2);
        const auto& p = (*ptr)->recover;
        if (p->Run()) {
            auto r = std::make_shared<OutputIterator<int>>(p->GetResult());
            return (shared_ptr<OutputIterator<int>>*)_instances.Add(r);
        }
        return NULL;
    }



    //std::shared_ptr<std::vector<unsigned char>> ToByteVector(const shared_ptr<InputIterator<int>>& src) {
    //    auto ret = std::make_shared<std::vector<unsigned char>>();
    //    try {
    //        for (;;) {
    //            ret->push_back((unsigned char)src->Next());
    //        }
    //    }
    //    catch (PyStopIteration&) {
    //        //nothsing to do
    //    }
    //    return ret;
    //}


    /**
    * BYTEデータからインスタンスを作る。
    */
    EXTERN_C shared_ptr<PcmData>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_PcmData_1(const shared_ptr<InputIterator<int>>* src, int sample_bits, int frame_rate) {
        std::vector<unsigned char> bsrc;
        try {
            for (;;) {
                bsrc.push_back((unsigned char)(*src)->Next());
            }
        }
        catch (PyStopIteration&) {
            //nothsing to do
        }
        //ファイルをロード
        return (shared_ptr<PcmData>*)_instances.Add(make_shared<PcmData>(bsrc.data(),bsrc.size(), sample_bits, frame_rate));
    }
    EXTERN_C shared_ptr<PcmData>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_PcmData_2(const shared_ptr<InputIterator<double>>* src, int sample_bits, int frame_rate) {
        //ファイルをロード
        auto& s = *(src->get());
//        int a = s.ToVector()->size();
//        EM_ASM_({ console.log($0); }, a);
        return (shared_ptr<PcmData>*)_instances.Add(make_shared<PcmData>(s, sample_bits, frame_rate));
    }



    EXTERN_C int EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_PcmData_GetSampleBits(const shared_ptr<PcmData>* ptr)
    {
        return (*ptr)->GetSampleBits();
    };
    EXTERN_C int EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_PcmData_GetFramerate(const shared_ptr<PcmData>* ptr)
    {
        return (*ptr)->GetFramerate();
    }
    EXTERN_C int EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_PcmData_GetByteslen(const shared_ptr<PcmData>* ptr) {
        return (*ptr)->GetByteslen();
    }
    EXTERN_C const shared_ptr<OutputIterator<double>>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_PcmData_DataAsFloat(const shared_ptr<PcmData>* ptr) {
        const std::shared_ptr<const std::vector<double>> r = std::move((*ptr)->DataAsFloat());
        auto iter_=std::make_shared<PyIterator<double>>(r);
        return (shared_ptr<OutputIterator<double>>*)_instances.Add(std::make_shared<OutputIterator<double>>(iter_));
    }
    EXTERN_C const shared_ptr<OutputIterator<int>>* EMSCRIPTEN_KEEPALIVE wasm_tbskmodem_PcmData_Dump(const shared_ptr<PcmData>* ptr) {
        class ToVector :public IBinaryWriter {
        private:
            std::vector<int>* _dest;
        public:
            ToVector(std::vector<int>* s) :_dest{ s } {
            }
            size_t WriteBytes(size_t size, const void* buf) {
                for (auto i = 0;i < size;i++) {
                    this->_dest->push_back(*(i + (const unsigned char*)buf));
                }
                return size;
            }

        };
        auto dest = std::make_shared<std::vector<int>>();
        ToVector v(dest.get());
        PcmData::Dump(*(ptr->get()),v);
        auto iter_ = std::make_shared<PyIterator<int>>(dest);
        return (shared_ptr<OutputIterator<int>>*)_instances.Add(std::make_shared<OutputIterator<int>>(iter_));
    }
    






//#ifdef __cplusplus
//extern "C" {
//#endif
//
//
//    extern  void TBSKmodem_api_load_();
//    void EMSCRIPTEN_KEEPALIVE load_apis() {
//        return TBSKmodem_api_load_();
//    }
//
//#ifdef __cplusplus
//}
//#endif

/*
void main()
{
    TBSKmodem_api_load_();
}*/