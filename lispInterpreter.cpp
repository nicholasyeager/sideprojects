#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cctype>
#include <math.h>
#include <fstream>
#include <sstream>

using namespace std;

enum AtomType { ATOM_LONG, ATOM_DOUBLE, ATOM_SYMBOL, ATOM_STRING };

struct Atom {
    AtomType type;
    union {
        long longValue;
        double doubleValue;
        char* symbolValue;
        char* stringValue;
    };
};

enum ValueType { ATOM_VAL, LIST_VAL, NIL_VAL, FUNC_VAL };

struct List;
struct Lambda;

struct Value {
    ValueType type;
    union {
        Atom atomValue;
        List* listValue;
        Value* (*func)(Value* args);
        Lambda* lambdaValue;
    };
};

struct Lambda {
    Value* params;
    Value* body;
    Value* env;
};

Value NIL_VALUE = { NIL_VAL, {} };
Value* NIL = &NIL_VALUE;

Value TRUE_VALUE;
Value* T;

struct List {
    Value* head;
    Value* tail;
};

Value* cons(Value* car, Value* cdr) {
    List* cell = new List;
    cell->head = car;
    cell->tail = cdr;

    Value* val = new Value;
    val->type = LIST_VAL;
    val->listValue = cell;

    return val;
}

Value* car(Value* list) {
    if (list->type != LIST_VAL) {
        return NIL;
    }
    return list->listValue->head;
}

Value* cdr(Value* list) {
    if (list->type != LIST_VAL) {
        return NIL;
    }
    return list->listValue->tail;
}

Value* makeLambda(Value* params, Value* body, Value* closureEnv) {
    Lambda* l = new Lambda{params, body, closureEnv};
    Value* v = new Value;
    v->type = FUNC_VAL;
    v->lambdaValue = l;
    return v;
}

Value* makeLong(long n) {
    Value* v = new Value;
    v->type = ATOM_VAL;
    v->atomValue.type = ATOM_LONG;
    v->atomValue.longValue = n;
    return v;
}

Value* makeDouble(double d) {
    Value* v = new Value;
    v->type = ATOM_VAL;
    v->atomValue.type = ATOM_DOUBLE;
    v->atomValue.doubleValue = d;
    return v;
}

Value* makeSymbol(const char* s) {
    Value* v = new Value;
    v->type = ATOM_VAL;
    v->atomValue.type = ATOM_SYMBOL;
    v->atomValue.symbolValue = strdup(s);
    return v;
}

Value* makeString(const char* s) {
    Value* v = new Value;
    v->type = ATOM_VAL;
    v->atomValue.type = ATOM_STRING;
    v->atomValue.stringValue = strdup(s);
    return v;
}

vector<string> tokenize(const string& input) {
    vector<string> tokens;
    string token;

    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];

        if (isspace(c)) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else if (c == '(' || c == ')') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back(string(1, c));
        } 
        else if (c == '"') {
            token.clear();
            ++i;
            while (i < input.size() && input[i] != '"') {
                token.push_back(input[i]);
                ++i;
            }
            tokens.push_back("\"" + token + "\"");
            token.clear();
        } 
        else {
            token.push_back(c);
        }
    }

    if (!token.empty()) {
        tokens.push_back(token);
    }

    return tokens;
}

Value* parseAtom(const string& token) {
    Value* v = new Value;
    Atom a;
    char* endptr;

    long l = strtol(token.c_str(), &endptr, 10);
    if (*endptr == '\0') {
        a.type = ATOM_LONG;
        a.longValue = l;
        v->type = ATOM_VAL;
        v->atomValue = a;
        return v;
    }

    double d = strtod(token.c_str(), &endptr);
    if (*endptr == '\0') {
        a.type = ATOM_DOUBLE;
        a.doubleValue = d;
        v->type = ATOM_VAL;
        v->atomValue = a;
        return v;
    }

    if (token.size() > 1 && token.front() == '"' && token.back() == '"') {
        a.type = ATOM_STRING;
        a.stringValue = strdup(token.substr(1, token.size() - 2).c_str());
        v->type = ATOM_VAL;
        v->atomValue = a;
        return v;
    }

    a.type = ATOM_SYMBOL;
    a.symbolValue = strdup(token.c_str());
    v->type = ATOM_VAL;
    v->atomValue = a;
    return v;
}

Value* parseList(vector<string>& tokens, size_t& pos);

Value* eval(Value* expr, Value* env);

