#pragma once

#include <sstream>
//Include map for symbol table.
#include <map>

#include "Tokenizer.h"

using namespace std;

//Much of class based off Quilt example, will change to better fit project.
class JavaNodeId {
 protected:
  Token operation;
  string name;
 public:
  string getName() {
    return name;
  }
  string description() {
    stringstream ss;
    ss << "Token " << operation << " " << name;
    return ss.str();
  }
  JavaNodeId(Token newOperation = EMPTYTOKEN, string newName = "") {
    operation = newOperation;
    name = newName;
  }
  friend ostream & operator <<(ostream &out, JavaNodeId jn) {
    return out << "Token " << (int)jn.operation << " [" << jn.name << "]";
  }
  friend bool operator<(JavaNodeId a, JavaNodeId b) {
    return a.name < b.name;
  }  
};

class JavaNode:public JavaNodeId {
  static int depth;
  vector<JavaNode *> children;
 public:
 JavaNode(Token newOperation = EMPTYTOKEN):JavaNodeId(newOperation) {
  }
  void setName(string newName) {
    name = newName;
  }
  void addChild(JavaNode *newChild) {
    children.push_back(newChild);
  }
  JavaNode *setOrAdd(Token t) {
    if (operation == EMPTYTOKEN) {
      operation = t;
      return this;
    }
    else {
      JavaNode *treePart = new JavaNode(t);
      this->addChild(treePart);
      return treePart;
    }
  }
  friend ostream &operator <<(ostream &out, JavaNode *j) {
    switch(j->operation) {
    case INTEGER: out << "Integer" ; break;
    case FLOAT: out << "Float" ; break;
    case BOOLEAN: out << "Boolean" ; break;
    case CHAR: out << "Char" ; break;
    case STRING: out << "String" ; break;
    case CLASS: out << "Class" ; break;
    case IDENTIFIER: out << "ID"; break;
    case RESULT: out << "Result"; break;
    case NEW: out << "New"; break;
    case MEMBER: out << "Member"; break;
    case PARAMETER: out << "Parameter"; break;
    case BLOCK: out << "Block"; break;
    case FIELD: out << "Field"; break;
    case METHOD: out << "Method"; break;      
    case SEMICOLON: out << "Semicolon" ; break;
    case STATIC: out << "Static" ; break;
    case EXPRESSION: out << "Expression" ; break;
    case NULL_LIT: out << "Lit NULL" ; break;
    case LITERAL: out << "Literal"; break;
    default: out << "Empty" ; break;
    }
    out << " Number of Children " << j->children.size() << " Name:" << j->name << endl;
    /*
    if (j->children.size() > 0) {
      depth++;
      for(int i = 0; i < depth; i++) {
        out << '+';
        for (int j = 0; j < depth; j++) {
          out << '-';
        }
        out << '>' << j->children[i];
      }
      depth--;
      out << endl;
      }*/
    return out;
  }
};
