/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * wffs.c
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


#include "logics.h"
#include <stdlib.h>


LlWFF*
ll_wff_get_avail_node (LlWFF *wff)
{
	LlWFF    *father = NULL,
	         *node = wff;

	printf ("Buscamos un argumento libre.\n");
	while (node) {
		if (node->type == LL_WFF_NODE_VAR) {
			return NULL;
		}
		else if (node->type == LL_WFF_NODE_U_CON) {
			printf ("El nodo es una conectiva unaria.\n");
			if (node->postarg == NULL) {
				return node;
			}
			else {
				node = node->postarg;
			}
		}
		else if (node->type == LL_WFF_NODE_B_CON) {
			printf ("El nodo es una conectiva binaria\n");
			if (node->prearg == NULL) {
				printf ("y su primer argumento está libre.\n");
				return node;
			}
			else {
				printf ("y buscamos en su primer argumento.\n\t");
				node = ll_wff_get_avail_node (node->prearg);
				return node;
			}

			if (node->postarg == NULL) {
				printf ("y su segundo argumento está libre.\n");
				return node;
			}
			else {
				printf ("y buscamos en el segundo argumento.\n\t");
				node = ll_wff_get_avail_node (node->postarg);
				return node;
			}
		}
	}
	return node;
}
//LlWFF*
//ll_wff_get_avail_node (LlWFF *wff)
//{
	//LlWFF    *father = NULL,
	         //*node = wff;

	//while (node) {
		//if (node->type = LL_WFF_NODE_VAR) {
			//return NULL;
		//}
		//else if (node->type = LL_WFF_NODE_U_CON) {
			//father = node;
			//node = node->postarg;
		//}
		//else if (node->type = LL_WFF_NODE_B_CON) {
			//if (node = ll_wff_get_avail_node (node->prearg) && node) {
				//return node;
			//}
			//else if (node = ll_wff_get_avail_node (node->postarg) && node) {
				//return node;
			//}
			//else {
				//return NULL;
			//}
		//}
	//}
	//return father;
//}


/**
 * Busca recursivamente el primer argumento libre en el árbol de fbf dado.
 * Después establece el nombre del elemento, el tipo y el valor correspondiente.
 *
 * @param wff Árbol de fórmula bien formada.
 * @param symbol_type Tipo de nodo.
 * @param symbol Símbolo del nodo.
 * @param value Valor del nodo.
 * @return true: si tiene éxito, false: en caso contrario.
 */
bool
ll_wff_add_node (LlWFF **wff, LlWFFType symbol_type, char *symbol, int value)
{
	LlWFF *father = NULL;
	LlWFF *node = *wff;

	while (node)
		{
			/* Las conectivas unarias tienen un sólo argumento: trivial */
			if (node->type == LL_WFF_NODE_U_CON)
				{
					father = node;
					node = node->postarg;
				}
			/* Con las conectivas binarias buscamos recursivamente */
			else if (node->type == LL_WFF_NODE_B_CON)
				{
					if (ll_wff_add_node (&node->prearg, symbol_type, symbol, value))
						return true;
					else if (ll_wff_add_node (&node->postarg, symbol_type, symbol, value))
						return true;
					else
						return false;
				}
			/* Las variables carecen de argumentos */
			else if (node->type == LL_WFF_NODE_VAR)
				return false;
		}

	/* Si el árbol está vacío, reservamos memoria y establecemos los valores */
	if (father == NULL)
		{
			(*wff) = (LlWFF*) malloc (sizeof (LlWFF));
			(*wff)->type = symbol_type;
			strcpy ((*wff)->symbol, symbol);
			(*wff)->value = value;
			(*wff)->prearg = (*wff)->postarg = NULL;
			return true;
		}
	else if (father->type == LL_WFF_NODE_U_CON)
		{
			node = (LlWFF*) malloc (sizeof (LlWFF));
			father->postarg = node;
			node->type = symbol_type;
			strcpy (node->symbol, symbol);
			node->value = value;
			node->prearg = node->postarg = NULL;
			return true;
		}
	else if (father->type == LL_WFF_NODE_B_CON)
		{
			if (!father->prearg)
				{
					node = (LlWFF*) malloc (sizeof (LlWFF));
					father->prearg = node;
					node->type = symbol_type;
					strcpy (node->symbol, symbol);
					node->value = value;
					node->prearg = node->postarg = NULL;
					return true;
				}
			else
				{
					node = (LlWFF*) malloc (sizeof (LlWFF));
					father->postarg = node;
					node->type = symbol_type;
					strcpy (node->symbol, symbol);
					node->value = value;
					node->prearg = node->postarg = NULL;
					return true;
				}
		}
	else
		{
			perror ("* Estableciendo el elemento de una fbf... Error inesperado.\n");
			return false;
		}
}
//LlWFF*
//ll_wff_add_node (LlWFF *wff, LlWFFType node_type, char symbol[], int value)
//{
	//LlWFF    *father = NULL;
	//LlWFF    *node = wff;

	//while (node)
		//{
			///* Las conectivas unarias tienen un sólo argumento: trivial */
			//if (node->type == LL_WFF_NODE_U_CON)
				//{
					//father = node;
					//node = node->postarg;
				//}
			///* Con las conectivas binarias buscamos recursivamente */
			//else if (node->type == LL_WFF_NODE_B_CON)
				//{
					//father = node;
					//node = ll_wff_add_node (node->prearg, node_type, symbol, value);
					//if (node) {
						//node = node->prearg;
					//}
					//node = ll_wff_add_node (node->postarg, node_type, symbol, value);
					//if (node) {
						//node = node->postarg;
					//}
				//}
			///* Las variables carecen de argumentos */
			//else if (node->type == LL_WFF_NODE_VAR)
				//return NULL;
		//}

	///* Si el árbol está vacío, reservamos memoria y establecemos los valores */
	//if (father == NULL)
		//{
			//wff = (LlWFF*) malloc (sizeof (LlWFF));
			//wff->type = node_type;
			//strcpy (wff->symbol, symbol);
			//wff->value = value;
			//wff->prearg = wff->postarg = NULL;
			//return wff;
		//}
	//else if (father->type == LL_WFF_NODE_U_CON)
		//{
			//node = (LlWFF*) malloc (sizeof (LlWFF));
			//father->postarg = node;
			//node->type = node_type;
			//strcpy (node->symbol, symbol);
			//node->value = value;
			//node->prearg = node->postarg = NULL;
			//return wff;
		//}
	//else if (father->type == LL_WFF_NODE_B_CON)
		//{
			//if (!father->prearg)
				//{
					//node = (LlWFF*) malloc (sizeof (LlWFF));
					//father->prearg = node;
					//node->type = node_type;
					//strcpy (node->symbol, symbol);
					//node->value = value;
					//node->prearg = node->postarg = NULL;
					//return wff;
				//}
			//else
				//{
					//node = (LlWFF*) malloc (sizeof (LlWFF));
					//father->postarg = node;
					//node->type = node_type;
					//strcpy (node->symbol, symbol);
					//node->value = value;
					//node->prearg = node->postarg = NULL;
					//return wff;
				//}
		//}
	//else
		//{
			//perror ("Estableciendo el elemento de una fbf... Error inesperado.\n");
			//return false;
		//}
