/*
*   Computer Systems
*   2020, Semester 02
*   Assignment 3
*
*   student (id): a1778478
*   student (name): Runtao Zhuge
*
*/
#include <string>
#include <vector>
#include "iobuffer.h"
#include "symbols.h"
#include "abstract-syntax-tree.h"
#include <iostream>
// to shorten our code:
using namespace std;
using namespace CS_IO_Buffers;
using namespace CS_Symbol_Tables;
using namespace Jack_Compiler;

// ***** WHAT TO DO *****
//
// MODIFY the skeleton code below to parse a Jack class and construct the equivalent abstract syntax tree, ast.
//        The main function will print the ast formatted as XML
//
// NOTE: the correct segment and offset is recorded with every variable in the ast
//       so the code generator does not need to use any symbol tables
//
// NOTE: use the fatal function below to print fatal error messages with the current
//       input file position marked.
//
// The skeleton code has one function per non-terminal in the Jack grammar
//  - comments describing how to create relevant ast nodes prefix most functions
//  - ast nodes are immutable - you must create all the sub-trees before creating a new node
//
// The structure of the skeleton code is just a suggestion
//  - it matches the grammar but does not quite match the ast structure
//  - you will need to change the parameters / results / functions to suit your own logic
//  - you can change it as much as you like
//
// NOTE: the best place to start is to write code to parse the input without attempting to create
//       any ast nodes. If correct this parser will report errors when it finds syntax errors but
//       correct programs will fail because the tree printer will not be provided with a valid ast.
//       Once the parsing works modify your code to create the ast nodes.
//
// NOTE: the return create_empty() ; statements are only present so that this skeleton code will compile.
//       these statements need to be replaced by code that creates the correct ast nodes.
//

// use the fatal_token_context() function to print fatal error messages with the current input file position marked.


// The Jack grammar to be recognised:
// program          ::= One or more classes, each class in a separate file named <class_name>'.Jack'
// class            ::= 'class' identifier '{' class_var_decs subr_decs '}'
// class_var_decs   ::= (static_var_dec | field_var_dec)*
// static_var_dec   ::= 'static' type identifier (',' identifier)* ';'
// field_var_dec    ::= 'field' type identifier (',' identifier)* ';'
// type             ::= 'int' | 'char' | 'boolean' | identifier
// vtype            ::= 'void' | type
// subr_decs        ::= (constructor | function | method)*
// constructor      ::= '   ' vtype identifier '(' param_list ')' subr_body
// method           ::= 'method' vtype identifier '(' param_list ')' subr_body
// param_list       ::= ((type identifier) (',' type identifier)*)?
// subr_body        ::= '{' var_decs statements '}'
// var_decs         ::= var_dec*
// var_dec          ::= 'var' type identifier (',' identifier)* ';'

// statements       ::= statement*
// statement        ::= let | if | while | do | return
// let              ::= 'let' identifier index? '=' expr ';'
// if               ::= 'if' '(' expr ')' '{' statements '}' ('else' '{' statements '}')?
// while            ::= 'while' '(' expr ')' '{' statements '}'
// do               ::= 'do' identifier (id_call | call) ';'
// return           ::= 'return' expr? ';'

// expr             ::= term (infix_op term)*
// term             ::= integer_constant | string_constant | 'true' | 'false' | 'null' | 'this' | '(' expr ')' | unary_op term | var_term
// var_term         ::= identifier (index | id_call | call)?
// index            ::= '[' expr ']'
// id_call          ::= '.' identifier call
// call             ::= '(' expr_list ')'
// expr_list        ::= (expr (',' expr)*)?
// infix_op         ::= '+' | '-' | '*' | '/' | '&' | '|' | '<' | '>' | '='
// unary_op         ::= '-' | '~'


// forward declarations of one function per non-terminal rule in the Jack grammar - except for program
// NOTE: parse_type(), parse_vtype(), parse_unary_op() all return a Token not an ast
// NOTE: parse_static_var_dec(), parse_field_var_dec(), parse_var_dec() all return vector<ast> not an ast
//

