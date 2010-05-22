#include <stdio.h>
#include <dotconf.h>

static DOTCONF_CB(cb_newoption)
{
	printf("[SECOND_MODULE] Line %ld: Option=%s int=%ld\n",
	       cmd->configfile->line, cmd->name, cmd->data.value);
	return NULL;
}

/* It's important to NOT make this const. That makes duplicate.c SEGV */
configoption_t options[] = {
	{"DupOption", ARG_INT, cb_newoption, 0, 0}
	,
	LAST_OPTION
};