//}
//LlWFF*
//ll_wff_add_node (LlWFF* wff, LlWFFType node_type, char symbol[], int value)
//{
	//LlWFF    *node,
	         //*father;

	///* Si el árbol está vacío, reservamos memoria y establecemos los valores */
	//if (!wff) {
		//printf ("Árbol vacío, establecemos el primer nodo %s.\n", symbol);
		//wff = (LlWFF*) malloc (sizeof (LlWFF));
		//wff->type = node_type;
		//strcpy (wff->symbol, symbol);
		//wff->value = value;
		//wff->prearg = wff->postarg = NULL;
	//}
	//else {
		//father = ll_wff_get_avail_node (wff);
		//if (!father) {
			//perror ("La fbf está completa.\n");
		//}
		//else {
			//node = (LlWFF*) malloc (sizeof (LlWFF));
			//node->type = node_type;
			//strcpy (node->symbol, symbol);
			//node->value = value;
			//node->prearg = node->postarg = NULL;
			//if (father->type == LL_WFF_NODE_U_CON) {
				//printf ("Establecemos el nodo como hijo de una conectiva unaria.\n");
				//father->postarg = node;
			//}
			//else if (father->type == LL_WFF_NODE_B_CON) {
				//if (!father->prearg) {
					//printf ("Establecemos el nodo hijo como preargumento de una conectiva binaria.\n");
					//father->prearg = node;
				//}
				//else {
					//printf ("Establecemos el nodo hijo como preargumento de una conectiva binaria.\n");
					//father->postarg = node;
				//}
			//}
			//else
				//return NULL;
		//}
	//}
	//return wff;
//}


/**
 * Elimina el árbol de fórmula bien formada dado como argumento liberando su
 * memoria.
 *
 * @param wff Un árbol de fórmula bien formada a borrar.
 */
void
ll_wff_free (LlWFF *wff)
{
	if (!wff)
		return;
	ll_wff_free (wff->prearg);
	ll_wff_free (wff->postarg);
	free (wff);
}


void
ll_wff_valuate (LlWFF *wff, LlLogic* logic)
{
	LlVar     *var;
	LlUCon    *ucon;
	LlBCon    *bcon;

	if (!wff) {
		return;
	}
	else if (wff->type == LL_WFF_NODE_VAR) {
		var = (LlVar*) ll_var_list_get_var_by_symbol (logic->vars, wff->symbol);
		if (!var)
			g_message ("No devuelve ninguna variable.\n");
		wff->value = var->value;
		return;
	}
	else if (wff->type == LL_WFF_NODE_U_CON) {
		ucon = (LlUCon*) ll_ucon_list_get_ucon_by_symbol (logic->ucons, wff->symbol);
		ll_wff_valuate (wff->postarg, logic);
		wff->value = ucon->matrix[wff->postarg->value];
		return;
	}
	else if (wff->type == LL_WFF_NODE_B_CON) {
		bcon = (LlBCon*) ll_bcon_list_get_bcon_by_symbol (logic->bcons, wff->symbol);
		ll_wff_valuate (wff->prearg, logic);
		ll_wff_valuate (wff->postarg, logic);
		wff->value = bcon->matrix[wff->prearg->value][wff->postarg->value];
		return;
	}
	else {
		perror ("* Evaluando la fórmula actual... Error inesperado.\n");
		return;
	}
}


/**
 * Calcula recursivamente y devuelve el valor que se asigna a una fórmula
 * recorriendo el árbol de la fbf, basándose en los valores que toman sus
 * variables y la semántica de las contectivas contenidas en una lógica de
 * contexto.
 *
 * @return n >= 0: el valor que toma la fórmula.\n
 *             -1: error, el árbol de la fbf está vacío.\n
 *             -2: error inesperado.
 */
int
ll_wff_get_value (LlWFF *wff, LlLogic* logic)
{
	ll_wff_valuate (wff, logic);
	return wff->value;
}
