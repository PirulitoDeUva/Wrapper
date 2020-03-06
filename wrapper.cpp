#include <windows.h>
#include <tchar.h>

#define ORIGINAL_DLL_PATH %temp%\\JPGLib.dll

typedef void (*PirulexInPeterex)(void);

#define TO_STR(A) _T(#A)

#define WRAPPER(A,B,C,D) PirulexInPeterex __##B=NULL;
#include "wrapper.lst"
#undef WRAPPER

#if defined(__GNUC__)
#pragma GCC push_options
#pragma GCC optimize("-O2")
#elif defined(_MSC_VER)
#pragma optimize("g", on)
#endif

#ifdef __cplusplus
extern "C" {
#endif
#define WRAPPER(A,B,C,D) void B(void) { __##B(); }
#include "wrapper.lst"
#undef WRAPPER
#ifdef __cplusplus
}
#endif

#if defined(__GNUC__)
#pragma GCC pop_options
#define WRAPPER(A,B,C,D) asm(".section .drectve\n.ascii \" -export:" #C "=" #B " @" #D "\"\n.section .text");
#include "wrapper.lst"
#undef WRAPPER
#elif defined(_MSC_VER)
#pragma optimize("", on)
#define WRAPPER(A,B,C,D) __pragma(comment(linker, "/export:" #C "=_" #B ",@" #D))
#include "wrapper.lst"
#undef WRAPPER
#endif

TCHAR *TranslateLibraryPath(const TCHAR *path)
{
	static TCHAR translated[MAX_PATH] = {0};
	ExpandEnvironmentStrings(path, translated, _countof(translated));
	return translated;
}

#define WRAPPER(A,B,C,D) __##B=(PirulexInPeterex)GetProcAddress(LoadLibrary(TranslateLibraryPath(TO_STR(A))), #C);
void Wrapper(void)
{
#include "wrapper.lst"
}
#undef WRAPPER

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if(fdwReason==DLL_PROCESS_ATTACH) Wrapper();
	return TRUE;
}

#if defined(__GNUC__) && defined(NOSTDLIB)
#ifdef __cplusplus
extern "C" void *cpp_new(size_t s) { return malloc(s); }
extern "C" void cpp_delete(void *p) { free(p); }
void *operator new(size_t s) __attribute__((alias("cpp_new")));
void *operator new[](size_t s) __attribute__((alias("cpp_new")));
void operator delete(void *p) __attribute__((alias("cpp_delete")));
void operator delete[](void *p) __attribute__((alias("cpp_delete")));
#endif

EXTERN_C BOOL WINAPI DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) { return DllMain(hinstDLL, fdwReason, lpvReserved); }
#endif