ast parse_class();

ast parse_class_var_decs();

Token parse_type();

Token parse_vtype();

ast parse_subr_decs();

ast parse_constructor();

ast parse_function();

ast parse_method();

ast parse_param_list(int num);

ast parse_subr_body();

ast parse_var_decs();

ast parse_statements();

ast parse_statement();

ast parse_let();

ast parse_if();

ast parse_while();

ast parse_do();

ast parse_return();

ast parse_expr();

ast parse_term();

ast parse_var_term();

ast parse_index();

ast parse_id_call();

ast parse_call();

ast parse_expr_list();

ast parse_infix_op();

Token parse_unary_op();
//create vector <ast> for storing

//used to store lcl
vector<ast> parseVarDec(int &lcl_count);

//used to store static
vector<ast> parseStaticVarDec();

//used to store field
vector<ast> parseFieldVarDec();

// the fatal_token_context() function to print fatal error messages with the current
//       input file position marked.
static void fatal(string message) {
    fatal_error(0, "***** Error:\n" + token_context() + message);
}

//create symbol tables
static symbols claVariableTable = 0;
static symbols subVariableTable = 0;

//setup global offsets counter to count the next free position for the local segment
static int staticOffsetCount = 0;
static int fieldOffsetCount = 0;
//Create myclass outside of the class to get
static string class_name;

// class ::= 'class' identifier '{' class_var_decs subr_decs '}'
// create_class(myclassname,class_var_`decs,class_subrs)
ast parse_class() {
    claVariableTable = create_variables();
    mustbe(tk_class);
    class_name = token_spelling(mustbe(tk_identifier));

    ast class_var_decs;
    ast subr_decs;
    mustbe(tk_lcb);
    class_var_decs = parse_class_var_decs();

    while (have(tg_starts_subroutine) || have(tg_starts_class_var)) {
        if (!have(tg_starts_class_var)) {
            if (have(tg_starts_subroutine)) {
                subr_decs = parse_subr_decs();
            }
        } else {
            class_var_decs = parse_class_var_decs();
        }
    }
    mustbe(tk_rcb);
    return create_class(class_name, class_var_decs, subr_decs);
}

// class_var_decs ::= (static_var_dec | field_var_dec)*
// returns ast_class_var_decs: create_class_var_decs(vector<ast> decs)
// create_class_var_decs must be passed a vector of ast_var_dec
//

ast parse_class_var_decs() {
    vector<ast> decs, temp;
    while (have(tg_starts_class_var)) {
        if (!have(tk_static)) {
            if (have(tk_field)) {
                temp = parseFieldVarDec();
                decs.insert(decs.end(), temp.begin(), temp.end());
            }
        } else {
            temp = parseStaticVarDec();
            decs.insert(decs.end(), temp.begin(), temp.end());
        }
    }
    return create_class_var_decs(decs);
}

// static_var_dec ::= 'static' type identifier (',' identifier)* ';'
// returns vector of ast_var_dec
//
// ast_var_dec: create_var_dec(string name,string segment,int offset,string type)
// create_var_dec must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
vector<ast> parseStaticVarDec() {
    vector<ast> static_var_dec;
    st_variable localVar;
    while (have(tk_static)) {
        mustbe(tk_static);
        string type = token_spelling(parse_type());
        string name = token_spelling(mustbe(tk_identifier));
        string segment = "static";
        ast var_dec = create_var_dec(name, segment, staticOffsetCount, type);
        static_var_dec.push_back(var_dec);
        st_variable lclVar(name, type, segment, staticOffsetCount);
        if (insert_variables(claVariableTable, name, lclVar)) {} else fatal("Empty");
        staticOffsetCount++;
        //incrementing the counter for the free positions
        //while have the comma it will continue to read
        while (have(tk_comma)) {
            mustbe(tk_comma);
            name = token_spelling(mustbe(tk_identifier));
            var_dec = create_var_dec(name, segment, staticOffsetCount, type);
            static_var_dec.push_back(var_dec);
            st_variable myvar1(name, type, segment, staticOffsetCount);
            if (insert_variables(claVariableTable, name, myvar1)) {} else fatal("Empty");
            staticOffsetCount++;
        }
    }
    mustbe(tk_semi);
    return static_var_dec;
}

