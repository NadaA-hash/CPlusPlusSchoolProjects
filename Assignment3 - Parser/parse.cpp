/*parse.cpp*/

#include "parse.h"

namespace Parser {
bool pushed_back = false;
Tok	pushed_token;

static Tok GetNextToken(istream& in, int& line) {
	if( pushed_back ) {
		pushed_back = false;
		return pushed_token;
	}
	return getNextToken(in, line);
}

static void PushBackToken(Tok& t) {
	if( pushed_back ) {
		abort();
	}
	pushed_back = true;
	pushed_token = t;
}

}

static int error_count = 0;

void
ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}


Pt *Prog(istream& in, int& line)
{
	Pt *sl = Sl(in, line);

	if( sl == 0 )
		ParseError(line, "No statements in program");

	if( error_count )
		return 0;

	return sl;
}

// Sl is a Statement followed by a Statement List
Pt *Sl(istream& in, int& line) {
	Pt *s = Stmt(in, line);
	if( s == 0 )
		return 0;
	if( Parser::GetNextToken(in, line) != SC ) {
		ParseError(line-1, "Missing semicolon");//not right it has to be fixed down in primary
		delete s;
		return 0;
	}
	return new StmtList(s, Sl(in,line));
}

Pt *Stmt(istream& in, int& line) {
    Pt *goTo = 0;
    Tok t = Parser::GetNextToken(in, line);
    cout << "Starting with.." << t << endl;
    switch(t.GetToken()){
        case PRINT:
            goTo = PrintStmt(in, line);
            break;
        case PRINTLN:
            goTo = PrintlnStmt(in, line);
            break;
        case REPEAT:
            goTo = RepeatStmt(in, line);
            break;
        case IDENT://everything else is considered an expression
            Parser::PushBackToken(t);
            goTo = Expr(in, line);
            break;
        default:
            break;
    }
    return goTo;
}

Pt *PrintStmt(istream& in, int& line) {
	int l = line;
	Pt *ex = Expr(in, line);
	if( ex == 0 ) {
		ParseError(line, "Missing expression after print");
		return 0;
	}

	return new Print(l, ex);
}

Pt *PrintlnStmt(istream& in, int& line) {
    int l = line;
	Pt *ex = Expr(in, line);
	if( ex == 0 ) {
		ParseError(line, "Missing expression after println");
		return 0;
	}

	return new Println(l, ex);
}

Pt *RepeatStmt(istream& in, int& line) {
    int l = line;

	Pt *s = Expr(in, line);
	if( s == 0 ) {
		ParseError(line, "Missing expression after repeat");
		return 0;
	}
	Tok t = Parser::GetNextToken(in, line);
    if (t!= BEGIN){
        ParseError(line, "Missing BEING after expression");
		return 0;
    }
    t = Parser::GetNextToken(in, line);
    if (t == ICONST || t == SCONST){
        ParseError(line, "Invalid statement");
        return 0;
    }
    else{
        Parser::PushBackToken(t);
    }

    s = Stmt(in, line);
    if( s == 0){//if the statement is a non zero then excute it
        ParseError(line, "Missing statement after BEGIN");
        return 0;
    }
    t = Parser::GetNextToken(in, line);

    if (t!= END){
        ParseError(line, "Missing END after statement");
        return 0;
    }

	return new Repeat(l, s);

}

Pt *Expr(istream& in, int& line) {// FIXME add code

    Pt *t1 = Sum(in, line);
	if( t1 == 0 ) {
		return 0;
	}

	while ( true ) {
		Tok t = Parser::GetNextToken(in, line);
        cout << "exp here, have =? " << t << endl;
		if( t != EQ) {
			Parser::PushBackToken(t);
            if ( t != SC && t != BEGIN && t != END && t != RPAREN && t != PRINT){
                    return Sum(in, line);
            }
			return t1;
		}

		Pt *t2 = Sum(in, line);
		if( t2 == 0 ) {
			ParseError(line, "Missing expression after operator");
			return 0;
		}

		if (t == EQ){
            t1 = new Assign(t.GetLinenum(), t1, t2);
        }
	}
}

Pt *Sum(istream& in, int& line) {
	Pt *t1 = Prod(in, line);
	if( t1 == 0 ) {
		return 0;
	}

	while ( true ) {
		Tok t = Parser::GetNextToken(in, line);
        cout << "sum, plus or minus? " << t << endl;
		if( t != PLUS && t != MINUS) {
			Parser::PushBackToken(t);
			return t1;
		}

		Pt *t2 = Prod(in, line);
		if( t2 == 0 ) {
			ParseError(line, "Missing expression after operator");
			return 0;
		}

		if( t == PLUS ){
			t1 = new PlusExpr(t.GetLinenum(), t1, t2);
		}
		else
			t1 = new MinusExpr(t.GetLinenum(), t1, t2);
	}
}

Pt *Prod(istream& in, int& line) {//calls primary
    Pt *t1 = Primary(in, line);
	if( t1 == 0 ) {
		return 0;
	}
	while ( true ) {
		Tok t = Parser::GetNextToken(in, line);
        cout << "It's prod, you star or slash? .. " << t << endl;
		if( t != STAR && t != SLASH) {
			Parser::PushBackToken(t);
			return t1;
		}
		Pt *t2 = Primary(in, line);
		if( t2 == 0 ) {
			ParseError(line, "Missing expression after operator");
			return 0;
		}
		if( t == STAR){
			t1 = new TimesExpr(t.GetLinenum(), t1, t2);
		}
		else{
			t1 = new DivideExpr(t.GetLinenum(), t1, t2);
		}
	}
}

Pt *Primary(istream& in, int& line) {
	Tok t = Parser::GetNextToken(in, line);
    cout << "Hi it's primary: " << t << "And lineCount? "<< line << endl;
	if( t == IDENT )
		return new Ident(t);
	else if (t == ICONST)
        return new IConst(t);
    else if (t == SCONST){
        return new SConst(t);
    }
    else if (t == LPAREN){//LPAREN Expr RPAREN
        Pt *t1 = Expr(in, line);//Take the Expression
        if( t1 == 0 ){//if returned nothing, error
            ParseError(line, "No expression after LPAREN.");
            return 0;
        }
        if (Parser::GetNextToken(in, line) == RPAREN)
        {
            return t1;
        }
        else{
            ParseError(line, "Missing RPAREN after expression.");
            return 0;
        }
    }
    return 0;
}
