#include <string>
#include <vector>
#include "iobuffer.h"
#include "symbols.h"
#include "abstract-syntax-tree.h"
#include <map>
// to shorten our code:
using namespace std ;
using namespace CS_IO_Buffers ;
using namespace CS_Symbol_Tables ;
using namespace Jack_Compiler ;


typedef struct node{
   int index;
   string segment;
   string type; 
}NODE;

map<string, NODE> local_map;
map<string, NODE> arg_map;
map<string, NODE> this_map;
map<string, NODE> static_map;
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

// fatal error handling
static void fatal(string message)
{
    fatal_error(0,"***** Error:\n" + token_context() + message) ;
}

// The Jack grammar to be recognised:
// program          ::= One or more classes, each class in a separate file named <class_name>'.Jack'
// class            ::= 'class' identifier '{' class_var_decs subr_decs '}'
// class_var_decs   ::= (static_var_dec | field_var_dec)*
// static_var_dec   ::= 'static' type identifier (',' identifier)* ';'
// field_var_dec    ::= 'field' type identifier (',' identifier)* ';'
// type             ::= 'int' | 'char' | 'boolean' | identifier
// vtype            ::= 'void' | type
// subr_decs        ::= (constructor | function | method)*
// constructor      ::= 'constructor' identifier identifier '(' param_list ')' subr_body
// function         ::= 'function' vtype identifier '(' param_list ')' subr_body
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
ast parse_class() ;
ast parse_class_var_decs() ;
vector<ast> parse_static_var_dec() ;
vector<ast> parse_field_var_dec() ;
Token parse_type() ;
Token parse_vtype() ;
ast parse_subr_decs() ;
ast parse_constructor() ;
ast parse_function() ;
ast parse_method() ;
ast parse_param_list() ;
ast parse_subr_body() ;
ast parse_var_decs() ;
vector<ast> parse_var_dec() ;

ast parse_statements() ;
ast parse_statement() ;
ast parse_let() ;
ast parse_if() ;
ast parse_while() ;
ast parse_do() ;
ast parse_return() ;

ast parse_expr() ;
ast parse_term() ;
ast parse_param_list(int i);
ast parse_index() ;
ast parse_id_call() ;
ast parse_call() ;
ast parse_expr_list() ;
ast parse_infix_op() ;
Token parse_unary_op() ;

// class ::= 'class' identifier '{' class_var_decs subr_decs '}'
// create_class(myclassname,class_var_decs,class_subrs)
string myclassname;
ast parse_class()
{
    push_error_context("parse_class()") ;
    mustbe(tk_class);
    myclassname = token_spelling();
    mustbe(tk_identifier);
    mustbe(tk_lcb); // {
    ast class_var_decs = parse_class_var_decs();
    ast class_subrs = parse_subr_decs();
    mustbe(tk_rcb); // }

    pop_error_context() ;
    return create_class(myclassname,class_var_decs,class_subrs);
}

