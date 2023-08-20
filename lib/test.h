#pragma once

void test1(void* in1, void *output, void* (*const func)(), int (*const compare)());

void test2(void* in1, void* in2, void *output, void* (*const func)(), int (*const compare)());

void test3(void* in1, void* in2, void* in3, void *output, void* (*const func)(), int (*const compare)());
