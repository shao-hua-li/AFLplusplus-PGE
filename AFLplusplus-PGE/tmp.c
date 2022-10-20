#include<stdio.h>
int a, b;

int foo(int x) {
	return x > a ? a : b;
}

int main() {
	int a = 0;
	int b = 1;
	return foo(a+b);
}
