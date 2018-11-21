/**
 * Header for the scanner
 *
 * Implemented by Gabriel Quirschfeld xquirs00
 */

#ifndef IFJ_SCANNER_H
#define IFJ_SCANNER_H

#define CHUNK 10

#include <stdio.h>

typedef enum {
    /* KEYWORDS */
/*0*/   kw_def,
/*1*/   kw_while,
/*2*/   kw_do,
/*3*/   kw_if,
/*4*/   kw_else,
/*5*/   kw_then,
/*6*/   kw_not,
/*7*/   kw_nil,
/*8*/   kw_end,
/*9*/   kw_inputs,
/*10*/  kw_inputi,
/*11*/  kw_inputf,
/*12*/  kw_print,
/*13*/  kw_length,
/*14*/  kw_substr,
/*15*/  kw_ord,
/*16*/  kw_chr,

    /* DATA TYPES */
/*17*/  dt_integer,
/*18*/  dt_float,
/*19*/  dt_string,

    /* SPECIFIC STATES */
/*20*/  s_id,           // id
/*21*/  s_add,          // addition +
/*22*/  s_sub,          // subtraction -
/*23*/  s_mul,          // multiplication *
/*24*/  s_div,          // division /
/*25*/  s_less,         // less than <
/*26*/  s_great,        // greater than >
/*27*/  s_less_eq,      // less than or equal <=
/*28*/  s_great_eq,     // greater than or equal >=
/*29*/  s_eqto,         // equals to ==
/*30*/  s_eq,           // equals sign =
/*31*/  s_noteq,        // not equal !=
/*32*/  s_comma,        // comma ,
/*33*/  s_string,       // string starting with "
/*34*/  s_lbrac,        // left bracket (
/*35*/  s_rbrac,        // right bracket )
/*36*/  s_lbrac_c,      // left bracket curly {
/*37*/  s_rbrac_c,      // right bracket curly }
/*38*/  s_int,          // integer name
/*39*/  s_float,        // float name
/*40*/  s_exp_int,      // exp of integer
/*41*/  s_exp_f,        // exp of float
/*42*/  s_exp_int_s,    // sign of exp of integer
/*43*/  s_exp_f_s,      // sign of exp of float
/*44*/  s_num_i,        // number following sign in integer
/*45*/  s_num_f,        // number following sign in float

    /* SPECIAL STATES */
/*46*/  ss_eol,         // end of line
/*47*/  ss_eof,         // end of file
/*48*/  ss_com,         // comment in line #
/*49*/  ss_com_bl_s,    // start of comment block =begin
/*50*/  ss_com_bl,      // continuous comment block
/*51*/  ss_com_bl_e,    // end of comment block =end
/*52*/  ss_esc,         // escape sequence
/*53*/  ss_esc_hex,     // hexadecimal number
/*54*/  ss_after_hex,   // space after hex
/*55*/  ss_error,       // error
/*56*/  ss_new,         // start a new scan
/*57*/  ss_final,       // end of scanning

    /*  VERY SPECIFIC SPECIAL STATE */
/*58*/  s_func_id,      // function id
/*59*/  s_func_expr     // function in expression

} TokenType;

typedef struct token {
    TokenType type;
    char* content;
    int size;
    int line;
} Token;

Token global_token;

int token_generate(FILE *file);

#endif