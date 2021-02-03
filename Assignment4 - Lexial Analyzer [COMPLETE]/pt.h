/*
 * pt.h
 */

#ifndef PT_H_
#define PT_H_

#include <iostream>
#include <vector>
#include <map>
#include <string>

using std::vector;
using std::map;
using std::string;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

#include "parse.h"
#include "lex.h"
#include "val.h"

// NodeType represents all possible types
enum NodeType { ERRTYPE, INTTYPE, STRTYPE };

class Pt {
public:
	int	linenum;
	Pt	*left;
	Pt	*right;

public:
	Pt(int linenum, Pt *l = 0, Pt *r = 0)
		: linenum(linenum), left(l), right(r) {}

	virtual ~Pt() {
		delete left;
		delete right;
	}

	int GetLineNumber() const { return linenum; }

	int Traverse(bool (Pt::*func)(void) const) const {
        int cnt = 0;
		if( left ) cnt += left->Traverse(func);
		if( right ) cnt += right->Traverse(func);
        if( (this->*func)() ) {
            ++cnt;
        }
		return cnt;
	}

	int MaxDepth() const {
        int lc = 0, rc = 0;
		if( left ) lc = left->MaxDepth();
		if( right ) rc += right->MaxDepth();
		if( lc > rc ) return lc + 1;
        return rc + 1;
	}

    virtual bool isPlus() const { return false; }
    virtual bool isEq() const { return false; }
	virtual bool isIdent() const { return false; }
	virtual bool isVar() const { return false; }
	virtual string getId() const { return ""; }

    void CheckAssignments(map<string,bool>& var) {
		if( left ) left->CheckAssignments(var);
		if( right ) right->CheckAssignments(var);
        this->CheckAssign(var);
    }

	virtual void CheckAssign(map<string,bool>& var) {
		return;
	}

	virtual Value Eval(map<string,Value>& symbols) const = 0;

	string runtimeStrings(string e) const {
	    return "RUNTIME ERROR at " + to_string(GetLineNumber()+1) + ": " + e;
	}
};

class StmtList : public Pt {

public:
	StmtList(Pt *l, Pt *r) : Pt(0, l, r) {}

	Value Eval(map<string, Value>& symbols) const override{
        left->Eval(symbols);//Evaluate the left child
        if(right) {//if there is a right child, evaluate the right child
            right->Eval(symbols);
        }
        return Value();
    }
};

class Print : public Pt {
public:
	Print(int line, Pt *e) : Pt(line, e) {}

	Value Eval(map<string, Value>& symbols) const override{
        Value result = left->Eval(symbols);
        if (result.IsStr()){//If it's a string, make sure to remove double quotes.
            string str = result.GetStr();
            str = str.substr(1, str.size() - 2);//remove quotes at first and last characters
            string str2;
            bool chkNxt = false;//includes escaped
            for (char const &c: str){//going through each character
                if (c == '\\'){
                    str2 +=c;
                    chkNxt = true;
                }
                else if (c != '"')
                    str2 += c;
                else if (chkNxt){
                    str2.erase(str2.size() - 1);
                    str2 += c;
                }
            }
            if (str2 == "\\n" || str2.substr( str2.length() - 2 ) == "\\n")
                cout << str2.substr(0, str2.length()-2) << endl;
            else
                cout << str2;
        }
        else{
            cout << result;
        }

        return Value();
    }
};

class Println : public Pt {
public:
	Println(int line, Pt *e) : Pt(line, e) {}

	Value Eval(map<string, Value>& symbols) const override{
	    Value result = left->Eval(symbols);
        if (result.IsStr()){//If it's a string, make sure to remove double quotes.
            string str = result.GetStr();
            str = str.substr(1, str.size() - 2);//remove quotes at first and last characters
            string str2;
            bool chkNxt = false;//includes escaped
            for (char const &c: str){//going through each character
                if (c == '\\'){
                    str2 +=c;
                    chkNxt = true;
                }
                else if (c != '"')
                    str2 += c;
                else if (chkNxt){
                    str2.erase(str2.size() - 1);
                    str2 += c;
                }
            }
            if (str2 == "\\n" || str2.substr( str2.length() - 2 ) == "\\n")
                cout << str2.substr(0, str2.length()-2) << endl;
            else
                cout << str2 << endl;
        }
        else{
            cout << result << endl;
        }

        return Value();
    }
};

