#include "iobuffer.h"
#include "symbols.h"
#include "tokeniser.h"
#include "abstract-syntax-tree.h"

using namespace std ;
using namespace CS_IO_Buffers ;
using namespace CS_Symbol_Tables ;
using namespace Workshop_Compiler ;

#pragma clang diagnostic ignored "-Wunused-function"



// the grammar we are recognising
// rules containing text literals are written using the matching tk_* or tg_* names
//
// TERM: DEFINITION
// program: declarations statement tk_eoi
// declarations: declaration*
// declaration: tk_var tk_identifier tk_identifier tk_semi
// statement:  whileStatement | ifStatement | letStatement | statementSequence
// whileStatement: tk_while tk_lrb condition tk_rrb statement
// ifStatement: tk_if tk_lrb condition tk_rrb statement (tk_else statement)?
// letStatement: tk_let tk_identifier tk_assign expression tk_semi
// statementSequence: tk_lcb statement* tk_rcb
// expression: term (tg_infix_op term)?
// condition: term tg_relop term
// term: tk_identifier | tk_integer
//
// Token groups for use with have()/have_next()/mustbe()/did_not_find():
// tg_starts_statement - matches any token that can start a statement
// tg_infix_op - matches any token that can be used as an infix_op
// tg_relop - matches any token that can be used as a relop
// tg_starts_term - matches any token that can start a term


// every parse function returns an abstract syntax tree representing what it has parsed
static ast parseProgram() ;
static ast parseDeclarations() ;
static ast parseDeclaration() ;
static ast parseStatement() ;
static ast parseWhileStatement() ;
static ast parseIfStatement() ;
static ast parseLetStatement() ;
static ast parseStatementSequence() ;
static ast parseExpression() ;
static ast parseCondition() ;
static ast parseTerm() ;

// note: we have added push/pop_error_context() calls so that 
//       you can see part of the call chain when an error occurs
// note: conditions are represented as an infix expression in the abstract syntax tree
// note: in a declaration the first identifier is the variable type

// *****           DO NOT EDIT THE CODE ABOVE           *****



/////////////////////////////////////////////////////////////////////////////
// ***** COMPLETE THE CODE BELOW TO COMPLETE THE PARSER *****

// In each parse_*() function write code to correctly initialise
// the variables passed to the tree node create*() function



// program: declarations statement tk_eoi
static ast parseProgram()
{
    push_error_context("parseProgram()") ;

    // a declaration node and a statement node
    ast decls = nullptr ;
    ast stat = nullptr ;

    // add parsing code here ...
    decls = parseDeclarations();
    stat = parseStatement();
    mustbe(tk_eoi);
    // return a program node
    ast ret = create_program(decls,stat) ;
    pop_error_context() ;
    return ret ;
}

// declarations: declaration*
static ast parseDeclarations()
{
    push_error_context("parseDeclarations()") ;

    // a vector of declaration nodes
    vector<ast> decls ;

    // add parsing code here ...
    while (have(tk_var)){
        decls.push_back(parseDeclaration());
    }
    // return a declarations node
    ast ret = create_declarations(decls) ;
    pop_error_context() ;
    return ret ;
}

// declare a variable of the given type and return the variable's tree node
static ast declare_variable(Token identifier,Token type) ;

// declaration: tk_var tk_identifier tk_identifier tk_semi
// the first identifier is the type
static ast parseDeclaration()
{
    push_error_context("parseDeclaration()") ;

    // a variable node returned by declare_variable()
    ast var = nullptr ;

    // add parsing code here ...
    mustbe(tk_var);
    Token type=mustbe(tk_identifier);
    Token id=mustbe(tk_identifier);
    var=declare_variable(id,type);
    mustbe(tk_semi);
    // return a declaration node
    ast ret = create_declaration(var) ;
    pop_error_context() ;
    return ret ;
}

// statement:  whileStatement | ifStatement | letStatement | statementSequence
static ast parseStatement()
{
    push_error_context("parseStatement()") ;
    
    // a statement node
    ast stat = nullptr ;

    // add parsing code here ...
    switch (token_kind()) {
        case tk_while:
            stat=parseWhileStatement();
            break;
        case tk_if:
            stat=parseIfStatement();
            break;
        case tk_let:
            stat=parseLetStatement();
            break;
        case tk_lcb:
            stat=parseStatementSequence();
            break;
        default:
            did_not_find(tg_starts_statement);
            break;
    }
    // return a statement node
    stat = create_statement(stat) ;
    pop_error_context() ;
    return stat ;
}

