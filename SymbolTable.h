#include "AST.h"
// Map already present in AST.h
// Inspired by quilt example.
class SymbolTable {
    map<JavaNodeId,JavaNode *> symbols;
    public:
    void clear() {
        symbols.clear();
    }
    bool exists(JavaNodeId *id) {
        bool res = symbols.find(*id)!=symbols.end();
        if(res) cout << *id << endl;
        return res;
    }
    void add(JavaNode *node, JavaNode *ast) {
        symbols[(JavaNodeId)(*node)] = ast;
    }
    friend ostream & operator <<(ostream &out, SymbolTable &st) {
        out << "Symbol Table " << endl;
        for (auto& [key, value]: st.symbols)
            out << key << "->" << value;
        return out<<endl;
    }
};

extern SymbolTable symbolTable;
