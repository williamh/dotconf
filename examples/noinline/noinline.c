#include <stdio.h>
#include <dotconf.h>

/*
	tabsize: 4
	shiftwidth: 4
*/

DOTCONF_CB(cb_noinline)
{
	int i;
	printf("[NoInline] Have %d args\n", cmd->arg_count);
	for (i = 0; i < cmd->arg_count; i++)
		printf("\t[NoInline] Arg #%d: %s\n", i, cmd->data.list[i]);
	return NULL;
}

static configoption_t options[] = {
	{"NoInline", ARG_LIST, cb_noinline, NULL, 0},
	LAST_OPTION
};

void readit(int flags)
{
	configfile_t *configfile;

	configfile = dotconf_create("noinline.conf", options, 0, flags);
	if (!dotconf_command_loop(configfile))
		fprintf(stderr, "Error reading config file\n");
	dotconf_cleanup(configfile);
}

int main(int argc, char **argv)
{
	printf("Reading the configuration with NO_INLINE_COMMENTS enabled\n");
	readit(NO_INLINE_COMMENTS);
	printf("\n\n");
	printf("Reading the configuration with NO_INLINE_COMMENTS disabled\n");
	readit(0);
	return 0;
}
