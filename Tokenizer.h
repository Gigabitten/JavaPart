#pragma once

#include <iostream>
#include <regex>
#include <sstream>
#include <cctype> // for isspace

using namespace std;

enum Token { EMPTYTOKEN, CLASS, SEMICOLON, LEFTPAREN, RIGHTPAREN, STATIC, COMMA, PRIMITIVE, IDENTIFIER
	     , RESULT, SIGN, INTEGER, FLOAT, BOOL, CHAR, STRING, NULL_LIT, NEW, };
enum Node { CLASSDECL, CLASSBOD, CLASSBODDECLS, CLASSMEMDECLS, FIELDDECL, VARDECLS, STATINIT
	    , CONSTRUCTOR, CONSTRUCTORDECL, FORMPARAMS, TYPE, METHODDECL, METHODHEAD, RESULTTYPE
	    , METHODDECLARATOR, METHODBOD, BLOCK, BLOCKSTATEMENTS, CLASSCREATE, LOCVAR, };

class Tokenizer {
 private:
  string text;
  smatch sm;
 public:
  string name;
  Tokenizer(string codeText) {
    text = codeText;
  }
  Token peek() {
    Token t = EMPTYTOKEN;
    name = "";
    regex expr;
    if(regex_search(text, sm, expr)) {
      if(sm[0] == "") t = EMPTYTOKEN;
    }
    return t;
  }
  void pop() {
    text = sm.suffix().str();
    while(isspace(text[0])) text = text.substr(1);
  }
  Token next() {
    Token t = peek();
    pop();
    return t;
  }
  void check() {
    cerr << "Tokenizer: " << text << endl;
  }
};
