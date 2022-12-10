#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "../../../src/tbskmodem/tbskmodem.h"
#include <cmath>
using namespace TBSKmodemCPP; 
using namespace std;

int main()
{
	{
		vector<double> a(16000);
		for (auto i = 0;i < a.size();i++) {
			a[i] = std::sin(i * 3.1415 * 2 / 80);
		}
		{
			FileWriter fw("./test.wav");
			PyIterator<double> b(a);
			PcmData pcm(b, 16, 16000);
			PcmData::Dump(pcm, fw);
		}
		{
			FileReader fr("./test.wav");
			PcmData pcm(fr);
			FileWriter fw("./test2.wav");
			PcmData::Dump(pcm, fw);

		}
	}
	
	_CrtDumpMemoryLeaks();
	return 0;
}
