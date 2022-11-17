/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * wffs_pn.c
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


/** @file wffs_pn.c
 *
 * Este archivo contiene el código que se encarga de manejar las fórmulas en
 * notación polaca. Contiene las funciones necesarias para determinar si una
 * fórmula dada es una fórmula bien formada en esta notación, detección de
 * caracteres erróneos, errores de sintaxis... Contiene a su vez el parser que
 * transforma una fbf en NP en un árbol de fbf.
 *
 * Para las fórmulas en notación polaca se emplea la siguiente convención: las
 * letras minúsculas son variables, las letras mayúsculas son conectivas,
 * cualquier otro caracter se considera erroneo.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "logics.h"


/**
 * Categoriza el símbolo dado según una lógica de contexto (según las conectivas
 * definidas) y según la siguiente convención: toda letra minúscula se considera
 * una variable, toda letra mayúscula se considera una conectiva.
 *
 * @param symbol símbolo a categorizar
 * @param logic lógica de contexto
 */
LlSymbolType
ll_symbol_pn_get_type (char symbol, LlLogic* logic)
{
	char symb[2];
	sprintf (symb, "%c", symbol);

	if (islower (symbol))
		return LL_SYMBOL_VAR;
	else if (ll_ucon_list_get_ucon_by_symbol (logic->ucons, symb))
		return LL_SYMBOL_U_CON;
	else if (ll_bcon_list_get_bcon_by_symbol (logic->bcons, symb))
		return LL_SYMBOL_B_CON;
	else
		return LL_SYMBOL_NONE;
}


/**
 * Comprueba que la fórmula dada sea una fórmula bien formada en notación
 * polaca, según una lógica de contexto. Los símbolos admitidos son sólo letras
 * mayúsculas o minúsculas. Aquí se sigue una convención, en una fbf en notación
 * polaca toda letra mayúscula es una conectiva y toda letra minúscula es una
 * variable; se comprueba que las conectivas estén en efecto definidas en la
 * lógica de contexto dada.
 *
 * @return TRUE: si es una fbf en notación polaca, FALSE en caso contrario.
 */
gboolean
ll_formula_is_wff_pn (char formula[], LlLogic* logic)
{
	int    deep = 1,
	       i;

	/* Comprobamos que sólo haya caracteres válidos */
	for (i = 0; i < (int) strlen (formula); i++) {
		if (!isalpha (formula[i])) {
			perror ("* La fórmula dada contiene caracteres no válidos.\n");
			return FALSE;
		}
	}

	for (i = 0; i < (int) strlen (formula); i++) {
		/* Las variables disminuyen la profundidad de la fórmula */
		if (ll_symbol_pn_get_type (formula[i], logic) == LL_SYMBOL_VAR)
			deep--;
		/* Las conectivas unarias dejan la profundidad inalterada */
		else if (ll_symbol_pn_get_type (formula[i], logic) == LL_SYMBOL_U_CON)
			continue;
		/* Las conectivas binarias aumentan la profundidad de la fórmula */
		else if (ll_symbol_pn_get_type (formula[i], logic) == LL_SYMBOL_B_CON)
			deep++;
		/* Sino la conectiva no está definida */
		else if (ll_symbol_pn_get_type (formula[i], logic) == LL_SYMBOL_NONE) {
			fprintf (stderr, "* La conectiva %c no está definida.\n", formula[i]);
			return FALSE;
		}

		/* Si en este punto las conectivas tienen completos sus argumentos pero aún
		 * resta fórmula, está mal formada por dos posibles motivos */
		if (deep == 0 && formula[i + 1] != 0) {
			/* Demasiadas variables */
			if (ll_symbol_pn_get_type (formula[i + 1], logic) == LL_SYMBOL_VAR)
				perror ("* Profundidad excedida; demasiadas variables. Revise la fórmula.\n");
			/* o empieza una nueva fórmula */
			else if (ll_symbol_pn_get_type (formula[i + 1], logic) == LL_SYMBOL_U_CON ||
			         ll_symbol_pn_get_type (formula[i + 1], logic) == LL_SYMBOL_B_CON)
				perror ("* Hay varias fórmulas juntas.\n");
			return FALSE;
		}
	}
	/* Si después de recorrer la fórmula aún restan argumentos, faltan variables */
	if (deep != 0) {
		perror ("* Profundidad insuficiente; muy pocas variables. Revise la fórmula.\n");
		return FALSE;
	}
	else
		return TRUE;
}


/**
 * Transforma una fbf en notación polaca en un árbol de fórmula bien formada.
 * Aquí se lee la fórmula de izquierda a derecha, estableciendo cada elemento
 * uno a uno, de la conectiva principal a sus argumentos. Esto da como resultado
 * un árbol de fórmula bien formada correcto, como es propio de la notación
 * polaca.
 */
//LlWFF*
//ll_wff_parse_formula_pn (LlWFF* wff, char formula_pn[], LlLogic* logic)
//{
	//LlVar    *var;
	//char     symbol[2];
	//int      i;

	//printf ("\n");
	//for (i = 0; i < (int) strlen (formula_pn); i++)
		//{
			//sprintf (symbol, "%c", formula_pn[i]);
			//if (ll_symbol_pn_get_type (formula_pn[i], logic) == LL_SYMBOL_VAR)
				//{
					//printf ("Encontrada variable %c\n", formula_pn[i]);
					//var = ll_var_list_get_var_by_symbol (logic->vars, symbol);
					//wff = ll_wff_add_node (wff, LL_WFF_NODE_VAR, symbol, var->value);
				//}
			//else if (ll_symbol_pn_get_type (formula_pn[i], logic) == LL_SYMBOL_U_CON) {
				//printf ("Encontrada conectiva unaria %c\n", formula_pn[i]);
				//wff = ll_wff_add_node (wff, LL_WFF_NODE_U_CON, symbol, 0);
			//}
			//else if (ll_symbol_pn_get_type (formula_pn[i], logic) == LL_SYMBOL_B_CON) {
				//printf ("Encontrada conectiva binaria %c\n", formula_pn[i]);
				//wff = ll_wff_add_node (wff, LL_WFF_NODE_B_CON, symbol, 0);
			//}
			//else
				//{
					//perror ("* Parsing... Error inesperado\n");
					//return NULL;
				//}
		//}
	//return wff;
//}
