#if defined(PTB_USE_WAYLAND_PRESENT) || defined(PTB_USE_WAYLAND)

/* 
 * Copyright © 2013-2014 Collabora, Ltd.
 * 
 * Permission to use, copy, modify, distribute, and sell this
 * software and its documentation for any purpose is hereby granted
 * without fee, provided that the above copyright notice appear in
 * all copies and that both that copyright notice and this permission
 * notice appear in supporting documentation, and that the name of
 * the copyright holders not be used in advertising or publicity
 * pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.
 * 
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

#include <stdlib.h>
#include <stdint.h>
#include "wayland-util.h"

extern const struct wl_interface presentation_feedback_interface;
extern const struct wl_interface wl_output_interface;
extern const struct wl_interface wl_surface_interface;

static const struct wl_interface *types[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&wl_surface_interface,
	&presentation_feedback_interface,
	&wl_output_interface,
};

static const struct wl_message presentation_requests[] = {
	{ "destroy", "", types + 0 },
	{ "feedback", "on", types + 7 },
};

static const struct wl_message presentation_events[] = {
	{ "clock_id", "u", types + 0 },
};

WL_EXPORT const struct wl_interface presentation_interface = {
	"presentation", 1,
	2, presentation_requests,
	1, presentation_events,
};

static const struct wl_message presentation_feedback_events[] = {
	{ "sync_output", "o", types + 9 },
	{ "presented", "uuuuuuu", types + 0 },
	{ "discarded", "", types + 0 },
};

WL_EXPORT const struct wl_interface presentation_feedback_interface = {
	"presentation_feedback", 1,
	0, NULL,
	3, presentation_feedback_events,
};

#endif
