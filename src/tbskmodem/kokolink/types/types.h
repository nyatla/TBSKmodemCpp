
#pragma once
#include <exception>
#include <vector>
#include <cassert>


namespace TBSKmodemCPP
{
	const static double TBSK_PI = 3.1415926535897932384626433832795;

	typedef unsigned char TBSK_BYTE;
	typedef unsigned short TBSK_UINT16;
	typedef unsigned long TBSK_UINT32;
	typedef unsigned long long TBSK_UINT64;
	
	typedef short TBSK_INT16;
	typedef int TBSK_INT32;
	typedef long long TBSK_INT64;

	#define TBSK_SAFE_DELETE(i_object) if((i_object)!=NULL){delete (i_object);(i_object)=NULL;}
	#define TBSK_ASSERT(i_exp) assert(i_exp)

	template <typename T> struct NullableResult{
		bool success;
		T value;
		NullableResult(const NullableResult& nb) :success{ nb.success }, value{nb.value} {
		}
		NullableResult() :success{false}, value{0}
		{
		}

		void set(T value) {
			this->success = true;
			this->value = value;
		}
		void set(){
			this->success = false;
		}
	};




}