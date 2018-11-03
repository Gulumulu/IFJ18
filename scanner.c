/**
 * Implemented by Gabriel Quirschfeld, xquirs00
 * Scanner
 */

#include "scanner.h"
#include "errors.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>

/**
 * Defining the keywords inside an array
 */
char* keywords[] = {
        "def\0", "while\0", "do\0", "if\0", "else\0", "then\0", "not\0", "nil\0", "end\0", "inputs\0", "inputi\0", "inputf\0", "print\0", "length\0", "substr\0", "ord\0", "chr\0"
};

/**
 * Function for determining whether a sequence of characters is an identifier or a keyword
 *
 * @return id of specific keyword or general identifier
 */
TokenType identify_keyword() {
    for (int i = 0; i < sizeof(keywords); i++) {
        if (strcmp(global_token.content, keywords[i]) == 0) {
            return i;
        }
        return s_id;
    }
}

/**
 * Function allocates memory for and initializes new token
 *
 * @param t token to initialize
 */
void create_token(Token* t) {
    t->type = NULL;
    t->line = NULL;
    t->content = (char*)malloc(sizeof(char) * CHUNK);
    if (t->content == NULL) {
        fprintf(stderr, "ERROR, not enough space in memory to allocate!");
        errorHandling(99);
    }
    t->size = CHUNK;
    t->content[0] = '\0';
}

/**
 * Function allocates needed amount of memory and initializes token
 *
 * @param t token to initialize
 * @param space needed memory
 */
void create_spec_token(Token* t, int space) {
    t->type = NULL;
    t->line = NULL;
    t->content = (char*)malloc(sizeof(char) * space);
    if (t->content == NULL) {
        fprintf(stderr, "ERROR, not enough space in memory to allocate!");
        errorHandling(99);
    }
    t->size = space;
    t->content[0] = '\0';
}

/**
 * Function deletes data from token and frees memory
 *
 * @param t token to destroy
 */
void destroy_token(Token* t) {
    t->size = NULL;
    t->type = NULL;
    t->line = NULL;
    free(t->content);
    t->content = NULL;
}

/**
 * Function reallocates memory to token and deletes all data inside it
 *
 * @param t token to empty
 */
void empty_token(Token* t) {
    t->type = NULL;
    t->line = NULL;
    t->content = (char*)realloc(t->content, sizeof(char) * CHUNK);
    if (t->content == NULL) {
        free(t->content);
        fprintf(stderr, "ERROR, not enough space in memory to allocate!");
        errorHandling(99);
    }
    t->size = CHUNK;
    t->content[0] = '\0';
}

/**
 * Function reallocates memory of token and pushes a character to the end of the token string
 *
 * @param t token to append the character to
 * @param c character that is being appended
 * @return 0 if there is no ERROR, otherwise 99
 */
int append_token(Token* t, char c) {
    int length = strlen(t->content);
    if ((length + 2) >= t->size) {
        t->content = (char*)realloc(t->content, sizeof(char) * (length + CHUNK));
        if (t->content == NULL) {
            free(t->content);
            fprintf(stderr, "ERROR, not enough space in memory to allocate!");
            errorHandling(99);
            return 99;
        }
        t->size = strlen(length + CHUNK);
    }
    t->content[length] = c;
    t->content[++length] = '\0';
    return 0;
}

/**
 * Main function of the scanner
 * Gets characters from the input file and generates specific tokens
 *
 * @return id of the input token
 */
