#include "./cpp_debug.h"
#include <stdio.h>
#if _TBSK_DBG==1 //stdio
	#ifdef __cplusplus
	extern "C" {
	#endif
		void dprint_int(const int v){printf("dprint:%d\n",v);};
		void dprint_str(const char* v){printf("dprint:%s\n",v);};
		void dprint_float(const double v){printf("dprint:%f\n",v);};
	#ifdef __cplusplus
	}
	#endif
#elif _TBSK_DBG==2	//extern
	#include <emscripten/bind.h>
	#include <emscripten.h>
	using namespace emscripten;
	#ifdef __cplusplus
	extern "C" {
	#endif
		
		EM_JS(void, dprint_int_, (const int message), {
			console.log("dprint:" + message);
			});
		EM_JS(void, dprint_str_, (const char* message), {
			console.log("dprint:" + message);
			});
		EM_JS(void, dprint_float_, (const double message), {
			console.log("dprint:" + message);
			});
		void dprint_int(const int v) { dprint_int_(v); };
		void dprint_str(const char* v) { dprint_str_(v); };
		void dprint_float(const double v) { dprint_float_(v); };

	#ifdef __cplusplus
	}
	#endif
#endif

