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

#ifndef PRESENTATION_TIMING_CLIENT_PROTOCOL_H
#define PRESENTATION_TIMING_CLIENT_PROTOCOL_H

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "wayland-client.h"

struct wl_client;
struct wl_resource;

struct presentation;
struct presentation_feedback;

extern const struct wl_interface presentation_interface;
extern const struct wl_interface presentation_feedback_interface;

#ifndef PRESENTATION_ERROR_ENUM
#define PRESENTATION_ERROR_ENUM
/**
 * presentation_error - fatal presentation errors
 * @PRESENTATION_ERROR_INVALID_TIMESTAMP: invalid value in tv_nsec
 * @PRESENTATION_ERROR_INVALID_FLAG: invalid flag
 *
 * These fatal protocol errors may be emitted in response to illegal
 * presentation requests.
 */
enum presentation_error {
	PRESENTATION_ERROR_INVALID_TIMESTAMP = 0,
	PRESENTATION_ERROR_INVALID_FLAG = 1,
};
#endif /* PRESENTATION_ERROR_ENUM */

/**
 * presentation - timed presentation related wl_surface requests
 * @clock_id: clock ID for timestamps
 *
 * 
 *
 * The main feature of this interface is accurate presentation timing
 * feedback to ensure smooth video playback while maintaining audio/video
 * synchronization. Some features use the concept of a presentation clock,
 * which is defined in presentation.clock_id event.
 *
 * Request 'feedback' can be regarded as an additional wl_surface method.
 * It is part of the double-buffered surface state update mechanism, where
 * other requests first set up the state and then wl_surface.commit
 * atomically applies the state into use. In other words, wl_surface.commit
 * submits a content update.
 *
 * When the final realized presentation time is available, e.g. after a
 * framebuffer flip completes, the requested
 * presentation_feedback.presented events are sent. The final presentation
 * time can differ from the compositor's predicted display update time and
 * the update's target time, especially when the compositor misses its
 * target vertical blanking period.
 */
struct presentation_listener {
	/**
	 * clock_id - clock ID for timestamps
	 * @clk_id: platform clock identifier
	 *
	 * This event tells the client in which clock domain the
	 * compositor interprets the timestamps used by the presentation
	 * extension. This clock is called the presentation clock.
	 *
	 * The compositor sends this event when the client binds to the
	 * presentation interface. The presentation clock does not change
	 * during the lifetime of the client connection.
	 *
	 * The clock identifier is platform dependent. Clients must be able
	 * to query the current clock value directly, not by asking the
	 * compositor.
	 *
	 * On Linux/glibc, the identifier value is one of the clockid_t
	 * values accepted by clock_gettime(). clock_gettime() is defined
	 * by POSIX.1-2001.
	 *
	 * Compositors should prefer a clock which does not jump and is not
	 * slewed e.g. by NTP. The absolute value of the clock is
	 * irrelevant. Precision of one millisecond or better is
	 * recommended.
	 *
	 * Timestamps in this clock domain are expressed as tv_sec_hi,
	 * tv_sec_lo, tv_nsec triples, each component being an unsigned
	 * 32-bit value. Whole seconds are in tv_sec which is a 64-bit
	 * value combined from tv_sec_hi and tv_sec_lo, and the additional
	 * fractional part in tv_nsec as nanoseconds. Hence, for valid
	 * timestamps tv_nsec must be in [0, 999999999].
	 *
	 * Note that clock_id applies only to the presentation clock, and
	 * implies nothing about e.g. the timestamps used in the Wayland
	 * core protocol input events.
	 */
	void (*clock_id)(void *data,
			 struct presentation *presentation,
			 uint32_t clk_id);
};

static inline int
presentation_add_listener(struct presentation *presentation,
			  const struct presentation_listener *listener, void *data)
{
	return wl_proxy_add_listener((struct wl_proxy *) presentation,
				     (void (**)(void)) listener, data);
}

#define PRESENTATION_DESTROY	0
#define PRESENTATION_FEEDBACK	1

static inline void
presentation_set_user_data(struct presentation *presentation, void *user_data)
{
	wl_proxy_set_user_data((struct wl_proxy *) presentation, user_data);
}

