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
		auto tone = make_shared<XPskSinTone>(10, 10);//10 point/cycle * 100 
		tone->Mul(0.5);
		auto preamble = make_shared<CoffPreamble>(tone);
		auto mod = make_shared<TbskModulator>(tone, preamble);
		//std::vector<int> bits{ 0,1,0,1,0,1,0,1 };
		//auto src = std::make_shared<PyIterator<int>>(bits);
		//auto a = std::make_shared<RoStream<int>>(src);
		std::vector<int> bytes{};
		auto src = std::make_shared<PyIterator<int>>(bytes);
		auto a = std::make_shared<RoStream<int>>(src);
		auto c = mod->Modulate(a);
		//auto d = Functions::ToVector<double>(c);
		auto d = make_shared<std::vector<double>>();

		FileWriter fw("./test2.wav");
		PcmData pcm(d, 16, 8000);
		PcmData::Dump(pcm, fw);

		auto dem = std::make_shared<TbskDemodulator>(tone,preamble);
		auto demod_bits=dem->DemodulateAsInt(std::make_shared<PyIterator<double>>(d));
		if (demod_bits) {
			printf("OK");
		}
		else {
			printf("NG");
		}
		auto decoded = Functions::ToVector<int>(demod_bits);
	}
	_CrtDumpMemoryLeaks();
	return 0;
}