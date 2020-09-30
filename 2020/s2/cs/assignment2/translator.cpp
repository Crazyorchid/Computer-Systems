// convert Hack VM Code into Hack Assembler

#include "iobuffer.h"
#include "tokeniser.h"
#include "assembler-internal.h"

// to make out programs a bit neater
using namespace std ;

using namespace CS_IO_Buffers ;
using namespace Hack_Virtual_Machine ;

////////////////////////////////////////////////////////////////
/************     MODIFY CODE BETWEEN HERE     **************/
static string current_function;

// translate vm operator command into assembly language

static void translate_vm_operator(TokenKind the_op)
{
    start_of_vm_operator_command(the_op) ;

    // ... your code goes here ...
    switch(the_op)
    {
      case tk_add:
      output_assembler("@SP");
      output_assembler("AM=M-1");
      output_assembler("D=M");
      output_assembler("A=A-1");
      output_assembler("M=D+M");
      break;

      case tk_sub:
      output_assembler("@SP");
      output_assembler("AM=M-1");
      output_assembler("D=M");
      output_assembler("A=A-1");
      output_assembler("M=D-M");
      break;

      case tk_neg:
      output_assembler("@SP");
      output_assembler("A=M");
      output_assembler("A=A-1");
      output_assembler("M=-M");
      break;

      case tk_and:
      output_assembler("@SP");
      output_assembler("AM=M-1");
      output_assembler("D=M");
      output_assembler("A=A-1");
      output_assembler("M=D&M");
      break;

      case tk_or:
      output_assembler("@SP");
      output_assembler("AM=M-1");
      output_assembler("D=M");
      output_assembler("A=A-1");
      output_assembler("M=D|M");
      break;

      case tk_not:
      output_assembler("@SP");
      output_assembler("A=M");
      output_assembler("A=A-1");
      output_assembler("M=!M");
      break;

      /*case tk_gt:
      output_assembler("@SP");
      output_assembler("AM=M-1");
      output_assembler("D=M");
      output_assembler("@SP");
      output_assembler("AM=M-1");
      output_assembler("D=M-D");
      output_assembler("@" + labelTrue);
      output_assembler("D;JGT");
      output_assembler("D=0");
      output_assembler("@"+labelFalse);
      output_assembler("0;JMP");
      output_assembler("("+labelTrue + ")", false);
      output_assembler("D=-1");
      output_assembler("(" + labelFalse + ")", false);
      output_assembler("@SP");
      output_assembler("A=M");
      output_assembler("M=D");
      output_assembler("@SP");
      output_assembler("M=M+1");
      break;

      case tk_eq:
      output_assembler("@SP");
      output_assembler("AM=M-1");
      output_assembler("D=M");
      output_assembler("@SP");
      output_assembler("AM=M-1");
      output_assembler("D=M-D");
      output_assembler("@" + labelTrue);
      output_assembler("D;JGT");
      output_assembler("D=0");
      output_assembler("@"+labelFalse);
      output_assembler("0;JMP");
      output_assembler("("+labelTrue + ")", false);
      output_assembler("D=-1");
      output_assembler("(" + labelFalse + ")", false);
      output_assembler("@SP");
      output_assembler("A=M");
      output_assembler("M=D");
      output_assembler("@SP");
      output_assembler("M=M+1");

      case tk_lt:
      output_assembler("@SP");
      output_assembler("AM=M-1");
      output_assembler("D=M");
      output_assembler("@SP");
      output_assembler("AM=M-1");
      output_assembler("D=M-D");
      output_assembler("@" + labelTrue);
      output_assembler("D;JGT");
      output_assembler("D=0");
      output_assembler("@"+labelFalse);
      output_assembler("0;JMP");
      output_assembler("("+labelTrue + ")", false);
      output_assembler("D=-1");
      output_assembler("(" + labelFalse + ")", false);
      output_assembler("@SP");
      output_assembler("A=M");
      output_assembler("M=D");
      output_assembler("@SP");
      output_assembler("M=M+1");*/


      default: ;
    }

    end_of_vm_command() ;
}

// translate vm jump command into assembly language
static void translate_vm_jump(TokenKind jump, string label)
{
    start_of_vm_jump_command(jump,label) ;

    // ... your code goes here ...

    end_of_vm_command() ;
}

