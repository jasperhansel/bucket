#include "miscellaneous/common.hxx"


#ifdef BUCKET_CLANG_CRASH_ON_EXCEPTION


#include <csignal>


extern "C" void __cxa_allocate_exception();


void __cxa_allocate_exception()
{
  std::raise(SIGSEGV);
}


#endif
