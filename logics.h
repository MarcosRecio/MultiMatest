/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * logics.h
 * Cabecera con tipos de datos y funciones para el manejo de lógicas en general.
 *
 * Copyright (C) 2008-2011 - César González Gutiérrez <ceguel@gmail.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */


#ifndef __LOGICS_H__
#define __LOGICS_H__


#include <string.h>
#include <stdbool.h>
#include <glib.h>

/* Algunas abreviaciones útiles */
#define MAX(a, b)	(((a) > (b)) ? (a) : (b))
#define MIN(a, b)	(((a) < (b)) ? (a) : (b))
#define DIM logic->dimension
#define MDV logic->mdv
#define MAXV logic->dimension - 1
#define SLEN 3

/** @enum Tipos de símbolos de una fórmula */
typedef enum
{
	LL_SYMBOL_VAR,         /**< Variable.                                       */
	LL_SYMBOL_CONST,       /**< Constante.                                      */
	LL_SYMBOL_U_CON,       /**< Conectiva unaria.                               */
	LL_SYMBOL_B_CON,       /**< Conectiva binaria.                              */
	LL_SYMBOL_AUX_OPEN,    /**< Símbolo auxiliar de apertura.                   */
	LL_SYMBOL_AUX_CLOSE,   /**< Símbolo auxiliar de cierre.                     */
	LL_SYMBOL_NONE         /**< Tipo nulo, para el control de errores.          */
}
LlSymbolType;


/** @enum Tipos de nodos de una fórmula bien formada */
typedef enum
{
	LL_WFF_NODE_VAR,       /**< Variable.                                       */
	LL_WFF_NODE_U_CON,     /**< Conectiva unaria.                               */
	LL_WFF_NODE_B_CON      /**< Conectiva binaria.                              */
}
LlWFFType;


/** @enum Tipos de notación */
typedef enum
{
	LL_STDN,               /**< Notación estándar.                              */
	LL_PN,                 /**< Notación polaca.                                */
	LL_RPN                 /**< Notación polaca inversa.                        */
}
LlNotaion;


/** @struct
 * Nodo variable proposicional.
 *
 * Entendemos aquí por variable proposicional a un tipo de dato que tiene
 * asociado un símbolo que la identifica, que puede tomar distintos valores
 * numéricos y que forma un grupo, una lista, con los otros elementos de la
 * misma clase que se hayan en uso.
 */
typedef struct _LlVar LlVar;
struct _LlVar
{
	char     symbol[2];   /**< Nombre de la variable o letra que la designa.    */
	int      value;       /**< Valor de la variable.                            */
};


/** @struct
 * Nodo conectiva unaria.
 *
 * Una conectiva unaria aquí es una estructura que contiene el símbolo con el
 * que designamos a dicha conectiva y un vector de valores numéricos que define
 * la semántica del operador del siguiente modo: el valor que asigna el operador
 * a su argumento cuando éste vale n, es el valor del n-ésimo subíndice, esto
 * es, \f$ v(\Delta n) = \Delta_n \f$, siendo \f$ \Delta \f$ una conectiva
 * cualquiera y \f$ n \f$ un valor cualquiera.
 */
typedef struct _LlUCon LlUCon;
struct _LlUCon
{
	char   symbol_pn[4]; /**< Símbolo que representa la conectiva (una letra
	                          mayúscula en la notación polaca).                 */
	char   symbol[4];    /**< Símbolo de la conectiva en notación estándar.     */
	int    *matrix;      /**< Matriz de la conectiva: una matriz línea que define
	                          la semántica de la conectiva; contiene los valores
	                          que ésta asignará a una variable.                 */
};


/** @struct
 * Nodo conectiva binaria.
 *
 * Una conectiva binaria aquí es una estructura que contiene el símbolo con el
 * que designamos a dicha conectiva y una matriz de valores numéricos que define
 * la semántica del operador del siguiente modo: el valor que asigna el operador
 * a sus argumentos cuando éstos valen n y m es el valor del elemento de la fila
 * n y columna m, esto es, \f$ v(n \Delta m) = \Delta_{nm} \f$, siendo la
 * expresión en notación estándar, \f$ \Delta \f$ una conectiva cualquiera y
 * \f$ n, m \f$ dos valores cualesquiera.
 */
typedef struct _LlBCon LlBCon;
struct _LlBCon
{
	char   symbol_pn[4]; /**< Símbolo que representa la conectiva en la notación
	                          polaca (una letra mayúscula).                     */
	char   symbol[4];    /**< Símbolo de la conectiva en notación estándar.     */
	int    **matrix;     /**< Matriz de la conectiva: una matriz cuadrada que
	                          define la semántica de la conectiva; contiene los
	                          valores que ésta asignará a un par de variables.  */
};


/** @struct
 * Nodo Fórmula Bien Formada (WFF: Well Formed Formula).
 * Una fbf es una estructura tipo árbol en la que el nodo principal es la
 * conectiva principal, sus nodos hijos pueden ser o bien otras conectivas o
 * bien variables, por último los nodos hoja son siempre variables. Puede darse
 * el caso de que el árbol esté formado por un sólo elemento, un nodo hoja, el
 * caso de que sea una variable proposicional.
 *
 * Cada nodo está bien formado si, o bien
 * - el tipo de nodo es de conectiva binaria y tanto el preargumento como el
 *   postargumento apuntan a sendos nodos, o bien
 * - el tipo de nodo es de conectiva unaria, el postargumento apunta a otro
 *   nodo y el preargumento es el puntero nulo, o bien
 * - el tipo de nodo es de variable y ambos argumentos son el puntero nulo.
 */
