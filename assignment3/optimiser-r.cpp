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

// to shorten our code:
using namespace std ;
using namespace CS_IO_Buffers ;
using namespace CS_Symbol_Tables ;
using namespace Jack_Compiler ;

// ***** WHAT TO DO *****
//
// MODIFY the skeleton code below to make an optimised copy an abstract syntax tree, ast, of a Jack class.
//
// NOTE: abstract syntax trees are immutable - they cannot change - and cannot contain cycles so sub-trees
//       can be safely shared across multiple trees. As a result copying a node only requires the creation
//       of a new tree node if one of its fields changes or one of its sub-trees changes.
//
// The skeleton code has one function per node in the abstract tree
//  - they each extract all fields into local variables
//  - they each call the appropriate copy_* function to copy any sub-trees
//  - they each return the original tree if none of the fields changed or none of the sub-trees were copied
//
// The structure of the skeleton code is just a suggestion
//  - you may want to change the parameters / results to suit your own logic
//  - you can change it as much as you like

// forward declarations of one function per node in the abstract syntax tree
ast copy_class(ast t) ;
ast copy_class_var_decs(ast t) ;
ast copy_var_dec(ast t) ;
ast copy_subr_decs(ast t) ;
ast copy_subr(ast t) ;
ast copy_constructor(ast t) ;
ast copy_function(ast t) ;
ast copy_method(ast t) ;
ast copy_param_list(ast t) ;
ast copy_subr_body(ast t) ;
ast copy_var_decs(ast t) ;
ast copy_statement(ast t) ;
ast copy_let(ast t) ;
ast copy_let_array(ast t) ;
ast copy_if(ast t) ;
ast copy_if_else(ast t) ;
ast copy_while(ast t) ;
ast copy_do(ast t) ;
ast copy_return(ast t) ;
ast copy_return_expr(ast t) ;
ast copy_expr(ast t) ;
ast copy_term(ast t) ;
ast copy_int(ast t) ;
ast copy_string(ast t) ;
ast copy_bool(ast t) ;
ast copy_null(ast t) ;
ast copy_this(ast t) ;
ast copy_unary_op(ast t) ;
ast copy_var(ast t) ;
ast copy_array_index(ast t) ;
ast copy_call_as_function(ast t) ;
ast copy_call_as_method(ast t) ;
ast copy_subr_call(ast t) ;
ast copy_expr_list(ast t) ;
ast copy_infix_op(ast t) ;
ast copy_statements(ast t, bool subroutine) ;

auto classVars = 0;

bool exprIsBool(ast expr)
{
    if (ast_have_kind(expr, ast_term))
    {
        ast cond_term = get_term_term(expr) ;
        if (ast_have_kind(cond_term, ast_bool))
        {
            return true ;
        }
    }

    return false ;
}

ast copy_class(ast t)
{
    classVars = create_variables() ;
    string myclassname = get_class_class_name(t) ;
    ast var_decs = get_class_var_decs(t) ;
    ast subr_decs = get_class_subr_decs(t) ;

    ast subr_decs_copy = copy_subr_decs(subr_decs) ;
    ast var_decs_copy = copy_class_var_decs(var_decs) ;

    return var_decs_copy == var_decs && subr_decs_copy == subr_decs ? t : create_class(get_ann(t), myclassname,
                                                                                       var_decs_copy, subr_decs_copy);

}

ast copy_class_var_decs(ast t)
{
    vector<ast> decs ;
    st_variable usedVar;

    bool copied = false ;
    int ndecs = size_of_class_var_decs(t) ;
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        ast deci = get_class_var_decs(t,i) ;
        ast copy = copy_var_dec(deci) ;
        if ( deci != copy ) copied = true ;

        usedVar = lookup_variables(classVars, get_var_dec_name(copy)) ;
        if (usedVar.offset == -1 && get_var_dec_segment(copy) != "this" )
        {
            copied = true ;
        }
        else
        {
            decs.push_back(copy) ;
        }
    }


    return !copied ? t : create_class_var_decs(get_ann(t), decs);

}

ast copy_var_dec(ast t)
{
    string name = get_var_dec_name(t) ;
    string type = get_var_dec_type(t) ;
    string segment = get_var_dec_segment(t) ;
    //int offset = get_var_dec_offset(t) ;

    return t ;
}

