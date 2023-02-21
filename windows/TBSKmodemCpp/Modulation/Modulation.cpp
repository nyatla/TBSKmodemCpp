#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#include "../../../src/tbskmodem/tbskmodem.h"
#include <memory>
#include <vector>
using namespace TBSKmodemCPP;
using namespace std;




int main()
{
	{
		auto tone = TbskTone::CreateXPskSin();//10 point/cycle * 100 
		auto mod = make_shared<TbskModulator>(tone);
		auto c = mod->Modulate("Hello");
		auto d = Functions::ToVector<double>(c);
		FileWriter fw("./test2.wav");
		PcmData pcm(d, 16, 8000);
		PcmData::Dump(pcm, fw);
	}
	_CrtDumpMemoryLeaks();
	return 0;
}