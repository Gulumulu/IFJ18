/**
 * Implemented by Gabriel Quirschfeld, xquirs00
 * Scanner
 */

#include "scanner.h"
#include "errors.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

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
    t->type = 0;
    t->line = 0;
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
    t->type = 0;
    t->line = 0;
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
    t->size = 0;
    t->type = 0;
    t->line = 0;
    free(t->content);
    t->content = NULL;
}

/**
 * Function reallocates memory to token and deletes all data inside it
 *
 * @param t token to empty
 */
void empty_token(Token* t) {
    t->type = 0;
    t->line = 0;
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
        t->size = length + CHUNK;
    }
    t->content[length] = c;
    t->content[++length] = '\0';
    return 0;
}

/**
 * Main function of the scanner
 * Gets characters from the input stdin and generates specific tokens
 *
 * @return id of the input token
 */
int token_generate(FILE *file)
{
    create_token(&global_token);
    char c;
    char* string;
    int error = 0;
    TokenType state = ss_new;

    if (global_token.type == s_eol) {
        global_token.line++;
    };

    while ((c = getc(file)) != EOF) {
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
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
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
                    fprintf(stderr, "LEX: At Line %d -> Error, token unknown!\n", global_token.line);
                    break;
                }
            }
            break;

            case s_id: { // getting the whole character sequence
                if (isalnum(c)) {
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
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
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
                    state = s_int;
                }
                else if (c == '.') { // float
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
                    state = s_float;
                }
                else if ((c == 'e') || (c == 'E')) { // integer with an exponent
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
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
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
                    state = s_num_i;
                }
                else if (isdigit(c)) { // exponent does not have a sign
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
                    state = s_exp_int;
                }
                else {
                    fprintf(stderr, "LEX: At line %d -> Error, exponent must be followed by a number or +/- sign!", global_token.line);
                    ungetc(c, stdin);
                    state = s_error;
                }
            }
            break;

            case s_num_i: {
                if (isdigit(c)) { // sign is followed by a number
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
                    state = s_exp_int;
                }
                else {
                    fprintf(stderr, "LEX: At line %d -> Error, sign must be followed by a number!", global_token.line);
                    ungetc(c, stdin);
                    state = s_error;
                }
            }
            break;

            case s_exp_int: {
                if (isdigit(c)) { // exponent of integer continues
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
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
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
                    state = s_float;
                }
                else if ((c == 'e') || (c == 'E')) { // float with an exponent
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
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
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
                    state = s_num_f;
                }
                else if (isdigit(c)) { // exponent does not have a sign
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
                    state = s_exp_f;
                }
                else {
                    fprintf(stderr, "LEX: At line %d -> Error, exponent must be followed by a number or +/- sign!", global_token.line);
                    ungetc(c, stdin);
                    state = s_error;
                }
            }
            break;

            case s_num_f: {
                if (isdigit(c)) { // sign is followed by a number
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
                    state = s_exp_f;
                }
                else {
                    fprintf(stderr, "LEX: At line %d -> Error, sign must be followed by a number!", global_token.line);
                    ungetc(c, stdin);
                    state = s_error;
                }
            }
            break;

            case s_exp_f: {
                if (isdigit(c)) { // exponent of float continues
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
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
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
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
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
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
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
                    state = ss_com_bl_s;
                }
                else if (c == 'e') { // comment block end
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
                    state = ss_com_bl_e;
                }
                else if (c == '=') { // equal to
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
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
                if ((c == '\n') && ((strcmp(global_token.content, "begin\0") == 0))) {
                    state = ss_com_bl;
                }
                else if (c == EOF) {
                    fprintf(stderr, "LEX: At line %d -> Error, comment block incomplete!", global_token.line);
                    ungetc(c, stdin);
                    state = s_error;
                }
                else {
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
                    state = ss_com_bl_s;
                }
            }
            break;

            case ss_com_bl: {
                if (c == '=') {
                    state = ss_com_bl_e;
                }
                else if (c == EOF) {
                    fprintf(stderr, "LEX: At line %d -> Error, comment block incomplete!", global_token.line);
                    ungetc(c, stdin);
                    state = s_error;
                }
                else {
                    state = ss_com_bl;
                }
            }
            break;

            case ss_com_bl_e: {
                if ((c == '\n') && ((strcmp(global_token.content, "end\0") == 0))) {
                    state = ss_new;
                }
                else if (c == EOF) {
                    fprintf(stderr, "LEX: At line %d -> Error, comment block incomplete!", global_token.line);
                    ungetc(c, stdin);
                    state = s_error;
                }
                else {
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
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
                    fprintf(stderr, "LEX: At line %d -> Error, undefined symbol sequence!", global_token.line);
                    ungetc(c, stdin);
                    state = s_error;
                }
            }
            break;

            case s_string: {
                int value = (int) c; // getting ascii value of char

                if ((c == EOF) || (c == '\n')) {
                    fprintf(stderr, "LEX: At line %d -> Error, string must end with the \" character!", global_token.line);
                    ungetc(c, stdin);
                    state = s_error;
                }
                else if (c == '"') { // empty string
                    global_token.type = state;
                    ungetc(c, stdin);
                    state = ss_final;
                }
                else if (c == '\\') { // escape sequence in the string
                    state = ss_esc;
                }
                else if (value <= 31) { // characters that cannot be just displayed
                    char temp[5];
                    sprintf(temp, "\\%03d", value);
                    for (int i = 0; temp[i] != '\0'; i++) {
                        error = append_token(&global_token, c);
                        if (error) {
                            destroy_token(&global_token);
                            state = s_error;
                            break;
                        };
                    }
                    state = s_string;
                }
                else {
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
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
                        esc = "\\009";
                    }
                    else if (c == 's') {
                        esc = "\\032";
                    }
                    else if (c == '\\') {
                        esc = "\\092";
                    }
                    for (int i = 0; esc[i] != '\0'; i++) {
                        error = append_token(&global_token, esc[i]);
                        if (error) {
                            destroy_token(&global_token);
                            state = s_error;
                            break;
                        };
                    }
                    state = s_string;
                }
                else if (c == 'x') { // escape sequence is a hex number
                    ungetc(c, stdin);
                    state = ss_esc_hex;
                }
                else {
                    fprintf(stderr, "LEX: At line %d -> Error, escape sequence incorrect!", global_token.line);
                    ungetc(c, stdin);
                    state = s_error;
                }
            }
            break;

            case ss_esc_hex: {
                if ((strlen(global_token.content) < 1) && (isalnum(c))) { // loading the hex values
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };
                    state = ss_esc_hex;
                }
                else if (isalnum(c)) {
                    error = append_token(&global_token, c);
                    if (error) {
                        destroy_token(&global_token);
                        state = s_error;
                        break;
                    };

                    int temp;
                }
                else {
                    fprintf(stderr, "LEX: At line %d -> Error, escape sequence incorrect!", global_token.line);
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
                destroy_token(&global_token);
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