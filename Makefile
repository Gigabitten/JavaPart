CPP=g++
CPPFLAGS=-Wall -g 
MAINSRC=ExpressionParse.cpp
OBJS=Testing.o Tokenizer.o AST.o

ExpressionParse: $(OBJS) $(MAINSRC) Syntax.h
	$(CPP) -o $@ $(OBJS) $(MAINSRC) $(CPPFLAGS)

AST.o: AST.cpp AST.h
	$(CPP) -c -o $@ $< $(CPPFLAGS)

Testing.o : Testing.cpp Testing.h
	$(CPP) -c -o $@ $< $(CPPFLAGS)

Tokenizer.o : Tokenizer.cpp Tokenizer.h 
	$(CPP) -c -o $@ $< $(CPPFLAGS)