// field_var_dec ::= 'field' type identifier (',' identifier)* ';'
// returns vector of ast_var_dec
//
// ast_var_dec: create_var_dec(string name,string segment,int offset,string type)
// create_var_dec must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
vector<ast> parseFieldVarDec() {
    vector<ast> field_var_dec;
    while (have(tk_field)) {
        mustbe(tk_field);
        string type = token_spelling(parse_type());
        string name = token_spelling(mustbe(tk_identifier));
        string segment = "this";
        ast var_dec = create_var_dec(name, segment, fieldOffsetCount, type);
        field_var_dec.push_back(var_dec);
        st_variable myvar(name, type, segment, fieldOffsetCount);
        if (insert_variables(claVariableTable, name, myvar)) {} else fatal("Having problems");
        fieldOffsetCount++;
        while (have(tk_comma)) {
            mustbe(tk_comma);
            name = token_spelling(mustbe(tk_identifier));
            var_dec = create_var_dec(name, segment, fieldOffsetCount, type);
            field_var_dec.push_back(var_dec);
            st_variable stVariable(name, type, segment, fieldOffsetCount);
            if (insert_variables(claVariableTable, name, stVariable)) {} else fatal("Having problems");
            fieldOffsetCount++;
        }
    }
    mustbe(tk_semi);
    return field_var_dec;
}

// type ::= 'int' | 'char' | 'boolean' | identifier
// returns the Token for the type
Token parse_type() {
    return mustbe(tg_starts_type);
}

// vtype ::= 'void' | type
// returns the Token for the type
Token parse_vtype() {
    return mustbe(tg_starts_vtype);
}

// subr_decs ::= (constructor | function | method)*
// returns ast_subr_decs: create_subr_decs(vector<ast> subrs)
// create_subr_decs must be passed an vector of ast_subr
//
// ast_subr: create_subr(ast subr)
// create_subr must be passed one of: ast_constructor, ast_function or ast_method

ast parse_subr_decs() {
    vector<ast> decs, subr;
    ast temp;
    while (have(tg_starts_subroutine)) {
        if (!have(tk_constructor)) {}
        else {
            temp = create_subr(parse_constructor());
            decs.push_back(temp);
        }
        if (!have(tk_function)) {}
        else {
            temp = create_subr(parse_function());
            decs.push_back(temp);
        }
        if (!have(tk_method)) continue;
        temp = create_subr(parse_method());
        decs.push_back(temp);
    }
    return create_subr_decs(decs);
}

// constructor ::= 'constructor' identifier identifier '(' param_list ')' subr_body
// returns ast_constructor: create_constructor(string vtype,string name,ast params,ast body)
// . vtype: the constructor's return type, this must be it's class name
// . name: the constructor's name within its class
// . params: ast_param_list - the constructor's parameters
// . body: ast_subr_body - the constructor's body

ast parse_constructor() {
    subVariableTable = create_variables();
    mustbe(tk_constructor);
    string vtype = token_spelling(mustbe(tk_identifier));
    if (!(class_name != vtype)) {}
    else {
        fatal("constructor return type must be its own class");
    }
    string name = token_spelling(mustbe(tk_identifier));
    mustbe(tk_lrb);

    ast params = parse_param_list(0);
    mustbe(tk_rrb);
    ast body = parse_subr_body();
    delete_variables(subVariableTable);
    return create_constructor(vtype, name, params, body);
}

