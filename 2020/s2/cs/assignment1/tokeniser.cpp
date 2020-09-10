// a skeleton implementation of a tokeniser

#include "tokeniser-extras.h"
#include<string>
#include<bits/stdc++.h>
// to shorten the code
using namespace std ;

/////////////////////////////////////////////////////////////////////////

namespace Assignment_Tokeniser {

    static TokenKind new_token_kind;

    // return the next Token object by reading more of the input
    // you must read input using the nextch() function
    // the last character read is in the static variable ch
    // always read one character past the end of the token being returned
    static void parse_extra(TokenKind kind) {
        //parse one extra ch so that ch can be the start of the next token
        new_token_kind = kind;
        nextch();
    }

    static void parse_wspace(TokenKind kind) {
        //parse the white space
        parse_extra(kind);
    }

    static void parse_identifier(TokenKind kind) {
        new_token_kind = kind;
        do nextch(); while (c_have(cg_extends_identifier));
    }

    static void parse_scientific() {


        c_mustbe(cg_start_of_exponent);
        c_have_next(cg_sign);

        if (!c_have_next('0')) {
            c_mustbe(cg_digit19);
            c_have_next(cg_digit);
            c_have_next(cg_digit);
        }
    }

    static void parse_fraction() {
        c_mustbe('.');
        while (c_have(cg_digit)) {
            c_mustbe(cg_digit);
        }
    }

    static void parse_in_string(){
        new_token_kind = tk_string;
        do {
            nextch();
        }while (c_have(cg_instring));
    }

    /*static void parse_string(TokenKind kind) {
        new_token_kind = tk_string;
        do {
            nextch();
        } while (c_have('"'));
        nextch();
    }*/

    static void parse_integer(TokenKind kind) {
        new_token_kind = tk_integer;
        if (c_have('0')) {
            nextch();
        } else {
            do nextch();
            while (c_have(cg_digit) && ch != '.');
            if (c_have('.')) {
                new_token_kind = tk_scientific;
                do nextch();
                while (c_have(cg_digit));
                if (c_have('e') || c_have('E')) {
                    nextch();
                    if (c_have('+') || c_have('-')) {
                        do nextch();
                        while ((c_have(cg_digit)));
                    }
                }
            }
             else {
                return;
            }
        }

    }

    static void parse_zero() {

        new_token_kind = tk_integer;
        nextch();

        if (ch == '.') {
            parse_fraction();
            parse_scientific();
        } else if (c_have(cg_start_of_exponent)) {
            parse_scientific();
        }
    }


    static void parse_digit() {
        new_token_kind = tk_integer;
        do nextch(); while (c_have(cg_digit19));
    }

    static void parse_exponent() {
        new_token_kind = tk_scientific;
        do nextch(); while (c_have(cg_start_of_exponent));
    }


    static void parse_shift(TokenKind kind) {
        new_token_kind = kind;
        nextch();
        if (c_have_next('<')) {
            if (c_have_next('<')) {
                new_token_kind = tk_lshift_l;
            } else {
                return;
            }
        } else if (c_have_next('>')) {
            if (c_have_next('>')) {
                new_token_kind = tk_rshift_l;
            } else {
                return;
            }
        }
    }

    static void parse_symbol(TokenKind kind) {
        new_token_kind = kind;
        switch (ch) {
            case '-':
                nextch();
                if (!c_have('=')) {
                    c_mustbe('=');
                }
                break;
            case '+':
                nextch();
                if (!c_have('=')) {
                    c_mustbe('=');
                }
                break;
            case '*':
                nextch();
                if (!c_have('=')) {
                    c_mustbe('=');
                }
                break;
            case '/':
                nextch();
                if (ch == '=') {
                    break;
                } else if (ch == '/') {
                    new_token_kind = tk_eol_comment;
                    do {
                        nextch();
                    } while (!c_have('\n'));
                    break;
                } else if (ch == '*') {
                    new_token_kind = tk_adhoc_comment;
                    while (true) {
                        nextch();
                        if (ch == '*') {
                            nextch();
                            if (c_have('/')) {
                                break;
                            }
                        }
                    }
                }
                break;

            case '!':
                nextch();
                if (!c_have('=')) {
                    c_mustbe('=');
                    break;
                }
            case '=':
                nextch();
                if (!c_have('=')) {
                    c_mustbe('=');
                    break;
                }

        }
        nextch();

                    //do nextch(); while (c_have(tk_at|tk_stop|tk_lcb|tk_rcb|tk_lrb|tk_rrb|tk_lsb|tk_rsb));
    }


