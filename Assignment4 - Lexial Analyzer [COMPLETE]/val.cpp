#include "val.h"

// utility to repeat “s” n times
static string rptn(string s, int n) {
    string result;
    result.reserve( n * s.length() );
    for( int i=0; i<n; i++ )
        result += s;
    return result;
}

// add op to this
Value Value::operator+(const Value& op) const{
    // type mismatch is error
    // note the test after the || ACTUALLY covers if
    // op is an error AND if this is an error: do you see why???
    if( GetType() != op.GetType() || IsErr() )
        return Value();
    if( IsInt() )
        return Value( I + op.GetInt() );

    return Value( S + op.GetStr() );
}
// NOTE the I and S in the return statements will// call Value(I) or Value(S), respectively

Value Value::operator-(const Value& op) const{
    if( GetType() != op.GetType() || IsErr() )
        return Value();

    if( IsInt() ) return Value( I -op.GetInt() );

    string removeMe = op.GetStr();
    removeMe = removeMe.substr(1, removeMe.size() - 2);
    // find the string op in this
    auto srch= S.find( removeMe );

    string result = S;
    if( srch!= string::npos)
        result.erase(srch, removeMe.length());
    return result;
}

// multiply this by op
Value Value::operator*(const Value& op) const{
    if( IsInt() && op.IsInt() )
        return Value( I * op.GetInt() );
    if( IsInt() && op.IsStr() ) {
        if( I < 0 )
            throw "RUNTIME ERROR: Multiply string by negative number";
        return rptn(op.GetStr(), I);
    }
    else if( IsStr() && op.IsInt() ) {
        if( op.GetInt() < 0 )
            throw "RUNTIME ERROR: Multiply string by negative number";
        return rptn(S, op.GetInt());
    }
    return Value();
}

// divide this by op
Value Value::operator/(const Value& op) const{
    if( !(IsInt() && op.IsInt()))
        return Value();
    if( op.GetInt() == 0 )
        throw "RUNTIME ERROR: Divide by zero";
    return Value( I / op.GetInt() );
}
