#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "../../../src/tbskmodem/tbskmodem.h"
#include <cmath>
#include <memory>
#include <vector>
using namespace TBSKmodemCPP;
using namespace std;




int main()
{
	{
		shared_ptr<TraitTone> tone = TbskTone::CreateXPskSin();//10 point/cycle * 100 
		auto mod = make_shared<TbskModulator>(tone);
		std::vector<int> bytes;
		for (int i = 0;i < 16*4;i++) {
			bytes.push_back(0);
			bytes.push_back(1);
		}
		auto src = std::make_shared<PyIterator<int>>(bytes);
		auto a = std::make_shared<RoStream<int>>(src);
		auto c = mod->ModulateAsBit(a);
		auto d = Functions::ToVector<double>(c);

		PcmData pcm(d, 16, 8000);
		{
			FileWriter fw("./test2.wav");
			PcmData::Dump(pcm, fw);
		}
		auto dem = std::make_shared<TbskDemodulator>(tone);
		{

			FileReader fr("./test2.wav");
			auto pcmsrc=PcmData::Load(fr);
			auto d2=pcmsrc->DataAsFloat();
			auto demod_bits = dem->DemodulateAsInt(std::make_shared<PyIterator<double>>(d2));
			if (demod_bits) {
				printf("OK");
			}
			else {
				printf("NG");
			}
			auto decoded = Functions::ToVector<int>(demod_bits);
		}

	}
	_CrtDumpMemoryLeaks();
	return 0;
}