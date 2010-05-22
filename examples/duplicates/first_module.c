#include <stdio.h>
#include <dotconf.h>

static DOTCONF_CB(cb_newoption)
{
	printf("[FIRST_MODULE] Line %ld: Option=%s str=%s\n",
	       cmd->configfile->line, cmd->name, cmd->data.str);
	return NULL;
}

configoption_t options[] = {
	{"DupOption", ARG_STR, cb_newoption, 0, 0}
	,
	LAST_OPTION
};
