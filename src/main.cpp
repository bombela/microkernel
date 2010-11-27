/*
 * main.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

int main();

extern "C" void _start()
{
	// inject asm here, setup a stack etc.
	main();
}

int main()
{
	while (1)
		;
	return 0;
}
