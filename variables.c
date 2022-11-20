/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * variables.c
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

/**
 * @file variables.c
 *
 * Contiene las funciones encargadas de manejar las variables proposicionales y
 * sus listas.
 */


#include "logics.h"

/**
 * Crea una variable proposicional nueva.
 *
 * @param symbol Letra o símbolo que representa la variable.
 * @param value Valor asignado a la variable.
 * @return La nueva variable proposicional.
 */
LlVar*
ll_var_new (char* symbol, int value)
{
	LlVar    *var;

	var = g_slice_new (LlVar);

	if (!var) {
		perror ("liblogics: Error al reservar memoria para una variable proposicional.\n");
		return NULL;
	}
	else {
		var->value = value;
		strcpy (var->symbol, symbol);
		return var;
	}
}


/**
 * @brief Libera la memoria de una variable proposicional.
 * @param var Una variable proposicional.
 * 
 * 
 */
void
ll_var_free (LlVar* var)
{
	g_slice_free (LlVar, var);
}


/**
 * Establece el valor dado en una variable proposicional.
 */
void
ll_var_set_value (LlVar *var, int value)
{
	if (var)
		var->value = value;
	else
    perror ("liblogics: La variable no existe.\n");
}


/**
 * Devuelve el valor de una variable proposicional.
 *
 * @return \f$n \geq 0\f$ : el valor de la variable.\n
 *         -1: error, la variable no existe.
 */
int
ll_var_get_value (LlVar *var)
{
	if (var)
		return var->value;
	else {
		perror ("liblogics: La variable no existe.\n");
		return -1;
	}
}


bool
ll_var_list_is_empty (GList *var_list)
{
	return (var_list == NULL);
}


/**
 * Procedimiento que elimina una lista de variables por completo liberando la
 * memoria reservada.
 */
void
ll_var_list_free (GList* var_list)
{
	g_list_foreach (var_list, (GFunc) ll_var_free, NULL);
	g_list_free (var_list);
}


/**
 * Busca una variable dada por su símbolo en una lista de variables.
 *
 * @return Puntero a la variable si existe, o el puntero nulo en caso contrario.
 */
LlVar*
ll_var_list_get_var_by_symbol (GList* var_list, char symbol[])
{
	GList    *iter = NULL;
	LlVar    *var;

	for (iter = var_list; iter; iter = iter->next)
		{
			var = (LlVar*) iter->data;
			if (!g_ascii_strcasecmp (symbol, var->symbol)) {
				return var;
			}
		}

	return NULL;
}


/**
 * Añade una variable proposicional a una lista si no existe una con su mismo
 * símbolo y la sitúa por orden alfabético, con lo que la lista queda ordenada.
 */
GList*
ll_var_list_add_var (GList* var_list, LlVar* var)
{
	/* No duplicamos las variables que ya existen */
	if (ll_var_list_get_var_by_symbol (var_list, var->symbol)) {
		return var_list;
	}
	/* Añadimos la variable ordenada alfabéticamente */
	else {
		return g_list_insert_sorted (var_list, var, (GCompareFunc) g_ascii_strcasecmp);
	}
}


/**
 * Devuelve el número de elementos que contiene una lista de variables.
 */
guint
ll_var_list_length (GList *var_list)
{
	return g_list_length (var_list);
}


/**
 * Procedimiento para registrar las variables de una fórmula. Añade todas las
 * variables presentes en una fórmula en la lista de variables de una lógica.
 *
 * @param logic Lógica a la que van destinadas las variables, además sirve de
 *        contexto para identificar los elementos que son variables.
 * @param formula Fórmula dada como cadena de caracteres.
 */
void
ll_logic_add_formula_vars (LlLogic* logic, char formula[])
{
	LlVar     *var;
	int       i;
	char      symbol[2];

	if (logic->vars) {
		ll_var_list_free (logic->vars);
		logic->vars = NULL;
	}

	for (i = 0; i < (int) strlen (formula); i++)
		{
			g_sprintf (symbol, "%c", formula[i]);
			if (ll_symbol_type (symbol, logic) == LL_SYMBOL_VAR)
				{
					var = ll_var_new (symbol, 0);
			    logic->vars = ll_var_list_add_var (logic->vars, var);
				}
		}
}
