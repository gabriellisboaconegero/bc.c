#include <stdio.h>
#include <stdlib.h>

#define NUM_NODE 1
#define OP_NODE 2

#define NUM data.num
#define LHS data.op.lhs
#define RHS data.op.rhs
#define PREC data.op.prec
#define OP_CODE data.op.op_code

#define P_1  0
#define P_2  1
#define P_3  2
#define PAR  99

#define OPERATION(lhs, op, rhs) create_op_node(op, lhs, rhs, -1)
#define OPERATION_P(lhs, op, rhs) create_op_node(op, lhs, rhs, PAR)
#define CONST(num) create_num_node(num)

typedef struct tree_node_t tree_node_t;

struct op_node_t {
    char op_code;
    int prec;
    tree_node_t *lhs;
    tree_node_t *rhs;
};
typedef struct op_node_t op_node_t;

union node_t {
    int num;
    struct op_node_t op;
} node_t;

struct tree_node_t {
    int type;
    union node_t data;
};

int precede(int op1, int op2){
    if (op1 - op2 > 0)
        return 1;
    return 0;
}

int prec_from_char(char ch){
    if (ch == '-' || ch == '+')
        return P_1;
    else if(ch == '*' || ch == '/')
        return P_2;
    else
        exit(69);
}

void print_node(tree_node_t *n, int depth){
    int i;
    if (n == NULL){
        printf("[NULL]\n");
        return;
    }

    if (n->type == NUM_NODE){
        printf("%d\n", n->NUM);
        return;
    }

    printf("{\n");
    for (i = 0; i < depth; i++)
        printf(" ");
    printf("op: %c\n", n->OP_CODE);

    for (i = 0; i < depth; i++)
        printf(" ");
    printf("prec: %d\n", n->PREC);

    for (i = 0; i < depth; i++)
        printf(" ");
    printf("lhs: ");
    print_node(n->LHS, depth + 2);

    for (i = 0; i < depth; i++)
        printf(" ");
    printf("rhs: ");
    print_node(n->RHS, depth + 2);

    for (i = 0; i < depth; i++)
        printf(" ");
    printf("}\n");
}

void print_equation(tree_node_t *n){
    if (n == NULL)
        return;

    if (n->type == NUM_NODE)
        printf("%d", n->NUM);
    else{

        if (n->LHS->type == OP_NODE && n->LHS->PREC == PAR)
            printf("(");
        print_equation(n->LHS);
        if (n->LHS->type == OP_NODE && n->LHS->PREC == PAR)
            printf(")");

        printf(" %c ", n->OP_CODE);

        if (n->RHS->type == OP_NODE && n->RHS->PREC == PAR)
            printf("(");
        print_equation(n->RHS);
        if (n->RHS->type == OP_NODE && n->RHS->PREC == PAR)
            printf(")");
    }
}

void fix_prec(tree_node_t **n){
    if (*n == NULL || (*n)->type == NUM_NODE)
        return;
    
    if ((*n)->RHS->type == OP_NODE &&
        (*n)->RHS->PREC != PAR &&
        !precede((*n)->RHS->PREC, prec_from_char((*n)->OP_CODE))
       )
    {
        tree_node_t *aux = (*n)->RHS;
        if ((*n)->PREC == PAR){
            (*n)->PREC = prec_from_char(aux->OP_CODE);
            aux->PREC = PAR;
        }
        (*n)->RHS = aux->LHS;
        aux->LHS = (*n);
        (*n) = aux;
        fix_prec(n);
    }

    fix_prec(&(*n)->LHS);
    fix_prec(&(*n)->RHS);

    if ((*n)->RHS->type == OP_NODE &&
        (*n)->RHS->PREC != PAR &&
        !precede((*n)->RHS->PREC, prec_from_char((*n)->OP_CODE))
       )
    {
        tree_node_t *aux = (*n)->RHS;
        if ((*n)->PREC == PAR){
            (*n)->PREC = prec_from_char(aux->OP_CODE);
            aux->PREC = PAR;
        }
        (*n)->RHS = aux->LHS;
        aux->LHS = (*n);
        (*n) = aux;
        fix_prec(n);
    }
}

int run_equation_(tree_node_t *n){
    if (n == NULL)
        exit(30);

    if (n->type == NUM_NODE)
        return n->NUM;

    int lhs = run_equation_(n->LHS);
    int rhs = run_equation_(n->RHS);

    switch (n->OP_CODE){
        case '-': return lhs - rhs;
        case '+': return lhs + rhs;
        case '*': return lhs * rhs;
        default: exit(9);
    }
}

int run_equation(tree_node_t **n){
    fix_prec(n);
    return run_equation_(*n);
}

tree_node_t *create_num_node(int num){
    tree_node_t *n = malloc(sizeof(tree_node_t));

    n->type = NUM_NODE;
    n->NUM = num;

    return n;
}

tree_node_t *create_op_node(char op, tree_node_t *lhs, tree_node_t *rhs, int op_prec){
    tree_node_t *n = malloc(sizeof(tree_node_t));
    if (op_prec == -1)
        op_prec = prec_from_char(op);
    n->type = OP_NODE;
    n->PREC = op_prec;
    n->OP_CODE = op;
    n->LHS = lhs;
    n->RHS = rhs;

    return n;
}

tree_node_t *destroy_node(tree_node_t *n){
    if (n == NULL)
        return NULL;
    if (n->type == NUM_NODE)
        free(n);
    else{
        destroy_node(n->LHS);
        destroy_node(n->RHS);
        free(n);
    }

    return NULL;
}

int main(){
    // 2 - 4 * (3 - 4 * (3 + 5) - (3 * 5)) - 2
    tree_node_t *root = OPERATION(
            CONST(2),
            '-',
            OPERATION(
                CONST(4),
                '*',
                OPERATION(
                    OPERATION_P(
                        CONST(3),
                        '-',
                        OPERATION(
                            CONST(4),
                            '*',
                            OPERATION(
                                OPERATION_P(
                                    CONST(3),
                                    '+',
                                    CONST(5)
                                ),
                                '-',
                                OPERATION_P(
                                    CONST(3),
                                    '*',
                                    CONST(5)
                                )
                            )
                        )
                    ),
                    '-',
                    CONST(2)
                )
            )
    );

    print_equation(root);
    printf(" = %d\n", run_equation(&root));
    destroy_node(root);

    return 0;
}
