#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define NUM data.num

#define IDEN_ID data.identifier_id

#define ARGS_SZ data.func.args_sz
#define ARGS data.func.args
#define NAME data.func.name_start
#define NAME_SZ data.func.name_sz

#define LHS data.op.lhs
#define RHS data.op.rhs
#define PREC data.op.prec
#define OP_CODE data.op.op_code

#define MAX_LINE 1024
#define MAX_TOKENS 100
#define MAX_ARGS 10

#define OPERATION(lhs, op, rhs) create_op_node(op, lhs, rhs)
#define CONST(num) create_num_node(num)
#define IDENTIFIER(iden_id) create_identifier_node(iden_id);
#define NULO_NODE (tree_node_t *)NULL

// Definitions //
enum Node_type {
    NUM_NODE = 0,
    OP_NODE,
    FUNCTION_NODE,
    IDENTIFIER_NODE,

    PAR_NODE,
    C_PAR_NODE,
    FIM_NODE
};

enum Token_type {
    TOKEN_MINUS = 0,
    TOKEN_PLUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_CIRCUNFLEX,
    TOKEN_EQUAL_EQUAL,
    TOKEN_BANG_EQUAL,
    TOKEN_EQUAL,
    TOKEN_OP_CODE_SEP,

    TOKEN_O_PAR,
    TOKEN_C_PAR,
    TOKEN_NUM,
    TOKEN_INDENTIFIER,
    TOKEN_FIM
};

enum Op_type {
    OP_PLUS = 0,
    OP_MINUS,
    OP_MULT,
    OP_DIV,
    OP_POW,
    OP_EQUALITY,
    OP_INEQUALITY,
    OP_SET
};

enum Op_type tok_op_table[] = {
    [TOKEN_MINUS] = OP_MINUS,
    [TOKEN_PLUS] = OP_PLUS,
    [TOKEN_STAR] = OP_MULT,
    [TOKEN_SLASH] = OP_DIV,
    [TOKEN_CIRCUNFLEX] = OP_POW,
    [TOKEN_EQUAL_EQUAL] = OP_EQUALITY,
    [TOKEN_BANG_EQUAL] = OP_INEQUALITY,
    [TOKEN_EQUAL] = OP_SET
};

char *ops_table[] = {
    [OP_PLUS] = "+",
    [OP_MINUS] = "-",
    [OP_MULT] = "*",
    [OP_DIV] = "/",
    [OP_POW] = "^",
    [OP_EQUALITY] = "==",
    [OP_INEQUALITY] = "!=",
    [OP_SET] = "=",
};

typedef struct tree_node_t tree_node_t;
typedef struct op_node_t op_node_t;
typedef struct token token_t;

struct op_node_t {
    enum Op_type op_code;
    int prec;
    tree_node_t *lhs;
    tree_node_t *rhs;
};

struct func_node_t {
    size_t args_sz;
    tree_node_t *args[MAX_ARGS];
    char *name_start;
    size_t name_sz;
};

struct tree_node_t {
    int type;
    union {
        int num;
        int identifier_id;
        struct func_node_t func;
        struct op_node_t op;
    }data;
};

struct token {
    enum Token_type type;
    char *start;
    size_t sz;
};
// Definitions //

int memory = 0;

// Node functions //
int prec_from_op(enum Op_type op){
    int prec = 0;
    if (op == OP_SET || (prec++ && 0));
    else if (op == OP_EQUALITY || op == OP_INEQUALITY || (prec++ && 0));
    else if (op == OP_MINUS || op == OP_PLUS || (prec++ && 0));
    else if (op == OP_MULT || op == OP_DIV || (prec++ && 0));
    else if (op == OP_POW || (prec++ && 0));
    else{
        fprintf(stderr, "[ERRO]: Operador com tipo '%s' tem precedencia desconhecida\n", ops_table[op]);
        exit(-1);
    }

    return prec;
}

void print_ident(int depth){
    for (int i = 0; i < depth; i++)
        printf(" ");
}

