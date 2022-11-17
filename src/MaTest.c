/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * MaTest.c
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

/** @file MaTest.c
 *
 * Este archivo contiene la función principal "main" y el manejador de opciones
 * de línea de comandos.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <gtk/gtk.h>
#include "MaTest.h"


/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif


/*
 * Fución principal main. Define las opciones de línea de comandos, establece
 * los valores por defecto y pasa al modo seleccionado.
 */
int
main (int argc, char *argv[])
{
	int     c,
	        index,
	        initdim = 0,
	        initmdv = 0,
	        option_index = 0;
	bool    use_gui = false,
	        use_tui = false;
	FILE    *mfile;
	char    filename[BUFSIZ];
	Work    *work;
	static struct option long_options[] = {
		{"version",    no_argument,       0, 'v'},
		{"help",       no_argument,       0, 'h'},
		{"dimension",  required_argument, 0, 'd'},
		{"mdv",        required_argument, 0, 'm'},
		{"formula",    required_argument, 0, 'f'},
		{"evaluate",   required_argument, 0, 'e'},
		{"gui",        no_argument,       0, 'g'},
		{"text",       no_argument,       0, 't'},
		{"batch",      no_argument,       0, 'b'},
		{0, 0, 0, 0}
	};

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	/* Definimos los elementos del trabajo inicial. */
	work = g_new0 (Work, 1);
	work->evaluation_style = ALL;  /* Muestra todos los valores por defecto */

	while ((c = getopt_long (argc, argv, "d:m:f:s:tghv", long_options, &option_index)) != -1) {
		switch (c) {
			case 'h':
				menu_usage();
				return 0;
			case 'v':
				menu_version();
				return 0;
			case 'd':
				initdim = atoi (optarg);
				break;
			case 'm':
				initmdv = atoi (optarg);
				break;
			case 'f':
				strcpy (work->formula, optarg);
				break;
			case 'e':
				if (strcmp (optarg, "a") == 0 || strcmp (optarg, "all") == 0)
					work->evaluation_style = ALL;
				else if (strcmp (optarg, "d") == 0 || strcmp (optarg, "designated") == 0)
					work->evaluation_style = DESIGNATED;
				else if (strcmp (optarg, "n") == 0 || strcmp (optarg, "not-designated") == 0)
					work->evaluation_style = NOT_DESIGNATED;
				break;
			case 't':
				use_tui = true;
				break;
			case 'g':
				use_gui = true;
				break;
			case '?':
				if (long_options[option_index].flag != 0)
					break;
				printf ("option %s", long_options[option_index].name);
				if (optarg)
					printf (" with arg %s", optarg);
				printf ("\n");
				break;
		}
	}

	if (optind < argc) {
		mfile = fopen (argv[optind++], "r");
		if (mfile) {
			read_matricesfile (work, mfile);
			fclose (mfile);
			if (use_tui)
				mode_tui (work);
			else
				mode_gui (argc, argv, work);
			return 0;
		}
		else {
			perror ("No se pudo abrir el archivo de matrices.\n");
			return 3;
		}
	}

	if (!work->logic) {
		work->logic = g_new0 (LlLogic, 1);
	}

	/* Establecemos la dimensión de las matrices */
	if (initdim >= 2)
		work->DIM = initdim;
	else
		work->DIM = 2;  /* Dimensión 2 por defecto */

	/* Establecemos el mínimo valor designado */
	if (initmdv > 0 && initmdv <= work->DIM)
		work->MDV = initmdv;
	else
		work->MDV = work->MAXV;  /* Máximo valor del conjunto por defecto */

	/* Definimos las conectivas por defecto */
	ll_logic_set_default_ucons_lukasiewicz (work->logic);
	ll_logic_set_default_bcons_lukasiewicz (work->logic);

	/* Establecemos la fórmula si está bien formada */
	work->wff = NULL;
	if (work->formula[0]) {
		ll_wff_parse_formula (&work->wff, work->formula, work->logic);
		if (!work->wff) {
			work->formula[0] = '\0';  /* si no, la dejamos sin definir */
		}
	}
	/* Pasamos al modo seleccionado, con interfaz gráfica por defecto */
	if (use_gui)
		mode_gui (argc, argv, work);
	else if (use_tui)
		mode_tui (work);
	else
		mode_gui (argc, argv, work);

	return (0);
}
