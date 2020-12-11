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
// <block statement> ::= <variable declaration>; | <class instance creation expression>;
// <variable declaration> ::= <type>? <variable declarator>
// <variable declarator> ::= <identifier> = <literal>
// <class instance creation expression> ::= new <identifier> ( <identifier list> )
// <identifier list> ::= <identifier> | <identifier> , <identifier list>
bool block(Tokenizer &tokens, JavaNode *ast) {
  Token t = tokens.next();
  if(t != LEFTBRACKET) throw SyntaxError("Expected { at start of block");

  while(true) {
    t = tokens.peek();
    if(isType(t) || t == IDENTIFIER) { // if local variable declaration
      if(isType(t)) {
        t = tokens.next();
      }
      if(t != IDENTIFIER) throw SyntaxError("Variable declaration needs identifier");
      t = tokens.next();
      if(t != EQUALS) throw SyntaxError("Variable must be initialized");
      t = tokens.next();
      if(t != LITERAL) throw SyntaxError("Variable must be initialized");
    } else if(t == NEW) { // if class instance creation expression
      t = tokens.next();
      if(t != IDENTIFIER) throw SyntaxError("No identifier named in constructor call");
      t = tokens.next();
      if(t != LEFTPAREN) throw SyntaxError("No ( in constructor call");
      t = tokens.peek();
      while(t != RIGHTPAREN) {
        t = tokens.next();
        if(t != IDENTIFIER) throw SyntaxError("Need identifier in argument list for constructor call");
        t = tokens.next();
        if(t == COMMA) {
          t = tokens.peek();
          if(t != IDENTIFIER) throw SyntaxError("Comma indicates more parameters, but no more found");
        }
      }
    } else break;
    t = tokens.next();
    if(t != SEMICOLON) throw SyntaxError("Missing ; in field declaration");      
  }
  
  t = tokens.next();
  if(t != LEFTBRACKET) throw SyntaxError("Expected } at end of block");
  return true;
}

// Note: every appearance of formal parameter list is as ( <formal parameter list>? )
// <formal parameter list> ::= <formal parameter> | <formal parameter list> , <formal parameter>
// <formal parameter> ::= <type> <identifier>
bool formalParameterList(Tokenizer &tokens, JavaNode *ast) {
  Token t = tokens.next();
  if(t != LEFTPAREN) throw SyntaxError("Need ( to begin parameter list");
  t = tokens.next();
  while(t != RIGHTPAREN) {
    if(!isType(t)) throw SyntaxError("Type name needed for parameter");
    t = tokens.next();
    if(t != IDENTIFIER) throw SyntaxError("Unnamed parameter in parameter list");
    t = tokens.next();
    if(t == COMMA) {
      t = tokens.peek();
      if(!isType(t)) throw SyntaxError("Comma indicates more parameters, but no more found");
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

// <field declaration> ::= static? <type> <variable declarator> ;
// <variable declarator> ::= <identifier> = <literal>
bool fieldDecl(Tokenizer &tokens, JavaNode *ast) {
  Token t = tokens.next();
  bool isStatic = false;
  if(t == STATIC) {
    isStatic = true;
    t = tokens.next();
  }
  if(!isType(t)) throw SyntaxError("Field declaration requires type");
  t = tokens.next();
  if(t != IDENTIFIER) throw SyntaxError("Field declaration needs identifier");
  t = tokens.next();
  if(t != EQUALS) throw SyntaxError("Field must be initialized");
  t = tokens.next();
  if(t != LITERAL) throw SyntaxError("Field must be initialized");
  t = tokens.next();
  if(t != SEMICOLON) throw SyntaxError("Missing ; in field declaration");
  return true;
}

// <method declaration> ::= <method header> <method body>
// <method header> ::= static? <result type> <method declarator>
// <result type> ::= <type> | void
// remember - formal parameter list handles the parentheses and the optionality of the parameters
// <method declarator> ::= <identifier> ( <formal parameter list>? )
// <method body> ::= <block> | ;
bool methodDecl(Tokenizer &tokens, JavaNode *ast) {
  Token t = tokens.next();
  bool isStatic = false;
  if(t == STATIC) {
    isStatic = true;
    t = tokens.next();
  }
  if(!(isType(t) || t == VOID)) throw SyntaxError("Method declaration needs type");
  t = tokens.next();
  if(t != IDENTIFIER) throw SyntaxError("Method requires identifier");
  t = tokens.next();
  formalParameterList(tokens, ast);
  t = tokens.peek();
  if(t == LEFTBRACKET) {
    block(tokens, ast);
  } else if(t == SEMICOLON) ; // nothing, you're done
  else throw SyntaxError("Need ; for method declaration without body");
  return true;
}

// <class member declaration> ::= <field declaration> | <method declaration>
bool classMemberDecl(Tokenizer &tokens, JavaNode *ast) {
  // both start with "static? type identifier" so we want to skip 3
  bool isVoid = false;
  Tokenizer temp = tokens;
  Token type = temp.next();
  Token t;
  if(type == STATIC) {
    type = temp.next();
  }
  if(!isType(type) && type != VOID) {
    tokens = temp; // in case you want to output tokens to help debug    
    throw SyntaxError("Type required when declaring member");
  }
  t = temp.next();
  if(t != IDENTIFIER) {
    tokens = temp;
    throw SyntaxError("Identifier required when declaring member");
  }
  t = tokens.peek();
  if(t == EQUALS) {
    if(type == VOID) throw SyntaxError("Field cannot be of type 'void'");
    fieldDecl(tokens, ast);
  } else if(t == LEFTBRACKET) {
    methodDecl(tokens, ast);
  } else throw SyntaxError("Malformed class member: must be a method or field declaration");
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
    try {
      classDeclaration(tokenizer, new JavaNode());    
    }
    catch(SyntaxError e) {
      cout << "  " << e;
      //tokenizer.check();
      return false;
    }
    return true;
  }
};

void Tests() {
  tests.push_back(new TTest1());
  tests.push_back(new ClassFileTest("Basic parse test", "test.java"));
  //tests.push_back(new ClassFileTest("Full parse test", "fulltest.java"));  
  runTests();
}

int main(int argc, char **argv) {
  Tests();
  return 0;
}

