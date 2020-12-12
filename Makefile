CPP=g++
CPPFLAGS=-Wall -g -std=gnu++1z
MAINSRC=ExpressionParse.cpp
OBJS=Testing.o Tokenizer.o AST.o SymbolTable.o

ExpressionParse: $(OBJS) $(MAINSRC) Syntax.h
	$(CPP) -o $@ $(OBJS) $(MAINSRC) $(CPPFLAGS)

AST.o: AST.cpp AST.h
	$(CPP) -c -o $@ $< $(CPPFLAGS)

Testing.o : Testing.cpp Testing.h
	$(CPP) -c -o $@ $< $(CPPFLAGS)

SymbolTable.o : SymbolTable.cpp SymbolTable.h
	$(CPP) -c -o $@ $< $(CPPFLAGS)

Tokenizer.o : Tokenizer.cpp Tokenizer.h 
	$(CPP) -c -o $@ $< $(CPPFLAGS)
