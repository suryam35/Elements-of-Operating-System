// Group No- 53
// Suryam Arnav Kalra - 19CS30050
// Kunal Singh - 19CS30025

#include "memlab.h"

void func1() {
	createArr("a", INT, 50000);
	createVar("i", INT);
	
	for(assignVar("i", INT, 0); getVar("i") < 50000; assignVar("i", INT, getVar("i")+1)) {
		
		assignArr("a", INT, rand()%10000, getVar("i"));
	}

	freeElem("a");
	freeElem("i");
	gc_run();
}

void func2() {
	createArr("a", MEDINT, 50000);
	createVar("i", INT);
	
	for(assignVar("i", INT, 0); getVar("i") < 50000; assignVar("i", INT, getVar("i")+1)) {
		
		assignArr("a", MEDINT, rand()%10000, getVar("i"));
	}

	freeElem("a");
	freeElem("i");
	gc_run();
}

void func3() {
	createArr("a", CHAR, 50000);
	createVar("i", INT);
	
	for(assignVar("i", INT, 0); getVar("i") < 50000; assignVar("i", INT, getVar("i")+1)) {
		
		assignArr("a", CHAR, rand()%26 + 'a', getVar("i"));
	}

	freeElem("a");
	freeElem("i");
	gc_run();
}

void func4() {
	createArr("a", BOOL, 50000);
	createVar("i", INT);
	
	for(assignVar("i", INT, 0); getVar("i") < 50000; assignVar("i", INT, getVar("i")+1)) {
		
		assignArr("a", BOOL, rand()%2, getVar("i"));
	}

	freeElem("a");
	freeElem("i");
	gc_run();
}

void func5() {
	createArr("a", INT, 50000);
	createVar("i", INT);
	
	for(assignVar("i", INT, 0); getVar("i") < 50000; assignVar("i", INT, getVar("i")+1)) {
		
		assignArr("a", INT, rand()%10000, getVar("i"));
	}

	freeElem("a");
	freeElem("i");
	gc_run();
}

void func6() {
	createArr("a", MEDINT, 50000);
	createVar("i", INT);
	
	for(assignVar("i", INT, 0); getVar("i") < 50000; assignVar("i", INT, getVar("i")+1)) {
		
		assignArr("a", MEDINT, rand()%10000, getVar("i"));
	}

	freeElem("a");
	freeElem("i");
	gc_run();
}

void func7() {
	createArr("a", CHAR, 50000);
	createVar("i", INT);
	
	for(assignVar("i", INT, 0); getVar("i") < 50000; assignVar("i", INT, getVar("i")+1)) {
		
		assignArr("a", CHAR, rand()%26 + 'a', getVar("i"));
	}

	freeElem("a");
	freeElem("i");
	gc_run();
}

void func8() {
	createArr("a", BOOL, 50000);
	createVar("i", INT);
	
	for(assignVar("i", INT, 0); getVar("i") < 50000; assignVar("i", INT, getVar("i")+1)) {
		
		assignArr("a", BOOL, rand()%2, getVar("i"));
	}

	freeElem("a");
	freeElem("i");
	gc_run();
}

void func9() {
	createArr("a", INT, 50000);
	createVar("i", INT);
	
	for(assignVar("i", INT, 0); getVar("i") < 50000; assignVar("i", INT, getVar("i")+1)) {
		
		assignArr("a", INT, rand()%10000, getVar("i"));
	}

	freeElem("a");
	freeElem("i");
	gc_run();
}

void func10() {
	createArr("a", MEDINT, 50000);
	createVar("i", INT);
	
	for(assignVar("i", INT, 0); getVar("i") < 50000; assignVar("i", INT, getVar("i")+1)) {
		
		assignArr("a", MEDINT, rand()%10000, getVar("i"));
	}

	freeElem("a");
	freeElem("i");
	gc_run();
}


int main() {
	int sz = 250000000;
	createMem(sz);
	createVar("x1", INT);
	createVar("y1", INT);
	createVar("x2", MEDINT);
	createVar("y2", MEDINT);
	createVar("x3", CHAR);
	createVar("y3", CHAR);
	createVar("x4", BOOL);
	createVar("y4", BOOL);

	func1();
	func2();
	func3();
	func4();
	func5();
	func6();
	func7();
	func8();
	func9();
	func10();

}