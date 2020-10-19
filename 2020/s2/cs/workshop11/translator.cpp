#include "iobuffer.h"
#include "symbols.h"
#include "abstract-syntax-tree.h"

using namespace std ;

using namespace CS_IO_Buffers ;
using namespace CS_Symbol_Tables ;
using namespace Workshop_Compiler ;

// keep global counters so we can create unique labels in while and if statements
static int while_counter = 0 ;
static int if_counter = 0 ;

// we have a legal infix operator, translate into VM command equivalent
static string translate_op(string op)
{
    int oplen = op.length() ;

    if ( oplen == 1 )
    {
        switch(op[0])
        {
        case '+': return "add" ;
        case '-': return "sub" ;
        case '*': return "call Math.multiply 2" ;
        case '/': return "call Math.divide 2" ;
        case '<': return "lt" ;
        case '>': return "gt" ;
        default:;
        }
    }
    else
    if ( oplen == 2 && op[1] == '=' )
    {
        switch(op[0])
        {
        case '<': return "gt\nnot" ;
        case '>': return "lt\nnot" ;
        case '=': return "eq" ;
        case '!': return "eq\nnot" ;
        default:;
        }
    }

    fatal_error(-1,"translate_op passed unknown op: " + op + "\n") ;
    return op ;
}

// the grammar we are recognising
// TERM: DEFINITION
// program: declarations statement eoi
// declarations: ('var' identifier ';')*
// statement:  whileStatement |
//         ifStatement |
//         letStatement |
//         '{' statementSequence '}'
// whileStatement: 'while' '(' condition ')' statement
// ifStatement: 'if' '(' condition ')' statement ('else' statement)?
// letStatement: 'let' identifier '=' expression ';'
// statementSequence: (statement)*
// expression: term (op term)?
// condition: term relop term
// term: identifier | integerConstant
// TOKEN: DEFINITION
// identifier: ('a'-'z'|'A'-'Z')('a'-'z'|'A'-'Z'|'0'-'9')*
// integerConstant:    ('0'-'9')('0'-'9')*
// relop:      '<' | '<=' | '==' | '!=' | '>' | >='
// op:         '+' | '-' | '*' | '/'
// keyword:    'var' | 'while' | 'if' | 'else' | 'let'
// symbol:     '{' | '}' | '(' | ')' | ';' | '='


// since parsing is recursive, forward declare one function to walk each non-terminal:
// note: conditions are represented by expressions
static void walkProgram(ast) ;
static int  walkDeclarations(ast) ;
static void walkStatement(ast) ;
static void walkWhileStatement(ast) ;
static void walkIfStatement(ast) ;
static void walkIfElseStatement(ast) ;
static void walkLetStatement(ast) ;
static void walkStatementSequence(ast) ;
static void walkExpression(ast) ;
static void walkTerm(ast) ;

// now implement the parsing functions
// ast create_program(ast declarations,ast statement)
static void walkProgram(ast n)
{
    push_error_context("walkProgram()") ;

    int nlocals = walkDeclarations(get_program_declarations(n)) ;

    // if the programs starts with x variable declarations, we must start with:
    //     function Main.main x
    // nextlocal is effectively a variable count so ...
    write_to_output("function Main.main " + to_string(nlocals) + "\n") ;

    walkStatement(get_program_statement(n)) ;

    // always finish with return so the VM code is a complete void function
    write_to_output("push constant 0\n") ;
    write_to_output("return\n") ;

    pop_error_context() ;
}

// ast create_declarations(vector<ast> variables)
static int walkDeclarations(ast n)
{
    push_error_context("walkDeclarations()") ;

    int ndecls = size_of_declarations(n) ;

    pop_error_context() ;
    return ndecls ;
}

// statement nodes can contain one of ast_while, ast_if, ast_if_else, ast_let or ast_statements
static void walkStatement(ast n)
{
    push_error_context("walkStatement()") ;

    ast stat = get_statement_statement(n) ;

    switch(ast_node_kind(stat))
    {
    case ast_while:
        walkWhileStatement(stat) ;
        break ;
    case ast_if:
        walkIfStatement(stat) ;
        break ;
    case ast_if_else:
        walkIfElseStatement(stat) ;
        break ;
    case ast_let:
        walkLetStatement(stat) ;
        break ;
    case ast_statements:
        walkStatementSequence(stat) ;
        break ;
    default:
        fatal_error(0,"Unknown kind of statement node found") ;
        break ;
    }

    pop_error_context() ;
}

// ast create_while(ast condition,ast body)
static void walkWhileStatement(ast n)
{
    push_error_context("walkWhileStatement()") ;

    string label = to_string(while_counter++) ;

    walkExpression(get_while_condition(n)) ;
    walkStatement(get_while_body(n)) ;

    pop_error_context() ;
}

// ast create_if(ast condition,ast if_true)
static void walkIfStatement(ast n)
{
    push_error_context("walkIfStatement()") ;

    string label = to_string(if_counter++) ;

    walkExpression(get_if_condition(n)) ;
    walkStatement(get_if_if_true(n)) ;

    pop_error_context() ;
}

// ast create_if_else(ast condition,ast if_true,ast if_false)
static void walkIfElseStatement(ast n)
{
    push_error_context("walkIfElseStatement()") ;

    string label = to_string(if_counter++) ;

    walkExpression(get_if_else_condition(n)) ;
    walkStatement(get_if_else_if_true(n)) ;
    walkStatement(get_if_else_if_false(n)) ;

    pop_error_context() ;
}

// ast create_let(ast variable,ast expression)
static void walkLetStatement(ast n)
{
    push_error_context("walkLetStatement()") ;

    ast var = get_let_variable(n) ;
    walkExpression(get_let_expression(n)) ;

    pop_error_context() ;
}

// ast create_statements(vector<ast> statements) ;
static void walkStatementSequence(ast n)
{
    push_error_context("walkStatementSequence()") ;

    int children = size_of_statements(n) ;
    for ( int i = 0 ; i < children ; i++ )
    {
        walkStatement(get_statements(n,i)) ;
    }

    pop_error_context() ;
}

// there are no expression nodes, only ast_infix_op, ast_variable and ast_int nodes
// ast create_infix_op(ast lhs,string op,ast rhs)
static void walkExpression(ast n)
{
    push_error_context("walkExpression()") ;

    ast expr = get_expression_expression(n) ;

    if ( ast_have_kind(expr,ast_infix_op) )
    {
        walkTerm(get_infix_op_lhs(expr)) ;
        walkTerm(get_infix_op_rhs(expr)) ;
        string op = get_infix_op_op(expr) ;
    }
    else
    {
        walkTerm(expr) ;
    }

    pop_error_context() ;
}

// there are no term nodes, only ast_variable and ast_int nodes
// ast create_variable(string name,string segment,int offset,string type)
static void walkTerm(ast n)
{
    push_error_context("walkTerm()") ;

    ast term = get_term_term(n) ;

    switch(ast_node_kind(term))
    {
    case ast_variable:
        break ;
    case ast_int:
        break ;
    default:
        fatal_error(0,"Unknown kind of term node found") ;
        break ;
    }

    pop_error_context() ;
}

// main program for workshop 11 XML to VM code translator
int main(int argc,char **argv)
{
    // make all output and errors appear immediately
    config_output(iob_immediate) ;
    config_errors(iob_immediate) ;

    walkProgram(ast_parse_xml()) ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}
