#include <stdio.h>
#include <dotconf.h>

/*
        tabsize: 4
        shiftwidth: 4
*/

DOTCONF_CB(cb_fallback);

static configoption_t options[] = {
	{"", ARG_NAME, cb_fallback, NULL, 0},
	LAST_OPTION
};

DOTCONF_CB(cb_fallback)
{
	int i;
	printf("Fallback handler called for \"%s\". Got %d args\n", cmd->name,
	       cmd->arg_count);
	for (i = 0; i < cmd->arg_count; i++)
		printf("\tArg #%d: %s\n", i, cmd->data.list[i]);
	return NULL;
}

int main(int argc, char **argv)
{
	configfile_t *configfile;

	configfile = dotconf_create("fallback.conf", options, 0, 0);

	if (dotconf_command_loop(configfile) == 0)
		fprintf(stderr, "Error reading configfile\n");

	dotconf_cleanup(configfile);
	return 0;
}
