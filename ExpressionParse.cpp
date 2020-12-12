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
#include "SymbolTable.h"

using namespace std;

bool javaExpression(Tokenizer &tokens, JavaNode *ast);

const Token types[] = { FLOAT, INTEGER, BOOLEAN, CHAR, STRING, BYTE, SHORT, LONG, DOUBLE };
// <type> ::= <primitive type> | <identifier>
// <primitive type> ::= byte | short | int | long | char | float | double | boolean
bool isType(Token t, string name = "") {
  if(name == "Ford") return true; // substitute for symbol table lookup
  for(int i = 0; i < sizeof(types) / sizeof(types[0]); i++) if(types[i] == t) return true;
  return false;
}

const Token litTokens[] = { FLOAT_LIT, INT_LIT, BOOL_LIT, CHAR_LIT, STRING_LIT, NULL_LIT, };
bool isLiteral(Token t) {
  for(int i = 0; i < sizeof(litTokens) / sizeof(litTokens[0]); i++) if(litTokens[i] == t) return true;
  return false;
}

// <block> ::= { <block statements>? }
// <block statements> ::= <block statement> | <block statements> <block statement>
// <block statement> ::= <variable declaration>;
// <variable declaration> ::= <type>? <variable declarator>
// <variable declarator> ::= <identifier> = <literal> | <class instance creation expression>
// <class instance creation expression> ::= new <identifier> ( <identifier list> )
// <identifier list> ::= <identifier> | <identifier> , <identifier list>
bool block(Tokenizer &tokens, JavaNode *ast) {
  Token t = tokens.next();
  if(t != LEFTBRACKET) throw SyntaxError("Expected { at start of block", tokens.location());
  bool isDecl = false;

  JavaNode *blook = new JavaNode(BLOCK);
  while(true) {
    t = tokens.peek();
    if(isType(t, tokens.name) || t == IDENTIFIER) { // if local variable declaration
      tokens.next();
      if(isType(t, tokens.name)) {
        t = tokens.next();
        isDecl = true;
        if(t != IDENTIFIER) throw SyntaxError("Variable declaration needs identifier", tokens.location());
      }
      JavaNode *blookMem = new JavaNode(FIELD);
      blookMem->setName(tokens.name);
      t = tokens.next();
      if(t != EQUALS) throw SyntaxError("Expected '='", tokens.location());
      t = tokens.next();
      if(isLiteral(t)) {
        JavaNode *litTemp = new JavaNode(t);
        litTemp -> setName(tokens.name);
        blookMem->addChild(litTemp);
        if(isDecl) {
          if(symbolTable.exists(blookMem)) throw SemanticError("Can't add duplicate", tokens.location());
          symbolTable.add(blookMem, litTemp);
        }
      } else if(t == NEW) { // if class instance creation expression
        t = tokens.next();
        if(t != IDENTIFIER) throw SyntaxError("No identifier named in constructor call", tokens.location());
        
        JavaNode *blookExpr = new JavaNode(EXPRESSION);

        t = tokens.next();
        if(t != LEFTPAREN) throw SyntaxError("No ( in constructor call", tokens.location());
        t = tokens.peek();
        while(t != RIGHTPAREN) {
          t = tokens.next();
          if(t != IDENTIFIER) throw SyntaxError("Need identifier in argument list for constructor call", tokens.location());
          JavaNode *parmTemp = new JavaNode(PARAMETER);
          parmTemp -> setName(tokens.name);
          blookExpr->addChild(parmTemp);
          t = tokens.next();
          if(t == COMMA) {
            t = tokens.peek();
            if(t != IDENTIFIER) throw SyntaxError("Comma indicates more parameters, but no more found", tokens.location());
          }
        }
        blookMem->addChild(blookExpr);
        if(isDecl) {
          if(symbolTable.exists(blookMem)) throw SemanticError("Can't add duplicate", tokens.location());
          symbolTable.add(blookMem, blookExpr);
        }
      } else throw SyntaxError("Expected rvalue in variable assignment", tokens.location());
      blook->addChild(blookMem);
      t = tokens.next();
      if(t != SEMICOLON) throw SyntaxError("Missing ; in field declaration", tokens.location());            
    } else break;
  ast->addChild(blook);
  }
  
  t = tokens.next();
  if(t != RIGHTBRACKET) throw SyntaxError("Expected } at end of block", tokens.location());
  return true;
}