// whileStatement: tk_while tk_lrb condition tk_rrb statement
static ast parseWhileStatement()
{
    push_error_context("parseWhileStatement()") ;

    // an expression node and a statement node
    ast cond = nullptr ;
    ast stat = nullptr ;

    // add parsing code here ...
    mustbe(tk_while);
    mustbe(tk_lrb);
    cond =parseExpression();
    mustbe(tk_rrb);
    stat=parseStatement();

    // return a while node
    ast ret = create_while(cond,stat) ;
    pop_error_context() ;
    return ret ;
}

// ifStatement: tk_if tk_lrb condition tk_rrb statement (tk_else statement)?
static ast parseIfStatement()
{
    push_error_context("parseIfStatement()") ;

    // an expression node and statement nodes
    ast cond = nullptr ;
    ast then_stat = nullptr ;
    ast else_stat = nullptr ;

    // add parsing code here ...
    mustbe(tk_if);
    mustbe(tk_lrb) ;
    cond = parseExpression();

    mustbe(tk_rrb) ;
    then_stat = parseStatement();
    if (have(tk_else))
    {
        mustbe(tk_else);
        else_stat = parseStatement();
        // return an if else node
        ast ret = create_if_else(cond,then_stat,else_stat) ;
        pop_error_context() ;
        return ret ;
    }

    // return an if node
    ast ret = create_if(cond,then_stat) ;
    pop_error_context() ;
    return ret ;
}

// return the variable's tree node or report a fatal error if it was not previously declared
static ast lookup_variable(Token identifier) ;

// letStatement: tk_let tk_identifier tk_assign expression tk_semi
static ast parseLetStatement()
{
    push_error_context("parseLetStatement()") ;

    // a variable node and an expression node
    ast id = nullptr ;
    ast expr = nullptr ;

    // add parsing code here ...
    mustbe(tk_let);
    id = lookup_variable(mustbe(tk_identifier));
    mustbe(tk_assign);
    expr = parseExpression();
    mustbe(tk_semi);

    // return a let node
    ast ret = create_let(id,expr) ;
    pop_error_context() ;
    return ret ;
}

// statementSequence: tk_lcb statement* tk_rcb
static ast parseStatementSequence()
{
    push_error_context("parseStatementSequence()") ;

    // a vector of statemnt nodes
    vector<ast> seq ;

    // add parsing code here ...
    mustbe(tk_lcb) ;
    while ( have(tg_starts_statement) )
    {
        seq.push_back(parseStatement()) ;
    }
    mustbe(tk_rcb) ;

    return create_statements(seq) ;
    // return a statements node
    ast ret = create_statements(seq) ;
    pop_error_context() ;
    return ret ;
}

// expression: term (tg_infix_op term)?
static ast parseExpression()
{
    push_error_context("parseExpression()") ;

    // an expression node, two term nodes and a Token
    ast lhs = nullptr ;
    ast rhs = nullptr ;
    Token infix_op = nullptr ;

    // add parsing code here ...
    lhs = parseTerm();
    if (have(tk_lt))
    {
        infix_op = mustbe(tk_lt);
        rhs = parseTerm();
    }
    else if (have(tk_times))
    {
        infix_op = mustbe(tk_times);
        rhs = parseTerm();
    }
    else if (have(tk_eq))
    {
        infix_op = mustbe(tk_eq);
        rhs = parseTerm();
    }
    else if (have(tk_gt))
    {
        infix_op = mustbe(tk_gt);
        rhs = parseTerm();
    }

    if (infix_op == nullptr)
    {
        // return a single term expression node
        ast ret = create_expression(lhs) ;
        pop_error_context() ;
        return ret ;
    }
    else
    {
        // if the expression has an infix operator
        // return an infix expression node
        ast ret = create_expression(create_infix_op(lhs,token_spelling(infix_op),rhs)) ;
        pop_error_context() ;
        return ret ;
    }


}

// condition: term tg_relop term
static ast parseCondition()
{
    push_error_context("parseCondition()") ;

    // two term nodes and a Token
    ast lhs = nullptr ;
    Token relop = nullptr ;
    ast rhs = nullptr ;

    // add parsing code here ...

    // return the parsed condition as an expression node
    ast ret = create_expression(create_infix_op(lhs,token_spelling(relop),rhs)) ;
    pop_error_context() ;
    return ret ;
}