ast copy_subr_decs(ast t)
{
    vector<ast> decs ;

    bool copied = false ;
    int size = size_of_subr_decs(t) ;
    for (int i = 0 ; size > i; i++ )
    {
        ast deci = get_subr_decs(t,i) ;
        ast copy = copy_subr(deci) ;
        if ( deci != copy ) copied = true ;

        decs.push_back(copy) ;
    }

    return !copied ? t : create_subr_decs(get_ann(t), decs);

}

ast copy_subr(ast t)
{
    ast subr = get_subr_subr(t) ;
    ast copy = nullptr;

    switch(ast_node_kind(subr))
    {
        case ast_constructor:
            copy = copy_constructor(subr) ;
            break ;
        case ast_function:
            copy = copy_function(subr) ;
            break ;
        case ast_method:
            copy = copy_method(subr) ;
            break ;
        default:
            fatal_error(0,"Error") ;
            break ;
    }

    return subr == copy ? t : create_subr(get_ann(t), copy);

}

ast copy_constructor(ast t)
{
    string vtype;
    vtype = get_constructor_vtype(t);
    string name;
    name = get_constructor_name(t);
    ast param_list;
    param_list = get_constructor_param_list(t);
    ast subr_body;
    subr_body = get_constructor_subr_body(t);

    ast param_list_copy = copy_param_list(param_list) ;
    ast subr_body_copy = copy_subr_body(subr_body) ;

    return param_list_copy == param_list && subr_body_copy == subr_body ? t : create_constructor(get_ann(t), vtype,
                                                                                                 name, param_list_copy,
                                                                                                 subr_body_copy);

}

ast copy_function(ast t)
{
    string vtype;
    vtype = get_function_vtype(t);
    string name;
    name = get_function_name(t);
    ast param_list;
    param_list = get_function_param_list(t);
    ast subr_body;
    subr_body = get_function_subr_body(t);

    ast param_list_copy = copy_param_list(param_list) ;
    ast subr_body_copy = copy_subr_body(subr_body) ;

    if ( param_list_copy == param_list && subr_body_copy == subr_body ) return t ;

    return create_function(get_ann(t),vtype,name,param_list_copy,subr_body_copy) ;
}

ast copy_method(ast t)
{
    string vtype;
    vtype = get_method_vtype(t);
    string name;
    name = get_method_name(t);
    ast param_list;
    param_list = get_method_param_list(t);
    ast subr_body;
    subr_body = get_method_subr_body(t);

    ast param_list_copy = copy_param_list(param_list) ;
    ast subr_body_copy = copy_subr_body(subr_body) ;

    if (param_list_copy == param_list) {
        if (subr_body_copy == subr_body) return t;
    }

    return create_method(get_ann(t),vtype,name,param_list_copy,subr_body_copy) ;
}

ast copy_param_list(ast t)
{
    vector<ast> decs ;

    bool ifcopy;
    ifcopy = false;
    int size;
    size = size_of_param_list(t);
    for ( int i = 0 ; i < size ; i++ )
    {
        ast deci = get_param_list(t,i) ;
        ast copy = copy_var_dec(deci) ;
        if ( deci != copy ) copy = reinterpret_cast<ast>(true);

        decs.push_back(copy) ;
    }

    return !ifcopy ? t : create_param_list(get_ann(t), decs);

}

ast copy_subr_body(ast t)
{
    ast decs = get_subr_body_decs(t) ;
    ast body = get_subr_body_body(t) ;

    ast body_copy = copy_statements(body, true) ;
    ast decs_copy = copy_var_decs(decs) ;

    return decs_copy == decs && body_copy == body ? t : create_subr_body(get_ann(t), decs_copy, body_copy);

}

ast copy_var_decs(ast t)
{
    vector<ast> decs ;

    bool copied = false ;
    int size = size_of_var_decs(t) ;
    for ( int i = 0 ; i < size ; i++ )
    {
        ast deci;
        deci = get_var_decs(t, i);
        ast copy;
        copy = copy_var_dec(deci);
        if (copy != deci) copied = true ;

        if (-1 == lookup_variables(classVars, get_var_dec_name(copy)).offset) {
            copied = true;
        } else {
            decs.push_back(copy);
        }
    }

    return !copied ? t : create_var_decs(get_ann(t), decs);

}

