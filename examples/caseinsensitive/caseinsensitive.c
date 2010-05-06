#include <stdio.h>
#include <dotconf.h>

/*
	tabsize: 4
	shiftwidth: 4
*/

DOTCONF_CB(cb_strncasecmp);

static configoption_t options[] = {
	{"StrNCAseCMp", ARG_STR, cb_strncasecmp, NULL, 0},
	LAST_OPTION
};

DOTCONF_CB(cb_strncasecmp)
{
	printf("[StrNCAseCMp] %s\n", cmd->data.str);
	return NULL;
}

int main(int argc, char **argv)
{
	configfile_t *configfile;

	printf("Reading the configuration with CASE_INSENSITIVE enabled\n");

	configfile =
	    dotconf_create("caseinsensitive.conf", options, 0,
			   CASE_INSENSITIVE);
	if (dotconf_command_loop(configfile) == 0)
		fprintf(stderr, "Error reading config file\n");
	dotconf_cleanup(configfile);
	return 0;
}
