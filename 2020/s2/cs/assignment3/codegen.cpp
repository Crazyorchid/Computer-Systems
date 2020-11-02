/*
*   Computer Systems
*   2020, Semester 02
*   Assignment 3
*
*   student (id): a1778478
*   student (name): Runtao Zhuge
*
*/
#include "iobuffer.h"
#include "symbols.h"
#include "abstract-syntax-tree.h"
#include <map>

// to shorten our code:
using namespace std ;
using namespace CS_IO_Buffers ;
using namespace Jack_Compiler ;

// ***** WHAT TO DO *****
//
// MODIFY the skeleton code below to walk an abstract syntax tree, ast, of a Jack class
//        and generate the equivalent Hack Virtual Machine Code.
//
// NOTE: the correct segment and offset is recorded with every variable in the ast
//       so the code generator does not need to use any symbol tables
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
void walk_var_dec(ast t) ;
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
void walk_var(ast t, string push_or_pop) ;
void walk_array_index(ast t) ;
void walk_call_as_function(ast t) ;
void walk_call_as_method(ast t) ;
void walk_subr_call(ast t) ;
void walk_expr_list(ast t) ;
void walk_infix_op(ast t) ;


#define PUSH "push"
#define POP "pop"
string myclassname;
int while_label = 0;
int if_else_label = 0;
int if_label = 0;
int this_counter = 0;
void write_to_output_ln(string str){
    write_to_output(str + "\n");
}

void walk_class(ast t)
{
    myclassname = get_class_class_name(t) ;
    ast var_decs = get_class_var_decs(t) ;
    ast subr_decs = get_class_subr_decs(t) ;

    walk_class_var_decs(var_decs) ;
    walk_subr_decs(subr_decs) ;
}

void walk_class_var_decs(ast t)
{
    int ndecs = size_of_class_var_decs(t) ;
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        walk_var_dec(get_class_var_decs(t,i)) ;
    }
}

void walk_var_dec(ast t)
{
    string segment = get_var_dec_segment(t) ;
    if (segment == "this")
        this_counter = this_counter + 1;
}

void walk_subr_decs(ast t)
{
    int size = size_of_subr_decs(t) ;
    for ( int i = 0 ; i < size ; i++ )
    {
        while_label = 0;
        if_label = 0;
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

    walk_param_list(param_list) ;
    write_to_output_ln("function "+myclassname+"."+name+" "+
                       to_string(size_of_var_decs(get_subr_body_decs(subr_body))));
    write_to_output_ln("push constant "+to_string(this_counter));
    write_to_output_ln("call Memory.alloc 1");
    write_to_output_ln("pop pointer 0");
    walk_subr_body(subr_body) ;
}

void walk_function(ast t)
{
    string vtype = get_function_vtype(t) ;
    string name = get_function_name(t) ;
    ast param_list = get_function_param_list(t) ;
    ast subr_body = get_function_subr_body(t) ;

    walk_param_list(param_list) ;
    write_to_output_ln("function "+myclassname+"."+name+" "+
                       to_string(size_of_var_decs(get_subr_body_decs(subr_body))));
    walk_subr_body(subr_body) ;
}

void walk_method(ast t)
{
    string vtype = get_method_vtype(t) ;
    string name = get_method_name(t) ;
    ast param_list = get_method_param_list(t) ;
    ast subr_body = get_method_subr_body(t) ;

    walk_param_list(param_list) ;
    write_to_output_ln("function "+myclassname+"."+name+" "+
                       to_string(size_of_var_decs(get_subr_body_decs(subr_body))));
    write_to_output_ln("push argument 0");
    write_to_output_ln("pop pointer 0");
    walk_subr_body(subr_body) ;
}

void walk_param_list(ast t)
{
    int ndecs = size_of_param_list(t) ;
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        walk_var_dec(get_param_list(t,i)) ;
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
        walk_var_dec(get_var_decs(t,i)) ;
    }
}

