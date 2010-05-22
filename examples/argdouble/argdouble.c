#include <stdio.h>
#include <dotconf.h>

/*
	tabsize: 4
	shiftwidth: 4
*/

DOTCONF_CB(cb_argdouble);

static configoption_t options[] = {
	{"ArgDouble", ARG_DOUBLE, cb_argdouble, NULL, 0},
	LAST_OPTION
};

DOTCONF_CB(cb_argdouble)
{
	printf("[ArgDouble] %2.15g\n", cmd->data.dvalue);
	return NULL;
}

int main(int argc, char **argv)
{
	configfile_t *configfile;

	configfile = dotconf_create("argdouble.conf", options, 0, 0);
	if (dotconf_command_loop(configfile) == 0)
		fprintf(stderr, "Error reading config file\n");
	dotconf_cleanup(configfile);
	return 0;
}
