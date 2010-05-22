
/* You need at least dot.conf 1.0.9 to compile and run this example !! */

#include <stdio.h>		/* fprintf(), stderr       */
#include <stdlib.h>		/* realloc()               */
#include <string.h>		/* strcmp()                */
/* this example does not work for WIN32 */

#ifndef WIN32
#include <dlfcn.h>		/* dlopen(), dlerror(), dlclose() */
#include <unistd.h>
#endif

#include <libpool.h>
#include <dotconf.h>

struct config_context {
	long current_context;
	char current_end_token[1024];

	pool_t *pool;
};

struct ptr_list {
	int n_entries;
	void **entries;
};

static DOTCONF_CB(section_open);
static DOTCONF_CB(section_close);
static DOTCONF_CB(common_option);
static DOTCONF_CB(addmodule_option);

/*
// the last field is used to specify the context needed for
// each option. These will be checked at runtime by our contextchecker
//
*/
static const configoption_t options[] = {
	{"AddModule", ARG_LIST, addmodule_option, NULL, 0},
	{"ToggleOption", ARG_TOGGLE, common_option, NULL, 0},
	LAST_OPTION
};

/*
// The pointer list of dynamic options.
// This is necessary to be able to free it up later.
//
*/
struct ptr_list memory_junk;

#define MAX_MODULES       10
static void *handles[MAX_MODULES];	/* handles of dynamically loaded modules */

const char *context_checker(command_t * cmd, unsigned long option_context)
{
	struct config_context *context = cmd->context;

	if (context->current_context != option_context) {
		return pool_strcat(context->pool, "Option '", cmd->name,
				   "' not allowed in <",
				   (strlen(context->current_end_token) >
				    2) ? context->current_end_token +
				   2 : "global>", " context", NULL);
	}

	return NULL;
}

FUNC_ERRORHANDLER(error_handler)
{
	fprintf(stderr, "%s:%ld:[error] %s\n",
		configfile->filename, configfile->line, msg);

	/* continue reading the configfile ; return 1 stop after first error found */
	return 0;
}

int main(int argc, char *argv[])
{
	configfile_t *configfile;
	struct config_context context;

	if (argc < 2) {
		fprintf(stderr, "Usage : %s <configfile>\n", argv[0]);
		return 1;
	}
	context.current_end_token[0] = '\0';
	context.current_context = 0;
	context.pool = pool_new(NULL);

	memory_junk.n_entries = 0;
	memory_junk.entries = 0;

	memset(handles, 0, sizeof(handles));

	configfile = dotconf_create(argv[1], options, (void *)&context,
				    CASE_INSENSITIVE | DUPLICATE_OPTION_NAMES);
	if (!configfile) {
		fprintf(stderr, "Error opening configuration file\n");
		return 1;
	}
	configfile->errorhandler = (dotconf_errorhandler_t) error_handler;
	configfile->contextchecker = (dotconf_contextchecker_t) context_checker;

	if (dotconf_command_loop(configfile) == 0) {
		fprintf(stderr, "Error reading configuration file\n");
		return 1;
	}

	dotconf_cleanup(configfile);
	pool_free(context.pool);

	/* clean up the possible memjunk which needed to stay in memory */
	if (memory_junk.n_entries) {
		int idx;
		for (idx = 0; idx < memory_junk.n_entries; idx++) {
			free(memory_junk.entries[idx]);
		}
	}
	free(memory_junk.entries);

	return 0;
}

DOTCONF_CB(section_open)
{
	struct config_context *context = (struct config_context *)ctx;
	const char *old_end_token = context->current_end_token;
	int prev_context = context->current_context;
	const char *err = 0;

	context->current_context = (long)cmd->option->info;
	sprintf(context->current_end_token, "</%s", cmd->name + 1);

	while (!cmd->configfile->eof) {
		err = dotconf_command_loop_until_error(cmd->configfile);
		if (!err) {
			err =
			    pool_strcat(context->pool, "</", cmd->name + 1,
					" is missing", NULL);
			break;
		}

		if (err == context->current_end_token) {
			break;
		}
		dotconf_warning(cmd->configfile, DCLOG_ERR, 0, err);
	}

	sprintf(context->current_end_token, "%s", old_end_token);
	context->current_context = prev_context;

	if (err != context->current_end_token) {
		return err;
	}

	return NULL;
}

DOTCONF_CB(section_close)
{
	struct config_context *context = (struct config_context *)ctx;

	if (!context->current_end_token) {
		return pool_strcat(context->pool, cmd->name,
				   " without matching <", cmd->name + 2,
				   " section", NULL);
	}

	if (strcmp(context->current_end_token, cmd->name) != 0) {
		return pool_strcat(context->pool, "Expected '",
				   context->current_end_token, "' but saw ",
				   cmd->name, NULL);
	}

	return context->current_end_token;
}

