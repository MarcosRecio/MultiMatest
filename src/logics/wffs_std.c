/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * wffs_std.c
 * This file is part of MaTest
 *
 * Copyright (C) 2008-2011 - César González Gutiérrez <ceguel@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include "logics.h"


typedef struct _LlStack LlStack;
struct _LlStack
{
	char                symbol[SLEN + 1];
	struct _LlStack    *next;
};


LlStack*
ll_stack_push (LlStack *stack, const char *symbol)
{
	LlStack    *new;

	/* Crear un nodo nuevo */
	new = g_new (LlStack, 1);
	strcpy (new->symbol, symbol);

	/* Añadimos la pila a continuación del nuevo nodo */
	new->next = stack;
	/* Ahora, el comienzo de nuestra pila es el nuevo nodo */
	return new;
}


gchar*
ll_stack_pop (LlStack **stack)
{
	LlStack    *node;
	char       *symbol;

	/* Nodo apunta al primer elemento de la pila */
	node = *stack;
	if (!node) {
		return NULL;
	}
	/* Asignamos a la pila toda la pila menos el primer elemento */
	*stack = node->next;
	/* Guardamos el valor de retorno */
	symbol = g_new0 (gchar, SLEN + 1);
	strcpy (symbol, node->symbol);
	//symbol = g_strdup_printf ("%s", node->symbol);
	/* Borrar el nodo */
	g_free (node);
	return symbol;
}


LlSymbolType
ll_symbol_type (const char *symbol, LlLogic* logic)
{
	if (ll_ucon_list_get_ucon_by_symbol (logic->ucons, symbol)) {
		return LL_SYMBOL_U_CON;
	}
	else if (ll_bcon_list_get_bcon_by_symbol (logic->bcons, symbol)) {
		return LL_SYMBOL_B_CON;
	}
	else if (strlen (symbol) == 1 && isalpha (symbol[0])) {
		return LL_SYMBOL_VAR;
	}
	else if (strcmp (symbol, "(") == FALSE
	         || strcmp (symbol, "[") == FALSE
	         || strcmp (symbol, "{") == FALSE) {
		return LL_SYMBOL_AUX_OPEN;
	}
	else if (strcmp (symbol, ")") == FALSE
	         || strcmp (symbol, "]") == FALSE
	         || strcmp (symbol, "}") == FALSE) {
		return LL_SYMBOL_AUX_CLOSE;
	}
	else {
		return LL_SYMBOL_NONE;
	}
}


int
ll_symbol_priority (const char *symbol, LlLogic* logic)
{
	switch (ll_symbol_type (symbol, logic)) {
		case LL_SYMBOL_AUX_OPEN :
			return 0;
		case LL_SYMBOL_U_CON :
			return 1;
		case LL_SYMBOL_B_CON :
			return 2;
		case LL_SYMBOL_AUX_CLOSE :
			return 3;
	}
}