Value* sexp(vector<string>& tokens, size_t& pos) {
    string token = tokens[pos];
    if (token == "(") {
        return parseList(tokens, pos);
    }

    if (token == ")") {
        ++pos;
        return NIL;
    }

    if (token == "\'") {
        ++pos;
        Value* quoted = sexp(tokens, pos);
        Value* quoteSym = makeSymbol("quote");
        return cons(quoteSym, cons(quoted, NIL));
    }

    ++pos;
    return parseAtom(token);
}

Value* parseList(vector<string>& tokens, size_t& pos) {
    ++pos;
    Value* list = NIL;
    Value* tail = NIL;

    while (pos < tokens.size() && tokens[pos] != ")") {
        Value* elem = sexp(tokens, pos);

        if (list == NIL) {
            list = cons(elem, NIL);
            tail = list;
        } else {
            tail->listValue->tail = cons(elem, NIL);
            tail = tail->listValue->tail;
        }
    }

    if (pos < tokens.size() && tokens[pos] == ")") {
        ++pos;
    }

    return list;
}

Value* globalEnv;

Value* makeEmptyEnv() {
    return cons(NIL, cons(NIL, NIL));
}

Value* extendEnv(Value* symbols, Value* values, Value* baseEnv) {
    Value* newSyms = symbols;
    Value* newVals = values;
    return cons(newSyms, cons(newVals, baseEnv));
}

Value* lookup(Value* sym, Value* env) {
    if (env == NIL || sym->type != ATOM_VAL || sym->atomValue.type != ATOM_SYMBOL) {
        return makeString("Not Found");
    }

    Value* currentFrame = env;
    
    while (currentFrame != NIL && currentFrame->type == LIST_VAL) {
        Value* syms = car(currentFrame);
        Value* vals = car(cdr(currentFrame));
        
        Value* symList = syms;
        Value* valList = vals;
        
        while (symList != NIL && valList != NIL) {
            Value* key = car(symList);
            Value* val = car(valList);
            
            if (key->type == ATOM_VAL && key->atomValue.type == ATOM_SYMBOL &&
                strcmp(key->atomValue.symbolValue, sym->atomValue.symbolValue) == 0) {
                return val;
            }
            
            symList = cdr(symList);
            valList = cdr(valList);
        }
        
        currentFrame = cdr(cdr(currentFrame));
    }
    
    return makeString("Not Found");
}

Value* defineVar(Value* sym, Value* val, Value* env) {
    if (env->type != LIST_VAL) return NIL;
    
    Value* syms = car(env);
    Value* vals = car(cdr(env));
    
    syms = cons(sym, syms);
    vals = cons(val, vals);
    
    env->listValue->head = syms;
    env->listValue->tail->listValue->head = vals;
    
    return val;
}

Value* nilp(Value* val){
    if (val == NIL){
        return T;
    }
    else{
        return NIL;
    }
}

Value* symbolp(Value* val) {
    if (val != NIL && val->type == ATOM_VAL && val->atomValue.type == ATOM_SYMBOL) {
        return T;
    } else {
        return NIL;
    }
}

Value* numberp(Value* val) {
    if (val != NIL && val->type == ATOM_VAL && (val->atomValue.type == ATOM_LONG || val->atomValue.type == ATOM_DOUBLE)) {
        return T;
    } else {
        return NIL;
    }
}

Value* stringp(Value* val) {
    if (val != NIL && val->type == ATOM_VAL && val->atomValue.type == ATOM_STRING) {
        return T;
    } else {
        return NIL;
    }
}

Value* listp(Value* val) {
    if (val == NIL || val->type == LIST_VAL) {
        return T;
    } else {
        return NIL;
    }
}

Value* eq(Value* args, Value* env){
    Value* left = eval(car(args), env);
    Value* right = eval(car(cdr(args)), env);

    if (left->type == NIL_VAL && right->type == NIL_VAL){
        return T;
    }

    if (left->type == ATOM_VAL && right->type == ATOM_VAL) {
        if (left->atomValue.type == right->atomValue.type) {
            if (left->atomValue.type == ATOM_LONG && left->atomValue.longValue == right->atomValue.longValue) return T;
            if (left->atomValue.type == ATOM_DOUBLE && left->atomValue.doubleValue == right->atomValue.doubleValue) return T;
            if (left->atomValue.type == ATOM_SYMBOL && strcmp(left->atomValue.symbolValue, right->atomValue.symbolValue) == 0) return T;
            if (left->atomValue.type == ATOM_STRING && strcmp(left->atomValue.stringValue, right->atomValue.stringValue) == 0) return T;
        }
        return NIL;
    }
    return NIL;
}