static inline void *
presentation_get_user_data(struct presentation *presentation)
{
	return wl_proxy_get_user_data((struct wl_proxy *) presentation);
}

static inline void
presentation_destroy(struct presentation *presentation)
{
	wl_proxy_marshal((struct wl_proxy *) presentation,
			 PRESENTATION_DESTROY);

	wl_proxy_destroy((struct wl_proxy *) presentation);
}

static inline struct presentation_feedback *
presentation_feedback(struct presentation *presentation, struct wl_surface *surface)
{
	struct wl_proxy *callback;

	callback = wl_proxy_marshal_constructor((struct wl_proxy *) presentation,
			 PRESENTATION_FEEDBACK, &presentation_feedback_interface, surface, NULL);

	return (struct presentation_feedback *) callback;
}

#ifndef PRESENTATION_FEEDBACK_KIND_ENUM
#define PRESENTATION_FEEDBACK_KIND_ENUM
/**
 * presentation_feedback_kind - bitmask of flags in presented event
 * @PRESENTATION_FEEDBACK_KIND_VSYNC: presentation was vsync'd
 * @PRESENTATION_FEEDBACK_KIND_HW_CLOCK: hardware provided the
 *	presentation timestamp
 * @PRESENTATION_FEEDBACK_KIND_HW_COMPLETION: hardware signalled the
 *	start of the presentation
 * @PRESENTATION_FEEDBACK_KIND_ZERO_COPY: presentation was done zero-copy
 *
 * These flags provide information about how the presentation of the
 * related content update was done. The intent is to help clients assess
 * the reliability of the feedback and the visual quality with respect to
 * possible tearing and timings. The flags are:
 *
 * VSYNC: The presentation was synchronized to the "vertical retrace" by
 * the display hardware such that tearing does not happen. Relying on user
 * space scheduling is not acceptable for this flag. If presentation is
 * done by a copy to the active frontbuffer, then it must guarantee that
 * tearing cannot happen.
 *
 * HW_CLOCK: The display hardware provided measurements that the hardware
 * driver converted into a presentation timestamp. Sampling a clock in user
 * space is not acceptable for this flag.
 *
 * HW_COMPLETION: The display hardware signalled that it started using the
 * new image content. The opposite of this is e.g. a timer being used to
 * guess when the display hardware has switched to the new image content.
 *
 * ZERO_COPY: The presentation of this update was done zero-copy. This
 * means the buffer from the client was given to display hardware as is,
 * without copying it. Compositing with OpenGL counts as copying, even if
 * textured directly from the client buffer. Possible zero-copy cases
 * include direct scanout of a fullscreen surface and a surface on a
 * hardware overlay.
 */
enum presentation_feedback_kind {
	PRESENTATION_FEEDBACK_KIND_VSYNC = 0x1,
	PRESENTATION_FEEDBACK_KIND_HW_CLOCK = 0x2,
	PRESENTATION_FEEDBACK_KIND_HW_COMPLETION = 0x4,
	PRESENTATION_FEEDBACK_KIND_ZERO_COPY = 0x8,
};
#endif /* PRESENTATION_FEEDBACK_KIND_ENUM */

/**
 * presentation_feedback - presentation time feedback event
 * @sync_output: presentation synchronized to this output
 * @presented: the content update was displayed
 * @discarded: the content update was not displayed
 *
 * A presentation_feedback object returns an indication that a wl_surface
 * content update has become visible to the user. One object corresponds to
 * one content update submission (wl_surface.commit). There are two
 * possible outcomes: the content update is presented to the user, and a
 * presentation timestamp delivered; or, the user did not see the content
 * update because it was superseded or its surface destroyed, and the
 * content update is discarded.
 *
 * Once a presentation_feedback object has delivered an 'presented' or
 * 'discarded' event it is automatically destroyed.
 */
