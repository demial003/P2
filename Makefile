all : test spell

spell : spell.c
	gcc -o spell spell.c
