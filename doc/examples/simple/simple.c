#include <stdio.h>
#include <string.h>
#include <dotconf.h>

static DOTCONF_CB(cb_list);
static DOTCONF_CB(cb_str);

static const configoption_t options[] = {
	{"ExampleStr", ARG_STR, cb_str, NULL, CTX_ALL},
	{"ExampleList", ARG_LIST, cb_list, NULL, CTX_ALL},
	LAST_OPTION
};

int main(int argc, char **argv)
{
	configfile_t *configfile;

	configfile = dotconf_create(argv[1] ? argv[1] : "simple.conf",
				    options, NULL, CASE_INSENSITIVE);
	if (!configfile) {
		fprintf(stderr, "Error opening config file\n");
		return 1;
	}

	if (dotconf_command_loop(configfile) == 0)
		fprintf(stderr, "Error reading config file\n");

	dotconf_cleanup(configfile);

	return 0;
}

DOTCONF_CB(cb_list)
{
	int i;
	printf("%s:%ld: %s: [  ",
	       cmd->configfile->filename, cmd->configfile->line, cmd->name);
	for (i = 0; i < cmd->arg_count; i++)
		printf("(%d) %s  ", i, cmd->data.list[i]);
	printf("]\n");
	return NULL;
}

DOTCONF_CB(cb_str)
{
	printf("%s:%ld: %s: [%s]\n",
	       cmd->configfile->filename, cmd->configfile->line,
	       cmd->name, cmd->data.str);
	return NULL;
}

/*
  vim:set ts=4:
  vim:set shiftwidth=4:
*/
