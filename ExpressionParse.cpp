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
bool isType(Token t) { return t == FLOAT || t == INTEGER || t == BOOLEAN || t == CHAR || t == STRING || t == NULL_LIT; }

// <block> ::= { <block statements>? }
// <block statements> ::= <block statement> | <block statements> <block statement>
// <block statement> ::= <local variable declaration> | <class instance creation expression>;
bool block(Tokenizer &tokens, JavaNode *ast) {
  Token t = tokens.next();
  if(t != LEFTBRACKET) throw SyntaxError("Expected { at start of block");

  while(true) {
    if(false) { // if local variable declaration
    } else if(false) { // if class instance creation expression
    } else break;
  }
  
  t = tokens.next();
  if(t != LEFTBRACKET) throw SyntaxError("Expected } at end of block");
  return true;
}

// Note: every appearance of formal parameter list is as ( <formal parameter list>? )
// <formal parameter list> ::= <formal parameter> | <formal parameter list> , <formal parameter>
// <formal parameter> ::= <type> <identifier>
bool formalParameterList(Tokenizer &tokens, JavaNode *ast) {
  if(t != LEFTPAREN) throw SyntaxError("Need ( to begin parameter list");
  t = tokens.next();
  while(t != RIGHTPAREN) {
    if(!isType(t)) throw SyntaxError("Type name needed for parameter");
    t = tokens.next();
    if(t != IDENTIFIER) throw SyntaxError("Unnamed parameter in parameter list");
    t = tokens.next();
    if(t == COMMA) {
      t = tokens.peek();
      if(t != IDENTIFIER) throw SyntaxError("Comma indicates more parameters, but no more found");
      tokens.next();
    }
  }
  return true;
}

// don't get confused - formal parameter list is responsible for the parentheses
// as well as the optionality of the list
// <constructor declaration> ::= <constructor declarator> <block>
// <constructor declarator> ::= <identifier> ( <formal parameter list>? )
bool constructorDecl(Tokenizer &tokens, JavaNode *ast) {
  Token t = tokens.next();
  if(t != IDENTIFIER) throw SyntaxError("Must name constructor");
  formalParameterList(tokens, ast);
  block(tokens, ast);
  return true;
}

// <class member declaration> ::= <field declaration> | <method declaration>
bool classMemberDecl(Tokenizer &tokens, JavaNode *ast) {
  
  return true;
}

// <class body> ::= { <class body declarations> }
// <class body declarations> ::= <class body declaration> | <class body declarations> <class body declaration>
// <class body declaration> ::= <class member declaration> | <static initializer> | <constructor declaration>
bool classBody(Tokenizer &tokens, JavaNode *ast) {
  Token t = tokens.next();
  bool isStatic = false;
  if(t != LEFTBRACKET) throw SyntaxError("{ required to begin class definition");

  while(true) {
    // Alright, we're going to have to do a lot of work to figure out what kind of thing we're working with
    // <class member declaration> starts with static? <type>
    // <static initializer> starts with static {
    // constructor declaration starts with an ID
  
    Tokenizer temp = tokens;
    t = temp.next();
    // first, we deal with the possibility of staticness
    if(t == STATIC) {
      t = temp.next();
      isStatic = true;
      // we'll do something with the AST here eventually
    }
  
    // now check if it's a constructor declaration
    if(t == IDENTIFIER) {
      if(isStatic) throw SyntaxError("Constructor can't be declared as static");
      constructorDecl(tokens, ast);
    } else if(t == LEFTBRACKET) {
      if(!isStatic) throw SyntaxError("You appear to be trying to declare a static block with no static keyword");
      tokens.next(); // clears out the word static
      block(tokens, ast);
    } else if(isType(t) || t == VOID) {
      classMemberDecl(tokens, ast);
    } else break; // basically just avoids a totally unnecessary bool
  }
  t = tokens.next();
  if(t != RIGHTBRACKET) throw SyntaxError("{ has no associated }");
  return true;
}

// <class declaration> ::= class <identifier> <class body>
bool classDeclaration(Tokenizer &tokens, JavaNode *ast) {
  if(tokens.next() != CLASS) throw SyntaxError("Class must begin with 'class' keyword");
  if(tokens.next() != IDENTIFIER) throw SyntaxError("Invalid or no class name");
  return classBody(tokens, ast);
}

/* not currently needed for anything
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
  else if (t == BOOLEAN) {

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
*/

// only tests that the file compiles error-free
class ClassFileTest : public Test {
  string fileName;
 public:
  ClassFileTest(string newName, string newFileName) : Test(newName) {
    fileName = newFileName;
  }
  bool checker() {
    ifstream inf(fileName);
    stringstream ss;
    // I found this online. It's weird, but it basically makes sense if you think about it.
    inf >> ss.rdbuf();
    Tokenizer tokenizer(ss.str(), name);
    return classDeclaration(tokenizer, new JavaNode());    
  }
};

void Tests() {
  tests.push_back(new TTest1());
  tests.push_back(new ClassFileTest("Basic parse test", "test.java"));
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

