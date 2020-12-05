#include <iostream>
#include <regex>
#include <sstream>
#include <fstream>
#include <vector>

//Remember to add syntax file!
#include "Testing.h"
#include "Tokenizer.h"
#include "Syntax.h"
#include "AST.h"

using namespace std;

bool javaExpression(Tokenizer &tokens, JavaNode *ast);
//Insert expressions...

bool classDeclaration(Tokenizer &tokens, JavaNode *ast) {

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
void *javaExpressionHelper(string texts, string filename, int linenumber) {
    Tokenizer tokens(texts, filename, linenumber);
    JavaNode *root = new JavaNode(EXPRESSION);
    try {
        javaExpression(tokens, root);

        delete root;
    } catch(SyntaxError e) {
        tokens.check();
        throw e;
    } catch(SemanticError e) {
        tokens.check();
        throw e;
    }
}
//Put in parse test.

void Tests() {

}

int main(int argc, char **argv) {
    Tests();
    return 0;
}