#include "iobuffer.h"
#include "symbols.h"
#include "abstract-syntax-tree.h"
#include <vector>
// to shorten our code:
using namespace std ;
using namespace CS_IO_Buffers ;
using namespace Jack_Compiler ;

// ***** WHAT TO DO *****
//
// MODIFY the skeleton code below to walk an abstract syntax tree, ast, of a Jack class
//        and pretty print the equivalent Jack source code.
//
// The skeleton code has one function per node in the abstract tree
//  - they each extract all fields into local variables
//  - they each call the appropriate walk_* function to walk any sub-trees
//
// The structure of the skeleton code is just a suggestion
//  - you may want to change the parameters / results to suit your own logic
//  - you can change it as much as you like

// forward declarations of one function per node in the abstract syntax tree
void walk_class(ast t) ;
void walk_class_var_decs(ast t) ;
void walk_var_dec(ast t, bool isEnd);
void walk_subr_decs(ast t) ;
void walk_subr(ast t) ;
void walk_constructor(ast t) ;
void walk_function(ast t) ;
void walk_method(ast t) ;
void walk_param_list(ast t) ;
void walk_subr_body(ast t) ;
void walk_var_decs(ast t) ;
void walk_statements(ast t) ;
void walk_statement(ast t) ;
void walk_let(ast t) ;
void walk_let_array(ast t) ;
void walk_if(ast t) ;
void walk_if_else(ast t) ;
void walk_while(ast t) ;
void walk_do(ast t) ;
void walk_return(ast t) ;
void walk_return_expr(ast t) ;
void walk_expr(ast t) ;
void walk_term(ast t) ;
void walk_int(ast t) ;
void walk_string(ast t) ;
void walk_bool(ast t) ;
void walk_null(ast t) ;
void walk_this(ast t) ;
void walk_unary_op(ast t) ;
void walk_var(ast t) ;
void walk_array_index(ast t) ;
void walk_call_as_function(ast t) ;
void walk_call_as_method(ast t) ;
void walk_subr_call(ast t) ;
void walk_expr_list(ast t) ;
void walk_infix_op(ast t) ;


int tab_counter = 0;

void outputTab(int times){
    for (int i = 0; i < times; i++)
        write_to_output("    ");
}

void outputSpace(){
    write_to_output(" ");
}

void outputNL(){
    write_to_output("\n");
}

void walk_class(ast t)
{
    string myclassname = get_class_class_name(t) ;
    ast var_decs = get_class_var_decs(t) ;
    ast subr_decs = get_class_subr_decs(t) ;
    write_to_output("class");
    outputSpace();
    write_to_output(myclassname);
    outputNL();
    write_to_output("{");
    outputNL();
    tab_counter++;
    walk_class_var_decs(var_decs) ;
    walk_subr_decs(subr_decs) ;
    tab_counter--;
    write_to_output("}");
    outputNL();
}

vector<string> static_vec;
vector<string> field_vec;

void walk_class_var_decs(ast t)
{
    int ndecs = size_of_class_var_decs(t) ;
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        ast var_dec = get_class_var_decs(t,i);
        string segment = get_var_dec_segment(var_dec) ;
        if (segment == "static")
            walk_var_dec(var_dec, i == ndecs - 1 || get_var_dec_segment(get_class_var_decs(t,i + 1)) != segment) ;
    }

    for ( int i = 0 ; i < ndecs ; i++ )
    {
        ast var_dec = get_class_var_decs(t,i);
        string segment = get_var_dec_segment(var_dec) ;
        if (segment == "this")
            walk_var_dec(var_dec, i == ndecs - 1 || get_var_dec_segment(get_class_var_decs(t,i + 1)) != segment) ;
    }
}

void walk_var_dec(ast t, bool isEnd)
{
    string name = get_var_dec_name(t) ;
    string type = get_var_dec_type(t) ;
    //int offset = get_var_dec_offset(t) ;
    string segment = get_var_dec_segment(t) ;

    if (segment != "argument")
        outputTab(tab_counter);
    if (segment == "local"){
        write_to_output("var");
    }
    else if (segment == "static"){
        write_to_output(segment);
    }else if (segment == "this"){
        write_to_output("field");
    }
    if (segment != "argument")
        outputSpace();
    write_to_output(type);
    outputSpace();
    write_to_output(name);
    if (segment != "argument"){
        outputSpace();
        write_to_output(";");
        outputNL();
    }
    if (isEnd)
    {
        outputNL();
    }
}

