#pragma once

#include <sstream>

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
    case INTEGER: out << "Integer Declaration" ; break;
    case FLOAT: out << "Float Declaration" ; break;
    case BOOL: out << "Boolean Declaration" ; break;
    case CHAR: out << "Char Declaration" ; break;
    case STRING: out << "String Declaration" ; break;
    case CLASS: out << "Class Declaration" ; break;
    case SEMICOLON: out << "Lit Semicolon" ; break;
    case STATIC: out << "Lit Static" ; break;
    case EXPRESSION: out << "Expression" ; break;
    case NULL_LIT: out << "Lit NULL" ; break;
    default: out << "Empty" ; break;
    }
    out << " Number of Children " << j->children.size() << " Name:" << j->name << endl;
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
    }
    return out;
  }
  void cleanUp() {
    for (unsigned i = 0; i < children.size(); i++) {
      delete children[i];
    }
  }
};
