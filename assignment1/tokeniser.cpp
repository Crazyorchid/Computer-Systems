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
    	do nextch(); while (c_have(tk_integer|tk_scientific)) ;

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

    /*static void parse_done()
{
    switch(ch)
    {
    case 'd':
    case 'o':
    case 'n':
    case 'e':
        nextch() ;
        break ;
    default:
        did_not_find("done") ;
    }
}

	static void parse_while()
	{
	    switch(ch)
	    {
	    case 'w':
	    case 'h':
	    case 'i':
	    case 'l':
	    case 'e':
	        nextch() ;
	        break ;
	    default:
	        did_not_find("done") ;
	    }
	}

	static void parse_procedure()
{
    mustbe('p') ;
    mustbe('r') ;
    mustbe('o') ;
    mustbe('c') ;
    mustbe('e') ; 
    mustbe('d') ;
    mustbe('u') ;
}

static void parse_procedure()
{
    mustbe('p') ;
    mustbe('r') ;
    mustbe('o') ;
    mustbe('c') ;
    mustbe('e') ; 
    mustbe('d') ;
    mustbe('u') ;
}*/

    static void parse_symbol(TokenKind kind)
    {
    	new_token_kind = kind ;
    	do nextch(); while (c_have(tk_at|tk_stop|tk_lcb|tk_rcb|tk_lrb|tk_rrb|tk_lsb|tk_rsb));
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

    	case 'A'...'Z':
    		parse_identifier();
    		break;

        //case '0':

        //case '('e'|'E') ('+'|'-')':
        //parse_number(tk_scientific);


        case '0'...'9':
        	parse_number(tk_integer);
        	break;

        /*case '+'|'-':
        parse_number(cg_start_of_exponent);*/



       
        /*case ' '|'!'|'#'-'~':
        	parse_string();
        	break;
        
        //'@'|'-='|'+='|'*='|'/='|'!='|'=='|'<<<'|'<<'|'>>>'|'>>'|'{'|'}'|'('|')'|'['|']'
        case '@':	
        	parse_symbol(tk_at);
        	break;
        
        case '-':
        case '=':	
        	parse_symbol(tk_sub_assign);
        	break;
        
        case '+':
        case '=':	
        	parse_symbol(tk_add_assign);
        	break;
        
        case '*':
        case '=':	
        	parse_symbol(tk_mult_assign);
        	break;
        
        case '/':
        case '=':	
        	parse_symbol(tk_div_assign);
        	break;
		
		case '!':
		case '=':	
        	parse_symbol(tk_not_eq);
        	break;
        
        case '=':
        nextch();	
        	parse_symbol(tk_eq);
        	break;
        
       
        case '<':	
        	parse_symbol(tk_lshift_l);
        	break;*/
        case '@':	
        	parse_symbol(tk_at);
        	break;

        case '.':
        	parse_symbol(tk_stop);
        	break;

       
        case '<':	
        	parse_symbol(tk_lshift);
        	break;
        
        /*case '>':	
        	parse_symbol(tk_rshift_l);
        	break;*/
        
        case '>':	
        	parse_symbol(tk_rshift);
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

