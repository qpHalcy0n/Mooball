
#ifdef __WIN32
    #include <windows.h>
#endif
#include "quickcpu.h"

int				nCPU;
int				CPUFamily;
char			CPUVendor[13];
char			CPUBrand[49];

bool			CPUCMOV,
				CPU3DNOW,
				CPU3DNOWEXT,
				CPUMMX,
				CPUMMXEXT,
				CPUSSE,
				CPUSSE2;

// CPUID pushes processor information into EAX, EBX, ECX, and EDX based on the
// value of eax
void cpuid_asm(unsigned int func, unsigned int* a, unsigned int* b, unsigned int* c, unsigned int* d)
{
    #ifdef __WIN32
        __asm
        {
            // Save values of EAX-EDX across the stack //
            push	eax
            push	ebx
            push	ecx
            push	edx

            // Push instruction into eax and invoke CPUID //
            mov		eax, func
            CPUID

            // Move data out into a, b, c, d //
            mov		edi, a
            mov		[edi], eax
            mov		edi, b
            mov		[edi], ebx
            mov		edi, c
            mov		[edi], ecx
            mov		edi, d
            mov		[edi], edx

            // Restore EAX-EDX //
            pop		edx
            pop		ecx
            pop		ebx
            pop		eax
        }

    #else
        asm(".intel_syntax noprefix\n");
        asm("push   [eax]\n");
        asm("push   [ebx]\n");
        asm("push   [ecx]\n");
        asm("push   [edx]\n");
        asm("mov    eax, func\n");
        asm("CPUID\n");
        asm("mov    edi, a\n");
        asm("mov    [edi], eax\n");
        asm("mov    edi, b\n");
        asm("mov    [edi], ebx\n");
        asm("mov    edi, c\n");
        asm("mov    [edi], ecx\n");
        asm("mov    edi, d\n");
        asm("mov    [edi], edx\n");
        asm("pop    [edx]\n");
        asm("pop    [ecx]\n");
        asm("pop    [ebx]\n");
        asm("pop    [eax]\n");
        asm(".att_syntax noprefix\n");
    #endif
}

void cpu_init()
{
#if defined(_WIN32)
	SYSTEM_INFO sInfo;
	GetSystemInfo(&sInfo);
	nCPU = sInfo.dwNumberOfProcessors;
#endif

	if(nCPU < 1) nCPU = 1;

	unsigned int maxi, maxei, a, b, c, d;
	CPUVendor[12] = '\0';
	CPUBrand[0]  = '\0';

	CPUCMOV		= false;
	CPUMMX		= false;
	CPUSSE		= false;
	CPUSSE2		= false;
	CPU3DNOW	= false;
	CPU3DNOWEXT = false;
	CPUMMXEXT	= false;
	CPUFamily	= 0;

	// Code 00H returns vendor information and maximum input value for CPID into maxi //
	cpuid(0, maxi, ((unsigned int*)CPUVendor)[0], ((unsigned int*)CPUVendor)[2], ((unsigned int*)CPUVendor)[1]);


	if(maxi >= 1)
	{
		// Code 01H returns versioning info into a, and otherwise we're only concerned with d //
		// Which is Feature information //
		cpuid(1, a, b, c, d);
		CPUCMOV		 = (d & 0x00008000) != 0;
		CPUMMX		 = (d & 0x00800000) != 0;
		CPUSSE		 = (d & 0x02000000) != 0;
		CPUSSE2		 = (d & 0x04000000) != 0;
		CPUFamily    = (a >> 8) & 0x0F;


		// Code 80000000H is reserved for other CPU vendors, in this case AMD //
		cpuid(0x80000000, maxei, b, c, d);
		if(maxei >= 0x80000001)
		{
			cpuid(0x80000001, a, b, c, d);
			CPU3DNOW		= (d & 0x80000000) != 0;
			CPU3DNOWEXT		= (d & 0x40000000) != 0;
			CPUMMXEXT		= (d & 0x00400000) != 0;

			if(maxei >= 0x80000002)
			{
				cpuid(0x80000002, ((unsigned int*)CPUBrand)[0], ((unsigned int*)CPUBrand)[1], ((unsigned int*)CPUBrand)[2], ((unsigned int*)CPUBrand)[3]);
				CPUBrand[16] = '\0';

				if(maxei > 0x80000003)
				{
					cpuid(0x80000003, ((unsigned int*)CPUBrand)[4], ((unsigned int*)CPUBrand)[5], ((unsigned int*)CPUBrand)[6], ((unsigned int*)CPUBrand)[7]);
					CPUBrand[32] = '\0';

					if(maxei >= 0x80000004)
					{
						cpuid(0x80000004, ((unsigned int*)CPUBrand)[8], ((unsigned int*)CPUBrand)[9], ((unsigned int*)CPUBrand)[10], ((unsigned int*)CPUBrand)[11]);
						CPUBrand[48] = '\0';
					}
				}
			}
		}
	}
}
