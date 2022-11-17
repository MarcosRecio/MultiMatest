#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "MaTest.h"


typedef struct _lifo
  {
    char symb;
    struct _lifo *next;
  }
  LIFOType;

typedef LIFOType *nLIFO;
typedef LIFOType *LIFO;


void
push (LIFO *lifo, char ch)
{
  nLIFO new;
  
  /* Crear un nodo nuevo */
  new = (nLIFO) malloc (sizeof (LIFOType));
  new->symb = ch;
  
  /* Añadimos la pila a continuación del nuevo nodo */
  new->next = *lifo;
  /* Ahora, el comienzo de nuestra pila es en nuevo nodo */
  *lifo = new;
}


char
pop (LIFO *lifo)
{
  nLIFO node; /* variable auxiliar para manipular nodo */
  char ch;      /* variable auxiliar para retorno */
  
  /* Nodo apunta al primer elemento de la pila */
  node = *lifo;
  if (!node)
    return 0; /* Si no hay nodos en la pila retornamos 0 */
  /* Asignamos a pila toda la pila menos el primer elemento */
  *lifo = node->next;
  /* Guardamos el valor de retorno */
  ch = node->symb;
  /* Borrar el nodo */
  free (node);
  return ch;
}


LogicSymbKind
symb_kind_std (char symb)
{
  if (islower (symb))
    return VARIABLE;
  else if (isupper (symb))
    {
      /*if (search_UCon (logic->UCons, symb))
        return UNYCON;
      else if (search_BCon (logic->BCons, symb))
        return BINCON;
      else
        return NONE;*/
      if (symb == 'N' || symb == 'L' || symb == 'M')
        return UNYCON;
      else
        return BINCON;
    }
  else if (symb == '(' || symb == '[' || symb == '{')
    return OPENAUX;
  else if (symb == ')' || symb == ']' || symb == '}')
    return CLOSEAUX;
  else
    return NONE;
}


int priority (char symb)
{
  switch (symb_kind_std (symb))
    {
      case OPENAUX :
        return 0;
      case UNYCON :
        return 1;
      case BINCON :
        return 2;
      case CLOSEAUX :
        return 3;
			default:
				return -1;
    }
}


void parse_std (LogicWFF *wff, char formula[])
{
  int i;
  char ch;
  LIFO lifo, aux;
  
  lifo = (LIFO) malloc (sizeof (LIFOType));
  aux = (LIFO) malloc (sizeof (LIFOType));
  
  lifo = aux = NULL;
  
  for (i = (int) strlen (formula) - 1; i >= 0; i--)
    {
      switch (symb_kind_std (formula[i]))
        {
          case VARIABLE :
            push (&lifo, formula[i]);
            break;
          case UNYCON :
          case BINCON :
            while (aux && (priority (aux->symb) <= priority (formula[i])))
              {
                ch = pop (&aux);
                push (&lifo, ch);
              }
            push (&aux, formula[i]);
            break;
          case CLOSEAUX :
            push (&aux, formula[i]);
            break;
          case OPENAUX :
            while (symb_kind_std (aux->symb) != CLOSEAUX)
              {
                ch = pop (&aux);
                push (&lifo, ch);
              }
            ch = pop (&aux);
            break;
					case NONE:
						break;
        }
    }
  
  while (aux)
    {
      ch = pop (&aux);
      push (&lifo, ch);
    }

	del_wff (wff);
  while (lifo)
		{
    	//printf ("%c", pop (&lifo));
			ch = pop (&lifo);
			set_atom (wff, symb_kind_std (ch), ch, NULL);
		}
  //printf ("\n");
}


void print_formula_pn (LogicWFF wff)
{
  if (!wff)
    return;

  printf ("%c", wff->name);
  print_formula_pn (wff->prearg);
  print_formula_pn (wff->postarg);
}


void print_formula_rpn (LogicWFF wff)
{
  if (!wff)
    return;

  print_formula_rpn (wff->prearg);
  print_formula_rpn (wff->postarg);
  printf ("%c", wff->name);
}


void print_formula_std (LogicWFF wff)
{
  if (!wff)
    return;

  if (isupper(wff->name))
    printf ("(");
  print_formula_std (wff->prearg);
  printf ("%c", wff->name);
  print_formula_std (wff->postarg);
  if (isupper(wff->name))
    printf (")");
}

/*
void parse_std (char formula[], LogicWFF *wff, Work work)
{
  int i, j, deep = 0;
  char presubf[BUFSIZ], postsubf[BUFSIZ];

  for (i = 0; i < strlen (formula); i++)
    {
      if (formula[i] == '(')
        deep++;
      else if (formula[i] == ')')
        deep--;
      else
        {
          if (deep <= 0 && isupper (formula[i]))
            {
              set_atom (wff, symbol_type_pn (formula[i], work->logic), formula[i], NULL);
              for (j = 0; j < i; j++)
                presubf[j] = formula[j];
              for (j = i + 1; j < strlen (formula); j++)
                postsubf[j - i - 1] = formula[j];
              printf ("pre: %s\npost: %s\n", presubf, postsubf);
              parse_std (presubf, wff, work);
              parse_std (postsubf, wff, work);
            }
          else if (deep <= 0 && islower (formula[i]))
            set_atom (wff, symbol_kind_pn (formula[i], work->logic), formula[i], NULL);
        }
    }
}
*/

int main ()
{
  char answer[BUFSIZ];
  Work work;
  
  work = (Work) malloc (sizeof (workType));
  work->logic = (Logic) malloc (sizeof (LogicType));
  work->formula_pn[0] = 0;
  work->wff = (LogicWFF) malloc (sizeof (LogicWFFtype));
	work->wff = NULL;
  work->logic->dim = 2;
  /* Define default Minimum Designated Value like in Łukasiewicz model */
  work->logic->mdv = work->logic->dim - 1;

  /* Define the default connectives */
  //set_default_UCons (work->logic);
  //set_default_BCons (work->logic);

  /* Show all evaluated values by default */
  work->eval_values = ALL;

  do {
  printf ("Write a formula: ");
  fgets (answer, BUFSIZ, stdin);
  sscanf (answer, "%s", work->formula_pn);
  /*
  if (work->logic->Vars)
    del_var_list (&work->logic->Vars);
  register_vars (work);
  */
  /*
  if (work->wff)
    {
      del_wff (&work->wff);
      work->wff = NULL;
    }
  */
  /* parse_polish (the_work->formula_pn, &the_work->wff, the_work->logic); */
  /* parse_std (the_work->formula_pn, &the_work->wff, the_work); */
  
  parse_std (&work->wff, work->formula_pn);
  printf (" PN: ");
  print_formula_pn (work->wff);
  printf ("\n RPN: ");
  print_formula_rpn (work->wff);
  printf ("\n std: ");
  print_formula_std (work->wff);
  printf ("\n\n");
  }
  while (work->formula_pn[0]);
  
}

