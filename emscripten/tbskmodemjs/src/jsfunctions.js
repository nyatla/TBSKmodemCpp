

function TBSKmodemJS_init()
{
    if (typeof TBSKmodemJS !== 'undefined') {
        return;
    }
    //ここから初期化コード
    let MOD = Module;
    console.log("Start TBSKmodemJS initialize.");

    function set_default(a, v) { return (a === undefined || a === null) ? v : a; }


    class Disposable {
        dispose() { }
    }
    class WasmProxy extends Disposable{
        constructor(wasm_instance) {
            super();
            this._wasm_instance = wasm_instance;
        }
        dispose() {
            MOD._wasm_tbskmodem_Dispose(this._wasm_instance);
            this._wasm_instance = null;
            super.dispose();
        }
    }


    class IntInputIterator extends WasmProxy {
        constructor() {
            super(MOD._wasm_tbskmodem_IntInputIterator());
        }
        put(v) {
            MOD._wasm_tbskmodem_IntInputIterator_put(this._wasm_instance, v);
            return this;
        }
        puts(v) {
            for (var i = 0; i < v.length; i++) {
                this.put(v[i] & 0x000000ff);
            }
            return this;
        }
    }
    class DoubleInputIterator extends WasmProxy {
        /**
         * 
         * @param {bool} recoverable
         */
        constructor(recoverable) {
            super(MOD._wasm_tbskmodem_DoubleInputIterator(recoverable));
        }
        put(v) {
            MOD._wasm_tbskmodem_DoubleInputIterator_put(this._wasm_instance, v);
            return this;
        }
        puts(v) {
            for (var i = 0; i < v.length; i++) {
                this.put(v[i]);
            }
            return this;
        }
    }


    class BasicOutputIterator extends WasmProxy {
        constructor(wasm_instance) {
            super(wasm_instance);
        }
        toArray() {
            let r = [];
            try {
                for (; ;) {
                    r.push(this.next());
                }
            } catch (e) {
                if (e instanceof StopIteration) {
                    console.log("StopIntertion");
                    //nothing to do
                } else {
                    console.log(e);
                    throw e;
                }
            }
            return r;
        }
    }


    class DoubleOutputIterator extends BasicOutputIterator {
        constructor(wasm_instance) {
            super(wasm_instance);
        }
        next(){
            let s = MOD._wasm_tbskmodem_DoubleOutputIterator_hasNext(this._wasm_instance);
            switch (s) {
                case 0:
                    return MOD._wasm_tbskmodem_DoubleOutputIterator_lastNext(this._wasm_instance);
                case 1:
                    throw new RecoverableStopIteration();
                case 2:
                    throw new StopIteration();
                default:
                    throw new Error();
            }
        }
    }
    class IntOutputIterator extends BasicOutputIterator {
        constructor(wasm_instance) {
            super(wasm_instance);
        }
        next() {
            let s = MOD._wasm_tbskmodem_IntOutputIterator_hasNext(this._wasm_instance);
            switch (s) {
                case 0:
                    return MOD._wasm_tbskmodem_IntOutputIterator_lastNext(this._wasm_instance);
                case 1:
                    throw new RecoverableStopIteration();
                case 2:
                    throw new StopIteration();
                default:
                    throw new Error();
            }
        }
    }


    //////////////////////////////////////////////////////////////////////////////////
    //  TBSK APIs
    //////////////////////////////////////////////////////////////////////////////////

    class StopIteration extends Error {
    }
    class RecoverableStopIteration extends StopIteration {
    }


    class TraitTone extends WasmProxy {
        constructor(double_array) {
            super(MOD._wasm_tbskmodem_TraitTone());
        }
    }
    class SieTone extends WasmProxy {
        constructor(poinsts, cycle) {
            let _cycle = set_default(cycle, 1);
            super(MOD._wasm_tbskmodemm_SinTone(points, _cycle));
        }
    }
    class XPskSinTone extends WasmProxy {
        constructor(poinsts, cycle, div) {
            let _cycle = set_default(cycle, 1);
            let _div = set_default(div, 8);
            super(MOD._wasm_tbskmodem_XPskSinTone(poinsts, _cycle, _div));
        }
    }
    class CoffPreamble extends WasmProxy {
        constructor(tone, threshold, cycle) {
            let _threshold = set_default(threshold, 1.0);
            let _cycle = set_default(cycle, 4);
            super(MOD._wasm_tbskmodem_CoffPreamble(tone._wasm_instance, _threshold, _cycle));
        }
    }



    class TbskModulator extends WasmProxy
    {
        /**
         * 
         * @param {Tone} tone
         * @param {Preamble} preamble
         */
        constructor(tone, preamble) {
            super(MOD._wasm_tbskmodem_TbskModulator(tone._wasm_instance, preamble._wasm_instance));
        }
        /**
         * @param {array[uint8]} src
         * @return {Int8Array}
         */
        modulate(src) {
            var buf = new IntInputIterator();
            try {
                buf.puts(src);
                //int*を渡して、[int,float...]のポインタを返してもらう。
                let wi = MOD._wasm_tbskmodem_TbskModulator_Modulate_A(this._wasm_instance, buf._wasm_instance);
                if (wi == null) {
                    throw new Error();
                }
                // arrayに変換
                let out = new DoubleOutputIterator(wi);
                try {
                    return out.toArray();
                } finally {
                    out.dispose();
                }
            } finally {
                buf.dispose();
            }
        }
        /**
         * JS拡張。オーディオコンテキストのバッファを返す。
         * @param {AudioContext} actx
         * @param {array[int]} src
         * @param {int} sampleRate
         * @return {BufferSource}
         */
        modulate2BufferSource(actx, src, sampleRate) {
            let f32_array = this.modulate(src);
            console.log(f32_array);
            let buf = actx.createBuffer(1, f32_array.length, sampleRate);
            buf.getChannelData(0).set(f32_array);
            let r = actx.createBufferSource();
            r.buffer = buf;
            return r;
        }
    }


    class TbskDemodulator extends WasmProxy {
        /**
         * 
         * @param {Tone} tone
         * @param {Preamble} preamble
         */
        constructor(tone, preamble) {
            super(MOD._wasm_tbskmodem_TbskDemodulator(tone._wasm_instance, preamble._wasm_instance));
        }
        /**
         * 
         * @param {DoubleInputIterator} buf
         */
        _demodulateAsInt(buf)
        {
            let r = MOD._wasm_tbskmodem_TbskDemodulator_DemodulateAsInt(this._wasm_instance, buf._wasm_instance);
            if (r == 0) {
                return null;
            }
            return new IntOutputIterator(r);
        }
        /**
         * @param src array[double]
         * @return array[uint8]
         */
        demodulate(src) {
            let buf = new DoubleInputIterator();
            try {
                buf.puts(src);
                //double*を渡して、intイテレータをもらう。リカバリしない。
                //                        let wi = MOD._wasm_tbskmodem_TbskDemodulator_DemodulateAsInt(this._wasm_instance, buf._wasm_instance);
                let out=this._demodulateAsInt(buf);
                if (out == null) {
                    return [];
                }
                // arrayに変換
                try {
                    return out.toArray();
                } finally {
                    out.dispose();
                }
            } finally {
                buf.dispose();
            }
        }
        /**
         * 未検出でストリームが中断したらnull
         * @param {any} src
         * @return {DemodulateResult}
         */
        _demodulateAsInt_B(src){
            let r = MOD._wasm_tbskmodem_TbskDemodulator_DemodulateAsInt_B(this._wasm_instance, src._wasm_instance);
            if (r == 0) {
                return null;
            }
            return new DemodulateResult(r);
        }
    }

    class DemodulateResult extends WasmProxy {
        constructor(wasm_instance) {
            super(wasm_instance);
        }
        getType() {
            return MOD._wasm_tbskmodem_TbskDemodulator_DemodulateResult_GetType(this._wasm_instance);
        }
        getOutput() {
            return new IntOutputIterator(MOD._wasm_tbskmodem_TbskDemodulator_DemodulateResult_GetOutput(this._wasm_instance));
        }
        getRecover() {
            let wi = MOD._wasm_tbskmodem_TbskDemodulator_DemodulateResult_Recover(this._wasm_instance);
            if (wi == null) {
                return null;
            }
            return new IntOutputIterator(wi);
        }
    }



    class TbskListener extends Disposable{
        /**
         * 
         * @param {Tone} tone
         * @param {Preamble} preamble
         */
        constructor(tone, preamble, onSignal, onData, onEndOfSignal) {
            super();
            this._demod=new TbskDemodulator(tone, preamble);
            this._input_buf = new DoubleInputIterator(true);
            this._currentGenerator = null;
        }
        dispose()
        {
            if (this._currentGenerator) {
                this._currentGenerator.dispose();
            }
            this._demod.dispose();
            this._input_buf.dispose();
        }
        push(src)
        {
            /**
             * 1回目のyieldで関数を返す。
             * @param {any} demod
             * @param {any} input_buf
             */
            function* workflow(demod, input_buf)
            {
                let out_buf = null;
                let dresult = null;
                dresult = demod._demodulateAsInt_B(input_buf);
                console.log(this);

                yield function () {
                    out_buf.dispose();
                    dresult.dispose();
                    out_buf = null;
                    dresult = null;
                }
                if (dresult == null) {
                    //未検出でinputが終端
                    console.log("input err");
                    return;//done
                }
                try {
                    switch (dresult.getType()) {
                        case 1://1 iter
                            console.log("signal detected");
                            out_buf = dresult.getOutput();
                            break;
                        case 2:// recover
                            for (; ;) {
                                console.log("recover");
                                out_buf = dresult.getRecover();
                                if (out_buf != null) {
                                    break;
                                }
                                //リカバリ再要求があったので何もしない。
                                yield;
                            }
                            break
                        default:
                            //継続不能なエラー
                            console.error("unknown type.");
                            return;//done
                    }
                } finally {
                    dresult.dispose();
                    dresult = null;
                }
                //outにイテレータが入っている。
                console.log("signal");
                //終端に達する迄取り出し
                let ra = [];
                for (; ;) {
                    try {
                        for (; ;) {
                            ra.push(out_buf.next());
                        }
                    } catch (e) {
                        if (e instanceof RecoverableStopIteration) {
                            //ここでdataイベント
                            console.log("data:");
                            console.log(ra);
                            ra = [];
                            yield;
                            continue;
                        } else if (e instanceof StopIteration) {
                            console.log("data:");
                            console.log(ra);
                            console.log("end");
                        }
                        //ここではStopインタレーションの区別がつかないから、次のシグナル検出で判断する。
                    }
                    console.log("END");
                    out_buf.dispose();
                    out_buf = null;
                    return;//done
                }
            //関数終了。
            }
            this._input_buf.puts(src);
            console.log("input_buf_len:" + src.length);
            if (this._currentGenerator == null) {
                this._currentGenerator = workflow(this._demod, this._input_buf);//新規生成
                this._currentGenerator.dispose = this._currentGenerator.next();
                console.log("new workflow");
            } else if (this._currentGenerator.done) {
                this._currentGenerator = workflow(this._demod, this._input_buf);//終わってたら再生成
                this._currentGenerator.dispose = this._currentGenerator.next();
                console.log("renew workflow");
            }
            this._currentGenerator.next();
        }
        onSignalStart() { console.log("start signal"); };
        onSignalEnd() { console.log("end signal"); };
        onData(v) { cosole.log("on data") };



    }




    TBSKmodemJS = {
        getPointerHolderSize: function () {
            return MOD._wasm_tbskmodem_PointerHolder_Size();
        },
        WasmProxy: WasmProxy,

        StopIteration: StopIteration,
        IntInputIterator: IntInputIterator,
        DoubleInputIterator: DoubleInputIterator,
        IntOutputIterator: IntOutputIterator,
        DoubleOutputIterator: DoubleOutputIterator,

        TraitTone: TraitTone,
        SieTone: SieTone,
        XPskSinTone: XPskSinTone,
        CoffPreamble: CoffPreamble,
        TbskModulator: TbskModulator,
        TbskDemodulator: TbskDemodulator,
        TbskListener: TbskListener
    };
    console.log("TBSKmodemJS is ready!");


}

mergeInto(LibraryManager.library, {
    TBSKmodemJS_init
});