// Note: every appearance of formal parameter list is as ( <formal parameter list>? )
// <formal parameter list> ::= <formal parameter> | <formal parameter list> , <formal parameter>
// <formal parameter> ::= <type> <identifier>
bool formalParameterList(Tokenizer &tokens, JavaNode *ast) {
  Token t = tokens.next();
  if(t != LEFTPAREN) throw SyntaxError("Need ( to begin parameter list", tokens.location());
  t = tokens.next();
  while(t != RIGHTPAREN) {
    if(!isType(t)) throw SyntaxError("Type name needed for parameter", tokens.location());
    t = tokens.next();
    if(t != IDENTIFIER) throw SyntaxError("Unnamed parameter in parameter list", tokens.location());
    JavaNode *formPara = new JavaNode(PARAMETER);
    formPara->setName(tokens.name);
    ast->addChild(formPara);
    t = tokens.next();
    if(t == COMMA) {
      t = tokens.next();
      if(!isType(t)) throw SyntaxError("Comma indicates more parameters, but no more found", tokens.location());
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
  if(t != IDENTIFIER) throw SyntaxError("Must name constructor", tokens.location());
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
  if(!isType(t)) throw SyntaxError("Field declaration requires type", tokens.location());
  t = tokens.next();
  if(t != IDENTIFIER) throw SyntaxError("Field declaration needs identifier", tokens.location());
  JavaNode *fieldMem = new JavaNode(FIELD);
  fieldMem->setName(tokens.name);
  t = tokens.next();
  if(t != EQUALS) throw SyntaxError("Field must be initialized", tokens.location());
  t = tokens.next();
  if(!isLiteral(t)) throw SyntaxError("Field must be initialized", tokens.location());
  //If function for literals created, move this there and use fieldMem as ast.
  JavaNode *litTemp = new JavaNode(t);
  litTemp -> setName(tokens.name);
  fieldMem->addChild(litTemp);
  ast->addChild(fieldMem);

  if (!symbolTable.exists(fieldMem)) {
    symbolTable.add(fieldMem, litTemp);
  } else throw SemanticError("Symbol "+ fieldMem->getName() + " already defined ", tokens.location());  

  t = tokens.next();
  if(t != SEMICOLON) throw SyntaxError("Missing ; in field declaration", tokens.location());
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
  if(!(isType(t) || t == VOID)) throw SyntaxError("Method declaration needs type", tokens.location());
  t = tokens.next();
  if(t != IDENTIFIER) throw SyntaxError("Method requires identifier", tokens.location());
  //Might need to cheek the placement, but general idea in place.
  JavaNode *metDecl = new JavaNode(METHOD);
  metDecl->setName(tokens.name);
  formalParameterList(tokens, metDecl);
  t = tokens.peek();
  JavaNode *metBlock = new JavaNode(BLOCK);
  if(t == LEFTBRACKET) {
    block(tokens, metBlock);
    metDecl -> addChild(metBlock);
  } else if(t == SEMICOLON) tokens.next();
  else throw SyntaxError("Need ; for method declaration without body", tokens.location());
  ast->addChild(metDecl);
  if(symbolTable.exists(metDecl)) throw SemanticError("Can't redeclare " + metDecl -> getName(), tokens.location());
  symbolTable.add(metDecl, metBlock);
  return true;
}

// <class member declaration> ::= <field declaration> | <method declaration>
bool classMemberDecl(Tokenizer &tokens, JavaNode *ast) {
  // both start with "static? type identifier" so we want to skip 3
  bool isVoid = false;
  Tokenizer temp = tokens.copy();
  Token type = temp.next();
  Token t;
  //We already add static node below but just to check.
  if(type == STATIC) {
    type = temp.next();
  }
  if(!isType(type) && type != VOID) {
    tokens = temp; // in case you want to output tokens to help debug    
    throw SyntaxError("Type required when declaring member", tokens.location());
  }
  t = temp.next();
  if(t != IDENTIFIER) {
    tokens = temp;
    throw SyntaxError("Identifier required when declaring member", tokens.location());
  }
  t = temp.peek();
  if(t == EQUALS) {
    if(type == VOID) throw SyntaxError("Field cannot be of type 'void'", tokens.location());
    fieldDecl(tokens, ast);
  } else if(t == LEFTPAREN) {
    methodDecl(tokens, ast);
  } else throw SyntaxError("Malformed class member: must be a method or field declaration", tokens.location());
  return true;
}

// <class body> ::= { <class body declarations> }
// <class body declarations> ::= <class body declaration> | <class body declarations> <class body declaration>
// <class body declaration> ::= <class member declaration> | <static initializer> | <constructor declaration>
bool classBody(Tokenizer &tokens, JavaNode *ast) {
  Token t = tokens.next();
  if(t != LEFTBRACKET) throw SyntaxError("{ required to begin class definition", tokens.location());

  while(true) {
    // Alright, we're going to have to do a lot of work to figure out what kind of thing we're working with
    // <class member declaration> starts with static? <type>
    // <static initializer> starts with static {
    // constructor declaration starts with an ID
  
    Tokenizer temp = tokens.copy();
    bool isStatic = false;    
    t = temp.next();
    // first, we deal with the possibility of staticness
    if(t == STATIC) {
      t = temp.next();
      isStatic = true;
      // we'll do something with the AST here eventually
    }
  
    // now check if it's a constructor declaration
    if(t == IDENTIFIER) {
      if(isStatic) throw SyntaxError("Constructor can't be declared as static", tokens.location());
      constructorDecl(tokens, ast);
    } else if(t == LEFTBRACKET) {
      if(!isStatic) throw SyntaxError("You appear to be trying to declare a static block with no static keyword", tokens.location());
      tokens.next(); // clears out the word static
      block(tokens, ast);
    } else if(isType(t) || t == VOID) {
      if(isStatic) {
        classMemberDecl(tokens, ast);
      }
      else classMemberDecl(tokens, ast);
    } else break; // basically just avoids a totally unnecessary bool
  }
  t = tokens.next();
  if(t != RIGHTBRACKET) throw SyntaxError("{ has no associated }", tokens.location());
  return true;
}

// <class declaration> ::= class <identifier> <class body>
bool classDeclaration(Tokenizer &tokens, JavaNode *ast) {
  if(tokens.next() != CLASS) throw SyntaxError("Class must begin with 'class' keyword", tokens.location());
  if(tokens.next() != IDENTIFIER) throw SyntaxError("Invalid or no class name", tokens.location());
  JavaNode *javaClass = new JavaNode(CLASS);
  javaClass->setName(tokens.name);
  ast->addChild(javaClass);
  JavaNode *body = new JavaNode(CLASS);
  classBody(tokens, body);
  javaClass -> addChild(body);
  if(symbolTable.exists(javaClass)) throw SemanticError("Can't redeclare " + javaClass -> getName(), tokens.location());
  symbolTable.add(javaClass, body);
  return true;
}

//Create
void javaDeclarationHelper(string text, string fileName, int lineNumber) {
  Tokenizer tokens(text, fileName, lineNumber);
  JavaNode *root = new JavaNode();
  try {
    classDeclaration(tokens, root);

    delete root;
  } catch(SyntaxError e) {
    tokens.check();
    throw e;
  } catch(SemanticError e) {
    throw e;
  }
}

// only tests that the file compiles error-free
class ClassFileTest : public Test {
  string fileName;
public:
  ClassFileTest(string newName, string newFileName) : Test(newName) {
    fileName = newFileName;
  }
  bool checker() {
    symbolTable.clear();
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
      tokenizer.check();
      return false;
    }
    catch(SemanticError e) {
      cout << " " << e << endl;
      cout << symbolTable << endl;
      return false;
    }
    cout << symbolTable << endl;
    return true;
  }
};

void Tests() {
  tests.push_back(new TTest1());
  tests.push_back(new ClassFileTest("Basic parse test", "test.java"));
  tests.push_back(new ClassFileTest("Full parse test", "fulltest.java"));
  runTests();
}

int main(int argc, char **argv) {
  Tests();
  return 0;
}

