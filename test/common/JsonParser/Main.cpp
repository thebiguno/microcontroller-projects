#include <signal.h>
#include <execinfo.h>
#include <unistd.h>

#include "JsonParserTest.h"

void handler(int sig) {
  void *array[10];
  size_t size = backtrace(array, 10);
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

int main() {
	signal(SIGSEGV, handler);
	signal(SIGABRT, handler);
	digitalcave::JsonParserTest().test();
}