// class_var_decs ::= (static_var_dec | field_var_dec)*
// returns ast_class_var_decs: create_class_var_decs(vector<ast> decs)
// create_class_var_decs must be passed a vector of ast_var_dec
//
ast parse_class_var_decs()
{
    push_error_context("parse_class_var_decs()") ;

    vector<ast> decs;
    while (have(tk_class_var))
    {
        vector<ast> result;
        if (have(tk_field)){
            result = parse_field_var_dec();
        }else{
            result = parse_static_var_dec();
        }
        decs.insert(decs.end(), result.begin(), result.end());
    }

    pop_error_context() ;
    return create_class_var_decs(decs) ;
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
int satatic_idx = 0;
vector<ast> parse_static_var_dec()
{
    vector<ast> decs ;
    push_error_context("parse_class()") ;
    mustbe(tk_static);
    string type;
    NODE node;
    if (have(tk_identifier)){
        type = token_spelling();
        mustbe(tk_identifier);
    }else {
        type = token_kind_to_string(token_kind(parse_type()));
        mustbe(tk_type);
    }
    string name = token_spelling();
    node.type = type;
    node.segment = "static";
    node.index = satatic_idx;
    static_map.insert(make_pair(name, node));
    decs.push_back( create_var_dec(name, "static", satatic_idx++, type) );
    mustbe(tk_identifier);
    while (have(tk_comma)){
        mustbe(tk_comma);
        name = token_spelling();
        node.type = type;
        node.segment = "static";
        node.index = satatic_idx;
        static_map.insert(make_pair(name, node));
        decs.push_back( create_var_dec(name, "static", satatic_idx++, type) );
        mustbe(tk_identifier);
    }mustbe(tk_semi);
    pop_error_context() ;
    return decs ;
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
int this_idx = 0;
vector<ast> parse_field_var_dec()
{
    vector<ast> decs ;
    push_error_context("parse_class()") ;
    mustbe(tk_field);
    string type;
    NODE node;
    if (have(tk_identifier)){
        type = token_spelling();
        mustbe(tk_identifier);
    }else {
        type = token_kind_to_string(token_kind(parse_type()));
        mustbe(tk_type);
    }
    string name = token_spelling();
    node.type = type;
    node.segment = "this";
    node.index = this_idx;
    this_map.insert(make_pair(name, node));
    decs.push_back( create_var_dec(name, "this", this_idx++, type) );
    mustbe(tk_identifier);
    while (have(tk_comma)){
        mustbe(tk_comma);
        name = token_spelling();
        node.type = type;
        node.segment = "this";
        node.index = this_idx;
        this_map.insert(make_pair(name, node));
        decs.push_back( create_var_dec(name, "this", this_idx++, type) );
        mustbe(tk_identifier);
    }mustbe(tk_semi);
    pop_error_context() ;
    return decs ;
}

// type ::= 'int' | 'char' | 'boolean' | identifier
// returns the Token for the type
Token parse_type()
{
    push_error_context("parse_type()") ;
    pop_error_context() ;
    return current_token();
}

// vtype ::= 'void' | type
// returns the Token for the type
Token parse_vtype()
{
    push_error_context("parse_vtype()") ;
    Token res = current_token();
    pop_error_context() ;
    return res ;
}

// subr_decs ::= (constructor | function | method)*
// returns ast_subr_decs: create_subr_decs(vector<ast> subrs)
// create_subr_decs must be passed an vector of ast_subr
//
// ast_subr: create_subr(ast subr)
// create_subr must be passed one of: ast_constructor, ast_function or ast_method
//
ast parse_subr_decs()
{
    push_error_context("parse_subr_decs()") ;

    vector<ast> subrs;
    while (have(tk_subroutine))
    {   
        ast subr;
        if (have(tk_constructor)){
            subr = parse_constructor();
        }else {
            subr = have(tk_method) ? parse_method() : parse_function();
        }
        subrs.push_back( create_subr( subr ) );
    }

    pop_error_context() ;
    return create_subr_decs(subrs) ;
}

// constructor ::= 'constructor' identifier identifier '(' param_list ')' subr_body
// returns ast_constructor: create_constructor(string vtype,string name,ast params,ast body)
// . vtype: the constructor's return type, this must be it's class name
// . name: the constructor's name within its class
// . params: ast_param_list - the constructor's parameters
// . body: ast_subr_body - the constructor's body
//
ast parse_constructor()
{
    push_error_context("parse_constructor()") ;
    mustbe(tk_constructor);
    
    string vtype;
    Token token;
    if (have(tk_identifier)){
        vtype =  token_spelling();
        next_token();
    }else {
        token = parse_type(); 
        vtype = token_kind_to_string(token_kind(token));
        mustbe(tk_vtype);
    }

    string name = token_spelling();
    mustbe(tk_identifier);
    mustbe(tk_lrb);
    ast params = parse_param_list(0);
    mustbe(tk_rrb);
    ast body = parse_subr_body();
    pop_error_context() ;
    return create_constructor(vtype, name, params, body)  ;
}

// function ::= 'function' vtype identifier '(' param_list ')' subr_body
// returns ast_function: create_function(string vtype,string name,ast params,ast body)
// . vtype: the function's return type
// . name: the function's name within its class
// . params: ast_param_list - the function's parameters
// . body: ast_subr_body - the function's body
//
ast parse_function()
{
    push_error_context("parse_function()") ;

    mustbe(tk_function);
    
    string vtype;
    Token token;
    if (have(tk_identifier)){
        vtype =  token_spelling();
        next_token();
    }else {
        token = parse_type(); 
        vtype = token_kind_to_string(token_kind(token));
        mustbe(tk_vtype);
    }

    string name = token_spelling();
    mustbe(tk_identifier);
    mustbe(tk_lrb);
    ast params = parse_param_list(0);
    mustbe(tk_rrb);
    ast body = parse_subr_body();

    pop_error_context() ;
    return create_function(vtype, name, params, body) ;
}

// method ::= 'method' vtype identifier '(' param_list ')' subr_body
// returns ast_method: create_method(string vtype,string name,ast params,ast body)
// . vtype: the method's return type
// . name: the method;s name within its class
// . params: ast_param_list - the method's explicit parameters
// . body: ast_subr_body - the method's body
//
ast parse_method()
{
    push_error_context("parse_method()") ;
    mustbe(tk_method);
    
    Token token = parse_vtype();
    string vtype = token_kind_to_string(token_kind(token));
    mustbe(tk_vtype);

    string name = token_spelling();
    mustbe(tk_identifier);
    mustbe(tk_lrb);
    ast params = parse_param_list(1);
    mustbe(tk_rrb);
    ast body = parse_subr_body();   
    pop_error_context() ;
    return create_method(vtype, name, params, body) ;
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
ast parse_param_list(int i)
{
    push_error_context("parse_param_list()") ;
    
    int arg_idx = i;
    vector<ast> params;
    if (have(tk_type))
    {
        string type;
        Token token;
        if (have(tk_identifier)){
            type =  token_spelling();
            next_token();
        }else {
            token = parse_type(); 
            type = token_kind_to_string(token_kind(token));
            mustbe(tk_type);
        }
        string name = token_spelling();
        mustbe(tk_identifier);
        NODE node = {arg_idx, "argument", type};
        arg_map.insert(make_pair(name, node));
        params.push_back( create_var_dec(name, node.segment, node.index, type) );
        arg_idx = arg_idx + 1;
        while (have(tk_comma)){
            mustbe(tk_comma);
            if (have(tk_identifier)){
                type =  token_spelling();
                next_token();
            }else {
                token = parse_type(); 
                type = token_kind_to_string(token_kind(token));
                mustbe(tk_type);
            }
            name = token_spelling();
            mustbe(tk_identifier);
            node.index = arg_idx;
            node.segment = "argument";
            node.type = type;
            arg_map.insert(make_pair(name, node));
            params.push_back( create_var_dec(name, node.segment, node.index, type) );
            arg_idx++;
        }
    }

    pop_error_context() ;
    return create_param_list(params) ;
}

// subr_body ::= '{' var_decs statements '}'
// returns ast_subr_body: create_subr_body(ast decs,ast body)
// create_subr_body must be passed:
// . decs: ast_var_decs - the subroutine's local variable declarations
// . body: ast_statements - the statements within the body of the subroutinue
//
int idx = 0;
ast parse_subr_body()
{
    push_error_context("parse_subr_body()") ;
    mustbe(tk_lcb);
    ast decs = parse_var_decs();
    ast body = parse_statements();
    mustbe(tk_rcb);
    local_map.clear();idx = 0;
    arg_map.clear();

    pop_error_context() ;
    return create_subr_body(decs, body);
}

// var_decs ::= var_dec*
// returns ast_var_decs: create_var_decs(vector<ast> decs)
// create_var_decs must be passed a vector of ast_var_dec
//
ast parse_var_decs()
{
    push_error_context("parse_var_decs()") ;

    vector<ast> decs;
    while (have(tk_var)){
        vector<ast> result = parse_var_dec();
        decs.insert(decs.end(), result.begin(), result.end());
    }
    pop_error_context() ;
    return create_var_decs(decs);
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
vector<ast> parse_var_dec()
{
    push_error_context("parse_var_dec()") ;
    vector<ast> decs ;
    mustbe(tk_var);
    string type;
    Token token;
    if (have(tk_identifier)){
        type =  token_spelling();
        next_token();
    }else {
        token = parse_type(); 
        type = token_kind_to_string(token_kind(token));
        mustbe(tk_type);
    }
    string name = token_spelling();
    mustbe(tk_identifier);
    NODE node = {idx, "local", type};
    local_map.insert(make_pair(name, node));
    decs.push_back( create_var_dec(name, "local", idx++, type) );
    while (have(tk_comma))
    {
        mustbe(tk_comma);
        name = token_spelling();
        mustbe(tk_identifier);
        NODE node = {idx, "local", type};
        local_map.insert(make_pair(name, node));
        decs.push_back( create_var_dec(name, "local", idx++, type) );  
    }
    mustbe(tk_semi);
    pop_error_context() ;
    return decs ;
}

// statements ::= statement*
// create_statements(vector<ast> statements)
// create_statements must be passed a vector of ast_statement
//
ast parse_statements()
{
    push_error_context("parse_statements()") ;
    vector<ast> statements;
    while(have(tk_statement)){
        ast statement = parse_statement();
        statements.push_back(statement);
    }
    pop_error_context() ;
    return create_statements(statements) ;
}

// statement ::= let | if | while | do | return
// create_statement(ast statement)
// create_statement initialiser must be one of: ast_let, ast_let_array, ast_if, ast_if_else, ast_while, ast_do, ast_return or ast_return_expr
//
ast parse_statement()
{
    push_error_context("parse_statement()") ;

    ast statement;
    switch (token_kind())
    {
    case tk_return:
        statement = parse_return();
        break ;
    case tk_while:
        statement = parse_while();
        break ;
    case tk_if :
        statement = parse_if();
        break;
    case tk_let:
        statement = parse_let();
        break;
    case tk_do:
        statement = parse_do();
        break;
    default:
        break;
    }

    pop_error_context() ;
    return create_statement(statement);
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
ast parse_let()
{
    push_error_context("parse_let()") ;
    mustbe(tk_let);
    string name = token_spelling();
    mustbe(tk_identifier);
    NODE node;
    if (local_map.find(name) != local_map.end()){
        node = local_map[name];
    }
    else if (static_map.find(name) != static_map.end()){
        node = static_map[name];
    }
    else if (this_map.find(name) != this_map.end()){
        node = this_map[name];
    }
    else if (arg_map.find(name) != arg_map.end()){
        node = arg_map[name];
    }
    ast var = create_var(name, node.segment, node.index, node.type);
    ast expr,let;
    if (have(tk_lsb)){ 
        ast index = parse_index();
        mustbe(tk_eq); 
        expr = parse_expr();
        mustbe(tk_semi); 
        let = create_let_array(var,index,expr);
    }else {
        mustbe(tk_eq); 
        expr = parse_expr();
        mustbe(tk_semi);
        let = create_let(var,expr);
    }
    pop_error_context() ;
    return let;
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
ast parse_if()
{
    push_error_context("parse_if()") ;
    mustbe(tk_if);
    mustbe(tk_lrb);
    ast condition = parse_expr();
    mustbe(tk_rrb);
    mustbe(tk_lcb);
    ast if_part = parse_statements();
    mustbe(tk_rcb);
    ast if_expr;

    if (!have(tk_else)){ 
        if_expr = create_if(condition,if_part);
    }else {
        mustbe(tk_else);
        mustbe(tk_lcb);
        ast else_part = parse_statements();
        if_expr = create_if_else(condition,if_part,else_part);
        mustbe(tk_rcb);
    }
    pop_error_context() ;
    return if_expr;
}

// while ::= 'while' '(' expr ')' '{' statements '}'
// returns ast_while: create_while(ast condition,ast body)
// . condition: ast_expr - the loop condition
// . body: ast_statements - the loop body
//
ast parse_while()
{
    push_error_context("parse_while()") ;
    mustbe(tk_while);
    mustbe(tk_lrb);
    ast condition = parse_expr();
    mustbe(tk_rrb);
    mustbe(tk_lcb);
    ast body = parse_statements();
    mustbe(tk_rcb);
    pop_error_context() ;
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
ast parse_do()
{
    push_error_context("parse_do()") ;
    mustbe(tk_do);
    string name = token_spelling();
    mustbe(tk_identifier);
    NODE node;
    bool is_undefined = false;
    if (static_map.find(name) != static_map.end()){
        node = static_map[name];
    }
    else if (this_map.find(name) != this_map.end()){
        node = this_map[name];
    }
    else if (arg_map.find(name) != arg_map.end()){
        node = arg_map[name];
    }
    else if (local_map.find(name) != local_map.end()){
        node = local_map[name];
    }
    else {
        is_undefined = true;
    }

    ast var = create_var(name, node.segment, node.index, node.type);
    if (have(tk_stop) && is_undefined){
        var = create_call_as_function(name, parse_id_call());
    }
    else if (have(tk_stop)){
        var = create_call_as_method(get_var_type(var), var, parse_id_call());
    }
    else if (have(tk_lrb)){
        ast subr_call = create_subr_call(name,parse_call());
        var = create_call_as_method(myclassname, create_this(),subr_call );
    }
    mustbe(tk_semi);
    pop_error_context() ;
    return create_do(var) ;
}

// return ::= 'return' expr? ';'
// returns one of: ast_return or ast_return_expr
//
// ast_return: create_return()
//
// ast_return_expr: create_return_expr(ast expr)
// create_return_expr must be passed an ast_expr
//
ast parse_return()
{
    push_error_context("parse_return()") ;

    mustbe(tk_return);
    ast return_statement;
    if (have(tk_semi)){
        return_statement = create_return();
    }else{
        ast expr = parse_expr();
        return_statement = create_return_expr(expr);
    }
    mustbe(tk_semi);

    pop_error_context() ;
    return return_statement ;
}

// expr ::= term (infix_op term)*
// returns ast_expr: create_expr(vector<ast> expr)
// the vector passed to create_expr:
// . must be an odd length of at least 1, ie 1, 3, 5, ...
// . must be  a vector alternating between ast_term and ast_infix_op nodes, ending with an ast_term
//
ast parse_expr()
{
    push_error_context("parse_expr()") ;
    vector<ast> expr;
    ast term = parse_term(), infix_op;
    expr.push_back(term);
    while (have(tk_infix_op)){
        infix_op = parse_infix_op();
        expr.push_back(infix_op);
        term = parse_term();
        expr.push_back(term);
    }

    pop_error_context() ;
    return create_expr(expr);
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
//
ast parse_var_term();
ast parse_term()
{
    push_error_context("parse_term()") ;
    ast term;
    Token type;
    Token unary_op;
    string op_str;
    // cout << token_kind(current_token())<<endl;
    switch (token_kind(current_token()))
    {
    case tk_true:
        term = create_bool(true);
        next_token();
        break ;
    case tk_false:
        term = create_bool(false);
        next_token();
        break;
    case tk_identifier:
        term = parse_var_term();
        break;
    case tk_stringConstant:
        term = create_string( token_spelling() );
        next_token();
        break;
    case tk_integerConstant:
        term = create_int( token_ivalue() );
        next_token();
        break;
    case tk_this:
        term = create_this();
        next_token();
        break;
    case tk_null:
        mustbe(tk_null);
        term = create_null();
        break;
    case tk_sub:
    case tk_not :
        unary_op = parse_unary_op();
        op_str = token_kind_to_string(token_kind(unary_op));
        term = parse_term();
        term = create_unary_op(op_str, term);
        break;
    case tk_lrb:
        next_token();
        term = parse_expr();
        mustbe(tk_rrb); 
        break;
    default:
        cout << "error " << endl;
        break;
    }

    pop_error_context() ;
    return create_term(term);
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
ast parse_var_term()
{
    push_error_context("parse_var_term()") ;
    string name = token_spelling();
    mustbe(tk_identifier);
    NODE node;
    bool is_undefined = false;
    if (static_map.find(name) != static_map.end()){
        node = static_map[name];
    }
    else if (this_map.find(name) != this_map.end()){
        node = this_map[name];
    }
    else if (arg_map.find(name) != arg_map.end()){
        node = arg_map[name];
    }
    else if (local_map.find(name) != local_map.end()){
        node = local_map[name];
    }
    else {
        is_undefined = true;
    }

    ast var = create_var(name, node.segment, node.index, node.type);
    if (have(tk_stop) && is_undefined){
        var = create_call_as_function(name, parse_id_call());
    }
    else if (have(tk_stop)){
        var = create_call_as_method(get_var_type(var), var, parse_id_call());
    }
    else if (have(tk_lrb)){
        ast subr_call = create_subr_call(name,parse_call());
        var = create_call_as_method(myclassname, create_this(),subr_call );
    }
    else if (have(tk_lsb)){ 
        ast index = parse_index();
        var = create_array_index(var,index);
    }
    // else if ()
    pop_error_context() ;
    return var ;
}

// index ::= '[' expr ']'
// returns ast_expr
ast parse_index()
{
    push_error_context("parse_index()") ;
    mustbe(tk_lsb);
    ast ast_expr = parse_expr();
    mustbe(tk_rsb);
    pop_error_context() ;
    return ast_expr;
}

// id_call ::= '.' identifier call
// returns ast_subr_call: create_subr_call(string subr_name,ast expr_list)
// create_subr_call must be passed:
// . subr_name: the constructor, function or method's name within its class
// . expr_list: ast_expr_list - the explicit arguments to the call
//
ast parse_id_call()
{
    push_error_context("parse_id_call()") ;
    next_token();
    string identifier = token_spelling();
    mustbe(tk_identifier);
    ast ast_call = parse_call();
    pop_error_context() ;
    return create_subr_call(identifier,ast_call);
}

// call ::= '(' expr_list ')'
// returns ast_expr_list
//
ast parse_call()
{
    push_error_context("parse_call()") ;
    mustbe(tk_lrb);
    ast expr_list = parse_expr_list();
    mustbe(tk_rrb);
    pop_error_context() ;
    return expr_list;
}

// expr_list ::= (expr (',' expr)*)?
// returns ast_expr_list: create_expr_list(vector<ast> exprs)
// create_expr_list must be passed: a vector of ast_expr
//
ast parse_expr_list()
{
    push_error_context("parse_expr_list()") ;
    vector<ast> exprs;
    ast expr;
    if (have(tk_term)){
        expr = parse_expr();
        exprs.push_back(expr);
        for (;have(tk_comma);){
            next_token();
            expr = parse_expr();
            exprs.push_back(expr);
        }
    }
    pop_error_context() ;
    return create_expr_list(exprs);
}

// infix_op ::= '+' | '-' | '*' | '/' | '&' | '|' | '<' | '>' | '='
// returns ast_op: create_infix_op(string infix_op)
// create_infix_op must be passed:
// infix_op: the infix op
//
ast parse_infix_op()
{
    push_error_context("parse_infix_op()") ;
    string infix_op = token_spelling();
    mustbe(tk_infix_op);
    pop_error_context() ;
    return create_infix_op(infix_op);
}

// unary_op ::= '-' | '~'
// returns Token for the unary_op
//
Token parse_unary_op()
{
    push_error_context("parse_unary_op()") ;
    Token unary_op = current_token();
    mustbe(tk_unary_op);
    pop_error_context() ;
    return unary_op;
}

ast jack_parser()
{
    // read the first token to get the tokeniser initialised
    next_token() ;

    // construct tree and return as result
    return parse_class() ;
}

// main program
int main(int argc,char **argv)
{
    // parse a Jack class and print the abstract syntax tree as XML
    ast_print_as_xml(jack_parser(),4) ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}