// the following helper functions can be used in parseTerm()

// turn an integer token into an ast node
static ast integer_to_ast(Token integer)
{
    return create_int(token_ivalue(integer)) ;
}

// To return the variable's tree node or report a fatal error if it was not previously declared, use:
// ast lookup_variable(Token identifier) ;

// term: tk_identifier | tk_integer
static ast parseTerm()
{
    push_error_context("parseTerm()") ;

    // a variable or integer node
    ast term = nullptr ;

    // add parsing code here ...
    if (have(tk_identifier))
    {
        term = lookup_variable(mustbe(tk_identifier));
    }
    else if (have(tk_integer))
    {
        term = integer_to_ast(mustbe(tk_integer));
    }
    // return the parsed term in a term node
    ast ret = create_term(term) ;
    pop_error_context() ;
    return ret ;
}


/////////////////////////////////////////////////////////////////////////////
// ***** DO NOT CHANGE CODE AFTER THIS POINT *****


// ****  SYMBOL TABLES  ****

// scope levels each have their own symbol tables, segment and next free location
// the offset to the next free location must start at 0
class scope
{
public:
    scope(string segment) : segment(segment), offset(0)
    {
        symbol_table = create_variables() ;
    }

    symbols symbol_table ;      // symbol table for this scope level
    string segment ;            // name of segment where variables are stored
    int offset ;                // offset into the segment where the next variable is stored
} ;

// symbol table management so we can lookup declared variables
static vector<scope> *scopes ;

// initialise the symbol tables
static void initialise_symbol_tables()
{
    scopes = new vector<scope>() ;
}

// push a new symbol table onto the stack of symbol tables
static void push_symbol_table(string segment)
{
    scopes->push_back(scope(segment)) ;
}

// pop a symbol table from the stack of symbol tables
static void pop_symbol_table()
{
    // first check there is something to pop
    int ntables = scopes->size() ;
    if ( ntables == 0 )
    {
        fatal_error(0,"attempted to pop a symbol table from an empty stack of symbol tables") ;
    }

    // remove the top scope from the stack and delete it
    symbols popped = scopes->back().symbol_table ;
    delete_variables(popped) ;
    scopes->pop_back() ;
}

// this function adds an identifier to the top symbol table on the symbol table stack
// it allocates the variable the next free offset in the current segment
// it returns a tree node representing the variable
static ast declare_variable(Token identifier,Token type)
{
    scope current = scopes->back() ;

    string varname = token_spelling(identifier) ;
    string tname = token_spelling(type) ;
    st_variable var(varname,tname,current.segment,scopes->back().offset++) ;

    if ( !insert_variables(current.symbol_table,varname,var) ) // it is an error to declare something twice
    {
        fatal_error(0,"\n" + token_context(identifier) + "Variable:  " + varname +  " has already been declared") ;
    }

    return create_variable(var.name,var.segment,var.offset,var.type) ;
}

// lookup the identifier, it is an error if it was not previously declared
// it returns a tree node representing the variable
static ast lookup_variable(Token identifier)
{
    // search symbol tables from top to bottom of the symbol table stack
    string varname = token_spelling(identifier) ;
    for ( int i = scopes->size() - 1 ; i >= 0 ; i-- )
    {
        st_variable var = lookup_variables(scopes->at(i).symbol_table,varname) ;
        if ( var.name == varname )
        {
            return create_variable(var.name,var.segment,var.offset,var.type) ;
        }
    }

    // variables not found - report a fatal error - the return is just so that the function compiles
    fatal_error(0,"\n" + token_context(identifier) + "Found undeclared variable:  " + varname) ;
    return nullptr ;
}

// **** MAIN PROGRAM ****

// main program for workshop 10/11 parser to XML
int main(int argc,char **argv)
{
    config_output(iob_immediate) ;      // make all output and errors appear immediately
    config_errors(iob_immediate) ;

    initialise_symbol_tables() ;        // initialise symbol tables
    push_symbol_table("local") ;        // push a new symbol table to hold declarations

    next_token() ;                      // read first token to initialise tokeniser
    ast program = parseProgram() ;      // parse a Program to get a parse tree

    pop_symbol_table() ;                // delete the symbol table

    ast_print_as_xml(program,4) ;       // write the tree to standard output formatted as XML

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}

