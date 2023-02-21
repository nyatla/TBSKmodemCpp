#pragma once
#include "../../interfaces.h"
#include "./toneblock.h"
#include "../../streams/rostreams.h"
#include "../../streams/BitStream.h"
#include "../../utils/math/SumIterator.h"
#include "../../types/types.h"
#include <deque>
#include <cmath>



namespace TBSKmodemCPP
{

    // """ ビット列をTBSK変調した振幅信号に変換します。出力レートは入力のN倍になります。
    //     N=len(trait_block)*2*len(tone)
    //     このクラスは、toneを一単位とした信号とtrail_blockとの積を返します。
    // """
#pragma warning( disable : 4250 )
    class TraitBlockEncoder :public  virtual IEncoder<TraitBlockEncoder, IBitStream, double>, BasicRoStream<double>
    {
    private:
        const shared_ptr<const TraitTone> _btone;
        TBSK_INT64 _pos;
        vector<int> _sblock;
        std::deque<double> _tone_q;
        shared_ptr<IBitStream> _src;
        bool _is_eos;

        // @overload
        // """
        //     Kビットは,N*K+1個のサンプルに展開されます。
        // """
    public:
        TraitBlockEncoder(const shared_ptr<const TraitTone>& tone);
        virtual ~TraitBlockEncoder();
        TraitBlockEncoder& SetInput(const shared_ptr<IBitStream>& src)override;
        double Next()override;
        // @property
        TBSK_INT64 GetPos()const override;
    };
#pragma warning( default : 4250 )
}

namespace TBSKmodemCPP
{

    class AverageInterator;
    // """ シンボル幅がNのTBSK相関信号からビットを復調します。

    // """
#pragma warning( disable : 4250 )
    class TraitBlockDecoder:public BasicRoStream<int>,public virtual IBitStream, public virtual IDecoder<TraitBlockDecoder,IRoStream<double>,int>
    {
    private:
        const int _trait_block_ticks;
        const double _threshold;
        unique_ptr<AverageInterator> _avefilter;
        bool _is_eos;
        int _pos = 0;
        vector<double> _samples;
        double _last_data = 0;
        int _preload_size = 0;
        int _block_skip_size = 0;
        int _block_skip_counter=0;
        int _shift = 0;
    public:
        TraitBlockDecoder(int trait_block_ticks, double threshold = 0.2);
        virtual ~TraitBlockDecoder();

        // """ 
        //     Args:
        //         src
        //             TBSK信号の開始エッジにポインタのあるストリームをセットします。
        // """
        TraitBlockDecoder& SetInput(const shared_ptr<IRoStream<double>>& src)override;

        int Next()override;
        // @property
        TBSK_INT64 GetPos()const override;
    };
#pragma warning( default : 4250 )
}
        