struct presentation_feedback_listener {
	/**
	 * sync_output - presentation synchronized to this output
	 * @output: presentation output
	 *
	 * As presentation can be synchronized to only one output at a
	 * time, this event tells which output it was. This event is only
	 * sent prior to the presented event.
	 *
	 * As clients may bind to the same global wl_output multiple times,
	 * this event is sent for each bound instance that matches the
	 * synchronized output. If a client has not bound to the right
	 * wl_output global at all, this event is not sent.
	 */
	void (*sync_output)(void *data,
			    struct presentation_feedback *presentation_feedback,
			    struct wl_output *output);
	/**
	 * presented - the content update was displayed
	 * @tv_sec_hi: high 32 bits of the seconds part of the
	 *	presentation timestamp
	 * @tv_sec_lo: low 32 bits of the seconds part of the
	 *	presentation timestamp
	 * @tv_nsec: nanoseconds part of the presentation timestamp
	 * @refresh: nanoseconds till next refresh
	 * @seq_hi: high 32 bits of refresh counter
	 * @seq_lo: low 32 bits of refresh counter
	 * @flags: combination of 'kind' values
	 *
	 * The associated content update was displayed to the user at the
	 * indicated time (tv_sec_hi/lo, tv_nsec). For the interpretation
	 * of the timestamp, see presentation.clock_id event.
	 *
	 * The timestamp corresponds to the time when the content update
	 * turned into light the first time on the surface's main output.
	 * Compositors may approximate this from the framebuffer flip
	 * completion events from the system, and the latency of the
	 * physical display path if known.
	 *
	 * This event is preceded by all related sync_output events telling
	 * which output's refresh cycle the feedback corresponds to, i.e.
	 * the main output for the surface. Compositors are recommended to
	 * choose the output containing the largest part of the wl_surface,
	 * or keeping the output they previously chose. Having a stable
	 * presentation output association helps clients predict future
	 * output refreshes (vblank).
	 *
	 * Argument 'refresh' gives the compositor's prediction of how many
	 * nanoseconds after tv_sec, tv_nsec the very next output refresh
	 * may occur. This is to further aid clients in predicting future
	 * refreshes, i.e., estimating the timestamps targeting the next
	 * few vblanks. If such prediction cannot usefully be done, the
	 * argument is zero.
	 *
	 * The 64-bit value combined from seq_hi and seq_lo is the value of
	 * the output's vertical retrace counter when the content update
	 * was first scanned out to the display. This value must be
	 * compatible with the definition of MSC in GLX_OML_sync_control
	 * specification. Note, that if the display path has a non-zero
	 * latency, the time instant specified by this counter may differ
	 * from the timestamp's.
	 *
	 * If the output does not have a constant refresh rate, explicit
	 * video mode switches excluded, then the refresh argument must be
	 * zero.
	 *
	 * If the output does not have a concept of vertical retrace or a
	 * refresh cycle, or the output device is self-refreshing without a
	 * way to query the refresh count, then the arguments seq_hi and
	 * seq_lo must be zero.
	 */
	void (*presented)(void *data,
			  struct presentation_feedback *presentation_feedback,
			  uint32_t tv_sec_hi,
			  uint32_t tv_sec_lo,
			  uint32_t tv_nsec,
			  uint32_t refresh,
			  uint32_t seq_hi,
			  uint32_t seq_lo,
			  uint32_t flags);
	/**
	 * discarded - the content update was not displayed
	 *
	 * The content update was never displayed to the user.
	 */
	void (*discarded)(void *data,
			  struct presentation_feedback *presentation_feedback);
};

static inline int
presentation_feedback_add_listener(struct presentation_feedback *presentation_feedback,
				   const struct presentation_feedback_listener *listener, void *data)
{
	return wl_proxy_add_listener((struct wl_proxy *) presentation_feedback,
				     (void (**)(void)) listener, data);
}

static inline void
presentation_feedback_set_user_data(struct presentation_feedback *presentation_feedback, void *user_data)
{
	wl_proxy_set_user_data((struct wl_proxy *) presentation_feedback, user_data);
}

static inline void *
presentation_feedback_get_user_data(struct presentation_feedback *presentation_feedback)
{
	return wl_proxy_get_user_data((struct wl_proxy *) presentation_feedback);
}

static inline void
presentation_feedback_destroy(struct presentation_feedback *presentation_feedback)
{
	wl_proxy_destroy((struct wl_proxy *) presentation_feedback);
}

#ifdef  __cplusplus
}
#endif

#endif