// translate vm call or function command into assembly language
static void translate_vm_function(TokenKind func, string label, int n)
{
    start_of_vm_func_command(func,label,n) ;

    // ... your code goes here ...
    //state the current function that has been used
    
    if ( func == tk_function )
    {

      output_assembler("(" + label + ")");
      current_function = label;
      for (int i = 0; i < n; i++)
      {
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=0");
        output_assembler("@SP");
        output_assembler("M=M+1");
      }
      

    }
    else
    {
      //implement call, 

      //push returnAddress
        output_assembler("@" + current_function + "$returnAddress");
        output_assembler("D=A");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=D");
        output_assembler("@SP");
        output_assembler("M=M+1");
      //push LCL
        output_assembler("@LCL");
        output_assembler("D=M");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=D");
        output_assembler("@SP");
        output_assembler("M=M+1");
        //push ARG
        output_assembler("@ARG");
        output_assembler("D=M");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=D");
        output_assembler("@SP");
        output_assembler("M=M+1");
        //push THIS
        output_assembler("@THIS");
        output_assembler("D=M");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=D");
        output_assembler("@SP");
        output_assembler("M=M+1");
      //push THAT
        output_assembler("@THAT");
        output_assembler("D=M");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=D");
        output_assembler("@SP");
        output_assembler("M=M+1");
        //*ARG = *SP-5-n
        output_assembler("@SP");
        output_assembler("D=M");
        output_assembler("@5");
        output_assembler("D=D-A");
        output_assembler("@" + to_string(n));
        output_assembler("D=D-A");
        output_assembler("@ARG");
        output_assembler("M=D");

        //*LCL = *SP
        output_assembler("@SP");
        output_assembler("D=M");
        output_assembler("@LCL");
        output_assembler("M=D");

        //goto g
        output_assembler("@" + label);
        output_assembler("0;JMP");

        //returnAddress
        output_assembler("(" + current_function + "$returnAddress" + ")");
        

    }

    end_of_vm_command() ;
}

// translate vm push or pop command into assembly language
static void translate_vm_stack(TokenKind stack,TokenKind segment,int offset)
{
    start_of_vm_stack_command(stack,segment,offset) ;

    /*switch(stack){
      case tk_push:
      case tk_pop:
    }*/

    end_of_vm_command() ;
}


/************            AND HERE              **************/
///////////////////////////////////////////////////////////////

// grammar to be parsed:
// vm_class ::= command* eoi
// command ::= vm_operator | vm_jump | vm_func | vm_stack
// vm_operator ::= 'add' | 'and' | 'eq' | 'gt' | 'lt' | 'neg' | 'not' | 'or' | 'sub' | 'return'
// vm_jump ::= jump label
// vm_func ::= func label number
// vm_stack ::= stack segment number

// forward declare parsing functions - one per rule
static void parse_class() ;
static void parse_command() ;
static void parse_op() ;
static void parse_jump() ;
static void parse_func() ;
static void parse_stack() ;

// Note: always read one token after the one recognised

// vm_class ::= command* eoi
static void parse_class()
{
    // tell the output system we are starting to translate VM commands for a Jack class
    start_of_vm_class() ;

    while ( have(tk_vm_command) ) parse_command() ;
    mustbe(tk_eoi) ;

    // tell the output system we have just finished translating VM commands for a Jack class
    end_of_vm_class() ;
}

// command ::= vm_operator | vm_jump | vm_func | vm_stack
static void parse_command()
{
    if ( have(tk_vm_operator) ) parse_op() ; else
    if ( have(tk_vm_jump) )     parse_jump() ; else
    if ( have(tk_vm_func) )     parse_func() ; else
    if ( have(tk_vm_stack) )    parse_stack() ; else
    mustbe(tk_vm_command) ;
}

// vm_operator ::= 'add' | 'and' | 'eq' | 'gt' | 'lt' | 'neg' | 'not' | 'or' | 'sub' | 'return'
static void parse_op()
{
    TokenKind the_op = token_kind(mustbe(tk_vm_operator)) ;

    translate_vm_operator(the_op) ;
}

// vm_jump ::= jump label
// jump command - label, goto label or if-goto label
static void parse_jump()
{
    TokenKind command = token_kind(mustbe(tk_vm_jump)) ;
    string label = token_spelling(mustbe(tk_a_label)) ;

    translate_vm_jump(command,label) ;
}

// vm_func ::= func label number
// call function-name #args or function function-name #locals
static void parse_func()
{
    TokenKind command = token_kind(mustbe(tk_vm_func)) ;
    string label = token_spelling(mustbe(tk_a_label)) ;
    int number = token_ivalue(mustbe(tk_a_number)) ;

    translate_vm_function(command,label,number) ;
}

// vm_stack ::= stack segment number
// push segment offset or pop segment offset
static void parse_stack()
{
    TokenKind stack = token_kind(mustbe(tk_vm_stack)) ;
    TokenKind segment = token_kind(mustbe(tk_vm_segment)) ;
    int number = token_ivalue(mustbe(tk_a_number)) ;

    translate_vm_stack(stack,segment,number) ;
}

// main program
int main(int argc,char **argv)
{
    // initialise the tokeniser by reading the first token
    next_token() ;

    // parse a class and output VM code
    parse_class() ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}