void walk_subr_decs(ast t)
{
    int size = size_of_subr_decs(t) ;
    for ( int i = 0 ; i < size ; i++ )
    {
        if (i != 0)
            outputNL();
        walk_subr(get_subr_decs(t,i)) ;
    }
}

void walk_subr(ast t)
{
    ast subr = get_subr_subr(t) ;

    switch(ast_node_kind(subr))
    {
        case ast_constructor:
            walk_constructor(subr) ;
            break ;
        case ast_function:
            walk_function(subr) ;
            break ;
        case ast_method:
            walk_method(subr) ;
            break ;
        default:
            fatal_error(0,"Unexpected subroutine kind") ;
            break ;
    }
}

void walk_constructor(ast t)
{
    string vtype = get_constructor_vtype(t) ;
    string name = get_constructor_name(t) ;
    ast param_list = get_constructor_param_list(t) ;
    ast subr_body = get_constructor_subr_body(t) ;

    outputTab(tab_counter);
    write_to_output("constructor");
    outputSpace();
    write_to_output(vtype);
    outputSpace();
    write_to_output(name);
    write_to_output("(");
    walk_param_list(param_list) ;
    write_to_output(")");
    outputNL();
    outputTab(tab_counter);
    write_to_output("{");
    outputNL();
    tab_counter++;
    walk_subr_body(subr_body) ;
    tab_counter--;
    outputTab(tab_counter);
    write_to_output("}");
    outputNL();
}

void walk_function(ast t)
{
    string vtype = get_function_vtype(t) ;
    string name = get_function_name(t) ;
    ast param_list = get_function_param_list(t) ;
    ast subr_body = get_function_subr_body(t) ;
    outputTab(tab_counter);
    write_to_output("function");
    outputSpace();
    write_to_output(vtype);
    outputSpace();
    write_to_output(name);
    write_to_output("(");
    walk_param_list(param_list) ;
    write_to_output(")");
    outputNL();
    outputTab(tab_counter);
    write_to_output("{");
    outputNL();
    tab_counter++;
    walk_subr_body(subr_body) ;
    tab_counter--;
    outputTab(tab_counter);
    write_to_output("}");
    outputNL();
}

void walk_method(ast t)
{
    string vtype = get_method_vtype(t) ;
    string name = get_method_name(t) ;
    ast param_list = get_method_param_list(t) ;
    ast subr_body = get_method_subr_body(t) ;

    outputTab(tab_counter);
    write_to_output("method");
    outputSpace();
    write_to_output(vtype);
    outputSpace();
    write_to_output(name);
    write_to_output("(");
    walk_param_list(param_list) ;
    write_to_output(")");
    outputNL();
    outputTab(tab_counter);
    write_to_output("{");
    outputNL();
    tab_counter++;
    walk_subr_body(subr_body) ;
    tab_counter--;
    outputTab(tab_counter);
    write_to_output("}");
    outputNL();
}

void walk_param_list(ast t)
{
    int ndecs = size_of_param_list(t) ;
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        walk_var_dec(get_param_list(t,i), false) ;
        if (i + 1 != ndecs)
            write_to_output(",");
    }
}

void walk_subr_body(ast t)
{
    ast decs = get_subr_body_decs(t) ;
    ast body = get_subr_body_body(t) ;

    walk_var_decs(decs) ;
    walk_statements(body) ;
}

void walk_var_decs(ast t)
{
    int ndecs = size_of_var_decs(t) ;
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        walk_var_dec(get_var_decs(t,i), i == ndecs - 1) ;
    }
}

bool statement_nl;

void walk_statements(ast t)
{
    int nstatements = size_of_statements(t) ;
    statement_nl = false;
    for ( int i = 0 ; i < nstatements ; i++ )
    {
        if (statement_nl){
            outputNL();
        }
        statement_nl = false;
        walk_statement(get_statements(t,i)) ;
    }
}

