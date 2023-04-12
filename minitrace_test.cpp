#include "minitrace.h"

#ifdef _WIN32
#include <windows.h>
#define usleep(x) Sleep(x/1000)
#else
#include <unistd.h>
#endif

void c() {
	MTR_SCOPE("c++", "c()");
	usleep(10000);
}

void b() {
	MTR_SCOPE("c++", "b()");
	usleep(20000);
	c();
	usleep(10000);
}

void a() {
	MTR_SCOPE("c++", "a()");
	usleep(20000);
	b();
	usleep(10000);
}

int main() {
	int i;
	mtr_init("trace.json", INTERNAL_MINITRACE_BUFFER_SIZE);
	mtr_register_sigint_handler();

	MTR_META_PROCESS_NAME("minitrace_test");
	MTR_META_THREAD_NAME("main thread");

	int long_running_thing_1;
	int long_running_thing_2;

	MTR_START("background", "long_running", &long_running_thing_1);
	MTR_START("background", "long_running", &long_running_thing_2);

	MTR_COUNTER("main", "greebles", 3);
	MTR_BEGIN("main", "outer");
	usleep(80000);
	MTR_BEGIN1("main", "foo", MTR_ARG_I("value", 42));
	usleep(80000);
	for (i = 0; i < 3; i++) {
		MTR_BEGIN("main", "inner");
		usleep(40000);
		MTR_END("main", "inner");
		usleep(10000);
		MTR_COUNTER("main", "greebles", 3 * i + 10);
	}
	MTR_END("main", "foo");
	MTR_STEP("background", "long_running", &long_running_thing_1, "middle step");
	usleep(80000);
	MTR_SINSTANT1("background", "long_running",  MTR_ARG_C("string val", "Something happened!"));
	usleep(80000);
	MTR_INSTANT1("background", "long_running",  MTR_ARG_I("int val", 223344));
	usleep(80000);
	MTR_END("main", "outer");
	MTR_INSTANT("main", "an instant");
	MTR_COUNTER("main", "greebles", 0);

	MTR_BEGIN1("main", "foo", MTR_ARG_C("message", "hello"));
	usleep(10000);
	a();
	usleep(100000);
	MTR_END("main", "foo");
	usleep(50000);
	MTR_COUNTER1("main", "greebles", MTR_ARG_I("greebles", 100));
	usleep(50000);
	MTR_SINSTANT("main", "the end");
	usleep(10000);
	MTR_SINSTANT_C("main", "the end", "value", "A char constant");
	usleep(10000);
	MTR_SINSTANT_I("main", "the end", "value", 1742);
	MTR_COUNTER1("main", "greebles", MTR_ARG_I("greebles", 5));
	usleep(10000);
	MTR_FINISH("background", "long_running", &long_running_thing_1);
	MTR_FINISH("background", "long_running", &long_running_thing_2);

	mtr_flush();
	mtr_shutdown();
	return 0;
}