void print_node(tree_node_t *n, int depth){
    if (n == NULL){
        printf("[NULL]\n");
        return;
    }

    if (n->type == NUM_NODE){
        printf("%d", n->NUM);
        return;
    }else if (n->type == IDENTIFIER_NODE){
        printf("$%d", n->IDEN_ID);
        return;
    }else if (n->type == FUNCTION_NODE){
        printf("{\n");
        print_ident(depth);
        printf("name: %.*s\n", (int)n->NAME_SZ, n->NAME);

        print_ident(depth);
        printf("args_sz: %ld\n", n->ARGS_SZ);

        print_ident(depth);
        printf("args: [");
        for (size_t i = 0; i < n->ARGS_SZ - 1; i++){
            print_node(n->ARGS[i], depth + 2);
            printf(", ");
        }
        print_node(n->ARGS[n->ARGS_SZ - 1], depth + 2);

        printf("]\n");
        print_ident(depth);
        printf("}");
    }else if (n->type == OP_NODE){

        printf("{\n");
        print_ident(depth);
        printf("op: %s\n", ops_table[n->OP_CODE]);

        print_ident(depth);
        printf("prec: %d\n", n->PREC);

        print_ident(depth);
        printf("lhs: ");
        print_node(n->LHS, depth + 2);
        printf("\n");

        print_ident(depth);
        printf("rhs: ");
        print_node(n->RHS, depth + 2);
        printf("\n");

        printf("\n");
        print_ident(depth);
        printf("}");
    }else{
        fprintf(stderr, "[ERRO]: Node type '%d' desconhecido\n", n->type);
        exit(-1);
    }
}

int run_program_(tree_node_t *n){
    int lhs;
    int rhs;

    if (n->type == NUM_NODE)
        return n->NUM;
    else if (n->type == IDENTIFIER_NODE){
        if (n->IDEN_ID == 0){
            return memory;
        }else{
            fprintf(stderr, "[ERRO]: Indentificadores diferentes de 'm' nao suportados");
            exit(-1);
        }
    }else if (n->type == OP_NODE){
        switch (n->OP_CODE){
            case OP_MINUS: return run_program_(n->LHS) - run_program_(n->RHS);
            case OP_PLUS: return run_program_(n->LHS) + run_program_(n->RHS);
            case OP_MULT: return run_program_(n->LHS) * run_program_(n->RHS);
            case OP_DIV: {
                lhs = run_program_(n->LHS);
                rhs = run_program_(n->RHS);

                if (rhs == 0 ){
                    fprintf(stderr, "[ERRO]: Divisao por 0\n");
                    exit(-1);
                }

                return lhs / rhs;
            };
            case OP_POW: return (int) pow(run_program_(n->LHS), run_program_(n->RHS));
            case OP_EQUALITY: return run_program_(n->LHS) == run_program_(n->RHS);
            case OP_INEQUALITY: return run_program_(n->LHS) != run_program_(n->RHS);
            case OP_SET: {
                if (n->LHS->type != IDENTIFIER_NODE){
                    fprintf(stderr, "[ERRO]: Lado esquerdo deve ser uma variavel, nao uma expressao\n");
                    exit(-1);
                }

                if (n->LHS->IDEN_ID != 0){
                    fprintf(stderr, "[ERRO]: Identificador '$%d' desconecido\n", n->LHS->IDEN_ID);
                    exit(-1);
                }

                return memory =  run_program_(n->RHS);
            };
            default:{
                fprintf(stderr, "[ERRO]: Operador '%s' desconhecido\n", ops_table[n->OP_CODE]);
                exit(-1);
            }
        }
    }else{
        fprintf(stderr, "[ERRO]: Tipo '%d' desconhecido\n", n->type);
        exit(-1);
        
    }
}

int run_program(tree_node_t *n){
    return run_program_(n);
}

tree_node_t *create_num_node(int num){
    tree_node_t *n = malloc(sizeof(tree_node_t));

    n->type = NUM_NODE;
    n->NUM = num;

    return n;
}

tree_node_t *create_identifier_node(int iden_id){
    tree_node_t *n = malloc(sizeof(tree_node_t));

    n->type = IDENTIFIER_NODE;
    n->IDEN_ID = iden_id;

    return n;
}