class Repeat : public Pt {
public:
	Repeat(int line, Pt *e, Pt *s) : Pt(line, e, s) {}

	 Value Eval(map<string, Value>& symbols) const override{
        if (left->Eval(symbols).IsInt()){
            int count = left->Eval(symbols).GetInt();//get how many times the expr should repeat
            for (int i = count; i > 0; i--)//use for loop to repeat
                right->Eval(symbols);
            }
        else
            throw runtimeStrings("control expression not an integer");
        return Value();
    }
};

class Assign : public Pt {
    string id;
public:
	Assign(int line, string id, Pt *l) : Pt(line,l), id(id) {}

    bool isEq() const { return true; }

	virtual void CheckAssign(map<string,bool>& var) {
		right->CheckAssign(var);
		var[ this->id ] = true;
	}

	Value Eval(map<string, Value>& symbols) const override{
        Value val = left->Eval(symbols);
        symbols.insert(pair<string, Value>(id,val));//Put value to symbol table

        if (symbols.count(id)>0){
            return symbols[id] = val;
        }
        throw runtimeStrings("Not a valid type to assign something to.");
    }
};

class PlusExpr : public Pt {
public:
	PlusExpr(int line, Pt *l, Pt *r) : Pt(line,l,r) {}
    bool isPlus() const { return true; }

    Value Eval(map<string, Value>& symbols) const override{
        return (left->Eval(symbols) + right->Eval(symbols));
    }
};

class MinusExpr : public Pt {
public:
	MinusExpr(int line, Pt *l, Pt *r) : Pt(line,l,r) {}

	Value Eval(map<string, Value>& symbols) const override{
        return (left->Eval(symbols) - right->Eval(symbols));
    }
};

class TimesExpr : public Pt {
public:
	TimesExpr(int line, Pt *l, Pt *r) : Pt(line,l,r) {}

	Value Eval(map<string, Value>& symbols) const override{
        if (left->Eval(symbols).IsStr() && right->Eval(symbols).IsStr()){
            throw runtimeStrings("Type error on multiplication");
        }
        return (left->Eval(symbols) * right->Eval(symbols));
    }
};

class DivideExpr : public Pt {
public:
	DivideExpr(int line, Pt *l, Pt *r) : Pt(line,l,r) {}

	Value Eval(map<string, Value>& symbols) const override{
        if (!left->Eval(symbols).IsInt() || !right->Eval(symbols).IsInt()){
            throw runtimeStrings("Type error on division");
        }
        if (right->Eval(symbols).GetInt() == 0)
            throw runtimeStrings("Divide by zero");

        return left->Eval(symbols).GetInt() / right->Eval(symbols).GetInt();
    }
};

class IConst : public Pt {
	int val;

public:
	IConst(Tok& t) : Pt(t.GetLinenum()) {
		val = stoi(t.GetLexeme());
	}

	Value Eval(map<string,Value>& symbols) const override {return Value(val);}
};

class SConst : public Pt {
	string val;

public:
	SConst(Tok& t) : Pt(t.GetLinenum()) {
		val = t.GetLexeme();
	}

	Value Eval(map<string,Value>& symbols) const override {return Value(val);}
};

class Ident : public Pt {
	string id;

public:
	Ident(Tok& t) : Pt(t.GetLinenum()), id(t.GetLexeme()) {}

	bool isIdent() const { return true; }
	string getId() const { return id; }
	virtual void CheckAssign(map<string,bool>& var) {
        if( var.find(getId()) == var.end() )
            std::cout << "UNDECLARED VARIABLE " << getId() << std::endl;
    }

    Value Eval(map<string,Value>& symbols) const override {
	    if(symbols.find(id)!=symbols.end()){//Look up the identifier in the symbol table
            return symbols[id];//return the Value from the symbol table
        }

        throw runtimeStrings("Variable used but not assigned");
    }
};

#endif /* PT_H_ */