// function ::= 'function' vtype identifier '(' param_list ')' subr_body
// returns ast_function: create_function(string vtype,string name,ast params,ast body)
// . vtype: the function's return type
// . name: the function's name within its class
// . params: ast_param_list - the function's parameters
// . body: ast_subr_body - the function's body
//
ast parse_function() {
    subVariableTable = create_variables();
    mustbe(tk_function);

    string vtype = token_spelling(parse_vtype());
    string name = token_spelling(mustbe(tk_identifier));
    mustbe(tk_lrb);
    ast params = parse_param_list(0);
    mustbe(tk_rrb);
    ast body = parse_subr_body();
    delete_variables(subVariableTable);
    pop_error_context();
    return create_function(vtype, name, params, body);
}

// method ::= 'method' vtype identifier '(' param_list ')' subr_body
// returns ast_method: create_method(string vtype,string name,ast params,ast body)
// . vtype: the method's return type
// . name: the method;s name within its class
// . params: ast_param_list - the method's explicit parameters
// . body: ast_subr_body - the method's body
//
ast parse_method() {
    subVariableTable = create_variables();
    mustbe(tk_method);
    string vtype = token_spelling(parse_vtype());
    string name = token_spelling(mustbe(tk_identifier));
    mustbe(tk_lrb);
    ast params = parse_param_list(1);
    mustbe(tk_rrb);
    ast body = parse_subr_body();
    delete_variables(subVariableTable);
    return create_method(vtype, name, params, body);
}

// param_list ::= ((type identifier) (',' type identifier)*)?
// returns ast_param_list: create_param_list(vector<ast> params)
// create_param_list must be passed a vector of ast_var_dec
//
// ast_var_dec: create_var_dec(string name,string segment,int offset,string type)
// create_var_dec must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//

ast parse_param_list(int num) {
    int argument_offset_count;
    vector<ast> params;
    if (num == 0) {
        argument_offset_count = 0;
    } else {
        argument_offset_count = 1;
    }
    if (have(tg_starts_type)) {
        string type = token_spelling(mustbe(tg_starts_type));
        string name = token_spelling(mustbe(tk_identifier));
        string segment = "argument";

        ast para = create_var_dec(name, segment, argument_offset_count, type);

        st_variable stVariable(name, type, segment, argument_offset_count);
        if (insert_variables(subVariableTable, name, stVariable)) {} else fatal("Having problems");
        params.push_back(para);
        argument_offset_count++;
        while (have(tk_comma)) {
            mustbe(tk_comma);
            type = token_spelling(mustbe(tg_starts_type));
            name = token_spelling(mustbe(tk_identifier));
            ast para = create_var_dec(name, segment, argument_offset_count, type);
            st_variable lclvar(name, type, segment, argument_offset_count);
            if (insert_variables(subVariableTable, name, lclvar)) {} else fatal("Having problems");
            params.push_back(para);
            argument_offset_count++;
        }

    }
    return create_param_list(params);
}

// subr_body ::= '{' var_decs statements '}'
// returns ast_subr_body: create_subr_body(ast decs,ast body)
// create_subr_body must be passed:
// . decs: ast_var_decs - the subroutine's local variable declarations
// . body: ast_statements - the statements within the body of the subroutinue
//
ast parse_subr_body() {
    mustbe(tk_lcb);
    ast decs, body;
    decs = parse_var_decs();
    body = parse_statements();
    mustbe(tk_rcb);
    return create_subr_body(decs, body);
}

// var_decs ::= var_dec*
// returns ast_var_decs: create_var_decs(vector<ast> decs)
// create_var_decs must be passed a vector of ast_var_dec
//
ast parse_var_decs() {
    vector<ast> decs;
    vector<ast> all_decs;
    int local_offset_count = 0;
    while (have(tk_var)) {
        decs = parseVarDec(local_offset_count);
        all_decs.insert(all_decs.end(), decs.begin(), decs.end());
    }
    return create_var_decs(all_decs);
}

// var_dec ::= 'var' type identifier (',' identifier)* ';'
// returns vector of ast_var_dec
//
// ast_var_dec: create_var_dec(string name,string segment,int offset,string type)
// create_var_dec must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//

