#include <iostream>
#include <algorithm>
#include "lex.h"
#include <array>

 std::array<string, 18> tokenNames = {"PRINT","PRINTLN","REPEAT","BEGIN","END","IDENT","ICONST",
 "SCONST","PLUS", "MINUS", "STAR", "SLASH", "EQ", "LPAREN","RPAREN", "SC","ERR","DONE"};

Token refToBegin = BEGIN;

char operators[] = {'+','-','*','=','/','(',')'};
char *rightOper;
char *peekOper;

Tok getNextToken(istream& in, int& linenum)
{
    enum TokState{
    BEGIN,
    INID,
    INSTRING,
    ININT,
    INCOMMENT
    };
    TokState lexstate = BEGIN;
    string lexeme;
    char ch;
    bool escaped = false;
    while(in.get(ch)) {
        rightOper = std::find (operators, operators+7, ch);
        peekOper = std::find (operators, operators+7, in.peek());
        if( ch == '\n'){
            linenum++;
        }
        switch (lexstate){
            case BEGIN:
                if(isspace(ch) )
                    continue;
                else if (ch == '/' && in.peek() == '/'){
                    lexstate = INCOMMENT;
                    break;
                }
                else if(rightOper != operators+7){
                    lexstate = ININT;
                    in.putback(ch);
                    break;
                }
                else if(isalpha(ch) )
                    lexstate = INID;
                else if (isdigit(ch)){
                    lexstate = ININT;
                    in.putback(ch);
                    break;
                    if (peekOper != operators+7){
                        lexeme += ch;
                        return Tok(ICONST,lexeme,linenum);
                    }
                }
                else if (ch == '"')
                    lexstate = INSTRING;
                else if (ch == ';')
                    return Tok(SC,lexeme,linenum);
                else if (ch == '_' || ch == '.' || ch == '$' || ch == '!'){
                    lexeme += ch;
                    return Tok(ERR, lexeme, linenum+1);
                }
            case INID://In an identifier
                if (!isspace(ch) && ch != ';' && (rightOper == operators+7)){
                    if (ch == '_' || ch == '.' || ch == '$' || ch == '!'){
                        if (lexeme == ""){
                            lexeme += ch;
                            return Tok(ERR,lexeme,linenum+1);
                        }
                        else{
                            in.putback(ch);
                            return Tok(IDENT,lexeme,linenum);
                        }
                    }
                    lexeme += ch;
                    continue;
                }
                if (ch == ';' || rightOper != operators+7){
                    in.putback(ch);
                }
                if ( (lexeme == "print") == 1 ){
                    return Tok(PRINT,"PRINT",linenum);
                }
                else if ((lexeme == "println") == 1){
                    return Tok(PRINTLN,"PRINTLN",linenum);
                }
                else if ((lexeme == "repeat") == 1){
                    return Tok(REPEAT,"REPEAT",linenum);
                }
                else if ((lexeme == "begin") == 1){
                    return Tok(refToBegin,"BEGIN",linenum);
                }
                else if ((lexeme == "end") == 1){
                    return Tok(END,"END",linenum);
                }
                else
                    return Tok(IDENT,lexeme,linenum);;
            case INSTRING://In a string
                if (ch == '\n')
                {
                    lexeme += ch;
                    return Tok(ERR, lexeme,linenum);
                }
                else if (ch == '\\' && in.peek() == '"')
                {
                    escaped = true;
                    lexeme += ch;
                    continue;
                }
                else if (ch == '"' && escaped)
                {
                    lexeme += ch;
                    escaped = false;
                    continue;
                }
                else if (!(ch == '"')){
                    lexeme += ch;
                    continue;
                }
                lexeme += ch;
                return Tok(SCONST,lexeme,linenum);
                break;
            case ININT://In a number
                if (rightOper != operators+7){
                            lexeme += ch;
                            if (ch == '+')
                                return Tok(PLUS,lexeme,linenum);
                            else if (ch == '-')
                                return Tok(MINUS,lexeme,linenum);
                            else if (ch == '*')
                                return Tok(STAR,lexeme,linenum);
                            else if (ch == '=')
                                return Tok(EQ,lexeme,linenum);
                            else if (ch == '/')
                                return Tok(SLASH,lexeme,linenum);
                            else if (ch == '(')
                                return Tok(LPAREN,lexeme,linenum);
                            else if (ch == ')')
                                return Tok(RPAREN,lexeme,linenum);
                }
                else if (isdigit(ch)){
                    lexeme += ch;
                    if (peekOper != operators+7)
                        return Tok(ICONST,lexeme,linenum);
                    continue;
                }
                else if (!isdigit(ch)){
                    if (ch == '\n')
                        linenum--;
                    in.putback(ch);
                }
                return Tok(ICONST,lexeme,linenum);
                break;
            case INCOMMENT://In a comment
                if (ch == '\n')
                    lexstate = BEGIN;
                break;
        }
    }
    return Tok(DONE,lexeme,linenum);
}

ostream& operator<<(ostream& out, const Tok& tok){
    if (tok == IDENT || tok == ICONST || tok == SCONST){
        return out << tokenNames[tok.GetToken()]<<"("<<tok.GetLexeme()<<")";
    }
    else if (tok == ERR){
        return out << "Error on line " << tok.GetLinenum() << " (" << tok.GetLexeme() << ")";
    }
    return out << tokenNames[tok.GetToken()];
}