gint
ll_wff_parse_formula (LlWFF **wff, char formula[], LlLogic* logic)
{
	LlVar      *var;
	LlStack    *stack = NULL,
	           *auxstack = NULL;
	char        symbol[SLEN + 1],
	            chsymb[2],
	           *auxsymb,
	           **symbarr;
	int         deep = 1,
	            i, j = 0, c;

	if (*wff) {
		ll_wff_free (*wff);
		*wff = NULL;
	}
	symbarr = g_new0 (char*, strlen (formula) + 1);
	for (i = 0; i < strlen (formula); i++) {
		symbarr[i] = g_new0 (char, SLEN + 1);
	}
	for (c = 0; c <= SLEN; c++) {
		symbol[c] = '\0';
	}
	for (i = 0; i < 2; i++) {
		chsymb[i] = '\0';
	}

	c = 0; j = 0;
	for (i = 0; i < strlen (formula); i++) {
		chsymb[0] = formula[i];
		if (c > 1 &&
		    (ll_symbol_type (symbol, logic) == LL_SYMBOL_U_CON
		     || ll_symbol_type (symbol, logic) == LL_SYMBOL_B_CON)) {
			strcpy (symbarr[j], symbol);
			j++;
			for (c = 0; c <= SLEN; c++) {
				symbol[c] = '\0';
			}
			c = 0;
			i--;
		}
		else if (isspace (formula[i])) {
			if (c > 0) {
				printf ("* No se ha definido la conectiva \'%s\'.\n", symbol);
				return 1;
			}
			else {
				continue;
			}
		}
		else if (ll_symbol_type (chsymb, logic) == LL_SYMBOL_VAR
		         || ll_symbol_type (chsymb, logic) == LL_SYMBOL_U_CON
		         || ll_symbol_type (chsymb, logic) == LL_SYMBOL_B_CON
				     || ll_symbol_type (chsymb, logic) == LL_SYMBOL_AUX_OPEN
		         || ll_symbol_type (chsymb, logic) == LL_SYMBOL_AUX_CLOSE) {
			strcpy (symbarr[j], chsymb);
			j++;
			if (c > 1) {
				printf ("* No se ha definido la conectiva \'%s\'.\n", symbol);
				return 1;
			}
		}
		else if (c < SLEN) {
			symbol[c] = formula[i];
			c++;
		}
		else {
			printf ("* No se ha definido la conectiva \'%s\'.\n", symbol);
			return 1;
		}
	}
	if (c > 0) {
		if (ll_symbol_type (symbol, logic) == LL_SYMBOL_U_CON
		    || ll_symbol_type (symbol, logic) == LL_SYMBOL_B_CON) {
			strcpy (symbarr[j], symbol);
			j++;
		}
		else {
			printf ("* No se ha definido la conectiva \'%s\'.\n", symbol);
			return 1;
		}
	}
/*
	printf ("Symbol array: ");
	for (i = 0; i < j; i++) {
		printf ("%s, ", symbarr[i]);
	}
	printf ("\n");
	*/

	for (i = j - 1; i >= 0; i--) {
		switch (ll_symbol_type (symbarr[i], logic)) {
			case LL_SYMBOL_VAR :
				stack = ll_stack_push (stack, symbarr[i]);
				break;
			case LL_SYMBOL_U_CON :
			case LL_SYMBOL_B_CON :
				while (auxstack && (ll_symbol_priority (auxstack->symbol, logic) <= ll_symbol_priority (symbarr[i], logic))) {
					auxsymb = ll_stack_pop (&auxstack);
					stack = ll_stack_push (stack, auxsymb);
				}
				auxstack = ll_stack_push (auxstack, symbarr[i]);
				break;
			case LL_SYMBOL_AUX_CLOSE :
				auxstack = ll_stack_push (auxstack, symbarr[i]);
				break;
			case LL_SYMBOL_AUX_OPEN :
				while (auxstack &&
				       ll_symbol_type (auxstack->symbol, logic) != LL_SYMBOL_AUX_CLOSE) {
					auxsymb = ll_stack_pop (&auxstack);
					stack = ll_stack_push (stack, auxsymb);
				}
				auxsymb = ll_stack_pop (&auxstack);
				break;
			case LL_SYMBOL_NONE :
				if (i > 0 && isspace (formula[i - 1])) {
					printf ("* No está definida la conectiva \'%s\'\n", symbol);
					return 1;
				}
				break;
		}
	}

	while (auxstack) {
		auxsymb = ll_stack_pop (&auxstack);
		stack = ll_stack_push (stack, auxsymb);
	}

	for (i = 0; i < strlen (formula); i++) {
		g_free (symbarr[i]);
	}
	g_free (symbarr);

	/* Revisamos la fórmula como en la notación polaca */
	for (auxstack = stack; auxstack; auxstack = auxstack->next) {
		/* Las variables disminuyen la profundidad de la fórmula */
		if (ll_symbol_type (auxstack->symbol, logic) == LL_SYMBOL_VAR)
			deep--;
		/* Las conectivas unarias dejan la profundidad inalterada */
		else if (ll_symbol_type (auxstack->symbol, logic) == LL_SYMBOL_U_CON)
			continue;
		/* Las conectivas binarias aumentan la profundidad de la fórmula */
		else if (ll_symbol_type (auxstack->symbol, logic) == LL_SYMBOL_B_CON)
			deep++;
		/* Sino la conectiva no está definida */
		//else if (ll_symbol_type (auxstack->symbol, logic) == LL_SYMBOL_NONE
		         //|| auxstack->symbol[0] == '\0') {
			//ll_stack_pop (&auxstack);
			//printf ("Limpiando un nodo vacío.\n");
		//}
		/* Si en este punto las conectivas tienen completos sus argumentos pero aún
		 * restan variables, hay variables libres */
		if (deep == 0
		    && auxstack->next
		    && ll_symbol_type (auxstack->next->symbol, logic) == LL_SYMBOL_VAR) {
			printf ("* Hay variables libres. Revise la fórmula.\n");
			return 2;
		}
	}
	/* Si después de recorrer la fórmula aún restan argumentos, faltan variables */
	if (deep != 0) {
		printf ("* Profundidad insuficiente; muy pocas variables. Revise la fórmula.\n");
		return 3;
	}

	ll_logic_add_formula_vars (logic, formula);

/*
	printf ("Symbol stack: ");
	for (auxstack = stack; auxstack; auxstack = auxstack->next) {
		printf ("%s, ", auxstack->symbol);
	}
	printf ("\n");
	*/

	while (stack) {
		auxsymb = ll_stack_pop (&stack);
		switch (ll_symbol_type (auxsymb, logic)) {
			case LL_SYMBOL_NONE :
				break;
			case LL_SYMBOL_VAR :
				var = ll_var_list_get_var_by_symbol (logic->vars, auxsymb);
				ll_wff_add_node (wff, LL_WFF_NODE_VAR, auxsymb, var->value);
				break;
			case LL_SYMBOL_U_CON :
				ll_wff_add_node (wff, LL_WFF_NODE_U_CON, auxsymb, 0);
				break;
			case LL_SYMBOL_B_CON :
				ll_wff_add_node (wff, LL_WFF_NODE_B_CON, auxsymb, 0);
				break;
			case LL_SYMBOL_AUX_OPEN :
			case LL_SYMBOL_AUX_CLOSE :
				break;
			default :
				printf ("* Parsing... Error inesperado\n");
				return 7;
		}
		g_free (auxsymb);
	}
}