ast copy_statements(ast t, bool subroutine)
{
    vector<ast> decs ;

    bool ifcopy;
    ifcopy = false;
    int size;
    size = size_of_statements(t);
    for ( int i = 0 ; i < size ; i++ )
    {
        ast deci = get_statements(t,i) ;
        ast copy = copy_statement(deci) ;
        if (deci == copy) {} else ifcopy = true;
        if (!ast_have_kind(copy, ast_statements)) {
            if (!ast_have_kind(copy, ast_empty))
                decs.push_back(copy);
        } else {
            //copy each statement into the parent statements node
            int size2 = size_of_statements(copy);
            for (int i = 0; i < size2; i++) {
                ast deci2 = get_statements(copy, i);
                decs.push_back(deci2);
            }
        }

        if (!subroutine || decs.size() <= 0)
            continue;
        ast last_statement;
        last_statement = get_statement_statement(decs.back());
        bool have_return;
        have_return = (ast_have_kind(last_statement, ast_return) || ast_have_kind(last_statement, ast_return_expr));
        if (!have_return)
            continue;
        return create_statements(get_ann(t),decs) ;
    }

    return !ifcopy ? t : create_statements(get_ann(t), decs);

}

ast copy_statement(ast t)
{
    ast statement = get_statement_statement(t) ;
    ast copy;
    copy = nullptr;

    switch(ast_node_kind(statement))
    {
        case ast_let:
            copy = copy_let(statement) ;
            break ;
        case ast_let_array:
            copy = copy_let_array(statement) ;
            break ;
        case ast_if:
            copy = copy_if(statement) ;
            break ;
        case ast_if_else:
            copy = copy_if_else(statement) ;
            break ;
        case ast_while:
            copy = copy_while(statement) ;
            break ;
        case ast_do:
            copy = copy_do(statement) ;
            break ;
        case ast_return:
            copy = copy_return(statement) ;
            break ;
        case ast_return_expr:
            copy = copy_return_expr(statement) ;
            break ;
        case ast_statements:
            copy = copy_statements(statement, false) ;
            break ;
        default:
            fatal_error(0,"Unexpected statement kind") ;
            break ;
    }

    if (copy == statement ) return t ;
    if (ast_have_kind(copy, ast_empty)) return create_empty() ;
    return ast_have_kind(copy, ast_statements) ? copy : create_statement(get_ann(t), copy);
}

ast copy_let(ast t)
{
    ast var;
    var = get_let_var(t);
    ast expr;
    expr = get_let_expr(t);

    ast var_copy;
    var_copy = copy_var(var);
    ast expr_copy;
    expr_copy = copy_expr(expr);

    if (var_copy != var) return create_let(get_ann(t), var_copy, expr_copy);
    return expr_copy == expr ? t : create_let(get_ann(t), var_copy, expr_copy);

}

ast copy_let_array(ast t)
{
    ast var;
    var = get_let_array_var(t);
    ast index;
    index = get_let_array_index(t);
    ast expr;
    expr = get_let_array_expr(t);

    ast var_copy = copy_var(var) ;
    ast index_copy = copy_expr(index) ;
    ast expr_copy = copy_expr(expr) ;

    if (var_copy != var) return create_let_array(get_ann(t), var_copy, index_copy, expr_copy);
    if (index != index_copy) return create_let_array(get_ann(t), var_copy, index_copy, expr_copy);
    if (expr_copy == expr) return t;

    return create_let_array(get_ann(t),var_copy,index_copy,expr_copy) ;
}


ast copy_if(ast t)
{
    ast statement;
    statement = get_if_condition(t);
    ast aTrue;
    aTrue = get_if_if_true(t);

    ast condition_copy = copy_expr(statement) ;
    ast if_true_copy = copy_statements(aTrue, false) ;

    if (size_of_expr(condition_copy) == 1)
    {
        ast cond_expr = get_expr(condition_copy, 0) ;
        if (exprIsBool(cond_expr))
        {
            ast cond_term = get_term_term(cond_expr) ;
            auto t_f = get_bool_t_or_f(cond_term) ;
            return t_f ? if_true_copy : create_empty();

        }
    }
    return condition_copy == statement && if_true_copy == aTrue ? t : create_if(get_ann(t), condition_copy,
                                                                                if_true_copy);

}