void walk_statements(ast t)
{
    int nstatements = size_of_statements(t) ;
    for ( int i = 0 ; i < nstatements ; i++ )
    {
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

    walk_expr(expr) ;
    walk_var(var, POP) ;
}

void walk_let_array(ast t)
{
    ast var = get_let_array_var(t) ;
    ast index = get_let_array_index(t) ;
    ast expr = get_let_array_expr(t) ;

    walk_expr(index) ;
    walk_var(var, PUSH) ;
    write_to_output_ln("add");
    walk_expr(expr) ;
    write_to_output_ln("pop temp 0");
    write_to_output_ln("pop pointer 1");
    write_to_output_ln("push temp 0");
    write_to_output_ln("pop that 0");
}

void walk_if(ast t)
{
    ast condition = get_if_condition(t) ;
    ast if_true = get_if_if_true(t) ;

    string label_idx = to_string(if_label++);
    walk_expr(condition) ;
    write_to_output_ln("if-goto IF_TRUE" + label_idx);
    write_to_output_ln("goto IF_FALSE" + label_idx);
    write_to_output_ln("label IF_TRUE" + label_idx);
    walk_statements(if_true) ;
    write_to_output_ln("label IF_FALSE" + label_idx);
}

void walk_if_else(ast t)
{
    ast condition = get_if_else_condition(t) ;
    ast if_true = get_if_else_if_true(t) ;
    ast if_false = get_if_else_if_false(t) ;

    string label_idx = to_string(if_label++);
    walk_expr(condition) ;
    write_to_output_ln("if-goto IF_TRUE" + label_idx) ;
    write_to_output_ln("goto IF_FALSE" + label_idx) ;
    write_to_output_ln("label IF_TRUE" + label_idx) ;

    walk_statements(if_true) ;
    write_to_output_ln("goto IF_END" + label_idx) ;
    write_to_output_ln("label IF_FALSE" + label_idx) ;
    walk_statements(if_false) ;
    write_to_output_ln("label IF_END" + label_idx) ;
}

void walk_while(ast t)
{
    ast condition = get_while_condition(t) ;
    ast body = get_while_body(t) ;

    string label_idx = to_string(while_label++);
    write_to_output_ln("label WHILE_EXP"+label_idx);
    walk_expr(condition) ;
    write_to_output_ln("not");
    write_to_output_ln("if-goto WHILE_END"+label_idx);
    walk_statements(body) ;
    write_to_output_ln("goto WHILE_EXP"+label_idx);
    write_to_output_ln("label WHILE_END"+label_idx);
}

void walk_do(ast t)
{
    ast call = get_do_call(t) ;

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
    write_to_output_ln("pop temp 0");
}

void walk_return(ast t)
{
    write_to_output_ln("push constant 0");
    write_to_output_ln("return");
}

void walk_return_expr(ast t)
{
    ast expr = get_return_expr(t) ;

    walk_expr(expr) ;
    write_to_output_ln("return");
}

map<string, string> op_map =
        {
                {"+", "add"},
                {"-", "sub"},
                {"*", "call Math.multiply 2"},
                {"/", "call Math.divide 2"},
                {"&", "and"},
                {"|", "or"},
                {">", "gt"},
                {"=", "eq"},
                {"<", "lt"}
        };
void walk_expr1(ast t)
{
    int term_ops = size_of_expr(t) ;
    for ( int i = 0 ; i < term_ops ; i+=2 )
    {
        ast term_op = get_expr(t,i) ;
        walk_term(term_op) ;
    }
    for ( int i = ((term_ops - 1 & 1) == 1 ? term_ops - 1 : term_ops - 2); i > 0 ; i-=2 )
    {
        ast term_op = get_expr(t,i) ;
        write_to_output_ln( op_map[get_infix_op_op(term_op)] );
    }
}


void walk_expr(ast t)
{
    int term_ops = size_of_expr(t) ;
    ast term_op = get_expr(t,0);
    walk_term(term_op);

    for ( int i = 0 ; i < term_ops-1 ; i++ )
    {
        if ( i % 2 != 0 )
        {
            int n = i;

            //walk_term(term_op) ;
            ast term_op = get_expr(t,n);
            write_to_output_ln( op_map[get_infix_op_op(term_op)] );

        }
        if (i % 2 == 0)
        {
            int m = i+2;
            ast term_op = get_expr(t,m);
            walk_term(term_op) ;
            //walk_infix_op(term_op) ;
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
            walk_expr(term) ;
            break ;
        case ast_unary_op:
            walk_unary_op(term) ;
            break ;
        case ast_var:
            walk_var(term, PUSH) ;
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
    write_to_output_ln("push constant "+to_string(_constant));
}

void walk_string(ast t)
{
    string _constant = get_string_constant(t) ;
    write_to_output_ln("push constant "+to_string(_constant.size()));
    write_to_output_ln("call String.new 1");
    for(int i = 0; i < _constant.size(); i++){
        string s = to_string( (int)_constant[i] );
        write_to_output_ln("push constant "+ s);
        write_to_output_ln("call String.appendChar 2");
    }
}

void walk_bool(ast t)
{
    bool _constant = get_bool_t_or_f(t) ;
    string r = _constant ? "\nnot" : "" ;
    write_to_output_ln("push constant 0" + r);
}

void walk_null(ast t)
{
    write_to_output_ln("push constant 0");
}

void walk_this(ast t)
{
    write_to_output_ln("push pointer 0");
}

void walk_unary_op(ast t)
{
    string uop = get_unary_op_op(t);
    ast term = get_unary_op_term(t) ;

    walk_term(term) ;
    write_to_output_ln(uop == "~" ? "not" : "neg");
}

void walk_var(ast t, string push_or_pop)
{
    string name = get_var_name(t) ;
    string type = get_var_type(t) ;
    string segment = get_var_segment(t) ;
    string offset = to_string(get_var_offset(t)) ;

    write_to_output_ln(push_or_pop + " " + segment+" "+ offset);
}

void walk_array_index(ast t)
{
    ast var = get_array_index_var(t) ;
    ast index = get_array_index_index(t) ;

    walk_expr(index) ;
    walk_var(var, PUSH) ;
    write_to_output_ln("add");
    write_to_output_ln("pop pointer 1");
    write_to_output_ln("push that 0");
}

void walk_call_as_function(ast t)
{
    string class_name = get_call_as_function_class_name(t) ;
    ast subr_call = get_call_as_function_subr_call(t) ;

    string subr_name = get_subr_call_subr_name(subr_call) ;
    ast expr_list = get_subr_call_expr_list(subr_call) ;
    walk_expr_list(expr_list) ;
    write_to_output_ln("call "+class_name+"."+subr_name+" "+to_string(size_of_expr_list(expr_list)));
}

void walk_call_as_method(ast t)
{
    string class_name = get_call_as_method_class_name(t) ;
    ast var = get_call_as_method_var(t) ;
    ast subr_call = get_call_as_method_subr_call(t) ;

    switch(ast_node_kind(var))
    {
        case ast_this:
            walk_this(var) ;
            break ;
        case ast_var:
            walk_var(var, PUSH) ;
            break ;
        default:
            fatal_error(0,"Expected var or this") ;
            break ;
    }
    string subr_name = get_subr_call_subr_name(subr_call) ;
    ast expr_list = get_subr_call_expr_list(subr_call) ;
    walk_expr_list(expr_list) ;
    write_to_output_ln("call "+class_name+"."+subr_name+" "+to_string(size_of_expr_list(expr_list) + 1));
}



void walk_expr_list(ast t)
{
    int nexpressions = size_of_expr_list(t) ;
    for ( int i = 0 ; i < nexpressions ; i++ )
    {
        walk_expr(get_expr_list(t,i)) ;
    }
}


// main program
int main(int argc,char **argv)
{
    // walk an AST parsed from XML and print VM code
    walk_class(ast_parse_xml()) ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}


