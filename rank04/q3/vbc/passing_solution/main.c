#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct node {
    enum { ADD, MULT, VAL } type;
    int val;
    struct node *l;
    struct node *r;
} node;

node *parse_expr(char **s);

node* new_node(node n) {
    node* ret = calloc(1, sizeof(node));
    if (!ret) return NULL;
    *ret = n;
    return ret;
}

void unexpected(char c) {
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of input\n");
}

int accept(char **s, char c) {
    if (**s == c) {
        (*s)++;
        return 1;
    }
    return 0;
}

int expect(char **s, char c) {
    if (accept(s, c)) {
        return 1;
    }
    unexpected(**s);
    return 0;
}

int eval_tree(node* tree) {
    if (tree->type == ADD)
        return eval_tree(tree->l) + eval_tree(tree->r);
    else if (tree->type == MULT)
        return eval_tree(tree->l) * eval_tree(tree->r);
    else
        return tree->val;
}

void destroy_tree(node* tree) {
    if (tree) {
        if (tree->type != VAL) {
            destroy_tree(tree->l);
            destroy_tree(tree->r);
        }
        free(tree);
    }
}

// solution below

node *parse_nbr(char **s) {
    if(isdigit(**s)) {
        node n = { .type = VAL, .val = **s - '0', .r = NULL, .l = NULL };
        (*s)++;
        return new_node(n);
    }
    unexpected(**s);
    return NULL;
}

node *parse_fact(char **s)
{
    if (accept(s, '('))
    {
        node *expr = parse_expr(s);
        if (!expect(s, ')'))
        {
            destroy_tree(expr);
            return NULL;
        }
        return expr;
    }
    return parse_nbr(s);
}


node *parse_term(char **s) {
    node *left = parse_fact(s);
    while (accept(s, '*')) {
        node *right = parse_fact(s);
        if (!right) {
            destroy_tree(left);
            return NULL;
        }
        node n = { .type = MULT , .l = left, .r = right };
        left = new_node(n);
    }
    return left;
}

node *parse_expr(char **s) {
    node *left = parse_term(s);
    while (accept(s, '+')) {
        node *right = parse_term(s);
        if (!right) {
            destroy_tree(left);
            return NULL;
        }
        node n = { .type = ADD, .l = left, .r = right };
        left = new_node(n);
    }
    return left;
}

int main(int ac, char** av) {
    if (ac != 2) {
        printf("Usage: ./calculator \"expression\"\n");
        return 1;
    }

    char *input = av[1];
    node *tree = parse_expr(&input);

    if (!tree) {
        destroy_tree(tree);
        return 1;
    }
    else if (*input) {
        unexpected(*input);
        destroy_tree(tree);
        return 1;
    }

    printf("%d\n", eval_tree(tree));
    destroy_tree(tree);
    return 0;
}