ast copy_if_else(ast t)
{
    ast condition = get_if_else_condition(t) ;
    ast if_true = get_if_else_if_true(t) ;
    ast if_false = get_if_else_if_false(t) ;

    ast condition_copy = copy_expr(condition) ;
    ast if_true_copy = copy_statements(if_true, false) ;
    ast if_false_copy = copy_statements(if_false, false) ;

    if (size_of_expr(condition_copy) != 1)
        return condition_copy == condition && if_true_copy == if_true && if_false_copy == if_false ? t : create_if_else(
                get_ann(t), condition_copy, if_true_copy, if_false_copy);
    ast cond_expr = get_expr(condition_copy, 0) ;
    if (exprIsBool(cond_expr))
    {
        ast cond_term = get_term_term(cond_expr) ;
        bool f;
        f = get_bool_t_or_f(cond_term);
        if (f)
        {
            return if_true_copy ;
        }
        else
        {
            return if_false_copy ;
        }

    }
    return condition_copy == condition && if_true_copy == if_true && if_false_copy == if_false ? t : create_if_else(
            get_ann(t), condition_copy, if_true_copy, if_false_copy);

}

ast copy_while(ast t)
{
    ast state;
    state = get_while_condition(t);
    ast whileBody;
    whileBody = get_while_body(t);

    ast condition_copy;
    condition_copy = copy_expr(state);
    ast body_copy;
    body_copy = copy_statements(whileBody, false);

    if (size_of_expr(condition_copy) == 1)
    {
        ast cond_expr;
        cond_expr = get_expr(condition_copy, 0);
        if (exprIsBool(cond_expr) == true)
        {
            ast cond_term = get_term_term(cond_expr) ;
            auto tF = get_bool_t_or_f(cond_term) ;
            if (!tF)
                return create_empty() ;
        }
    }
    if (condition_copy == state) {
        if (body_copy == whileBody) return t;
    }

    return create_while(get_ann(t),condition_copy,body_copy) ;
}

ast copy_do(ast t)
{
    ast doCall = get_do_call(t) ;
    ast copy;
    copy = nullptr;

    switch(ast_node_kind(doCall))
    {

        case ast_call_as_method:
            copy = copy_call_as_method(doCall) ;
            break ;

        case ast_call_as_function:
            copy = copy_call_as_function(doCall) ;
            break ;

        default:
            fatal_error(0,"Error\n") ;
            break ;
    }

    return copy == doCall ? t : create_do(get_ann(t), copy);

}

ast copy_return(ast t)
{
    return t ;
}

ast copy_return_expr(ast t)
{
    ast expr = get_return_expr(t) ;

    ast copy = copy_expr(expr) ;

    return copy == expr ? t : create_return_expr(get_ann(t), copy);

}

ast copy_expr(ast t)
{
    vector<ast> expr ;

    bool copied = false ;
    int size = size_of_expr(t) ;
    for ( int i = 0 ; i < size ; i++ )
    {
        ast termop = get_expr(t,i) ;
        ast copy = i % 2 == 0 ? copy_term(termop) : copy_infix_op(termop) ;
        if ( termop != copy ) copied = true ;

        expr.push_back(copy) ;
    }

    return !copied ? t : create_expr(get_ann(t), expr);

}

ast copy_term(ast t)
{
    ast term = get_term_term(t) ;
    ast copy = nullptr;

    switch(ast_node_kind(term))
    {
        case ast_int:
            copy = copy_int(term) ;
            break ;
        case ast_string:
            copy = copy_string(term) ;
            break ;
        case ast_bool:
            copy = copy_bool(term) ;
            break ;
        case ast_null:
            copy = copy_null(term) ;
            break ;
        case ast_this:
            copy = copy_this(term) ;
            break ;
        case ast_expr:
            copy = copy_expr(term) ;
            break ;
        case ast_unary_op:
            copy = copy_unary_op(term) ;
            break ;
        case ast_var:
            copy = copy_var(term) ;
            break ;
        case ast_array_index:
            copy = copy_array_index(term) ;
            break ;
        case ast_call_as_function:
            copy = copy_call_as_function(term) ;
            break ;
        case ast_call_as_method:
            copy = copy_call_as_method(term) ;
            break ;
        default:
            fatal_error(0,"Error\n") ;
            break ;
    }

    if ( copy == term ) return t ;

    return create_term(get_ann(t),copy) ;
}