//local, the parameterts
vector<ast> parseVarDec(int &lcl_count) {
    vector<ast> my_ast_var_dec;
    mustbe(tk_var);
    string type = token_spelling(parse_type());
    string name = token_spelling(mustbe(tk_identifier));
    string segment = "local";
    ast var = create_var_dec(name, segment, lcl_count, type);
    my_ast_var_dec.push_back(var);
    st_variable myvar(name, type, segment, lcl_count);

    if (insert_variables(subVariableTable, name, myvar)) {} else fatal("Error!");
    lcl_count++;
    while (have(tk_comma)) {
        mustbe(tk_comma);
        name = token_spelling(mustbe(tk_identifier));
        ast var = create_var_dec(name, segment, lcl_count, type);
        my_ast_var_dec.push_back(var);
        st_variable myvar(name, type, segment, lcl_count);
        if (insert_variables(subVariableTable, name, myvar)) {} else fatal("Error!");
        lcl_count++;
    }
    mustbe(tk_semi);
    return my_ast_var_dec;
}

// statements ::= statement*
// create_statements(vector<ast> statements)
// create_statements must be passed a vector of ast_statement
//
ast parse_statements() {
    vector<ast> decs;
    while (have(tg_starts_statement)) {
        decs.push_back(parse_statement());
    }
    return create_statements(decs);
}

// statement ::= let | if | while | do | return
// create_statement(ast statement)
// create_statement initialiser must be one of: ast_let, ast_let_array, ast_if, ast_if_else, ast_while, ast_do, ast_return or ast_return_expr
//
ast parse_statement() {
    if (!have(tk_let)) {
        if (have(tk_if)) {
            return create_statement(parse_if());
        } else if (have(tk_while)) {
            return create_statement(parse_while());
        } else if (have(tk_do)) {
            return create_statement(parse_do());
        } else if (have(tk_return)) {
            return create_statement(parse_return());
        }
    } else {
        return create_statement(parse_let());
    }
    return 0;
}

// let ::= 'let' identifier index? '=' expr ';'
// return one of: ast_let or ast_let_array
//
// create_let(ast var,ast expr)
// . var: ast_var - the variable being assigned to
// . expr: ast_expr - the variable's new value
//
// create_let_array(ast var,ast index,ast expr)
// . var: ast_var - the variable for the array being assigned to
// . index: ast_expr - the array index
// . expr: ast_expr - the array element's new value
//
ast parse_let() {
    ast var, expr;
    mustbe(tk_let);
    string name = token_spelling(mustbe(tk_identifier));
    st_variable subr_myvar = lookup_variables(subVariableTable, name);
    st_variable class_myvar = lookup_variables(claVariableTable, name);
    if (-1 == subr_myvar.offset) {
        if (class_myvar.offset != -1) {
            var = create_var(class_myvar.name, class_myvar.segment, class_myvar.offset, class_myvar.type);
        }
    } else {
        var = create_var(subr_myvar.name, subr_myvar.segment, subr_myvar.offset, subr_myvar.type);
    }
    if (!have(tk_lsb)) {
        mustbe(tk_eq);
        expr = parse_expr();
        mustbe(tk_semi);
        return create_let(var, expr);
    } else {
        ast index = parse_index();
        mustbe(tk_eq);
        expr = parse_expr();
        mustbe(tk_semi);
        return create_let_array(var, index, expr);
    }
}

// if ::= 'if' '(' expr ')' '{' statements '}' ('else' '{' statements '}')?
// return one of: ast_if or ast_if_else
//
// create_if(ast condition,ast if_true)
// . condition: ast_expr - the if condition
// . if_true: ast_statements - the if true branch
//
// create_if_else(ast condition,ast if_true,ast if_false)
// . condition: ast_expr - the if condition
// . if_true: ast_statements - the if true branch
// . if_false: ast_statements - the if false branch
//
ast parse_if() {

    ast condition;
    ast if_true;
    ast if_false;
    mustbe(tk_if);
    mustbe(tk_lrb);
    condition = parse_expr();
    mustbe(tk_rrb);
    mustbe(tk_lcb);
    if_true = parse_statements();
    mustbe(tk_rcb);
    if (!have(tk_else)) return create_if(condition, if_true);
    mustbe(tk_else);
    mustbe(tk_lcb);
    if_false = parse_statements();
    mustbe(tk_rcb);
    return create_if_else(condition, if_true, if_false);
}

