

function TBSKmodem_api_load_()
{
    //�������珉�����R�[�h
    let MOD = Module;
    if ("tbskmodem" in MOD) {
        console.log("tbskmodem api is already initialized.");
        return;
    }

    console.log("Start tbskmodem initialize.");

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
        constructor(is_recoverable=false) {
            super(MOD._wasm_tbskmodem_DoubleInputIterator(is_recoverable));
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
        next() {
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
            //console.log(_threshold,_cycle);
            super(MOD._wasm_tbskmodem_CoffPreamble(tone._wasm_instance, _threshold, _cycle));
        }
    }

    class PassDecoder {
        reset() { }
        put(data) { return data; }

    }

    class Utf8Decoder {
        constructor() {
            this._decoder = new TextDecoder("utf8", { fatal: true });
            this._q = [];
            this._tmp = [];
        }
        reset() {
            this._q = [];
            this._tmp = [];
        }
        put(data) {
//            console.log("q:", this._q);
            for (let i = 0; i < data.length; i++) {
                this._q.push(data[i]);
            }
//            console.log("q2:", this._q);
            let ret = [];
            ML:
            while (this._q.length > 0) {
                for (var i = 0; i < this._q.length; i++) {
                    try {
                        let inp = new Uint8Array(this._q.slice(0, i + 1));
//                        console.log(inp);
                        ret.push(this._decoder.decode(inp));
                        this._q = this._q.slice(i + 1);
                        continue ML;//q�X�V
                    } catch (e) {
//                        console.log("OOPD");
                        if (i > 8) {
                            ret.push(this._q[0]);
                            this._q = this._q.slice(1);
                            continue ML;//q�X�V
                        }
                    }
                }
                break;
            }
            if (ret.length > 0) {
                return ret;
            }

            return undefined;
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
         * @param {array[uint8]|string} src
         * @return {FloatArray}
         */
        modulate(src) {
            var buf = new IntInputIterator();
            try {
                if (typeof (src) == "string") {
                    let te = new TextEncoder();                    
                    buf.puts(te.encode(src));
                } else {
                    buf.puts(src);
                }
                //int*��n���āA[int,float...]�̃|�C���^��Ԃ��Ă��炤�B
                let wi = MOD._wasm_tbskmodem_TbskModulator_Modulate_A(this._wasm_instance, buf._wasm_instance);
                if (wi == null) {
                    throw new Error();
                }
                // array�ɕϊ�
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
         * JS�g���B�I�[�f�B�I�R���e�L�X�g�̃o�b�t�@��Ԃ��B
         * @param {AudioContext} actx
         * @param {array[int]} src
         * @param {int} sampleRate
         * @return {BufferSource}
         */
        modulate2AudioBuffer(actx, src, sampleRate) {
            let f32_array = this.modulate(src);
            //console.log(f32_array);
            let buf = actx.createBuffer(1, f32_array.length, sampleRate);
            buf.getChannelData(0).set(f32_array);
            return buf;
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
        demodulate(src, decoder = undefined)
        {
            if (decoder == "utf8") {
                decoder = new Utf8Decoder();
            } else {
                decoder = new PassDecoder();
            }
            let buf = new DoubleInputIterator();
            try {
                buf.puts(src);
                //double*��n���āAint�C�e���[�^�����炤�B���J�o�����Ȃ��B
                //                        let wi = MOD._wasm_tbskmodem_TbskDemodulator_DemodulateAsInt(this._wasm_instance, buf._wasm_instance);
                let out=this._demodulateAsInt(buf);
                if (out == null) {
                    return [];
                }
                // array�ɕϊ�
                try {
                    if (decoder) {
                        return decoder.put(out.toArray());
                    } else {
                        return out.toArray();
                    }
                } finally {
                    out.dispose();
                }
            } finally {
                buf.dispose();
            }
        }
        /**
         * �����o�ŃX�g���[�������f������null
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
            if (wi == 0) {
                return null;
            }
            return new IntOutputIterator(wi);
        }
    }



    class TbskListener extends Disposable{
        /**
         * onStart,onData,onEnd��push�֐����g���K�[�ɔ񓯊��ɌĂяo���܂��B
         * 
         * �f�R�[�h�I�u�W�F�N�g�́AT put(data),reset()�̊֐������I�u�W�F�N�g�ŁA�^����ꂽdata����T�������ł����Ƃ���T��Ԃ��܂��B�����ł��Ȃ��ꍇ��undefined�ł��Breset�ŏ�Ԃ����������܂��B
         * 
         * @param {Tone} tone
         * @param {Preamble} preamble
         */
        constructor(tone, preamble, events = {}, decoder = undefined) {
            super();
            if (!("onStart" in events)) { events.onStart = null; }
            if (!("onData" in events)) { events.onData = null; }
            if (!("onEnd" in events)) { events.onEnd = null; }
            let _t = this;
            this._decoder = decoder == "utf8" ? new Utf8Decoder() : new PassDecoder();
            this._demod=new TbskDemodulator(tone, preamble);
            this._input_buf = new DoubleInputIterator(true);
            this._callOnStart = () => {
                new Promise((resolve) => {
                    resolve();
                }).then(() => { if (events.onStart) { events.onStart() } });
            };
            this._callOnData = (data) => {
                new Promise((resolve) => {
                    resolve();
                }).then(() => { if (events.onData) { events.onData(data) } });
            };
            this._callOnEnd = () => {
                new Promise((resolve) => {
                    resolve();
                }).then(() => { if (events.onEnd) { events.onEnd() } });
            };
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
             * 1��ڂ�yield�Ŋ֐���Ԃ��B
             * @param {any} demod
             * @param {any} input_buf
             */
            function* workflow(demod, input_buf,callOnStart,callOnData,callOnEnd,decoder)
            {
                decoder.reset();
//                console.log("workflow called!");
                let out_buf = null;
                let dresult = null;
                dresult = demod._demodulateAsInt_B(input_buf);

                yield function () {
                    out_buf.dispose();
                    dresult.dispose();
                    out_buf = null;
                    dresult = null;
                }
                if (dresult == null) {
                    //�����o��input���I�[
                    console.error("input err");
                    return;//done
                }
                try {
                    switch (dresult.getType()) {
                        case 1://1 iter
//                            console.log("signal detected");
                            out_buf = dresult.getOutput();
                            break;
                        case 2:// recover
                            for (; ;) {
//                                console.log("recover");
                                out_buf = dresult.getRecover();
                                if (out_buf != null) {
                                    break;
                                }

                                //���J�o���ėv�����������̂ŉ������Ȃ��B
                                yield;
                            }
                            break
                        default:
                            //�p���s�\�ȃG���[
                            console.error("unknown type.");
                            return;//done
                    }
                } finally {
                    dresult.dispose();
                    dresult = null;
                }
                //out�ɃC�e���[�^�������Ă���B
                console.log("Signal detected!");
                callOnStart();
                //�I�[�ɒB���閘���o��
                let ra = [];
                for (; ;) {
                    try {
                        for (; ;) {
                            let w = out_buf.next();
                            ra.push(w);
                        }
                    } catch (e) {
                        if (e instanceof RecoverableStopIteration) {
//                            console.log("RecoverableStopIteration");
                            if (ra.length > 0) {
                                //������data�C�x���g
                                console.log("data:");
//                                console.log(ra);
                                if (decoder) {
                                    let rd = decoder.put(ra);
                                    if (rd) {
                                        callOnData(rd);
                                    }
                                } else {
                                    callOnData(ra);
                                }
                                ra = [];
                            }
                            yield;
                            continue;
                        } else if (e instanceof StopIteration) {
                            if (ra.length > 0) {
                                //console.log("StopIteration");
                                console.log("data:");
                                //console.log(ra);
                                if (decoder) {
                                    let rd = decoder.put(ra);
                                    if (rd) {
                                        callOnData(rd);
                                    }
                                } else {
                                    callOnData(ra);
                                }
                                ra = [];
                            }
                            console.log("Signal lost!");
                            callOnEnd();
                        }
                        //�����ł�Stop�C���^���[�V�����̋�ʂ����Ȃ�����A���̃V�O�i�����o�Ŕ��f����B
                    }
                    out_buf.dispose();
                    out_buf = null;
                    return;//done
                }
            //�֐��I���B
            }
//            console.log("push callead!");
            this._input_buf.puts(src);
//            console.log("input_buf_len:" + src.length);
            if (this._currentGenerator == null) {
                this._currentGenerator = workflow(this._demod, this._input_buf, this._callOnStart, this._callOnData, this._callOnEnd, this._decoder);//�V�K����
                this._currentGenerator.dispose = this._currentGenerator.next();
            }

            if (this._currentGenerator.next().done) {
                this._currentGenerator = null;
            }
        }



    }


    class PcmData extends WasmProxy {
        /**
         * ���̊֐��͂��̂܂܂����ǂ���
         */
        constructor(wasm_instance) {
            super(wasm_instance);
        }
        static create(float_data, sample_bits, frame_rate)
        {
            //float iterator�֕ϊ�
            let input_src = new DoubleInputIterator();
            input_src.puts(float_data);
            return new PcmData(MOD._wasm_tbskmodem_PcmData_2(input_src._wasm_instance, sample_bits, frame_rate));
        }
        /** return to Uint8 Array
         */
        dump() {
            let iter = new IntOutputIterator(MOD._wasm_tbskmodem_PcmData_Dump(this._wasm_instance));
            return new Uint8Array(iter.toArray());
        }

    }

    MOD.tbskmodem = {
        getPointerHolderSize: function () {
            return MOD._wasm_tbskmodem_PointerHolder_Size();
        },
        Utf8Decoder: Utf8Decoder,
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
        TbskListener: TbskListener,
        PcmData: PcmData
    };
    console.log("tbskmodem initialized.", this._instance);
    return this._instance;
}


mergeInto(LibraryManager.library, {
    TBSKmodem_api_load_
});




