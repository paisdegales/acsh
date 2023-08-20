#include "test.h"
#include <stdio.h>

void test1(void* in1, void *expected_output, void* (*const func)(), int (*const compare)()){
	void* output = func(in1);
	int res = compare(output, expected_output);
	char const * str = res? "TEST PASSED" : "TEST NOT PASSED";
	printf("%s", str);
}

void test2(void* in1, void* in2, void *expected_output, void* (*const func)(), int (*const compare)()){
	void* output = func(in1, in2);
	int res = compare(output, expected_output);
	char const * str = res? "TEST PASSED" : "TEST NOT PASSED";
	printf("%s", str);
}

void test3(void* in1, void* in2, void* in3, void *expected_output, void* (*const func)(), int (*const compare)()){
	void* output = func(in1, in2, in3);
	int res = compare(output, expected_output);
	char const * str = res? "TEST PASSED" : "TEST NOT PASSED";
	printf("%s", str);
}