// while ::= 'while' '(' expr ')' '{' statements '}'
// returns ast_while: create_while(ast condition,ast body)
// . condition: ast_expr - the loop condition
// . body: ast_statements - the loop body
//
ast parse_while() {
    ast condition, body;
    mustbe(tk_while);
    mustbe(tk_lrb);
    condition = parse_expr();
    mustbe(tk_rrb);
    mustbe(tk_lcb);
    body = parse_statements();
    mustbe(tk_rcb);
    return create_while(condition, body);
}

// do ::= 'do' identifier (call | id_call) ';'
// returns ast_do: create_do(ast call)
// create_do must be passed one of: ast_call_as_function or ast_call_as_method
//
// ast_call_as_function: create_call_as_function(string class_name,ast subr_call)
// create_ast_call_as_function must be passed:
// . class_name: name of the function's class
// . subr_call: ast_subr_call - the function's name within it's class and it's explicit arguments
//
// ast_call_as_method: create_call_as_method(string class_name,ast object,ast subr_call)
// create_ast_call_as_method must be passed:
// . class_name: name of the method's class
// . object: ast_expr - the object the method is applied to
// . subr_call: ast_subr_call - the method's name within it's class and it's explicit arguments
//

//call: (    id_call: .
ast parse_do() {
    mustbe(tk_do);
    string name = token_spelling(mustbe(tk_identifier));
    st_variable subr_myvar = lookup_variables(subVariableTable, name);
    st_variable class_myvar = lookup_variables(claVariableTable, name);
    if (!have(tk_lrb)) {
        if (!have(tk_stop)) return 0;
        if (-1 == subr_myvar.offset) {
            if (class_myvar.offset != -1) {
                ast subrCall1 = parse_id_call();
                ast object = create_var(class_myvar.name, class_myvar.segment, class_myvar.offset, class_myvar.type);
                ast callAsMethod1 = create_call_as_method(class_myvar.type, object, subrCall1);
                mustbe(tk_semi);
                return create_do(callAsMethod1);

            } else {
                mustbe(tk_stop);
                string basicString = token_spelling(mustbe(tk_identifier));
                mustbe(tk_lrb);
                ast exprList = parse_expr_list();
                mustbe(tk_rrb);
                ast subrCall = create_subr_call(basicString, exprList);
                ast call_as_function = create_call_as_function(name, subrCall);
                mustbe(tk_semi);
                return create_do(call_as_function);
            }
        } else {
            ast subrCall2 = parse_id_call();
            ast object = create_var(subr_myvar.name, subr_myvar.segment, subr_myvar.offset, subr_myvar.type);
            ast callAsMethod = create_call_as_method(subr_myvar.type, object, subrCall2);
            mustbe(tk_semi);
            return create_do(callAsMethod);

        }
    } else {
        ast expr_list = parse_call();
        mustbe(tk_semi);
        ast subr_call = create_subr_call(name, expr_list);
        ast call_as_method = create_call_as_method(class_name, create_this(), subr_call);
        pop_error_context();
        return create_do(call_as_method);
    }
}

ast parse_return() {
    ast expr;
    mustbe(tk_return);
    if (!have(tg_starts_term)) {
        mustbe(tk_semi);
        return create_return();
    } else {
        expr = parse_expr();
        mustbe(tk_semi);
        return create_return_expr(expr);
    }
}