tree_node_t *create_op_node(enum Op_type op, tree_node_t *lhs, tree_node_t *rhs){
    tree_node_t *n = malloc(sizeof(tree_node_t));
    n->type = OP_NODE;
    n->PREC = prec_from_op(op);;
    n->OP_CODE = op;
    n->LHS = lhs;
    n->RHS = rhs;

    return n;
}

tree_node_t *destroy_node(tree_node_t *n){
    if (n == NULL)
        return NULL;
    if (n->type == NUM_NODE ||
        n->type == IDENTIFIER_NODE ||
        n->type == C_PAR_NODE ||
        n->type == FIM_NODE ||
        n->type == PAR_NODE)
        free(n);
    else if (n->type == OP_NODE){
        destroy_node(n->LHS);
        destroy_node(n->RHS);
        free(n);
    }else{
        fprintf(stderr, "[ERRO]: Tipo de node '%d' nao suportdo para ser liberado\n", n->type);
        exit(-1);
    }

    return NULL;
}
// Node functions //

// Parsing //
int precede(int op1, int op2){
    if (op1 - op2 > 0)
        return 1;
    return 0;
}

int c_is_num(char c){
    return ('0' <= c && c <= '9');
}

int get_num(char *str, size_t str_sz, int *num){
    *num = 0;
    size_t i = 0;
    while (i < str_sz && c_is_num(str[i])){
        *num = *num * 10 + (str[i] - '0');
        i++;
    }

    return i == str_sz;
}

tree_node_t *peek(token_t *tokens, size_t tokens_sz, size_t i){
    tree_node_t *r = NULO_NODE;
    int num;
    token_t tok;

    if (i < tokens_sz){
        tok = tokens[i];
        if (tok.type == TOKEN_FIM){
            r = CONST(0);
            r->type = FIM_NODE;
        }else if (tok.type == TOKEN_NUM){
            get_num(tokens[i].start, tokens[i].sz, &num);
            r = CONST(num);
        }else if (tok.type == TOKEN_MINUS    ||
                tok.type == TOKEN_PLUS       ||
                tok.type == TOKEN_STAR       ||
                tok.type == TOKEN_SLASH      ||
                tok.type == TOKEN_CIRCUNFLEX ||
                tok.type == TOKEN_BANG_EQUAL ||
                tok.type == TOKEN_EQUAL_EQUAL||
                tok.type == TOKEN_EQUAL
        ){
            r = OPERATION(NULL, tok_op_table[tok.type], NULL);
        }else if(tok.type == TOKEN_O_PAR){
            r = CONST(0);
            r->type = PAR_NODE;
        }else if (tok.type == TOKEN_C_PAR){
            r = CONST(0);
            r->type = C_PAR_NODE;
        }else if(tok.type == TOKEN_INDENTIFIER){
            if (*(tok.start) == 'm'){
                r = IDENTIFIER(0);
            }else{
                fprintf(stderr, "[ERRO]: Identificador '%.*s' desconecido\n", (int)tok.sz, tok.start);
                exit(-1);
            }
        }else{
            fprintf(stderr, "[ERRO]: Token '%.*s' de tipo '%d' desconecido\n", (int)tok.sz, tok.start, tok.type);
            exit(-1);
        }
    }

    return r;
}

