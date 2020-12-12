#pragma once
#include <string>
#include <ostream>
using namespace std;

// This is just a nice utility bit; no reason to change it.
// It could be recreated, but it'd turn out basically the same. 

class Error {
  string location; // A file for the exception
  string explanation; // An explanation of the exception
  public:
  Error(string newExplanation,string newLocation) {
    explanation=newExplanation;
    location=newLocation;
  }
  friend ostream &operator <<(ostream &out,const Error &e) {
    return out<< e.location << "SyntaxError:  " << e.explanation << endl;
  }
};

class SyntaxError:Error {
  //string location; // A file for the exception
  //string explanation; // An explanation of the exception
 public:
  SyntaxError(string newExplanation,string newLocation):Error(newExplanation, newLocation) {  }
  friend ostream &operator <<(ostream &out,const SyntaxError &e) {
    return out<< (Error)e << " Syntax Error" << endl;
  }
};

class SemanticError:Error {
  //string location; // A file for the exception
  //string explanation; // An explanation of the exception
 public:
  SemanticError(string newExplanation,string newLocation):Error(newExplanation, newLocation) {  }
  friend ostream &operator <<(ostream &out,const SemanticError &e) {
    return out<< (Error)e << " Semantic Error" << endl;
  }
};