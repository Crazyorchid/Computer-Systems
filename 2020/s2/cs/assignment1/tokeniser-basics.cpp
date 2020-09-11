// a skeleton implementation of a tokeniser
#include <vector>
#include "tokeniser-extras.h"

// to shorten the code
using namespace std ;

/////////////////////////////////////////////////////////////////////////

namespace Assignment_Tokeniser
{

    // the current input character
    // the eof marker can be confused with a legal character but not one accepted by our tokeniser
    int line = 0;
    int column = 0;
    char for_tab;
    int column_of_new;


    string one_line ;
    vector<string> all_lines;

    char ch ;

    // to create a new token we need the characters read since the last token was created
    // the tokeniser always reads one character past a token so ch is never part of a new token
    // when nextch() is called the old ch value must be part of the token currently being parsed
    // this minimal implementation just remembers the spelling of the current token
    static string spelling ;

    // create a new token using characters remembered since the last token was created
    // in the final submission tests new_token() will require the correct line and column numbers
    // this will require keeping an entire history of the input, not just the latest spelling
    // NOTE: the first character in the input is in column 1 of line 1
    Token new_token(TokenKind kind)
    {
        // create a new token object and reset spelling
        //Token token = new_token(kind,spelling,0,0) ;
        Token token;
        if(kind == tk_newline) {
            if(all_lines.size()>0){
                if(all_lines.back().back() == '$') {
                    one_line.back() += '$';
                    all_lines.push_back(one_line);
                }
                else{
                    all_lines.back() += "$";
                }
            }else
                all_lines.push_back(one_line + "$");

            token = new_token(kind, spelling, line - 1, all_lines.back().size());
            one_line += "";
        }else if(kind == tk_adhoc_comment){
            string copy = spelling;
            int nl_num = 0;

            for(int i = 0; i < copy.size(); i++){
                if(copy[i] == '\n') {
                    string part = copy.substr(0, i);
                    part += "$";
                    if(all_lines.back().back() != '$')
                        all_lines.back()+=part;
                    else
                        all_lines.push_back(part);

                    copy = copy.substr(i);
                    nl_num++;
                    i = 0;

                }
                all_lines.push_back(copy);
            }
            token = new_token(kind,spelling,line-nl_num,column) ;
        }
        else if (kind == tk_eol_comment){
            string tmp = spelling;
            tmp.pop_back();
            tmp += "$";
            all_lines.push_back(tmp);
            token = new_token(kind,spelling,line-1,column) ;
        }else{
            one_line += spelling;
            token = new_token(kind,spelling,0,column - spelling.length()) ;
        }


        // reset the spelling
        spelling = "" ;

        return token ;
    }

    // generate a context string for the given token
    // all lines are shown after the preprocssing of special characters
    // it shows the line before the token, with a '$' marking the end of line,
    // the line containing the token up to the end of the token, and
    // a line of spaces ending with a ^ under the token's first character to mark its position
    // the lines are prefixed by line numbers, right justified in four spaces, followed by ": ", eg "  45: "
    // NOTE: if a token includes newline character(s), only display the token before the first newline followed by a '$'
    // NOTE: this function is not required for the milestone submission
    // NOTE: it is a good idea to build a data structure to remember where each line starts
    string token_context(Token token)
    {
        string syh = "";
        string dqh = "";
        string xyh = "";

        int cur_line = token_line(token);
        int pre_line = cur_line - 1;

        //leading spaces;

        if(cur_line < 10){
            dqh += "   " + std::__cxx11::to_string(cur_line) + ": ";
        }
        else if(cur_line < 100) {
            dqh += "  " + std::__cxx11::to_string(cur_line) + ": ";
        }else if(cur_line < 1000) {
            dqh += " " + std::__cxx11::to_string(cur_line) + ": ";
        }else if(cur_line < 1000) {
            dqh += std::__cxx11::to_string(cur_line) + ": ";
        }



        if(cur_line > 1)
        {
            if(pre_line < 10){
                dqh += "   " + std::__cxx11::to_string(pre_line) + ": ";
            }
            else if(cur_line < 100) {
                dqh += "  " + std::__cxx11::to_string(pre_line) + ": ";
            }else if(cur_line < 1000) {
                dqh += " " + std::__cxx11::to_string(pre_line) + ": ";
            }else if(cur_line < 1000) {
                dqh += std::__cxx11::to_string(pre_line) + ": ";
            }

            int length = token_column(token) + token_original(token).length() - 1;
            syh += all_lines[pre_line-1].substr(0,length) + "\n";
        }


        dqh += all_lines[cur_line-1] + "\n";;

       // xyh += "     " + token_column(token);" ";

        for(int i = 0; i < token_column(token)+ 5; i++){
            xyh += "";
        }



        return syh + dqh + xyh + "^\n";
    }

    // read next character if not at the end of input and update the line and column numbers
    // additional code will be required here to handle preprocessing of '\t' and '\r'
    // in some cases you may wish to remember a character to use next time instead of calling read_char()
    static char repeat = EOF;
    void nextch()
    {
    if ( ch == EOF ) return ;           // stop reading once we have read EOF

    spelling += ch ;

    if ( ch == '\n' ) { line++ ; column = 1 ; } else column++ ;

    if ( repeat != EOF ) { ch = repeat ; repeat = EOF ; } else ch = read_char() ;

    if ( ch == '\t' ) { if ( column % 4 != 0 ) repeat = '\t' ; ch = ' ' ; } else
    if ( ch == '\r' ) { ch = read_char() ; if ( ch != '\n' ) repeat = ch ; ch = '\n' ; }
}

    // initialise the tokeniser
    void initialise_tokeniser()
    {
                                            // add any other initialisation code you need here
                                            // ...
        ch = '\n' ;                         // initialise ch to avoid accidents
        nextch() ;                          // make first call to nextch to initialise ch using the input
        spelling = "" ;                     // discard the initial '\n', it is not part of the input
    }
}