tree_node_t *parse_expr(tree_node_t *lhs, int min_prec, size_t *i, token_t *tokens, size_t tokens_sz);
int parse_primary(token_t *tokens, size_t tokens_sz, tree_node_t **value, size_t *i){
    tree_node_t *temp;

    if (*i >= tokens_sz)
        return 0;

    tree_node_t *lhs, *close_par;

    *value = peek(tokens, tokens_sz, *i);
    if ((*value)->type == NUM_NODE){
        // advance
        (*i)++;
        return 1;
    }else if ((*value)->type == IDENTIFIER_NODE){
        (*i)++;

        temp = peek(tokens, tokens_sz, *i);
        if (temp->type == PAR_NODE){
            
        }

        destroy_node(temp);
        return 1;
    }else if ((*value)->type == PAR_NODE){
        // free node '('
        destroy_node(*value);
        // advance '('
        (*i)++;
        if (!parse_primary(tokens, tokens_sz, &lhs, i))
            return 0;

        *value = parse_expr(lhs, 0, i, tokens, tokens_sz);
        close_par = peek(tokens, tokens_sz, *i);
        if (close_par == NULO_NODE || close_par->type != C_PAR_NODE){
            fprintf(stderr, "[ERRO]: Parentesis nao foi fechado\n");
            exit(-1);
        }
        // free node ')'
        destroy_node(close_par);
        // advance ')'
        (*i)++;

        return 1;
    }else if ((*value)->type == FIM_NODE){
        destroy_node(*value);
        return 0;
    }

    fprintf(stderr, "[ERRO]: Token '%.*s' inesperado\n", (int)tokens[*i].sz, tokens[*i].start);
    exit(-1);
}

tree_node_t *parse_expr(tree_node_t *lhs, int min_prec, size_t *i, token_t *tokens, size_t tokens_sz){
    tree_node_t *op, *rhs, *lookahead;

    lookahead = peek(tokens, tokens_sz, *i);

    while (lookahead != NULO_NODE && lookahead->type == OP_NODE && lookahead->PREC >= min_prec){
        op = lookahead;
        // advance op
        (*i)++;

        if (!parse_primary(tokens, tokens_sz, &rhs, i)){
            fprintf(stderr, "[ERRO]: Deveria ter operando apos operador '%s'\n", ops_table[op->OP_CODE]);
            exit(1);
        }
        lookahead = peek(tokens, tokens_sz, *i);

        while (lookahead != NULO_NODE && lookahead->type == OP_NODE && lookahead->PREC > op->PREC){   
            rhs = parse_expr(rhs, lookahead->PREC, i, tokens, tokens_sz);
            destroy_node(lookahead);
            lookahead = peek(tokens, tokens_sz, *i);
        }

        op->LHS = lhs;
        op->RHS = rhs;
        lhs = op;
    }

    destroy_node(lookahead);

    return lhs;
}

tree_node_t *parse(token_t *tokens, size_t tokens_sz){
    size_t i = 0;
    tree_node_t *lhs;

    if (!parse_primary(tokens, tokens_sz, &lhs, &i))
        return NULO_NODE;

    return parse_expr(lhs, 0, &i, tokens, tokens_sz);
}
// Parsing //

// Tokenization //
void print_tokens(token_t *tokens, size_t tokens_sz){
    for (size_t i = 0; i < tokens_sz; i++)
        printf("'%.*s' ", (int)tokens[i].sz, tokens[i].start);
    printf("\n");
}

size_t read_line(char *buffer, size_t buf_sz){
    if (buffer == NULL)
        return 0;
    
    size_t read_sz = 0;
    char c;

    while ((c = getchar()) != '\n' && c != EOF  && read_sz < buf_sz)
        buffer[read_sz++] = c;
    buffer[read_sz] = '\0';

    return read_sz;
}


int is_space(char c){
    return c == ' ';
}

int is_num(char c){
    return '0' <= c && c <= '9';
}