DOTCONF_CB(common_option)
{
	fprintf(stderr,
		"common_option : Option %s called  Not doing anything with it...\n",
		cmd->name);
	return NULL;
}

/*
// We expect  option     name  filename
//      e.g.  AddModule first ./plugins/decision-test.so
//
// So in the list :
//  0 -> name
//  1 -> so_filename
*/
DOTCONF_CB(addmodule_option)
{
	struct config_context *context = (struct config_context *)ctx;
	configoption_t *module_options;
	const char *error = 0;
	int handle_idx = -1;
	char filename[FILENAME_MAX] = "";
	void *shared_object = 0;

	fprintf(stderr, "addmodule_option : Option %s called\n", cmd->name);
	if (cmd->arg_count < 2) {
		return pool_strcat(context->pool,
				   "Not enough parameters to option ",
				   cmd->name, " expected 2", NULL);
	}
	// load the damn thing
	for (handle_idx = 0; handle_idx < MAX_MODULES; handle_idx++) {
		if (handles[handle_idx] == 0) {
			snprintf(filename, 128, "./%s.so", cmd->data.list[1]);
			if (access(filename, R_OK) == 0) {	/* if file access is permitted */
				/* load library */
				handles[handle_idx] =
				    dlopen(filename, RTLD_LAZY);
				if (!handles[handle_idx]) {
					fprintf(stderr,
						"Error opening library: %s\n",
						dlerror());
					return "Error opening library";
				}
				shared_object = handles[handle_idx];
				break;
			} else {
				return pool_strcat(context->pool,
						   "Can't open file ", filename,
						   NULL);
			}
		}
	}
	if (handle_idx == MAX_MODULES) {
		return "Out of handle space. Not loading module\n";
	}
	// get the options
	module_options = dlsym(shared_object, "options");
	error = dlerror();
	if (error) {
		fprintf(stderr,
			"addmodule_option() : Error finding the options variable in %s p=%p (%s)\n",
			cmd->data.list[1], shared_object, error);
		dlclose(shared_object);
		handles[handle_idx] = 0;
		return NULL;
	}

	/*
	   // Scope the options of this module to a <NAME></NAME> block where NAME is
	   // the name that was specified in the configfile.
	   //
	   // The context field holds a unique identifier so we can verify in our
	   // contextchecker that this option belongs to the right scope.
	   //
	 */
	{
		char *begin_context_tag =
		    (char *)malloc(strlen(cmd->data.list[1]) + 2 + 1);
		char *end_context_tag =
		    (char *)malloc(strlen(cmd->data.list[1]) + 3 + 1);
		configoption_t *scope_options = 0;
		int opt_idx = -1;

		scope_options =
		    (configoption_t *) malloc(3 * sizeof(configoption_t));
		if (!scope_options || !begin_context_tag || !end_context_tag) {
			return "Error allocating memory";
		}
		sprintf(begin_context_tag, "<%s>", cmd->data.list[0]);
		sprintf(end_context_tag, "</%s>", cmd->data.list[0]);

		// create our two extra options (scope begin/end) and a NULL option to close
		// the list
		scope_options[0].name = begin_context_tag;
		scope_options[0].type = ARG_NONE;
		scope_options[0].callback = section_open;
		scope_options[0].info = shared_object;
		scope_options[0].context = CTX_ALL;

		scope_options[1].name = end_context_tag;
		scope_options[1].type = ARG_NONE;
		scope_options[1].callback = section_close;
		scope_options[1].info = NULL;
		scope_options[1].context = (long)shared_object;

		scope_options[2].name = "";
		scope_options[2].type = 0;
		scope_options[2].callback = NULL;
		scope_options[2].info = NULL;
		scope_options[2].context = 0;

		/* Set the context field of all options from the module to the identifier */
		for (opt_idx = 0; strlen(module_options[opt_idx].name);
		     opt_idx++) {
			module_options[opt_idx].context = (long)shared_object;
		}

		memory_junk.entries = realloc(memory_junk.entries,
					      (memory_junk.n_entries +
					       3) * sizeof(void *));
		memory_junk.entries[memory_junk.n_entries++] =
		    begin_context_tag;
		memory_junk.entries[memory_junk.n_entries++] = end_context_tag;
		memory_junk.entries[memory_junk.n_entries++] = scope_options;

		dotconf_register_options(cmd->configfile, scope_options);
		dotconf_register_options(cmd->configfile, module_options);
	}

	fprintf(stderr, "Successfully loaded module %s (%s)\n",
		cmd->data.list[1], cmd->data.list[1]);

	return NULL;
}
