/**
 * Implemented by Gabriel Quirschfeld, xquirs 00
 * Header for the scanner
 */

#ifndef IFJ_SCANNER_H
#define IFJ_SCANNER_H

#define CHUNK 10

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
/**/  s_id,           // id
/**/  s_add,          // addition +
/**/  s_sub,          // subtraction -
/**/  s_mul,          // multiplication *
/**/  s_div,          // division /
/**/  s_less,         // less than <
/**/  s_great,        // greater than >
/**/  s_less_eq,      // less than or equal <=
/**/  s_great_eq,     // greater than or equal >=
/**/  s_eqto,         // equals to ==
/**/  s_eq,           // equals sign =
/**/  s_noteq,        // not equal !=
/**/  s_comma,        // comma ,
/**/  s_string,       // string starting with "
/**/  s_lbrac,        // left bracket (
/**/  s_rbrac,        // right bracket )
/**/  s_lbrac_c,      // left bracket curly {
/**/  s_rbrac_c,      // right bracket curly }
/**/  s_int,          // integer value
/**/  s_float,        // float value
/**/  s_exp_int,      // exp of integer
/**/  s_exp_f,        // exp of float
/**/  s_exp_int_s,    // sign of exp of integer
/**/  s_exp_f_s,      // sign of exp of float
/**/  s_num_i,        // number following sign in integer
/**/  s_num_f,        // number following sign in float
/**/  s_eol,          // end of line
/**/  s_eof,          // end of file
/**/  s_error,        // error

    /* SPECIAL STATES */
/**/  ss_com,         // comment in line #
/**/  ss_com_bl_s,    // start of comment block =begin
/**/  ss_com_bl,      // continuous comment block
/**/  ss_com_bl_e,    // end of comment block =end
/**/  ss_esc,         // escape sequence
/**/  ss_esc_hex,     // hexadecimal number
/**/  ss_new,         // start a new scan
/**/  ss_final        // end of scanning

} TokenType;

typedef struct token {
    TokenType type;
    char* content;
    int size;
    int line;
} Token;

Token global_token;

int token_generate();

#endif