Value* set(Value* args, Value* env) {
    Value* sym = car(args);
    Value* val = eval(car(cdr(args)), env);
    return defineVar(sym, val, env);
}

Value* define(Value* args, Value* env) {
    Value* first = car(args);
    Value* rest = cdr(args);
    
    if (first->type == ATOM_VAL && first->atomValue.type == ATOM_SYMBOL) {
        Value* funcName = first;
        Value* second = car(rest);
        
        if (second != NIL && second->type == LIST_VAL) {
            Value* params = second;
            Value* body = cdr(rest);
            Value* lambda = makeLambda(params, body, env);
            return defineVar(funcName, lambda, env);
        }
        else {
            Value* val = eval(second, env);
            return defineVar(funcName, val, env);
        }
    }
    
    cerr << "Error: Invalid define syntax" << endl;
    return NIL;
}

Value* arithmetic(Value* args, string fname, Value* env){
    double total = 0;
    Value* cur = args;
    long iterator = 0;
    while (cur != NIL) {
        Value* arg = eval(car(cur), env);
        if (arg->type == ATOM_VAL) {
            if (iterator == 0){
                if (arg->atomValue.type == ATOM_LONG) {
                    total += arg->atomValue.longValue;
                } else if (arg->atomValue.type == ATOM_DOUBLE) {
                    total += arg->atomValue.doubleValue;
                } else {
                    cerr << "Error: expects numbers" << endl;
                    return NIL;
                }
                iterator++;
            }
            else{
                if (arg->atomValue.type == ATOM_LONG) {
                    if (fname == "add"){
                        total += arg->atomValue.longValue;
                    }
                    else if (fname == "sub"){
                        total -= arg->atomValue.longValue;
                    }
                    else if (fname == "mul"){
                        total *= arg->atomValue.longValue;
                    }
                    else if (fname == "div"){
                        if (arg->atomValue.longValue == 0){
                            cerr << "Error: Div by 0";
                            return NIL;
                        }
                        total /= arg->atomValue.longValue;
                    }
                    else if (fname == "mod"){
                        int temp = (int)total % arg->atomValue.longValue;
                        total = temp;
                    }
                } 
                else if (arg->atomValue.type == ATOM_DOUBLE) {
                    if (fname == "add"){
                        total += arg->atomValue.doubleValue;
                    }
                    else if (fname == "sub"){
                        total -= arg->atomValue.doubleValue;
                    }
                    else if (fname == "mul"){
                        total *= arg->atomValue.doubleValue;
                    }
                    else if (fname == "div"){
                        if (arg->atomValue.doubleValue == 0.0){
                            cerr << "Error: Div by 0";
                            return NIL;
                        }
                        total /= arg->atomValue.doubleValue;
                    }
                    else if (fname == "mod"){
                        cerr << "Error: mod needs an integer";
                        return NIL;
                    }
                } 
                else {
                    cerr << "Error: expects numbers" << endl;
                    return NIL;
                }
            }
        }
        else{
            cerr << "Error: Expected a Number";
            return NIL;
        }
        cur = cdr(cur);
    }
    if (floor(total) == total) {
        return makeLong((long)total);
    } else {
        return makeDouble(total);
    }
}

Value* inequality(Value* args, string fname, Value* env){
    Value* left = eval(car(args), env);
    Value* right = eval(car(cdr(args)), env);
    if (left->type != ATOM_VAL || right->type != ATOM_VAL){
        cerr << "Error: Needs a number";
        return NIL;
    }
    if (!((left->atomValue.type == ATOM_LONG || left->atomValue.type == ATOM_DOUBLE) && (right->atomValue.type == ATOM_LONG || right->atomValue.type == ATOM_DOUBLE))){
        cerr << "Error: Needs a number";
        return NIL;
    }
    double leftVal;
    double rightVal;
    if (left->atomValue.type == ATOM_LONG){
        leftVal = (double)left->atomValue.longValue;
    }
    else {
        leftVal = left->atomValue.doubleValue;
    }
    if (right->atomValue.type == ATOM_LONG){
        rightVal = (double)right->atomValue.longValue;
    }
    else {
        rightVal = right->atomValue.doubleValue;
    }
    if (fname == "lt"){
        if (leftVal < rightVal){
            return T;
        }
        else{
            return NIL;
        }
    }
    else if (fname == "lte"){
        if (leftVal <= rightVal){
            return T;
        }
        else{
            return NIL;
        }
    }
    else if (fname == "gt"){
        if (leftVal > rightVal){
            return T;
        }
        else{
            return NIL;
        }
    }
    else if (fname == "gte"){
        if (leftVal >= rightVal){
            return T;
        }
        else{
            return NIL;
        }
    }
    return NIL;
}

