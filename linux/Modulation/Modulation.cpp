#include <stdlib.h>
#include "../../src/tbskmodem/tbskmodem.h"
#include <memory>
#include <vector>
using namespace TBSKmodemCPP;
using namespace std;




int main()
{
	{
		auto tone = make_shared<XPskSinTone>(8, 10);//10 point/cycle * 100 
		auto preamble = make_shared<CoffPreamble>(tone);
		auto mod = make_shared<TbskModulator>(tone, preamble);
		std::vector<int> bits{ 0,1,0,1,0,1,0,1 };
		auto src = std::make_shared<PyIterator<int>>(bits);
		auto a = std::make_shared<RoStream<int>>(src);
		auto c = mod->Modulate("Hello");
		auto d = Functions::ToVector<double>(c);
		FileWriter fw("./test2.wav");
		PcmData pcm(d, 16, 8000);
		PcmData::Dump(pcm, fw);
	}
	return 0;
}