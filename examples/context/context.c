#include <stdio.h>
#include <dotconf.h>

#include <libpool.h>

/* vim:set ts=4:
   vim:set shiftwidth=4:
*/

/*
   this is a simple sample structure that we use to keep track
   of the current context in the configfile when reading it.

   It just has a field for the currently granted permissions.
   Later, when the option-table is defined, we'll specify the
   the needed permissions for each option.
 */
struct mycontext {
	int permissions;
	const char *current_end_token;

	pool_t *pool;
};

enum permissions {
	O_ROOT = 1,
	O_SOMESECTION = 2,
	O_OTHERSECTION = 4,
	O_LAST = 8
};

static DOTCONF_CB(option_SomeSection_open);
static DOTCONF_CB(option_OtherSection_open);
static DOTCONF_CB(common_section_close);
static DOTCONF_CB(common_option);

/*
   the constant Section End tokens. We define these separately
   to be able to reuse pointers. We need this for our context
   'sensitivity' system.
 */
static const char end_SomeSection[] = "</SomeSection>";
static const char end_OtherSection[] = "</OtherSection>";

/*
   the last field is used to specify the permissions needed for
   each option. These will be checked at runtime by our contextchecker
 */
static const configoption_t options[] = {
	{"<SomeSection>", ARG_NONE, option_SomeSection_open, NULL, CTX_ALL},
	{end_SomeSection, ARG_NONE, common_section_close, NULL, O_SOMESECTION},
	{"<OtherSection>", ARG_NONE, option_OtherSection_open, NULL, CTX_ALL},
	{end_OtherSection, ARG_NONE, common_section_close, NULL,
	 O_OTHERSECTION},
	{"RootOption", ARG_NONE, common_option, NULL, CTX_ALL},
	{"SomeOption", ARG_NONE, common_option, NULL, O_SOMESECTION},
	{"OtherOption", ARG_NONE, common_option, NULL, O_OTHERSECTION},
	{"Hybrid", ARG_NONE, common_option, NULL,
	 O_SOMESECTION | O_OTHERSECTION},
	LAST_OPTION
};

const char *context_checker(command_t * cmd, unsigned long mask)
{
	struct mycontext *context = cmd->context;

	/*
	 * this test is quite simple: if the permissions needed for the
	 * to-be-called command are not granted, we'll deny service
	 */

	/* Root Context granted and Root Context given? */
	if (!mask && !context->permissions)
		return NULL;

	if (!(context->permissions & mask)) {
		return pool_strcat(context->pool, "Option '", cmd->name,
				   "' not allowed in <",
				   context->current_end_token ? context->
				   current_end_token + 2 : "global>",
				   " context", NULL);
	}

	return NULL;
}

FUNC_ERRORHANDLER(error_handler)
{
	fprintf(stderr, "[error] %s\n", msg);

	/* continue reading the configfile ; return 1 stop after first error found */
	return 0;
}

int main(void)
{
	configfile_t *configfile;
	struct mycontext context;

	context.current_end_token = 0;
	context.permissions = 0;

	context.pool = pool_new(NULL);
	configfile =
	    dotconf_create("./context.conf", options, (void *)&context,
			   CASE_INSENSITIVE);
	if (!configfile) {
		fprintf(stderr, "Error opening configuration file\n");
		return 1;
	}
	configfile->errorhandler = (dotconf_errorhandler_t) error_handler;
	configfile->contextchecker = (dotconf_contextchecker_t) context_checker;
	if (dotconf_command_loop(configfile) == 0)
		fprintf(stderr, "Error reading configuration file\n");

	dotconf_cleanup(configfile);
	pool_free(context.pool);

	return 0;
}

DOTCONF_CB(common_section_close)
{
	struct mycontext *context = (struct mycontext *)ctx;

	if (!context->current_end_token)
		return pool_strcat(context->pool,
				   cmd->name, " without matching <",
				   cmd->name + 2, " section", NULL);

	if (context->current_end_token != cmd->name)
		return pool_strcat(context->pool, "Expected '",
				   context->current_end_token, "' but saw ",
				   cmd->name, NULL);

	return context->current_end_token;
}

DOTCONF_CB(common_option)
{
	printf("Option %s called\n", cmd->name);
	return NULL;
}

DOTCONF_CB(option_SomeSection_open)
{
	struct mycontext *context = (struct mycontext *)ctx;
	const char *old_end_token = context->current_end_token;
	int old_override = context->permissions;
	const char *err = 0;

	if (context->permissions & O_SOMESECTION)
		return "<SomeSection> cannot be nested";

	context->permissions |= O_SOMESECTION;
	context->current_end_token = end_SomeSection;

	while (!cmd->configfile->eof) {
		err = dotconf_command_loop_until_error(cmd->configfile);
		if (!err) {
			err = "</SomeSection> is missing";
			break;
		}

		if (err == context->current_end_token)
			break;

		dotconf_warning(cmd->configfile, DCLOG_ERR, 0, err);
	}

	context->current_end_token = old_end_token;
	context->permissions = old_override;

	if (err != end_SomeSection)
		return err;

	return NULL;
}

DOTCONF_CB(option_OtherSection_open)
{
	struct mycontext *context = (struct mycontext *)ctx;
	const char *old_end_token = context->current_end_token;
	int old_override = context->permissions;
	const char *err = 0;

	if (context->permissions & O_OTHERSECTION)
		return "<OtherSection> Cannot be nested";

	context->permissions |= O_OTHERSECTION;
	context->current_end_token = end_OtherSection;

	while (!cmd->configfile->eof) {
		err = dotconf_command_loop_until_error(cmd->configfile);
		if (!err) {
			err = "</SomeSection> is missing";
			break;
		}

		if (err == context->current_end_token)
			break;

		dotconf_warning(cmd->configfile, DCLOG_ERR, 0, err);
	}

	context->current_end_token = old_end_token;
	context->permissions = old_override;

	if (err != end_OtherSection)
		return err;

	return NULL;
}