void printVal(Value* val) {
    if (val == NIL) {
        cout << "()";
        return;
    }

    if (val->type == FUNC_VAL){
        return;
    }

    if (val->type == ATOM_VAL) {
        if (val->atomValue.type == ATOM_LONG) {
            cout << val->atomValue.longValue;
        } 
        else if (val->atomValue.type == ATOM_DOUBLE) {
            cout << val->atomValue.doubleValue;
        } 
        else if (val->atomValue.type == ATOM_SYMBOL) {
            cout << val->atomValue.symbolValue;
        } 
        else if (val->atomValue.type == ATOM_STRING) {
            cout << "\"" << val->atomValue.stringValue << "\"";
        }
        return;
    }

    if (val->type == LIST_VAL) {
        cout << "(";
        Value* cur = val;
        bool first = true;
        while (cur != NIL) {
            if (!first) {
                cout << " ";
            }
            if (cur->type != LIST_VAL) {
                cout << ". ";
                printVal(cur);
                break;
            }
            printVal(cur->listValue->head);
            cur = cur->listValue->tail;
            first = false;
        }
        cout << ")";
        return;
    }
}

Value* And(Value* args, Value* env){
    Value* left = eval(car(args), env);
    Value* right = eval(car(cdr(args)), env);

    if (left == NIL){
        return NIL;
    }
    if (right == NIL){
        return NIL;
    }
    else{
        return T;
    }
}

Value* Or(Value* args, Value* env){
    Value* left = eval(car(args), env);
    Value* right = eval(car(cdr(args)), env);

    if (left != NIL){
        return T;
    }
    if (right == NIL){
        return NIL;
    }
    else{
        return T;
    }
}

Value* If(Value* args, Value* env){
    Value* e1 = eval(car(args), env);
    Value* e2 = (car(cdr(args)));
    Value* e3 = (car(cdr(cdr(args))));

    if (e1 != NIL){
        return eval(e2, env);
    }
    else{
        return eval(e3, env);
    }
}

Value* cond(Value* args, Value* env) {
    while (args != NIL) {
        Value* clause = car(args);
        Value* testExpr = car(clause);
        Value* body = cdr(clause);

        if (testExpr->type == ATOM_VAL &&
            testExpr->atomValue.type == ATOM_SYMBOL &&
            strcmp(testExpr->atomValue.symbolValue, "T") == 0) {
            Value* result = NIL;
            while (body != NIL) {
                result = eval(car(body), env);
                body = cdr(body);
            }
            return result;
        }

        Value* testVal = eval(testExpr, env);
        if (testVal != NIL) {
            Value* result = NIL;
            while (body != NIL) {
                result = eval(car(body), env);
                body = cdr(body);
            }
            return result;
        }

        args = cdr(args);
    }

    return NIL;
}

Value* evalSequence(Value* exprs, Value* env) {
    Value* result = NIL;
    while (exprs != NIL) {
        result = eval(car(exprs), env);
        exprs = cdr(exprs);
    }
    return result;
}

Value* applyLambda(Lambda* lambda, Value* args, Value* currentEnv) {
    Value* evaledArgs = NIL;
    Value* tail = NIL;
    Value* curArg = args;
    
    while (curArg != NIL) {
        Value* val = eval(car(curArg), currentEnv);
        if (evaledArgs == NIL) {
            evaledArgs = cons(val, NIL);
            tail = evaledArgs;
        } else {
            tail->listValue->tail = cons(val, NIL);
            tail = tail->listValue->tail;
        }
        curArg = cdr(curArg);
    }
    Value* newEnv = extendEnv(lambda->params, evaledArgs, lambda->env);
    
    return evalSequence(lambda->body, newEnv);
}

