#pragma once
#if defined(_MSC_VER)
#define MU_NOINLINE __declspec(noinline)
#endif

#if defined(__GNUC__)
#define MU_NOINLINE __attribute__ ((noinline))
#endif