                static void parse_eol_comment() {


                }

                static void pare_adhoc_comment() {

                }


                Token next_token() {
                    // ch is always the next char to read

                    // this should follow the style used in the workshops ...
                    // but remember that the token grammar is different in this assignment
                    //
                    // add additional case labels here for characters that can start tokens
                    // call a parse_*() function to parse the token
                    //


                    // End of Inptut


                    switch (ch) {


                        case ' ':               // white space tokens
                            parse_wspace(tk_space);
                            break;

                        case '\n':
                            parse_wspace(tk_newline);
                            break;

                        case 'a'...'z':

                            parse_identifier(tk_identifier);
                            break;

                        case 'A'...'Z':
                            parse_identifier(tk_identifier);
                            break;

                        case '0':
                            parse_zero();
                            break;

                        case '1'...'9':
                            parse_integer(tg_number);
                            break;


                        case '#' - '~':
                            parse_in_string();
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
                            parse_symbol(tk_not_eq);
                            break;

                        case '-':
                            parse_symbol(tk_sub_assign);
                            break;

                        case '+':
                            parse_symbol(tk_add_assign);
                            break;

                        case '*':
                            parse_symbol(tk_mult_assign);;
                            break;

                        case '/':
                            parse_symbol(tk_div_assign);
                            break;

                        case '=':
                            parse_symbol(tk_eq);
                            break;

                        case '<':
                            parse_shift(tk_lshift);
                            break;


                        case '>':
                            parse_shift(tk_rshift);
                            break;

                        case '$':
                            parse_identifier(tk_identifier);
                            break;


                        case EOF:
                            new_token_kind = tk_eoi;
                            break;

                        default:
                            c_did_not_find(cg_start_of_token);
                            break;
                    }

                    Token token = new_token(new_token_kind);

                    /*if(token_kind(token) == tk_string)
                        set_token_spelling(token, token_spelling(token).substr(2, token_spelling(token).length()-1));*/

                    if (token_spelling(token) == "this") {
                        set_token_kind(token, tk_this);
                    } else if (token_spelling(token) == "done") {
                        set_token_kind(token, tk_done);
                    } else if (token_spelling(token) == "while") {
                        set_token_kind(token, tk_while);
                    } else if (token_spelling(token) == "procedure") {
                        set_token_kind(token, tk_procedure);
                    } else if (token_spelling(token) == "if-goto") {
                        set_token_kind(token, tk_if_goto);
                    }

                    if (token_kind(token) == tk_eol_comment) {
                        int index = 0;
                        string str = token_spelling(token);
                        str.erase(std::remove(str.begin(), str.end(), '\n'),str.end());
                        str.erase(std::remove(str.begin(), str.end(), '\r'),str.end());
                        set_token_spelling(token, str.substr(2, str.length()));

                    }else if(token_kind(token) == tk_adhoc_comment){
                        string str =token_spelling(token);


                        set_token_spelling(token,str.substr(2,str.length()-4));

                    }
                    else if (token_kind(token) == tk_string){
                        string s = token_spelling(token);

                        //set_token_spelling(token,s.substr((2,s.length()-1)));
                    }

                    // before returning a token check if the kind or spelling needs updating
                    // ...

                    return token;
                }
        }