Value* eval(Value* expr, Value* env) {
    if (expr == NIL) {
        return NIL;
    }

    if (expr == T){
        return T;
    }

    if (expr->type == ATOM_VAL) {
        if (expr->atomValue.type == ATOM_SYMBOL){
            if (strcmp(expr->atomValue.symbolValue, "\'") != 0) {
                Value* val = lookup(expr, env);
                if (val->type == ATOM_VAL &&
                    val->atomValue.type == ATOM_STRING &&
                    strcmp(val->atomValue.stringValue, "Not Found") == 0) {
                    cerr << "Symbol not found: " << expr->atomValue.symbolValue << endl;
                    exit(1);
                } else {
                    return val;
                }
            }
        }
        return expr;
    }

    if (expr->type == LIST_VAL) {
        Value* op = car(expr);   
        Value* args = cdr(expr);

        if (op->type == ATOM_VAL && op->atomValue.type == ATOM_SYMBOL) {
            string fname = op->atomValue.symbolValue;

            if (fname == "quote"){
                return car(args);
            }

            if (fname == "cons"){
                Value* carVal = eval(car(args), env);
                Value* cdrVal = eval(car(cdr(args)), env);
                return cons(carVal, cdrVal);
            }

            if (fname == "car") {
                Value* list = eval(car(args), env);
                return car(list);
            }

            if (fname == "cdr") {
                Value* list = eval(car(args), env);
                return cdr(list);
            }

            if (fname == "if"){
                return If(args, env);
            }

            if (fname == "cond"){
                return cond(args, env);
            }

            if (fname == "eq"){
                return eq(args, env);
            }

            if (fname == "set"){
                return set(args, env);
            }

            if (fname == "define"){
                return define(args, env);
            }

            if (fname == "and"){
                return And(args, env);
            }

            if (fname == "or"){
                return Or(args, env);
            }

            if (fname == "lt" || fname == "gt" || fname == "lte" || fname == "gte") {
                return inequality(args, fname, env);
            }

            if (fname == "not") {
                Value* arg = eval(car(args), env);
                if (arg == NIL) return T;
                return NIL;
            }

            if (fname == "mul" || fname == "add" || fname == "div" || fname == "sub" || fname == "mod") {
                return arithmetic(args, fname, env);
            }

            if (fname == "symbolp") {
                return symbolp(car(args));
            }
            if (fname == "numberp") {
                return numberp(eval(car(args), env));
            }
            if (fname == "stringp") {
                return stringp(eval(car(args), env));
            }
            if (fname == "listp") {
                return listp(eval(car(args), env));
            }
            if (fname == "nilp") {
                return nilp(eval(car(args), env));
            }
            if (fname == "lambda") {
                Value* params = car(args);
                Value* body = cdr(args);
                return makeLambda(params, body, env);
            }
            
            Value* fval = lookup(op, env);
            if (fval->type == ATOM_VAL && fval->atomValue.type == ATOM_STRING &&
                strcmp(fval->atomValue.stringValue, "Not Found") == 0) {
                cerr << "Error: " << fname << " is not a function" << endl;
                return NIL;
            } else if (fval->type == FUNC_VAL && fval->lambdaValue != nullptr) {
                return applyLambda(fval->lambdaValue, args, env);
            }
        } else if (op->type == FUNC_VAL && op->lambdaValue != nullptr) {
            return applyLambda(op->lambdaValue, args, env);
        }
        else {
            Value* evaledOp = eval(op, env);
            if (evaledOp->type == FUNC_VAL && evaledOp->lambdaValue != nullptr) {
                return applyLambda(evaledOp->lambdaValue, args, env);
            }
        }
    }
    return expr;
}

void initGlobalEnv() {
    TRUE_VALUE.type = ATOM_VAL;
    TRUE_VALUE.atomValue.type = ATOM_SYMBOL;
    TRUE_VALUE.atomValue.symbolValue = strdup("T");
    T = &TRUE_VALUE;

    globalEnv = makeEmptyEnv();
    defineVar(makeSymbol("T"), T, globalEnv);
}

int main(int argc, char *argv[]) {
    initGlobalEnv();
    string line;
    vector<string> tokens;
    size_t pos = 0;
    if (argc == 1){
        string input;
        while (getline(cin, input)){
            tokens = tokenize(input);  
            pos = 0;

            Value* expr = sexp(tokens, pos);     

            Value* result = eval(expr, globalEnv);            

            printVal(result);                       
            cout << endl;
        }
    }
    else{
        string filename = argv[1];
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Cannot open file " << filename << endl;
            return 0;
        }
        stringstream buffer;
        while (getline(file, line)) {
            buffer << line << " ";
        }
        string content = buffer.str();
        tokens = tokenize(content);
        pos = 0;

        cout << "Syntax is the test expression input followed by Pass/Fail with a dotted pair representing expected versus output" << endl;
        cout << endl;

        while (pos < tokens.size()) {
            Value* expr = sexp(tokens, pos);
            Value* result = eval(expr, globalEnv);
            printVal(expr);
            cout << endl;
            printVal(result);
            cout << endl;
        }

        file.close();
    }

    return 0;
}