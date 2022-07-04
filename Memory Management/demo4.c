// Group No- 53
// Suryam Arnav Kalra - 19CS30050
// Kunal Singh - 19CS30025

#include "memlab.h"

int main() {
	createMem(1000);
	createVar("x", INT);
	createVar("y", MEDINT);
	createVar("z", BOOL);
	createVar("c", CHAR);

	assignVar("y", INT, 123);
	assignVar("z", MEDINT, 98);
	assignVar("c", CHAR, '?');
	assignVar("x", INT, 87);

	assignVar("xyz", INT, 54343);
	assignArr("arr", CHAR, '6', 5);
	freeElem("sdjksd");

	assignVar("x", INT, -10);
	printf("value of x: %d\n", getVar("x"));

	assignVar("y", MEDINT, 1<<25);

	printf("Value of c: %c\n", (char)getVar("c"));

}