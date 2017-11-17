#include "miscellaneous/common.hxx"


#ifdef BUCKET_CLANG_CRASH_ON_EXCEPTION


#include <csignal>
using namespace std;


extern "C" void __cxa_allocate_exception();


void __cxa_allocate_exception()
{
  raise(SIGSEGV);
}


#endif
