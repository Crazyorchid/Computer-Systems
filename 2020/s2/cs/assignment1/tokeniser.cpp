// a skeleton implementation of a tokeniser

#include "tokeniser-extras.h"

// to shorten the code
using namespace std ;

/////////////////////////////////////////////////////////////////////////

namespace Assignment_Tokeniser
{

    static TokenKind new_token_kind ;

    // return the next Token object by reading more of the input
    // you must read input using the nextch() function
    // the last character read is in the static variable ch
    // always read one character past the end of the token being returned
    static void parse_extra(TokenKind kind)
    {
        //parse one extra ch so that ch can be the start of the next token
        new_token_kind = kind ;
        nextch();
    }

    static void parse_wspace(TokenKind kind)
    {
        //parse the white space
        parse_extra(kind);
    }

    static void parse_identifier()
    {
        new_token_kind = tk_identifier;
        do nextch() ; while ( c_have(cg_extends_identifier) ) ;
    }

    static void parse_number(TokenKind kind)
    {
    	new_token_kind = kind;
    	do nextch(); while (c_have(cg_digit|tk_scientific)) ;

    }

    static void parse_string()
    {
    	new_token_kind = tk_string;
    	do nextch() ; while (c_have(cg_instring));

    }

    static void parse_keyword(TokenKind kind)
    {
    	new_token_kind = kind;
    	do nextch(); while(c_have(tk_done|tk_while|tk_if_goto|tk_this)); //keyword ::= 'done'|'while'|'procedure'|'if-goto'|'this' 

    }

    static void parse_symbol()
    {

    }

    static void parse_eol_comment()
    {

    }

    static void pare_adhoc_comment()
    {

    }

        

    

    
    
    Token next_token()
    {
                        // ch is always the next char to read
   
                        // this should follow the style used in the workshops ...
                        // but remember that the token grammar is different in this assignment
                        //
                        // add additional case labels here for characters that can start tokens
                        // call a parse_*() function to parse the token
                        //

                     
                        // End of Inptut

    switch(ch)
    {
    	case ' ':               // white space tokens
            parse_wspace(tk_space) ;
            break ;
        case '\n':
            parse_wspace(tk_newline) ;
            break ;

    	case 'a'...'z':

    		parse_identifier();
    		break;

        /*case '0':

        parse_number();
        break;

        case '0'...'9':
        parse_number(tk_integer);*/

        case ' '|'!'|'#'-'~':
        	parse_string();
        	break;

        case 'done':
        	parse_keyword(tk_done);
        	break;

        case 'while':
        	parse_keyword(tk_while);
        	break;

		case 'procedure':
        	parse_keyword(tk_procedure);
        	break;

		case 'if-goto':
        	parse_keyword(tk_if_goto);
        	break;

		case 'this':
        	parse_keyword(tk_this);
        	break;


        case EOF:
            new_token_kind = tk_eoi ;
            break ;
        
        default:
            c_did_not_find(cg_start_of_token) ;
            break ;
        }

        Token token = new_token(new_token_kind) ;

                        // before returning a token check if the kind or spelling needs updating
                        // ...

        return token ;
    }
 }

