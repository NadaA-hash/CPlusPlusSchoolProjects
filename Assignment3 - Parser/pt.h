/* pt.h*/

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

    virtual bool isPlus() const { return false; }
    virtual bool isEq() const { return false; }
	virtual bool isIdent() const { return false; }
	virtual bool isVar() const { return false; }
	virtual string getId() const { return ""; }

	int MaxDepth() const {
        int leftPath = 0;
        int rightPath = 0;
        if( left )
            leftPath += left->MaxDepth();
        if( right )
            rightPath += right->MaxDepth();
        if( leftPath > rightPath)
            return leftPath+1;
        else
            return rightPath+1;
    }
	int plusCount() const{
        int pCount = 0;
        if (left){
            pCount += left->plusCount();
        }
        if (right){
            pCount += right->plusCount();
        }
        //cout << "!" << isPlus() << endl;
        if(isPlus()){
            pCount++;
        }
        return pCount;
	}

	int eqCount() const{
        int eCount = 0;
        if (left){
            eCount += left->eqCount();
        }
        if (right){
            eCount += right->eqCount();
        }
        //cout << "?" << isEq() << endl;
        if(isEq()){
            eCount++;
        }
        return eCount;
	}

};

class StmtList : public Pt {
public:
	StmtList(Pt *l, Pt *r) : Pt(0, l, r) {
	}
};

class Print : public Pt {
public:
	Print(int line, Pt *e) : Pt(line, e) {}
};

class Println : public Pt {
// FIXME add code
public:
    Println(int line, Pt *e) : Pt(line, e){}
};

class Repeat : public Pt {
// FIXME add code
public:
    Repeat(int line, Pt *e) : Pt(line, e){}
};

class Assign : public Pt {
public:
    Assign(int line, Pt* e, Pt* o) : Pt(line, e, o){}
    bool isEq() const override{
        return true;
    }
};

class PlusExpr : public Pt {
public:
    PlusExpr(int line, Pt* l, Pt* r) : Pt(line, l, r) {}
      bool isPlus() const {
          return true;
      }
};

class MinusExpr : public Pt {
public:
    MinusExpr(int line, Pt *e, Pt *o) : Pt(line, e, o) {}

};

class TimesExpr : public Pt {
public:
    TimesExpr(int line, Pt *e, Pt *o) : Pt(line, e, o) {}
};

class DivideExpr : public Pt {
public:
    DivideExpr(int line, Pt *e, Pt *o) : Pt(line, e, o) {}
};

class IConst : public Pt {
	int val;

public:
	IConst(Tok& t) : Pt(t.GetLinenum()) {
		val = stoi(t.GetLexeme());
	}
};

class SConst : public Pt {
	string val;

public:
	SConst(Tok& t) : Pt(t.GetLinenum()) {
		val = t.GetLexeme();
	}
};

class Ident : public Pt {
// FIXME add code
string val;

public:
	Ident(Tok& t) : Pt(t.GetLinenum()) {
		val = t.GetLexeme();
	}
};

#endif /* PT_H_ */

