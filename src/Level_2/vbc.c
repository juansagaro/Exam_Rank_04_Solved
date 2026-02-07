/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vbc.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsagaro- <jsagaro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 12:52:46 by jsagaro-          #+#    #+#             */
/*   Updated: 2026/02/07 13:44:15 by jsagaro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct node {
    enum {
        ADD,
        MULTI,
        VAL
    }   type;
    int val;
    struct node *l;
    struct node *r;
}   node;

/* --- Funciones Proporcionadas (Helpers) --- */

node    *new_node(node n)
{
    node *ret = calloc(1, sizeof(n));
    if (!ret)
        return (NULL);
    *ret = n;
    return (ret);
}

void    destroy_tree(node *n)
{
    if (!n)
        return ;
    if (n->type != VAL)
    {
        destroy_tree(n->l);
        destroy_tree(n->r);
    }
    free(n);
}

void    unexpected(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of input\n");
}

int accept(char **s, char c)
{
    if (**s == c)
    {
        (*s)++;
        return (1);
    }
    return (0);
}

int expect(char **s, char c)
{
    if (accept(s, c))
        return (1);
    unexpected(**s);
    return (0);
}

/* --- Implementación del Parser (Tu Parte) --- */

// Prototipos necesarios para la recursividad
node *parse_sum(char **s);

// Nivel 3: Factores (Números y Paréntesis)
// Prioridad máxima. Un factor es un número O una expresión entre paréntesis.
node *parse_factor(char **s)
{
    node *n = NULL;

    // Caso 1: Paréntesis '('
    if (accept(s, '('))
    {
        n = parse_sum(s); // Volvemos a empezar la jerarquía dentro del paréntesis
        if (!n) return (NULL); // Si falló lo de dentro, propagamos error
        
        // Esperamos obligatoriamente el cierre
        if (!expect(s, ')'))
        {
            destroy_tree(n); // Limpiamos lo que habíamos creado
            return (NULL);
        }
        return (n);
    }

    // Caso 2: Números (0-9)
    if (isdigit(**s))
    {
        // Creamos un nodo tipo VALor
        node tmp = {VAL, **s - '0', NULL, NULL};
        n = new_node(tmp);
        (*s)++; // Avanzamos el puntero del string
        return (n);
    }

    // Si no es ni '(' ni número, es un error
    unexpected(**s);
    return (NULL);
}

// Nivel 2: Términos (Multiplicación)
// Un término es: Factor * Factor * ...
node *parse_term(char **s)
{
    node *lhs = parse_factor(s); // Lado izquierdo (Left Hand Side)
    if (!lhs) return (NULL);

    while (accept(s, '*')) // Mientras encontremos multiplicaciones
    {
        node *rhs = parse_factor(s); // Lado derecho (Right Hand Side)
        if (!rhs)
        {
            destroy_tree(lhs);
            return (NULL);
        }

        // Creamos nodo padre MULTI que conecta lhs y rhs
        node tmp = {MULTI, 0, lhs, rhs};
        lhs = new_node(tmp); // El nuevo nodo pasa a ser el lhs para la siguiente vuelta
    }
    return (lhs);
}

// Nivel 1: Expresiones (Sumas)
// Una expresión es: Término + Término + ...
node *parse_sum(char **s)
{
    node *lhs = parse_term(s); // Obtenemos el término (que ya procesó multiplicaciones)
    if (!lhs) return (NULL);

    while (accept(s, '+')) // Mientras encontremos sumas
    {
        node *rhs = parse_term(s);
        if (!rhs)
        {
            destroy_tree(lhs);
            return (NULL);
        }

        // Creamos nodo padre ADD
        node tmp = {ADD, 0, lhs, rhs};
        lhs = new_node(tmp);
    }
    return (lhs);
}

/* --- Función Principal de Parseo --- */

node    *parse_expr(char *s)
{
    node *ret = parse_sum(&s); // Iniciamos el análisis pasando la dirección del puntero

    // Si al terminar el parseo la cadena NO ha llegado al final ('\0'), 
    // significa que hay basura o sintaxis inválida (ej: "1+2)")
    if (ret && *s) 
    {
        unexpected(*s);
        destroy_tree(ret);
        return (NULL);
    }
    return (ret);
}

/* --- Evaluación y Main --- */

int eval_tree(node *tree)
{
    switch (tree->type)
    {
        case ADD:
            return (eval_tree(tree->l) + eval_tree(tree->r));
        case MULTI:
            return (eval_tree(tree->l) * eval_tree(tree->r));
        case VAL:
            return (tree->val);
    }
    return (0);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return (1);
    node *tree = parse_expr(argv[1]);
    if (!tree)
        return (1);
    printf("%d\n", eval_tree(tree));
    destroy_tree(tree);
    return (0);
}