int token_generate()
{
    create_token(global_token);
    char c;
    char* string;
    int error;
    TokenType state = ss_new;

    if (global_token.type = s_eol) {
        global_token.line++;
    }

    while (c = getc(stdin)) {
        switch (state) {
            case ss_new: {
                if (c == '\n') {
                    state = s_eol;
                    ungetc(c, stdin);
                    break;
                }
                else if (isspace(c)) {
                    state = ss_new;
                    break;
                }
                else if ((islower(c) || c == '_')) {
                    state = s_id;
                }
                else if (isdigit(c)) {
                    state = s_int;
                }
                else if (c == '#') {                    // comment line
                    state = ss_com;
                }
                else if (c == '+') {                    // addition
                    state = s_add;
                }
                else if (c == '-') {                    // subtraction
                    state = s_sub;
                }
                else if (c == '*') {                    // multiplication
                    state = s_mul;
                }
                else if (c == '/') {                    // division
                    state = s_div;
                }
                else if (c == '<') {                    // less than OR less than or equal to
                    state = s_less;
                }
                else if (c == '>') {                    // greater than OR greater than or equal to
                    state = s_great;
                }
                else if (c == '=') {                    // equal to OR block comment start OR block comment end OR equal
                    state = s_eq;
                }
                else if (c == '!') {                    // not equal to
                    state = s_noteq;
                }
                else if (c == '(') {                    // left bracket
                    state = s_lbrac;
                }
                else if (c == ')') {                    // right bracket
                    state = s_rbrac;
                }
                else if (c == ',') {                    // comma
                    state = s_comma;
                }
                else if (c == '"') {                    // string starting with "
                    state = s_string;
                }
                else if (c == '{') {                    // left curly bracket
                    state = s_lbrac_c;
                }
                else if (c == '}') {                    // right curly bracket
                    state = s_rbrac_c;
                }
                else if (c == EOF) {
                    state = s_eof;
                }
                else {
                    ungetc(c, stdin);
                    state = s_error;
                    fprintf(stderr, "LEX: At Line **NUMBER TO DO** -> Error, token unknown!\n");
                    break;
                }
            }
            break;

            case s_id: { // getting the whole character sequence
                if (isalnum(c)) {
                    append_token(global_token.content, c);
                    state = s_id;
                }
                else if ((c == '?') || (c == '!')) { // last character is ? or !
                    global_token.type = s_id;
                    ungetc(c, stdin);
                    state = ss_final;
                }
                else { // identifier OR keyword
                    global_token.type = identify_keyword();
                    ungetc(c, stdin);
                    state = ss_final;
                }
            }
            break;

            case s_int: {
                if (isdigit(c)) { // integer continues
                    append_token(global_token.content, c);
                    state = s_int;
                }
                else if (c == '.') { // float
                    append_token(global_token.content, c);
                    state = s_float;
                }
                else if ((c == 'e') || (c == 'E')) { // integer with an exponent
                    append_token(global_token.content, c);
                    state = s_exp_int_s;
                }
                else { // integer loaded
                    global_token.type = state;
                    ungetc(c, stdin);
                    state = ss_final;
                }
            }
            break;

            case s_exp_int_s: {
                if ((c == '+') || (c == '-')) { // exponent has a sign
                    append_token(global_token.content, c);
                    state = s_num_i;
                }
                else if (isdigit(c)) { // exponent does not have a sign
                    append_token(global_token.content, c);
                    state = s_exp_int;
                }
                else {
                    fprintf(stderr, "LEX: At line **NUMBER TO DO** -> Error, exponent must be followed by a number or +/- sign!");
                    ungetc(c, stdin);
                    state = s_error;
                }
            }
            break;

            case s_num_i: {
                if (isdigit(c)) { // sign is followed by a number
                    append_token(global_token.content, c);
                    state = s_exp_int;
                }
                else {
                    fprintf(stderr, "LEX: At line **NUMBER TO DO** -> Error, sign must be followed by a number!");
                    ungetc(c, stdin);
                    state = s_error;
                }
            }
            break;

            case s_exp_int: {
                if (isdigit(c)) { // exponent of integer continues
                    append_token(global_token.content, c);
                    state = s_exp_int;
                }
                else { // exponent loaded
                    global_token.type = state;
                    ungetc(c, stdin);
                    state = ss_final;
                }
            }
            break;

            case s_float: {
                if (isdigit(c)) { // float continues
                    append_token(global_token.content, c);
                    state = s_float;
                }
                else if ((c == 'e') || (c == 'E')) { // float with an exponent
                    append_token(global_token.content, c);
                    state = s_exp_f_s;
                }
                else { // float loaded
                    global_token.type = state;
                    ungetc(c, stdin);
                    state = ss_final;
                }
            }
            break;

            case s_exp_f_s: {
                if ((c == '+') || (c == '-')) { // exponent has a sign
                    append_token(global_token.content, c);
                    state = s_num_f;
                }
                else if (isdigit(c)) { // exponent does not have a sign
                    append_token(global_token.content, c);
                    state = s_exp_f;
                }
                else {
                    fprintf(stderr, "LEX: At line **NUMBER TO DO** -> Error, exponent must be followed by a number or +/- sign!");
                    ungetc(c, stdin);
                    state = s_error;
                }
            }
            break;

            case s_num_f: {
                if (isdigit(c)) { // sign is followed by a number
                    append_token(global_token.content, c);
                    state = s_exp_f;
                }
                else {
                    fprintf(stderr, "LEX: At line **NUMBER TO DO** -> Error, sign must be followed by a number!");
                    ungetc(c, stdin);
                    state = s_error;
                }
            }
            break;

            case s_exp_f: {
                if (isdigit(c)) { // exponent of float continues
                    append_token(global_token.content, c);
                    state = s_exp_f;
                }
                else { // exponent loaded
                    global_token.type = state;
                    ungetc(c, stdin);
                    state = ss_final;
                }
            }
            break;

            case s_less: {
                if (c == '=') { // less than or equal to
                    append_token(global_token.content, c);
                    state = s_less_eq;
                }
                else { // less than
                    global_token.type = state;
                    ungetc(c, stdin);
                    state = ss_final;
                }
            }
            break;

            case s_great: {
                if (c == '=') { // greater than or equal to
                    append_token(global_token.content, c);
                    state = s_great_eq;
                }
                else { // greater than
                    global_token.type = state;
                    ungetc(c, stdin);
                    state = ss_final;
                }
            }
            break;

            case s_eq: {
                if (c == 'b') { // comment block start
                    append_token(global_token.content, c);
                    state = ss_com_bl_s;
                }
                else if (c == 'e') { // comment block end
                    append_token(global_token.content, c);
                    state = ss_com_bl_e;
                }
                else if (c == '=') { // equal to
                    append_token(global_token.content, c);
                    state = s_eqto;
                }
                else { // equals sign
                    global_token.type = state;
                    ungetc(c, stdin);
                    state = ss_final;
                }
            }
            break;

            case ss_com_bl_s: {
                if ((c == EOL) && ((strcmp(global_token.content, "begin\0") == 0))) {
                    state = ss_com_bl;
                }
                else if (c == "EOF") {
                    fprintf(stderr, "LEX: At line **NUMBER TO DO"" -> Error, comment block incomplete!");
                    ungetc(c, stdin);
                    state = s_error;
                }
                else {
                    append_token(global_token.content, c);
                    state = ss_com_bl_s;
                }
            }
            break;

            case ss_com_bl: {
                if (c == '=') {
                    state = ss_com_bl_e;
                }
                else if (c == EOF) {
                    fprintf(stderr, "LEX: At line **NUMBER TO DO"" -> Error, comment block incomplete!");
                    ungetc(c, stdin);
                    state = s_error;
                }
                else {
                    state = ss_com_bl;
                }
            }
            break;

            case ss_com_bl_e: {
                if ((c == EOL) && ((strcmp(global_token.content, "end\0") == 0))) {
                    state = ss_new;
                }
                else if (c == EOF) {
                    fprintf(stderr, "LEX: At line **NUMBER TO DO** -> Error, comment block incomplete!");
                    ungetc(c, stdin);
                    state = s_error;
                }
                else {
                    append_token(global_token.content, c);
                    state = ss_com_bl_e;
                }
            }
            break;

            case s_noteq: {
                if (c == '=') { // not equal to
                    global_token.type = identify_keyword();
                    ungetc(c, stdin);
                    state = ss_final;
                }
                else { // misplaced exclamation point
                    fprintf(stderr, "LEX: At line **NUMBER TO DO** -> Error, undefined symbol sequence!");
                    ungetc(c, stdin);
                    state = s_error;
                }
            }
            break;

            case s_string: {
                int value = (int) c; // getting ascii value of char

                if ((c == EOF) || (c == '\n')) {
                    fprintf(stderr, "LEX: At line **NUMBER TO DO** -> Error, string must end with the \" character!");
                    ungetc(c, stdin);
                    state = s_error;
                }
                else if (c == '"') { // empty string
                    global_token.type = state;
                    ungetc(c, stdin);
                    state = ss_final;
                }
                else if (c == "\\") { // escape sequence in the string
                    state = ss_esc;
                }
                else if (value <= 31) { // characters that cannot be just displayed

                }
                else {
                    append_token(global_token.content, c);
                    state = s_string;
                }
            }
            break;

            case ss_esc: {
                if ((c == '\"') || (c == 'n') || (c == 't') || (c == 's') || (c == '\\')) { // escape sequence other than a hex number
                    char* esc;

                    if (c == '\"') {
                        esc = "\\034";
                    }
                    else if (c == 'n') {
                        esc = "\\010";
                    }
                    else if (c == 't') {
                        esc = "\\009"
                    }
                    else if (c == 's') {
                        esc = "\\032"
                    }
                    else if (c == '\\') {
                        esc = "\\092";
                    }
                    for (int i = 0; esc[i] != '\0'; i++) {
                        append_token(global_token.content, esc[i]);
                    }
                    state = s_string;
                }
                else if (c == 'x') { // escape sequence is a hex number
                    ungetc(c, stdin);
                    state = ss_esc_hex;
                }
                else {
                    fprintf(stderr, "LEX: At line **NUMBER TO DO** -> Error, escape sequence incorrect!");
                    ungetc(c, stdin);
                    state = s_error;
                }
            }
            break;

            case ss_esc_hex: {
                if ((strlen(global_token.content) < 2) && (isalnum(c)))) { // loading the hex values
                    append_token(global_token.content, c);
                    state = ss_esc_hex;
                }
                else if {

                }
                else {
                    fprintf(stderr, "LEX: At line **NUMBER TO DO** -> Error, escape sequence incorrect!");
                    ungetc(c, stdin);
                    state = s_error;
                }
            }

            case s_add:
            case s_sub:
            case s_mul:
            case s_div:
            case s_lbrac:
            case s_rbrac:
            case s_comma:
            case s_lbrac_c:
            case s_rbrac_c:
            case s_eof: {
                global_token.type = state;
                ungetc(c, stdin);
                state = ss_final;
            }
            break;

            case s_eol: {
                global_token.type = state;
                state = ss_final;
            }
            break;

            case s_error: {
                error = 1;
                global_token.type = state;
            }
            break;

            case ss_final: {
                ungetc(c, stdin);
            }
            break;

            default: {
                errorHandling(1);
            }
            break;
        }
    }
    printf("token: %d ... %s \n", global_token.type, global_token.content);
    return error;
}
