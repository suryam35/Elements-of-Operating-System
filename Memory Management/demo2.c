// Group No- 53
// Suryam Arnav Kalra - 19CS30050
// Kunal Singh - 19CS30025

#include "memlab.h"

void fibonacci() {
	createArr("arr", INT, getVar("k"));
	assignArr("arr", INT, 0, 0);
	assignArr("arr", INT, 1, 1);
	createVar("x", INT);
	createVar("y", INT);
	createVar("i", INT);
	
	for(assignVar("i", INT, 2); getVar("i") < getVar("k"); assignVar("i", INT, getVar("i")+1)) {
		
		assignVar("x", INT, getArrElem("arr", getVar("i")-1));
		
		assignVar("y", INT, getArrElem("arr", getVar("i")-2));
		assignArr("arr", INT, getVar("x")+getVar("y"), getVar("i"));
	}
	

	freeElem("x");
	freeElem("y");
	freeElem("i");
	// compactPhase();
	gc_run();
}

int fibonacciProduct(){

	fibonacci();

	createVar("i", INT);
	createVar("ans", INT);
	assignVar("ans",INT,1);

	// cout << "First " << getVar("k") << " fibonacci numbers are : ";
	// printf("First %d fibnonacci numbers are: ", getVar("k"));

	// for(assignVar("i", INT, 0); getVar("i") < getVar("k"); assignVar("i", INT, getVar("i")+1)) {
	// 	printf("%d ", getArrElem("arr", getVar("i")));
	// }

	// cout << "\n";
	// printf("\n");

	for(assignVar("i", INT, 1); getVar("i") < getVar("k"); assignVar("i", INT, getVar("i")+1)) {
		assignVar("ans", INT, getVar("ans")*getArrElem("arr", getVar("i")));
		// cout << getVar("ans") << endl;

	}
	// cout << "The product is : ";
	freeElem("i");
	freeElem("arr");
	// compactPhase();
	gc_run();
	return getVar("ans");
}

int main() {
	createMem(1000);
	createVar("k", INT);

	assignVar("k", INT, 10);

	printf("The product is: %d\n", fibonacciProduct());
	// createMem(100);
	// func();
	// cout << endl << endl;
	// func();
}