void walk_statement(ast t)
{
    ast statement = get_statement_statement(t) ;

    switch(ast_node_kind(statement))
    {
        case ast_let:
            walk_let(statement) ;
            break ;
        case ast_let_array:
            walk_let_array(statement) ;
            break ;
        case ast_if:
            walk_if(statement) ;
            break ;
        case ast_if_else:
            walk_if_else(statement) ;
            break ;
        case ast_while:
            walk_while(statement) ;
            break ;
        case ast_do:
            walk_do(statement) ;
            break ;
        case ast_return:
            walk_return(statement) ;
            break ;
        case ast_return_expr:
            walk_return_expr(statement) ;
            break ;
        case ast_statements:
            walk_statements(statement) ;
            break ;
        default:
            fatal_error(0,"Unexpected statement kind") ;
            break ;
    }
}

void walk_let(ast t)
{
    ast var = get_let_var(t) ;
    ast expr = get_let_expr(t) ;

    outputTab(tab_counter);
    write_to_output("let");
    outputSpace();
    walk_var(var) ;
    outputSpace();
    write_to_output("=");
    outputSpace();
    walk_expr(expr) ;
    outputSpace();
    write_to_output(";");
    outputNL();
}

void walk_let_array(ast t)
{
    ast var = get_let_array_var(t) ;
    ast index = get_let_array_index(t) ;
    ast expr = get_let_array_expr(t) ;

    outputTab(tab_counter);
    write_to_output("let");
    outputSpace();
    walk_var(var) ;

    write_to_output("[");
    walk_expr(index) ;
    write_to_output("]");

    outputSpace();
    write_to_output("=");
    outputSpace();
    walk_expr(expr) ;
    outputSpace();
    write_to_output(";");
    outputNL();
}

void walk_if(ast t)
{
    ast condition = get_if_condition(t) ;
    ast if_true = get_if_if_true(t) ;


    outputTab(tab_counter);
    write_to_output("if");
    outputSpace();
    write_to_output("(");
    walk_expr(condition) ;
    write_to_output(")");
    outputNL();

    outputTab(tab_counter);
    write_to_output("{");
    outputNL();
    tab_counter++;

    walk_statements(if_true) ;

    tab_counter--;
    outputTab(tab_counter);
    write_to_output("}");
    outputNL();
    statement_nl = true;
}

void walk_if_else(ast t)
{
    ast condition = get_if_else_condition(t) ;
    ast if_true = get_if_else_if_true(t) ;
    ast if_false = get_if_else_if_false(t) ;

    outputTab(tab_counter);
    write_to_output("if");
    outputSpace();
    write_to_output("(");

    walk_expr(condition) ;

    write_to_output(")");
    outputNL();

    outputTab(tab_counter);
    write_to_output("{");
    outputNL();
    tab_counter++;

    walk_statements(if_true) ;

    tab_counter--;
    outputTab(tab_counter);
    write_to_output("}");
    outputNL();

    outputTab(tab_counter);
    write_to_output("else");
    outputNL();
    outputTab(tab_counter);
    write_to_output("{");
    outputNL();
    tab_counter++;

    walk_statements(if_false) ;

    tab_counter--;
    outputTab(tab_counter);
    write_to_output("}");
    outputNL();

    statement_nl = true;
}

void walk_while(ast t)
{
    ast condition = get_while_condition(t) ;
    ast body = get_while_body(t) ;
    outputTab(tab_counter);
    write_to_output("while");
    outputSpace();
    write_to_output("(");
    walk_expr(condition) ;
    write_to_output(")");
    outputNL();
    outputTab(tab_counter);
    write_to_output("{");
    outputNL();
    tab_counter++;

    walk_statements(body) ;

    tab_counter--;
    outputTab(tab_counter);
    write_to_output("}");
    outputNL();
    statement_nl = true;
}

void walk_do(ast t)
{
    ast call = get_do_call(t) ;
    outputTab(tab_counter);
    write_to_output("do");
    outputSpace();
    switch(ast_node_kind(call))
    {
        case ast_call_as_function:
            walk_call_as_function(call) ;
            break ;
        case ast_call_as_method:
            walk_call_as_method(call) ;
            break ;
        default:
            fatal_error(0,"Unexpected call kind") ;
            break ;
    }
    outputSpace();
    write_to_output(";");
    outputNL();
}

void walk_return(ast t)
{
    outputTab(tab_counter);
    write_to_output("return");
    outputSpace();
    write_to_output(";");
    outputNL();
}

void walk_return_expr(ast t)
{
    ast expr = get_return_expr(t) ;
    outputTab(tab_counter);
    write_to_output("return");
    outputSpace();
    walk_expr(expr) ;
    outputSpace();
    write_to_output(";");
    outputNL();
}

