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
        do nextch() ; while (c_have(cg_extends_identifier)) ;
    }

    /*static void parse_integer(TokenKind kind)
    {
    	new_token_kind = kind;
    	do nextch(); while (c_have(tk_integer)) ;

    }*/


    static void parse_digit(){
    	new_token_kind = tk_integer;
    	do nextch(); while (c_have(cg_digit19));
    }

    static void parse_string()
    {
    	new_token_kind = tk_string;
    	do nextch() ; while (c_have(cg_instring));

    }

    static void parse_keyword(TokenKind kind)
    {
    	new_token_kind = kind;
    	do nextch(); while(c_have(tk_done|tk_while|tk_if_goto|tk_this));

    }

    static void parse_eq(TokenKind kind)
    {
        new_token_kind = kind;
        do nextch(); while(c_have_next(tk_eq));
    }

    
    static void parse_symbol(TokenKind kind)
    {
    	new_token_kind = kind ;
    	do nextch(); while (c_have(tk_at|tk_stop|tk_lcb|tk_rcb|tk_lrb|tk_rrb|tk_lsb|tk_rsb));
    }


    /*static void parse_not_eq(){
    	new_token_kind = tk_not_eq ;
    	if(c_have('!')) c_have_next('='); else
    	c_did_not_find(tk_not_eq);

    }*/

    /*static void parse_eq(){
    	new_token_kind = tk_eq ;
    	if(c_have('=')) c_have_next('='); else
    	c_did_not_find(tk_not_eq);

    } */

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
    	/*case '=':
    		parse_string();
    		break;

    	case '!':
    		parse_string();
    		break;*/

    	case ' ':               // white space tokens
            parse_wspace(tk_space) ;
            break ;
        
        case '\n':
            parse_wspace(tk_newline) ;
            break ;

    	case 'a'...'z':

    		parse_identifier();
    		break;

    	case 'A'...'Z':
    		parse_identifier();
    		break;

        //case '0':

        //case '('e'|'E') ('+'|'-')':
        //parse_number(tk_scientific);


        /*case '0':
        	parse_integer(tk_integer);
        	break;*/


        case '0'...'9':
        	parse_digit();
        	break;

        /*case '+'|'-':
        parse_number(cg_start_of_exponent);*/



       
        /*case '=':
            parse_eq(tk_eq);
            break;*/

        case '#'-'~':
        	parse_string();
        	break;
        
        //'@'|'-='|'+='|'*='|'/='|'!='|'=='|'<<<'|'<<'|'>>>'|'>>'|'{'|'}'|'('|')'|'['|']'
        case '@':	
        	parse_symbol(tk_at);
        	break;


        case '.':
        	parse_symbol(tk_stop);
        	break;
        
        case '{':	
        	parse_symbol(tk_lcb);
        	break;
        
        case '}':	
        	parse_symbol(tk_rcb);
        	break;

        case '(':	
        	parse_symbol(tk_lrb);
        	break;

        case ')':	
        	parse_symbol(tk_rrb);
        	break;

        case '[':
        	parse_symbol(tk_lsb);
        	break;
        
        case ']':
        	parse_symbol(tk_rsb);
        	break;

        case '!':
        	new_token_kind = tk_not_eq ;
        	nextch() ; c_mustbe('=') ;
        	break;

        case '-':
        	new_token_kind = tk_sub_assign ;
        	nextch() ; c_mustbe('=');
        	break;

        case '=':
        	new_token_kind = tk_eq ;
        	nextch(); c_mustbe('=');
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

