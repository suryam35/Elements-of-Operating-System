// Group No- 53
// Suryam Arnav Kalra - 19CS30050
// Kunal Singh - 19CS30025

#include "memlab.h"

int main() {
	createMem(12);
	createVar("x", INT);
	createVar("y", INT);
	createVar("z", INT);
	createVar("a", INT);
	freeElem("y");
	gc_run();
	createVar("a", INT);
	freeElem("x");
	freeElem("a");
	gc_run();
	createArr("arr", INT, 3);
	createArr("arr", INT, 2);
}