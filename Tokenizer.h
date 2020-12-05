#pragma once

#include <iostream>
#include <regex>
#include <sstream>
#include <cctype> // for isspace

#include "Testing.h"

using namespace std;

enum Token {EMPTYTOKEN, CLASS, SEMICOLON, LEFTPAREN, RIGHTPAREN, LEFTBRACKET, RIGHTBRACKET, STATIC, COMMA, PRIMITIVE, IDENTIFIER,
  RESULT, SIGN, EQUALS, INTEGER, FLOAT, BOOL, CHAR, STRING, EXPRESSION, NULL_LIT, NEW};


/*These nodes more based in expression parse?
enum Node {CLASSDECL, CLASSBOD, CLASSBODDECLS, CLASSMEMDECLS, FIELDDECL, VARDECLS, STATINIT,
  CONSTRUCTOR, CONSTRUCTORDECL, FORMPARAMS, TYPE, METHODDECL, METHODHEAD, RESULTTYPE,
  METHODDECLARATOR, METHODBOD, BLOCK, BLOCKSTATEMENTS, CLASSCREATE, LOCVAR};
*/

class Tokenizer {
  private:
  string text;
  smatch sm;
  int originalLength;
  //For debugging
  int lineNumber;
  string filename;
  int charNumber;

  public:
  string name;
  //Should return location of errors. Based off quilt example.
  string location() {
    stringstream ss;
    ss << filename << ":" << lineNumber << ":" << charNumber << ":";
  }

  Tokenizer(string codeText, string newFilename, int newLineNumber = 0) {
    lineNumber = newLineNumber;
    filename = newFilename;

    text = codeText;
  }
  Token peek() {
    Token t = EMPTYTOKEN;
    name = "";
    //Will we need to do specify brackets?
    regex keywordExpr("^(class|;|\\(|\\)|{|}|static|,|+|-|=|int|float|bool|char|string|NULL|([a-zA-Z]|_)([a-zA-Z]|[0-9]|_))");
    regex idExpr("^([a-zA-Z]|_)([a-zA-Z]|[0-9]|_)*");
    if(regex_search(text, sm, keywordExpr)) {
      if(sm[0] == "") t = EMPTYTOKEN;
      else if(sm[0] == "class") t = CLASS;
      else if(sm[0] == ";") t = SEMICOLON;
      else if(sm[0] == "(") t = LEFTPAREN;
      else if(sm[0] == ")") t = RIGHTPAREN;
      else if(sm[0] == "{") t = LEFTBRACKET;
      else if(sm[0] == "}") t = RIGHTBRACKET;
      else if(sm[0] == "static") t = STATIC;
      else if(sm[0] == ",") t = COMMA;
      else if(sm[0] == "-" || sm[0] == "+") t = SIGN;
      else if(sm[0] == "=") t = EQUALS;
      else if(sm[0] == "int") t = INTEGER;
      else if(sm[0] == "float") t = FLOAT;
      else if(sm[0] == "bool") t = BOOL;
      else if(sm[0] == "char") t = CHAR;
      else if(sm[0] == "string") t = STRING;
      else if(sm[0] == "NULL") t = NULL_LIT;
      else if(regex_search(text, sm, idExpr)) {
        t = IDENTIFIER;
        name = sm[0];
      }
    }
    return t;
  }
  void pop() {
    text = sm.suffix().str();
    charNumber - originalLength - text.size();
    //Leave reference for white space in case of errors.
    //if(text[0] == ' ') text = text.substr(1);
    while(isspace(text[0])) text = text.substr(1);
  }
  Token next() {
    Token t = peek();
    pop();
    return t;
  }
  void check() {
    cerr << "Tokenizer: " << text << ":" << endl;
  }
};

class TTest1:public Test{
	public:
	TTest1():Test("Check Each Symbol"){
	}
	bool checker() {
	  Tokenizer tokenizer("class ; ( ) { } static , - + = int float bool char string NULL zap","TTest1",1);
	  if (tokenizer.next()!=CLASS) return false;
	  if (tokenizer.next()!=SEMICOLON) return false;
	  if (tokenizer.next()!=LEFTPAREN) return false;
	  if (tokenizer.next()!=RIGHTPAREN) return false;
	  if (tokenizer.next()!=LEFTBRACKET) return false;
	  if (tokenizer.next()!=RIGHTBRACKET) return false;
	  if (tokenizer.next()!=STATIC) return false;
    if (tokenizer.next()!=COMMA) return false;
    if (tokenizer.next()!=SIGN) return false;
    if (tokenizer.next()!=SIGN) return false;
    if (tokenizer.next()!=EQUALS) return false;
    if (tokenizer.next()!=INTEGER) return false;
    if (tokenizer.next()!=FLOAT) return false;
    if (tokenizer.next()!=BOOL) return false;
    if (tokenizer.next()!=CHAR) return false;
    if (tokenizer.next()!=STRING) return false;
    if (tokenizer.next()!=NULL_LIT) return false;
    if (tokenizer.next()!=IDENTIFIER) return false;
   	return true;
	} 
};