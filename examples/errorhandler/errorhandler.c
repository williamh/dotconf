#include <stdio.h>
#include <dotconf.h>

/*
        tabsize: 4
        shiftwidth: 4
*/

FUNC_ERRORHANDLER(errorhandler);
DOTCONF_CB(cb_errorhandler);

static configoption_t options[] = {
	{"ErrorHandler", ARG_STR, cb_errorhandler, NULL, 0},
	LAST_OPTION
};

FUNC_ERRORHANDLER(errorhandler)
{
	printf("ERROR %s\n", msg);
	return 0;
}

DOTCONF_CB(cb_errorhandler)
{
	printf("[ErrorHandler Option] %s\n", cmd->data.str);
	return NULL;
}

int main(int argc, char **argv)
{
	configfile_t *configfile;

	configfile = dotconf_create("errorhandler.conf", options, 0, 0);
	configfile->errorhandler = (dotconf_errorhandler_t) errorhandler;

	if (dotconf_command_loop(configfile) == 0)
		fprintf(stderr, "Error reading configfile\n");

	dotconf_cleanup(configfile);
	return 0;
}
