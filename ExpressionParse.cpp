#include <iostream>
#include <regex>
#include <sstream>
#include <fstream>
#include <vector>
#include <fstream>
#include <sstream>

#include "Testing.h"
#include "Tokenizer.h"
#include "Syntax.h"
#include "AST.h"

using namespace std;

bool javaExpression(Tokenizer &tokens, JavaNode *ast);

// <class body> ::= { <class body declarations> }
// <class body declarations> ::= <class body declaration> | <class body declarations> <class body declaration>
// <class body declaration> ::= <class member declaration> | <static initializer> | <constructor declaration>
bool classBody(Tokenizer &tokens, JavaNode *ast) {
  if(tokens.next() != LEFTBRACKET) throw SyntaxError("{ required to begin class definition");
  // need class body decls
  if(tokens.next() != RIGHTBRACKET) throw SyntaxError("{ has no associated }");
  return true;
}

// <class declaration> ::= class <identifier> <class body>
bool classDeclaration(Tokenizer &tokens, JavaNode *ast) {
  if(tokens.next() != CLASS) throw SyntaxError("Class must begin with 'class' keyword");
  if(tokens.next() != IDENTIFIER) throw SyntaxError("Invalid or no class name");
  return classBody(tokens, ast);
}

bool javaExpression(Tokenizer &tokens, JavaNode *ast) {
  Token t = tokens.peek();

  Tokenizer temp = tokens;

  if (t == CLASS) {
    return classDeclaration(tokens, ast);
  }
  else if (t == STATIC) {
    
  }
  else if (t == INTEGER) {

  }
  else if (t == FLOAT) {

  }
  else if (t == BOOL) {

  }
  else if (t == CHAR) {

  }
  else if (t == STRING) {

  }

  return false;
}

//Would expression helper be a void function?
void javaExpressionHelper(string texts, string filename, int linenumber) {
  Tokenizer tokens(texts, filename, linenumber);
  JavaNode *root = new JavaNode(EXPRESSION);
  try {
    javaExpression(tokens, root);
    delete root;
  } catch(SyntaxError e) {
    tokens.check();
    throw e;
  }
}

class ParseTest:public Test{
public:
  ParseTest():Test("Basic class") {
  }
  bool checker() {
    ifstream inf("test.java");
    stringstream ss;
    // I found this online. It's weird, but it basically makes sense if you think about it.
    inf >> ss.rdbuf();
    Tokenizer tokenizer(ss.str(), "ParseTest");
    return classDeclaration(tokenizer, new JavaNode());
  } 
};

void Tests() {
  tests.push_back(new TTest1());
  tests.push_back(new ParseTest());
  try {
    runTests();
  }
  catch (SyntaxError e) {
    cout << "  " << e;
  }
}

int main(int argc, char **argv) {
  Tests();
  return 0;
}

