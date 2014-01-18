

#ifndef __QUICKCPU_H_
#define __QUICKCPU_H_



extern int			nCPU;
extern int			CPUFamily;
extern char			CPUVendor[13];
extern char			CPUBrand[49];
extern bool			CPUCMOV,
					CPU3DNOW,
					CPU3DNOWEXT,
					CPUMMX,
					CPUMMXEXT,
					CPUSSE,
					CPUSSE2;


// Native Win64 development environments have "long long" definition
// In win32, 64 bit integral types are intrinsically specified by __int64
#ifdef _WIN32
	typedef signed	 __int64		int64;
	typedef unsigned __int64		uint64;
#else
	typedef signed	long long		int64;
	typedef unsigned long long		uint64;
#endif


#if defined (_WIN32)
	#define CPUID __asm _emit 0x0F __asm _emit 0xA2
	#define RDTSC __asm _emit 0x0F __asm _emit 0x31

	// Native win64 development environments also dont allow for inline assembly code
	#ifdef _WIN64
	#include <intrin.h>
	#define cpuid(func, a, b, c, d) { int res[4]; __cpuid(res, func); a = res[0]; b = res[1]; c = res[2]; d = res[3]; }

	#else
	void cpuid_asm(unsigned int func, unsigned int* a, unsigned int* b, unsigned int* c, unsigned int* d);
	#define cpuid(func, a, b, c, d) cpuid_asm(func, &a, &b, &c, &d);

	#pragma warning(disable: 4035)
	inline uint64 getCycleNum()
	{
		__asm
		{
			RDTSC
		}
	}
	#endif
#else
    #define cpuid(func, a, b, c, d) cpuid_asm(func, &a, &b, &c, &d);
#endif


#define LENDIAN     0
#define BENDIAN     1

int get_machine_endianness()
{
    int i = 1;
    char* p = (char*)&i;
    if(p[0] == 1)
        return LENDIAN;
    else
        return BENDIAN;
}

//void cpu_init();








#endif
