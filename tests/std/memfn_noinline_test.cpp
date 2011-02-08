/*
 * memfn_noinline_test.cpp
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

struct Titi {
   	virtual int adder(int, int)	const = 0;
};

struct Tata: Titi {
	int adder(int a, int b) const;
};

int Tata::adder(int a, int b) const { return a + b; }

const Titi& getTata() {
	static Tata r;
	return r;
}
