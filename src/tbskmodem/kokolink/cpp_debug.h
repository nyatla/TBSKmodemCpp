#pragma once
#ifndef _TBSK_DBG
	#define _TBSK_DBG 0
#endif
#if _TBSK_DBG ==0
	#define dprint_int(v)
	#define dprint_str(v)
	#define dprint_float(v)
#elif _TBSK_DBG==1 || _TBSK_DBG==2//stdio
	#ifdef __cplusplus
	extern "C" {
	#endif
		void dprint_int(const int v);
		void dprint_str(const char* v);
		void dprint_float(const double v);
	#ifdef __cplusplus
	}
	#endif
#endif
