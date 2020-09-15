// a skeleton implementation of a tokeniser

#include "tokeniser-extras.h"

// to shorten the code
using namespace std ;

// all code is part of the Exam_Tokeniser namespace
namespace Exam_Tokeniser
{
    // records the kind of the token being parsed
    static TokenKind new_token_kind ;

 
    // ******   ADD NEW CODE HERE  ******
    // add your own parse_*() functions here ...


    // read an extra character and set the token kind
    static void parse_extra(TokenKind kind)
    {
        new_token_kind = kind ;
        nextch() ;
    }

    static void parse_identifier(TokenKind kind)
    {
        new_token_kind = tk_identifier;
        do {
            nextch();
        }
        while (c_have(cg_extends_identifier));
    }


    static void parse_operation(TokenKind kind)
    {
        new_token_kind = kind;
        nextch();
    }


    static void parse_number(TokenKind kind)
    {
        new_token_kind = tk_number;
        nextch();
    }


    static void parse_plusminus(TokenKind kind) {
        new_token_kind = kind;
        nextch();
        if (c_have('+')) {
            if (c_have_next('+')) {
                new_token_kind = tk_operator;
            } else {
                return;
            }
        } else if (c_have('-')) {
            if (c_have_next('-')) {
                new_token_kind = tk_operator;
            } else {
                return;
            }
        }
    }

    // return the next Token object by reading more of the input
    // you must read input using the nextch() function
    // the last character read is in the static variable ch
    // always read one character past the end of the token being returned
    Token next_token()
    {
        switch(ch)      // ch is always the next char to read
        {
            case'~':
                parse_operation(tk_operator);
                break;
            case'[':
                parse_operation(tk_operator);
                break;
            case']':
                parse_operation(tk_operator);
                break;
            case'&':
                parse_operation(tk_operator);
                break;


            case'>':
                new_token_kind = tk_operator;
                nextch();
                c_mustbe('=');
                break;
            case'<':
                new_token_kind = tk_operator;
                nextch();
                c_mustbe('=');
                break;

            case'+':
                new_token_kind = tk_operator;
                nextch();
                c_mustbe('+');
                break;

            case'-':
                new_token_kind = tk_operator;
                nextch();
                c_mustbe('-');
                break;


            case '$':
                parse_identifier(tk_identifier);
                break;
            case '1'-'9':
                parse_identifier(tk_identifier);
                break;
            case '.':
                parse_identifier(tk_identifier);
                break;
            case 'c'-'p':
                parse_identifier(tk_identifier);
                break;

            case '0':
                parse_number(tk_number);
                break;

            case 'A'-'F':
                parse_number(tk_number);
                break;









                        // ******   ADD NEW CODE HERE  ******
                        // add case labels and calls for the each token kind here ...



                        // End of Inptut


        case EOF:
            new_token_kind = tk_eoi ;
            break ;

        default:        // catch all single byte characters not starting any other token
            parse_extra(tk_byte) ;
            break ;
        }

        Token token = new_token(new_token_kind) ;

                        // before returning a token check if the kind or spelling needs updating
                        // ...

        return token ;
    }
}

