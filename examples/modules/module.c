#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* this example does not work for WIN32 */

#ifndef WIN32
#include <dlfcn.h>		/* for dlopen(), dlerror(), dlclose() */
#include <unistd.h>
#endif

#include <dotconf.h>

/* define our callbacks; */
/* for definition of DOTCONF_CB see dotconf.h */

static DOTCONF_CB(cb_example);
static DOTCONF_CB(cb_moreargs);
static DOTCONF_CB(cb_addmodule);
static DOTCONF_CB(cb_multiline);	/* here document test */
static DOTCONF_CB(cb_unknown);	/* fallback for unknwon options */

/* define the Option ExampleOption that expects a string
 * and tell the parser to call the function cb_example
 */
static const configoption_t options[] = {
	{"ExampleOption", ARG_STR, cb_example, 0, 0},
	{"MoreArgs", ARG_LIST, cb_moreargs, 0, 0},
	{"AddModule", ARG_STR, cb_addmodule, 0, 0},
	{"MultiLineRaw", ARG_STR, cb_multiline, 0, 0},
	{"", ARG_NAME, cb_unknown, 0, 0},
	LAST_OPTION
};

#define NUM_MODULES 10
static void *handles[NUM_MODULES];	/* handles of dynamically loaded modules */

int main(int argc, char **argv)
{
	int i;
	configfile_t *configfile;

	memset(handles, 0, sizeof(handles));
	putenv("TESTUSER=lukas");

	/*
	 * start reading config, CASE_INSENSITIVE (specify NONE or 0 for the
	 * the default behaviour which matches case sensitive
	 */
	configfile = dotconf_create(argv[1] ? argv[1] : "sample.conf",
				    options, 0, CASE_INSENSITIVE);
	if (!configfile) {
		fprintf(stderr, "Error opening config file\n");
		return 1;
	}

	if (dotconf_command_loop(configfile) == 0)
		fprintf(stderr, "Error reading config file\n");

	dotconf_cleanup(configfile);

	for (i = 0; i < NUM_MODULES && handles[i]; i++)
		dlclose(handles[i]);

	return 0;
}

/* the error-handler; a new feature of v0.7.0 to filter out messages
   issued using config_warning
FUNC_ERRORHANDLER(my_errorhandler)
{
  printf("ERROR [type=%d][%d] %s\n", type, dc_errno, msg);
}
*/

/* declare our callback function */
DOTCONF_CB(cb_example)
{
	printf("%s:%ld: ExampleOption: [arg=%s]\n", cmd->configfile->filename,
	       cmd->configfile->line, cmd->data.str);
	return NULL;
}

/*
 * we dont need the userdata, so dont mention it
 * otherwise we should've appended it as argument 3
 */
DOTCONF_CB(cb_moreargs)
{
	int i;
	for (i = 0; i < cmd->arg_count; i++)
		printf("%s:%ld: [MoreArgs] Arg #%d '%s'\n",
		       cmd->configfile->filename, cmd->configfile->line, i + 1,
		       cmd->data.list[i]);
	return 0;
}

DOTCONF_CB(cb_addmodule)
{
	int i;
	char filename[128];	/* filename of modules */

	for (i = 0; (i < NUM_MODULES) && (handles[i] != 0); i++) ;

	snprintf(filename, 128, "./%s.so", cmd->data.str);

	if (!access(filename, R_OK)) {	/* if file access is permitted */
		/* load library */
		handles[i] = dlopen(filename, RTLD_LAZY);
		if (!handles[i])
			printf("Error opening library: %s\n", dlerror());
		dotconf_register_options(cmd->configfile,
					 dlsym(handles[i], "new_options"));
	}
	printf("Module %s successfully loaded\n", cmd->data.str);
	return NULL;
}

DOTCONF_CB(cb_multiline)
{
	printf("%s:%ld: [MultiLine - START] -%s- [MultiLine - END]\n",
	       cmd->configfile->filename, cmd->configfile->line, cmd->data.str);
	return NULL;
}

DOTCONF_CB(cb_unknown)
{
	int i = 0;

	printf("%s:%ld: UNKNOWN [%s]",
	       cmd->configfile->filename, cmd->configfile->line, cmd->name);

	for (i = 0; cmd->data.list[i]; i++)
		printf(", %s", cmd->data.list[i]);
	printf("\n");
	return NULL;
}