// expr ::= term (infix_op term)*
// returns ast_expr: create_expr(vector<ast> expr)
// the vector passed to create_expr:
// . must be an odd length of at least 1, ie 1, 3, 5, ...
// . must be  a vector alternating between ast_term and ast_infix_op nodes, ending with an ast_term
//
ast parse_expr() {
    vector<ast> expr;
    expr.push_back(parse_term());
    while (have(tg_infix_op)) {
        expr.push_back(parse_infix_op());
        expr.push_back(parse_term());

    }
    return expr.size() % 2 != 0 ? create_expr(expr) : 0;
}

// term ::= integer_constant | string_constant | 'true' | 'false' | 'null' | 'this' | '(' expr ')' | unary_op term | var_term
// returns ast_term: create_term(ast term)
// create_term must be passed one of: ast_int, ast_string, ast_bool, ast_null, ast_this, ast_expr,
//                                    ast_unary_op, ast_var, ast_array_index, ast_call_as_function, ast_call_as_method
//
// ast_int: create_int(int _constant)
// create_int must be passed an integer value in the range 0 to 32767
//
// ast_string: create_string(string _constant)
// create_string must be passed any legal Jack string literal
//
// ast_bool: create_bool(bool t_or_f)
// create_bool must be passed true or false
//
// ast_null: create_null()
//
// ast_this: create_this()
//
// ast_unary_op: create_unary_op(string op,ast term)
// create_unary_op must be passed:
// . op: the unary op
// . term: ast_term

ast parse_term() {
    if (!have(tk_integerConstant)) {
        if (have(tk_stringConstant)) {
            ast string_constant = create_string(token_spelling(mustbe(tk_stringConstant)));
            return create_term(string_constant);
        }
    } else {
        int constant = token_ivalue();
        mustbe(tk_integerConstant);
        ast integer_constant = create_int(constant);
        return create_term(integer_constant);
    }
    if (have(tk_true)) {
        next_token();
        return create_term(create_bool(true));
    } else {
        if (have(tk_false)) {
            next_token();
            return create_term(create_bool(false));
        } else if (have(tk_null)) {
            next_token();
            return create_term(create_null());
        } else if (have(tk_this)) {
            next_token();
            return create_term(create_this());
        } else if (have(tk_lrb)) {
            mustbe(tk_lrb);
            ast term = parse_expr();
            mustbe(tk_rrb);
            return create_term(term);
        } else if (have(tg_unary_op)) {
            string op = token_spelling(parse_unary_op());
            ast term = parse_term();
            return create_term(create_unary_op(op, term));
        } else {
            ast term = parse_var_term();
            return create_term(term);
        }
    }
}

