#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <float.h>
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <wayland-server-protocol.h>
#include "log.h"
#include "util.h"

int wrap(int i, int max) {
	return ((i % max) + max) % max;
}

bool parse_color(const char *color, uint32_t *result) {
	if (color[0] == '#') {
		++color;
	}
	int len = strlen(color);
	if ((len != 6 && len != 8) || !isxdigit(color[0]) || !isxdigit(color[1])) {
		return false;
	}
	char *ptr;
	uint32_t parsed = strtoul(color, &ptr, 16);
	if (*ptr != '\0') {
		return false;
	}
	*result = len == 6 ? ((parsed << 8) | 0xFF) : parsed;
	return true;
}

void color_to_rgba(float dest[static 4], uint32_t color) {
	dest[0] = ((color >> 24) & 0xff) / 255.0;
	dest[1] = ((color >> 16) & 0xff) / 255.0;
	dest[2] = ((color >> 8) & 0xff) / 255.0;
	dest[3] = (color & 0xff) / 255.0;
}

bool parse_boolean(const char *boolean, bool current) {
	if (strcasecmp(boolean, "1") == 0
			|| strcasecmp(boolean, "yes") == 0
			|| strcasecmp(boolean, "on") == 0
			|| strcasecmp(boolean, "true") == 0
			|| strcasecmp(boolean, "enable") == 0
			|| strcasecmp(boolean, "enabled") == 0
			|| strcasecmp(boolean, "active") == 0) {
		return true;
	} else if (strcasecmp(boolean, "toggle") == 0) {
		return !current;
	}
	// All other values are false to match i3
	return false;
}

float parse_float(const char *value) {
	errno = 0;
	char *end;
	float flt = strtof(value, &end);
	if (*end || errno) {
		sway_log(SWAY_DEBUG, "Invalid float value '%s', defaulting to NAN", value);
		return NAN;
	}
	return flt;
}


const char *sway_wl_output_subpixel_to_string(enum wl_output_subpixel subpixel) {
	switch (subpixel) {
	case WL_OUTPUT_SUBPIXEL_UNKNOWN:
		return "unknown";
	case WL_OUTPUT_SUBPIXEL_NONE:
		return "none";
	case WL_OUTPUT_SUBPIXEL_HORIZONTAL_RGB:
		return "rgb";
	case WL_OUTPUT_SUBPIXEL_HORIZONTAL_BGR:
		return "bgr";
	case WL_OUTPUT_SUBPIXEL_VERTICAL_RGB:
		return "vrgb";
	case WL_OUTPUT_SUBPIXEL_VERTICAL_BGR:
		return "vbgr";
	}
	sway_assert(false, "Unknown value for wl_output_subpixel.");
	return NULL;
}

const char *sway_wl_output_format_to_string(enum wl_shm_format format) {
	switch (format) {
	case WL_SHM_FORMAT_ARGB8888:
		return "32-bit ARGB format, [31:0] A:R:G:B 8:8:8:8 little endian";
	case WL_SHM_FORMAT_XRGB8888:
		return "32-bit RGB format, [31:0] x:R:G:B 8:8:8:8 little endian";
	case WL_SHM_FORMAT_C8:
		return "8-bit color index format, [7:0] C";
	case WL_SHM_FORMAT_RGB332:
		return "8-bit RGB format, [7:0] R:G:B 3:3:2";
	case WL_SHM_FORMAT_BGR233:
		return "8-bit BGR format, [7:0] B:G:R 2:3:3";
	case WL_SHM_FORMAT_XRGB4444:
		return "16-bit xRGB format, [15:0] x:R:G:B 4:4:4:4 little endian";
	case WL_SHM_FORMAT_XBGR4444:
		return "16-bit xBGR format, [15:0] x:B:G:R 4:4:4:4 little endian";
	case WL_SHM_FORMAT_RGBX4444:
		return "16-bit RGBx format, [15:0] R:G:B:x 4:4:4:4 little endian";
	case WL_SHM_FORMAT_BGRX4444:
		return "16-bit BGRx format, [15:0] B:G:R:x 4:4:4:4 little endian";
	case WL_SHM_FORMAT_ARGB4444:
		return "16-bit ARGB format, [15:0] A:R:G:B 4:4:4:4 little endian";
	case WL_SHM_FORMAT_ABGR4444:
		return "16-bit ABGR format, [15:0] A:B:G:R 4:4:4:4 little endian";
	case WL_SHM_FORMAT_RGBA4444:
		return "16-bit RBGA format, [15:0] R:G:B:A 4:4:4:4 little endian";

	case WL_SHM_FORMAT_XRGB2101010:
		return "10bit";
	default:
		return "other";
	}
	sway_assert(false, "Unknown value for wl_shm_format.");
	return NULL;
}

bool sway_set_cloexec(int fd, bool cloexec) {
	int flags = fcntl(fd, F_GETFD);
	if (flags == -1) {
		sway_log_errno(SWAY_ERROR, "fcntl failed");
		return false;
	}
	if (cloexec) {
		flags = flags | FD_CLOEXEC;
	} else {
		flags = flags & ~FD_CLOEXEC;
	}
	if (fcntl(fd, F_SETFD, flags) == -1) {
		sway_log_errno(SWAY_ERROR, "fcntl failed");
		return false;
	}
	return true;
}