gchar*
ll_wff_sprint_formula (LlWFF *wff, LlLogic *logic)
{
	GString    *formula = NULL;
	gchar      *auxstr;

	formula = g_string_new (NULL);

  if (!wff)
    return g_string_free (formula, FALSE);

  if (ll_symbol_type (wff->symbol, logic) == LL_SYMBOL_B_CON) {
    g_string_append (formula, "(");
	}
  auxstr = ll_wff_sprint_formula (wff->prearg, logic);
  g_string_append_printf (formula, "%s", auxstr);
  g_free (auxstr);
  if (ll_symbol_type (wff->symbol, logic) == LL_SYMBOL_B_CON) {
		g_string_append_printf (formula, " %s ", wff->symbol);
	}
	else {
		g_string_append_printf (formula, "%s", wff->symbol);
	}
  auxstr = ll_wff_sprint_formula (wff->postarg, logic);
  g_string_append_printf (formula, "%s", auxstr);
  g_free (auxstr);
  if (ll_symbol_type (wff->symbol,logic) == LL_SYMBOL_B_CON) {
    g_string_append (formula, ")");
	}
	return g_string_free (formula, FALSE);
}


gchar*
ll_wff_sprint_formula_values (LlWFF *wff, LlLogic *logic)
{
	GString    *formula = NULL;
	gchar      *auxstr;

	formula = g_string_new (NULL);

  if (!wff) {
    return g_string_free (formula, FALSE);
	}

  if (ll_symbol_type (wff->symbol, logic) == LL_SYMBOL_B_CON) {
    g_string_append (formula, "(");
	}
  auxstr = ll_wff_sprint_formula_values (wff->prearg, logic);
  g_string_append_printf (formula, "%s", auxstr);
  g_free (auxstr);
  if (ll_symbol_type (wff->symbol, logic) == LL_SYMBOL_B_CON) {
		switch (strlen (wff->symbol)) {
			case 1 :
				g_string_append_printf (formula, " %x ", wff->value);
				break;
			case 2 :
				g_string_append_printf (formula, "  %x ", wff->value);
				break;
			case 3 :
				g_string_append_printf (formula, "  %x  ", wff->value);
				break;
		}
	}
	else if (ll_symbol_type (wff->symbol, logic) == LL_SYMBOL_U_CON) {
		switch (strlen (wff->symbol)) {
			case 1 :
				g_string_append_printf (formula, "%x", wff->value);
				break;
			case 2 :
				g_string_append_printf (formula, "%x ", wff->value);
				break;
			case 3 :
				g_string_append_printf (formula, " %x ", wff->value);
				break;
		}
	}
	else {
		g_string_append_printf (formula, "%x", wff->value);
	}
  auxstr = ll_wff_sprint_formula_values (wff->postarg, logic);
  g_string_append_printf (formula, "%s", auxstr);
  g_free (auxstr);
  if (ll_symbol_type (wff->symbol,logic) == LL_SYMBOL_B_CON) {
    g_string_append (formula, ")");
	}
	return g_string_free (formula, FALSE);
}
