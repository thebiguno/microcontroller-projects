#include <signal.h>
#include <execinfo.h>
#include <unistd.h>

#include "ESP8266Test.h"

void handler(int sig) {
  void *array[10];
  size_t size = backtrace(array, 10);
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

int main(void) {
	signal(SIGSEGV, handler);
	digitalcave::ESP8266Test().test();
	return 0;
}
