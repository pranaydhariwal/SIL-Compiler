all:	build compile

build:
		lex expl.l
		yacc -d expl.y

compile:	lex.yy.c y.tab.c
		gcc lex.yy.c y.tab.c -o exe

run:
		./exe

clean:
		rm lex.yy.c y.tab.c exe y.tab.h
