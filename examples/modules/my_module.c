#include <stdio.h>
#include <dotconf.h>

/* lets get access to the current line inside of the config.file */
/* this only resolves if the executable was link with the -rdynamic flag! */

static DOTCONF_CB(cb_newoption)
{
	printf("[MODULE] Line %ld: NewOption: %s\n",
	       cmd->configfile->line, cmd->data.str);
	return NULL;
}

/* example.c loads the symbol 'new_options' */
const configoption_t new_options[] = {
	{"NewOption", ARG_STR, cb_newoption, 0, 0},
	LAST_OPTION
};