int is_char(char c){
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

int is_identifier_char(char c){
    return is_char(c) || c == '_' || is_num(c);
}

size_t advance_while(char *buf, size_t buf_sz, size_t start, int (*predicate)(char)){
    if (buf == NULL){
        fprintf(stderr, "[ERRO DEV]: Buffer nulo krl, mosca nao\n");
        exit(-1);
    }

    size_t i = start;
    while (i < buf_sz && predicate(buf[i]))
        i++;

    return i;
}

int consume_if_equal(char *start, size_t *end, const char *value){
    size_t n = strlen(value);
    if (strncmp(start, value, n) == 0){
        *end += n;
        return 1;
    }
    
    return 0;
}

size_t tokenize(char *buf, size_t buf_sz, token_t *tokens, size_t tokens_sz){
    size_t start = 0;
    size_t end = 0;
    size_t tokens_read = 0;
    enum Token_type i;
    token_t tok = {0};

    while (start < buf_sz){
        start = end = advance_while(buf, buf_sz, end, &is_space);
        tok.start = &buf[start];
        if (start < buf_sz){
            for (i = 0; i < TOKEN_OP_CODE_SEP; i++){
                if (consume_if_equal(tok.start, &end, ops_table[tok_op_table[i]])){
                    tok.type = i;
                    break;
                }
            }
            if (i != TOKEN_OP_CODE_SEP);
            else if (consume_if_equal(tok.start, &end, "("))
                tok.type = TOKEN_O_PAR;
            else if (consume_if_equal(tok.start, &end, ")"))
                tok.type = TOKEN_C_PAR;
            else if (is_num(tok.start[0])){
                tok.type = TOKEN_NUM;
                end = advance_while(buf, buf_sz, start, &is_num);
            }else if (is_char(tok.start[0])){
                tok.type = TOKEN_INDENTIFIER;
                end = advance_while(buf, buf_sz, start, &is_identifier_char);
            }else{
                fprintf(stderr, "[ERRO]: Nao foi possivel reconhecer o caracter '%c', tokenizacao\n", *tok.start);
                exit(-1);
            }

            // end sempre menor que buf_sz, garantido
            tok.sz = end - start;
            if ( tokens_read > tokens_sz){
                fprintf(stderr, "[ERRO]: Nao foi possivel adicionar o token '%.*s'\n", (int)tok.sz, tok.start);
                exit(-1);
            }
            tokens[tokens_read++] = tok;
            //printf("Token: %.*s\n", (int)tokens[tokens_read - 1].sz, tokens[tokens_read - 1].start); 
        }
    }

    tok.start = "[FIM]";
    tok.sz = 5;
    tok.type = TOKEN_FIM;
    tokens[tokens_read++] = tok;

    return tokens_read;
}
// Tokenization //


int main(){
    tree_node_t *i = CONST(0);
    i->NAME = "teste";
    i->NAME_SZ = 5;
    i->type = FUNCTION_NODE;
    i->ARGS_SZ = 3;
    //i->ARGS[0] = OPERATION(CONST(4), OP_DIV, CONST(8));
    i->ARGS[0] = CONST(4);
    i->ARGS[1] = CONST(6);
    i->ARGS[2] = CONST(-17);

    print_node(i, 0);

    return 0;
}

int main1(){

    char buf[MAX_LINE + 1];
    size_t buf_read;

    printf(">>");
    buf_read = read_line(buf, MAX_LINE);
    token_t tokens[MAX_TOKENS] = {0};
    //printf("Chars read: %zu\n<<%s>>\n", buf_read, buf);
    size_t tokens_sz = tokenize(buf, buf_read, tokens, MAX_TOKENS);
    printf("Tokens sz: %zu\n", tokens_sz);
    printf("Tokens: ");
    print_tokens(tokens, tokens_sz);

    tree_node_t *i = NULO_NODE;
    i = parse(tokens, tokens_sz);
    print_node(i, 0);

    if (0 && i != NULO_NODE){
        memory = run_program(i);
        printf("%d\n", memory);
    }

    i = destroy_node(i);
    

    return 0;
}

int main2(){
    char buf[MAX_LINE + 1];
    size_t buf_read;

    printf(">>");
    while ((buf_read = read_line(buf, MAX_LINE)) != 0){
        token_t tokens[MAX_TOKENS] = {0};
        //printf("Chars read: %zu\n<<%s>>\n", buf_read, buf);
        size_t tokens_sz = tokenize(buf, buf_read, tokens, MAX_TOKENS);
        //printf("%zu\n", tokens_sz);
        //print_tokens(tokens, tokens_sz);

        tree_node_t *i = NULO_NODE;
        i = parse(tokens, tokens_sz);
        //print_node(i, 0);

        if (i != NULO_NODE){
            memory = run_program(i);
            printf("%d\n", memory);
        }

        i = destroy_node(i);
        printf(">>");
    }

    return 0;
}
