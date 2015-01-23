all: group compiler

group:
	@echo "Yu Liu"

compiler: Micro 

LEX = flex

YACC = bison

CC = g++ -std=gnu++0x

src = src

gen = generated

bld = build

Micro: $(bld)/lex.yy.o $(bld)/Micro.tab.o $(bld)/symtable.o $(bld)/ast.o | $(bld) 
	$(CC) $(bld)/lex.yy.o $(bld)/Micro.tab.o $(bld)/symtable.o $(bld)/ast.o -o $(bld)/Micro
$(bld)/lex.yy.o: $(gen)/lex.yy.c $(gen)/Micro.tab.h | $(bld)
	$(CC) -o $(bld)/lex.yy.o -c $(gen)/lex.yy.c
$(bld)/Micro.tab.o: $(gen)/Micro.tab.c | $(bld)
	$(CC) -o $(bld)/Micro.tab.o -c $(gen)/Micro.tab.c
$(bld)/ast.o: $(src)/ast.cpp $(src)/ast.h | $(bld)
	$(CC) -o $(bld)/ast.o -c $(src)/ast.cpp
$(bld)/symtable.o: $(src)/symtable.cpp $(src)/symtable.h | $(bld)
	$(CC) -o $(bld)/symtable.o -c $(src)/symtable.cpp
$(gen)/Micro.tab.h $(gen)/Micro.tab.c: $(src)/Micro.y | $(gen)
	$(YACC) -o $(gen)/Micro.tab.h -o $(gen)/Micro.tab.c -d $(src)/Micro.y
$(gen)/lex.yy.c: $(src)/Micro.l | $(gen)
	$(LEX) -o$(gen)/lex.yy.c $(src)/Micro.l
$(gen):
	mkdir $(gen)
$(bld):
	mkdir $(bld)
	
clean:
	rm -rf $(bld) $(gen)