typedef struct _LlWFF LlWFF;
struct _LlWFF
{
	LlWFFType    type;         /**< Tipo del elemento.                          */
	char         symbol[4];    /**< Símbolo que lo representa.                  */
	int          value;        /**< Valor del nodo.                             */
	LlWFF        *prearg;      /**< Puntero al argumento precedente.            */
	LlWFF        *postarg;     /**< Puntero al argumento posterior.             */
};

//typedef LlWFFNode *LlWFF;  /**< Declaración de una fórmula bien formada. */


/** @struct
 * Registro con todos los datos que definen una lógica.
 * Una lógica, aquí, es un conjunto de elementos consistentes entre sí, tales
 * que:
 * - hay un valor entero al que llamamos dimensión, el cual toma un valor
 *   arbitrario mayor que 2,
 * - hay un valor entero que denominamos mínimo valor designado, que toma un
 *   valor entre 1 y la dimensión menos 1,
 * - se define un conjunto de variables cuyos valores pueden variar entre 0 y la
 *   dimensión menos 1,
 * - se define un conjunto de conectivas unarias cuya semántica está definida
 *   por una matriz línea de un número de elementos igual a la dimensión, con
 *   valores enteros comprendidos entre 0 y la dimensión menos 1,
 * - se define un conjunto de conectivas binarias cuya semántica está definida
 *   por una matriz cuadrada de orden igual a la dimensión, con valores enteros
 *   comprendidos entre 0 y la dimensión menos 1.
 */
typedef struct
{
	int      dimension;   /**< Dimensión de las matrices.                       */
	int      mdv;         /**< Mínimo Valor Designado.                          */
	GList    *vars;       /**< Lista de variables proposicionales.              */
	GList    *ucons;      /**< Lista de conectivas unarias.                     */
	GList    *bcons;      /**< Lista de conectivas binarias.                    */
}
LlLogic;


/* Variables proposicionales */
LlVar*        ll_var_new                        (char          *symbol,
                                                 int            value);
void          ll_var_set_value                  (LlVar         *var,
                                                 int            value);
int           ll_var_get_value                  (LlVar         *var);
bool          ll_var_list_is_empty              (GList         *var_list);
void          ll_var_list_free                  (GList         *var_list);
LlVar*        ll_var_list_get_var_by_symbol     (GList         *var_list,
                                                 char          *symbol);
guint         ll_var_list_length                (GList         *var_list);
GList*        ll_var_list_add_var               (GList         *var_list,
                                                 LlVar         *var);

/* Conectivas */
LlUCon*       ll_ucon_new                       (const char    *symbol,
                                                 const char    *symbol_pn,
                                                 int           *matrix,
                                                 int            dimension);
LlBCon*       ll_bcon_new                       (const char    *symbol,
                                                 const char    *symbol_pn,
                                                 int           **matrix,
                                                 int            dimension);
LlUCon*       ll_ucon_list_get_ucon_by_symbol   (GList         *ucon_list,
                                                 const char    *symbol);
LlBCon*       ll_bcon_list_get_bcon_by_symbol   (GList         *bcon_list,
                                                 const char    *symbol);
GList*        ll_ucon_list_append               (GList         *ucon_list,
                                                 LlUCon        *ucon);
GList*        ll_bcon_list_append               (GList         *bcon_list,
                                                 LlBCon        *bcon);
void          ll_con_delete_by_symbol           (LlLogic       *logic,
                                                 const char    *symbol);
void          ll_logic_set_default_ucons_lukasiewicz
                                                (LlLogic       *logic);
void          ll_logic_set_default_bcons_lukasiewicz
                                                (LlLogic       *logic);
void          ll_ucon_list_free                 (GList         *ucon_list);
void          ll_bcon_list_free                 (GList         *bcon_list,
                                                 int            dimension);

/* Fórmulas bien formadas */
void          ll_wff_free                       (LlWFF         *wff);
void          ll_wff_valuate                    (LlWFF         *wff,
                                                 LlLogic       *logic);
bool          ll_wff_add_node                   (LlWFF         **wff,
                                                 LlWFFType      node_type,
                                                 char          *symbol,
                                                 int            value);
int           ll_wff_get_value                  (LlWFF         *wff,
                                                 LlLogic       *logic);

/* Fórmulas bien formadas en notación polaca */
LlSymbolType  ll_symbol_pn_get_type             (char           symbol,
                                                 LlLogic       *logic);
bool          ll_formula_is_wff_pn              (char          *formula,
                                                 LlLogic       *logic);
LlWFF*        ll_wff_parse_formula_pn           (LlWFF         *tree,
                                                 char          *formula_pn,
                                                 LlLogic       *logic);

/* Fórmulas bien formadas en notación estándar */
gint          ll_wff_parse_formula              (LlWFF         **wff,
                                                 char          *formula,
                                                 LlLogic       *logic);
LlSymbolType
ll_symbol_type (const char *symbol, LlLogic* logic);
gchar*        ll_wff_sprint_formula             (LlWFF         *wff,
                                                 LlLogic       *logic);
gchar*        ll_wff_sprint_formula_values      (LlWFF         *wff,
                                                 LlLogic       *logic);

#endif /* __LOGICS_H__ */