// var_term ::= identifier (index | id_call | call)?
// returns one of: ast_var, ast_array_index, ast_call_as_function or ast_call_as_method
//
// ast_var: create_var(string name,string segment,int offset,string type)
// create_ast_var must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
// ast_array_index: create_array_index(ast var,ast index)
// create_ast_array_index must be passed:
// . var: ast_var - the array variable
// . index: ast_expr - the index into the array
//
// ast_call_as_function: create_call_as_function(string class_name,ast subr_call)
// create_ast_call_as_function must be passed:
// . class_name: name of the function's class
// . subr_call: ast_subr_call - the function's name within it's class and it's explicit arguments
//
// ast_call_as_method: create_call_as_method(string class_name,ast object,ast subr_call)
// create_ast_call_as_method must be passed:
// . class_name: name of the method's class
// . object: ast_expr - the object the method is applied to
// . subr_call: ast_subr_call - the method's name within it's class and it's explicit arguments
//
ast parse_var_term() {
    string name = token_spelling(mustbe(tk_identifier));
    if (have(tk_stop)) {
        ast subr_call = parse_id_call();
        st_variable subrMyvar = lookup_variables(subVariableTable, name);
        st_variable stVariable = lookup_variables(claVariableTable, name);
        if (-1 != subrMyvar.offset) {
            ast createVar = create_var(subrMyvar.name, subrMyvar.segment, subrMyvar.offset, subrMyvar.type);
            name = get_var_type(createVar);
            return create_call_as_method(name, createVar, subr_call);
        } else if (stVariable.offset != -1) {
            ast object = create_var(stVariable.name, stVariable.segment, stVariable.offset, stVariable.type);
            name = get_var_type(object);
            return create_call_as_method(name, object, subr_call);
        } else {
            return create_call_as_function(name, subr_call);
        }
    }

    if (!have(tk_lrb)) {
        if (have(tk_lsb)) {
            st_variable subr_myvar = lookup_variables(subVariableTable, name);
            st_variable class_myvar = lookup_variables(claVariableTable, name);
            if (subr_myvar.offset != -1) {
                ast var = create_var(subr_myvar.name, subr_myvar.segment, subr_myvar.offset, subr_myvar.type);
                ast index = parse_index();
                return create_array_index(var, index);

            } else {
                if (class_myvar.offset == -1) return 0;
                ast var1 = create_var(class_myvar.name, class_myvar.segment, class_myvar.offset, class_myvar.type);
                ast index1 = parse_index();
                return create_array_index(var1, index1);
            }

        } else {
            st_variable subrMyvar = lookup_variables(subVariableTable, name);
            st_variable classMyvar = lookup_variables(claVariableTable, name);
            if (-1 == subrMyvar.offset) {
                if (classMyvar.offset != -1) {

                    ast var = create_var(classMyvar.name, classMyvar.segment, classMyvar.offset, classMyvar.type);
                    return var;
                }
            } else {
                ast var = create_var(subrMyvar.name, subrMyvar.segment, subrMyvar.offset, subrMyvar.type);

                return var;
            }
        }
    } else {
        ast object = create_this();
        ast subr_call = create_subr_call(name, parse_call());
        return create_call_as_method(class_name, object, subr_call);

    }
    return 0;
}

// index ::= '[' expr ']'
// returns ast_expr
ast parse_index() {
    ast expr;
    mustbe(tk_lsb);
    expr = parse_expr();
    mustbe(tk_rsb);
    return expr;
}

// id_call ::= '.' identifier call
// returns ast_subr_call: create_subr_call(string subr_name,ast expr_list)
// create_subr_call must be passed:
// . subr_name: the constructor, function or method's name within its class
// . expr_list: ast_expr_list - the explicit arguments to the call
//
ast parse_id_call() {
    string subr_name;
    ast expr_list;
    mustbe(tk_stop);
    subr_name = token_spelling(mustbe(tk_identifier));
    expr_list = parse_call();
    return create_subr_call(subr_name, expr_list);
}

// call ::= '(' expr_list ')'
// returns ast_expr_list
//
ast parse_call() {
    ast ast_expr_list;
    mustbe(tk_lrb);
    ast_expr_list = parse_expr_list();
    mustbe(tk_rrb);
    return ast_expr_list;
}

// expr_list ::= (expr (',' expr)*)?
// returns ast_expr_list: create_expr_list(vector<ast> exprs)
// create_expr_list must be passed: a vector of ast_expr
//
ast parse_expr_list() {
    vector<ast> exprs;
    if (have(tg_starts_term)) {
        exprs.push_back(parse_expr());
        while (have(tk_comma)) {
            mustbe(tk_comma);
            exprs.push_back(parse_expr());
        }
    }
    return create_expr_list(exprs);
}

// infix_op ::= '+' | '-' | '*' | '/' | '&' | '|' | '<' | '>' | '='
// returns ast_op: create_infix_op(string infix_op)
// create_infix_op must be passed:
// infix_op: the infix op
//
ast parse_infix_op() {
    return create_infix_op(token_spelling(mustbe(tg_infix_op)));
}

// unary_op ::= '-' | '~'
// returns Token for the unary_op
//
Token parse_unary_op() {
    return mustbe(tg_unary_op);
}

ast jack_parser() {
    next_token();
    return parse_class();
}

// main program
int main(int argc, char **argv) {
    config_errors(iob_immediate);
    // parse a Jack class and print the abstract syntax tree as XML
    ast_print_as_xml(jack_parser(), 4);

    // flush the output and any errors
    print_output();
    print_errors();
}