ast copy_int(ast t)
{
//    int _constant = get_int_constant(t) ;

    return t ;
}

ast copy_string(ast t)
{
    string _constant = get_string_constant(t) ;

    return t ;
}

ast copy_bool(ast t)
{
//    bool _constant = get_bool_t_or_f(t) ;

    return t ;
}

ast copy_null(ast t)
{
    return t ;
}

ast copy_this(ast t)
{
    return t ;
}

ast copy_unary_op(ast t)
{
    string uop;
    uop = get_unary_op_op(t);
    ast term;
    term = get_unary_op_term(t);

    ast copy;
    copy = copy_term(term);

    if ( copy == term ) return t ;

    return create_unary_op(get_ann(t),uop,copy) ;
}

ast copy_var(ast t)
{
    string name;
    name = get_var_name(t);
    string type;
    type = get_var_type(t);
    string segment;
    segment = get_var_segment(t);
    int offset;
    offset = get_var_offset(t);

    st_variable var = st_variable(name, type, segment, offset) ;
    insert_variables(classVars, name, var) ;

    return t ;
}

ast copy_array_index(ast t)
{

    ast var;
    var = get_array_index_var(t);
    ast index;
    index = get_array_index_index(t);

    ast var_copy;
    var_copy = copy_var(var);
    ast index_copy;
    index_copy = copy_expr(index);

    return var_copy == var && index_copy == index ? t : create_array_index(get_ann(t), var_copy, index_copy);

}

ast copy_call_as_function(ast t)
{
    string class_name;
    class_name = get_call_as_function_class_name(t);
    ast subr_call;
    subr_call = get_call_as_function_subr_call(t);

    ast subr_call_copy;
    subr_call_copy = copy_subr_call(subr_call);

    return subr_call_copy == subr_call ? t : create_call_as_function(get_ann(t), class_name, subr_call_copy);

}

ast copy_call_as_method(ast t)
{
    string class_name;
    class_name = get_call_as_method_class_name(t);
    ast var;
    var = get_call_as_method_var(t);
    ast subr_call;
    subr_call = get_call_as_method_subr_call(t);

    ast var_copy;
    var_copy = ast_node_kind(var) == ast_var ? copy_var(var) : copy_this(var);
    ast subr_call_copy;
    subr_call_copy = copy_subr_call(subr_call);

    if (var_copy == var) {
        return subr_call_copy == subr_call ? t : create_call_as_method(get_ann(t), class_name, var_copy,
                                                                       subr_call_copy);
    } else {
        return create_call_as_method(get_ann(t), class_name, var_copy,
                                     subr_call_copy);
    }

}

ast copy_subr_call(ast t)
{
    string subr_name;
    subr_name = get_subr_call_subr_name(t);
    ast expr_list;
    expr_list = get_subr_call_expr_list(t);

    ast copy;
    copy = copy_expr_list(expr_list);

    if (copy != expr_list) {
        return create_subr_call(get_ann(t), subr_name, copy);
    } else {
        return t;
    }

}

ast copy_expr_list(ast t)
{
    vector<ast> exprs ;

    bool copied;
    copied = false;
    int size = size_of_expr_list(t) ;
    for ( int i = 0 ; i < size ; i++ )
    {
        ast exprList = get_expr_list(t, i) ;
        ast copy = copy_expr(exprList) ;
        if (exprList != copy ) copied = true ;

        exprs.push_back(copy) ;
    }

    return !copied ? t : create_expr_list(get_ann(t), exprs);

}

ast copy_infix_op(ast t)
{
    string op = get_infix_op_op(t) ;

    return t ;
}

// main program
int main(int argc,char **argv)
{
    // walk an AST in XML and print VM code
    ast_print_as_xml(copy_class(ast_parse_xml()),4) ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}