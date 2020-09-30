#ifndef WORKSHOP_TOKENISER_H
#define WORKSHOP_TOKENISER_H

#include <string>

// Interface for the workshop tokeniser
namespace Workshop_Tokeniser
{
    // shorthand name for the string type
    typedef std::string string ;

    // the ID used to reference a Token - the implementation is hidden
    class _Token ;
    typedef _Token *Token ;

    // The kinds of token that are recognised?
    enum TokenKind : unsigned int
    {
        // *** BNF syntax for tokens
        // * literals inside ' '
        // * grouping ( )
        // * ASCII ranges -
        // * alternatives |
        // * 0 or 1 ?
        // * 0 or more *
        // * 1 or more +
        //

        // Workshop Example Language Tokeniser Tokens
        // Grammar rule:    Definition
        // input:           (wspace | token)* eoi
        // wspace:          '\n' | ' '
        // token:           identifier | integer | op | varop | symbol | keyword
        // identifier:      ('a'-'z'|'A'-'Z')('a'-'z'|'A'-'Z'|'0'-'9')*
        // integer:         ('0'-'9')('0'-'9')*
        // op:              '+' | '-' | '*' | '/'
        // varop:           lt_le | assign_eq | not_ne | gt_gte
        // lt_le:           '<' | '<='
        // assign_eq:       '=' | '=='
        // not_ne:          '!' | '!='
        // gt_gte:          '>' | '>='
        // symbol:          '@' | '{' | '}' | '(' | ')' | ':' | ';' | '.' | ',' | '"'
        // keyword:         'var' | 'while' | 'if' | 'else' | 'let'
        // eoi:             the end of the input or any unexpected character
        //
        // * all input after returning an eoi token is ignored
        // * all the keywords satisfy the grammar rule for an identifier
        // * after an identifier token has been completed the keyword_or_identifier()
        //   function must be used to check if the identifier is actually a keyword
        // * the only acceptable characters in the input are those used in the grammar

                            // the following token kinds are returned by the tokeniser
                            // these tokens belong to the group tg_wspace
        tk_newline,         // '\n'
        tk_carriage_return, // '\r'
        tk_space,           // ' '

        tk_identifier,      // any identifier
        tk_integer,         // any integer

                            // these tokens belong to the group tg_op
        tk_add,             // '+'
        tk_sub,             // '-'
        tk_times,           // '*'
        tk_divide,          // '/'

                            // these tokens belong to the group tg_varop
        tk_lt,              // '<'
        tk_le,              // '<='
        tk_assign,          // '='
        tk_eq,              // '=='
        tk_not,             // '!'
        tk_ne,              // '!='
        tk_ge,              // '>='
        tk_gt,              // '>'

                            // these tokens belong to the group tg_symbol
        tk_at,              // '@'
        tk_lcb,             // '{'
        tk_rcb,             // '}'
        tk_lrb,             // '('
        tk_rrb,             // ')'
        tk_colon,           // ':'
        tk_semi,            // ';'
        tk_dot,             // '.'
        tk_comma,           // ','
        tk_dquote,          // '"'

                            // these tokens belong to the group tg_keyword
        tk_var,             // 'var'
        tk_while,           // 'while'
        tk_if,              // 'if'
        tk_else,            // 'else'
        tk_let,             // 'let'

        tk_eoi,             // end of input reached or an unexpected character was found

                            // the following are not produced by the tokeniser

                            // these are provided to describe groups of tokens, hence named tg_* not tk_*
        tg_wspace,          // one of tk_newline, tk_carriage_return, tk_space
        tg_op,              // one of tk_add, tk_sub, tk_times, tk_divide
        tg_varop,           // one of tk_lt, tk_le, tk_assign, tk_eq, tk_not, tk_ne, tk_ge, tk_gt
        tg_symbol,          // one of tk_at, tk_lcb, tk_rcb, tk_lrb, tk_rrb, tk_colon, tk_semi,
                            //        tk_dot, tk_comma or tk_dquote
        tg_keyword,         // one of tk_var, tk_while, tk_if, tk_else, tk_let

        tk_oops             // for error handling
    } ;

    // ***** the following are implemented in the pre-compiled lib/lib.a object file *****

    // return a string representation of the given token kind
    // returns "" if the kind is not recognised
    extern string to_string(TokenKind kind) ;

    // return the TokenKind for the given token
    // it is a fatal error if the token was not returned by a call to next_token()
    extern TokenKind token_kind(Token token) ;

    // return the spelling for the given token
    // it is a fatal error if the token was not returned by a call to next_token()
    extern string token_spelling(Token token) ;

    // return a string representation of the given token
    // it is a fatal error if the token was not returned by a call to next_token()
    extern string to_string(Token token) ;

    // is the token of the given kind or does it belong to the given group of tokens?
    // eg token_is(t,tk_colon) returns true if token_kind(t) is tk_colon
    // eg token_is(t,tg_op) returns true if token_kind(t) is tk_add, tk_sub, tk_times, tk_divide or tg_op
    extern bool token_is(Token token,TokenKind kind_or_grouping) ;

    // initialiase the tokeniser
    extern void initialise_tokeniser() ;

    // ***** the workshop involves implementing the following functions by completing tokeniser.cpp *****

    // read the next token from standard input
    extern Token next_token() ;
}

#endif //WORKSHOP_TOKENISER_H