void walk_expr(ast t)
{
    int term_ops = size_of_expr(t) ;
    for ( int i = 0 ; i < term_ops ; i++ )
    {
        ast term_op = get_expr(t,i) ;
        if ( i % 2 == 0 )
        {
            walk_term(term_op) ;
        }
        else
        {
            walk_infix_op(term_op) ;
        }
    }
}

void walk_term(ast t)
{
    ast term = get_term_term(t) ;

    switch(ast_node_kind(term))
    {
        case ast_int:
            walk_int(term) ;
            break ;
        case ast_string:
            walk_string(term) ;
            break ;
        case ast_bool:
            walk_bool(term) ;
            break ;
        case ast_null:
            walk_null(term) ;
            break ;
        case ast_this:
            walk_this(term) ;
            break ;
        case ast_expr:
            write_to_output("(");
            walk_expr(term) ;
            write_to_output(")");
            break ;
        case ast_unary_op:
            walk_unary_op(term) ;
            break ;
        case ast_var:
            walk_var(term) ;
            break ;
        case ast_array_index:
            walk_array_index(term) ;
            break ;
        case ast_call_as_function:
            walk_call_as_function(term) ;
            break ;
        case ast_call_as_method:
            walk_call_as_method(term) ;
            break ;
        default:
            fatal_error(0,"Unexpected term kind") ;
            break ;
    }
}

void walk_int(ast t)
{
    int _constant = get_int_constant(t) ;
    string constant = to_string(_constant);
    write_to_output(constant);
}

void walk_string(ast t)
{
    string _constant = get_string_constant(t) ;
    write_to_output("\"" + _constant + "\"");
}

void walk_bool(ast t)
{
    bool _constant = get_bool_t_or_f(t) ;
    _constant == true ? write_to_output("true") : write_to_output("false");
}

void walk_null(ast t)
{
    write_to_output("null");
}

void walk_this(ast t)
{

    write_to_output("this");
}

void walk_unary_op(ast t)
{
    string uop = get_unary_op_op(t);
    ast term = get_unary_op_term(t) ;

    write_to_output(uop);
    walk_term(term) ;
}

void walk_var(ast t)
{
    string name = get_var_name(t) ;
    string type = get_var_type(t) ;
    string segment = get_var_segment(t) ;
    //int offset = get_var_offset(t) ;

    write_to_output(name);
}

void walk_array_index(ast t)
{
    ast var = get_array_index_var(t) ;
    ast index = get_array_index_index(t) ;

    walk_var(var) ;
    write_to_output("[");
    walk_expr(index) ;
    write_to_output("]");
}

void walk_call_as_function(ast t)
{
    string class_name = get_call_as_function_class_name(t) ;
    ast subr_call = get_call_as_function_subr_call(t) ;
    write_to_output(class_name);
    write_to_output(".");
    walk_subr_call(subr_call) ;
}

void walk_call_as_method(ast t)
{
    string class_name = get_call_as_method_class_name(t) ;
    ast var = get_call_as_method_var(t) ;
    ast subr_call = get_call_as_method_subr_call(t) ;

    switch(ast_node_kind(var))
    {
        case ast_this:
            // walk_this(var) ;
            break ;
        case ast_var:
            walk_var(var) ;
            write_to_output(".");
            break ;
        default:
            fatal_error(0,"Expected var or this") ;
            break ;
    }
    walk_subr_call(subr_call) ;
}

void walk_subr_call(ast t)
{
    string subr_name = get_subr_call_subr_name(t) ;
    ast expr_list = get_subr_call_expr_list(t) ;
    write_to_output(subr_name);
    write_to_output("(");
    walk_expr_list(expr_list) ;
    write_to_output(")");
}

void walk_expr_list(ast t)
{
    int nexpressions = size_of_expr_list(t) ;
    for ( int i = 0 ; i < nexpressions ; i++ )
    {
        walk_expr(get_expr_list(t,i)) ;
        if(i + 1 < nexpressions){
            write_to_output(",");
        }
    }
}

void walk_infix_op(ast t)
{
    string op = get_infix_op_op(t) ;
    outputSpace();
    write_to_output(op);
    outputSpace();
}

// main program
int main(int argc,char **argv)
{
    // walk an AST parsed from XML and pretty print equivalent Jack code
    walk_class(ast_parse_xml()) ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}
