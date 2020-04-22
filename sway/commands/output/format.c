#include <string.h>
#include "log.h"
#include "sway/commands.h"
#include "sway/config.h"
#include "sway/output.h"

struct cmd_results *output_cmd_format(int argc, char **argv) {
	if (!config->handler_context.output_config) {
		return cmd_results_new(CMD_FAILURE, "Missing output config");
	}
	if (!argc) {
		return cmd_results_new(CMD_INVALID, "Missing format argument.");
	}
	enum wl_shm_format format;

	if (strcmp(*argv, "8bit") == 0) {
		format = WL_SHM_FORMAT_ARGB8888;
	} else if (strcmp(*argv, "10bit") == 0) {
		format = WL_SHM_FORMAT_XRGB2101010;
	} else {
		return cmd_results_new(CMD_INVALID, "Invalid output format.");
	}

	struct output_config *oc = config->handler_context.output_config;
	config->handler_context.leftovers.argc = argc - 1;
	config->handler_context.leftovers.argv = argv + 1;

	oc->format = format;
	return NULL;
}
