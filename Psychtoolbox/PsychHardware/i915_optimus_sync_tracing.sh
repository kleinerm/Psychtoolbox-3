#!/bin/bash
#
# Trace execution of Intel-kms function related to kms-pageflips
# which have to be serialized with rendering into dmabuf's, ie.
# have to wait for a dmabuf_fence being signalled on render-completion
# of an external producer into the new dmabuf framebuffer.
#
# Typical use case: Optimus/Enduro/Prime config where the Intel gpu does
# the display and pageflip, and an external offload gpu does the rendering,
# and intel has to wait for render-completion of the offload gpu into
# the dmabuf, e.g. a NVidia dGPU or AMD dGPU.
#
# legacy page flip: intel_mmio_flip_work_func -> reservation_object_timeout_rcu
# (waits for dmabuf_fence aka offload gpu render completion) -> intel_do_mmio_flip
# executes vsync'ed flip at next vblank.
#
# atomic flip:
# intel_prepare_plane_fb -> reservation_object_wait_timeout_rcu (wait for render completion)
# -> intel_pin_and_fence_fb_obj finalize prep work for fb for a future atomic page flip.

#
# Usage:
# 0. chmod a+x ./i915_optimus_sync_tracing.sh
# 1. sudo su
# 2. Start script with ./i915_optimus_sync_tracing.sh
# 3. Run animation on DRI3/Present Optimus/Enduro/Prime config with page flipped swaps.
# 4. Look at script output.

# Go to tracing directory:
cd /sys/kernel/debug/tracing

# Select function_graph tracer:
echo function_graph > current_tracer

# Select driver functions to trace:
#echo nouveau_bo_* *prime* *pin intel_crtc_page_flip > set_ftrace_filter
#echo nouveau* drm* intel* i915* ttm* > set_ftrace_filter
echo intel_mmio_flip_work_func intel_prepare_plane_fb reservation_object_wait_timeout_rcu > set_ftrace_filter
#echo reservation_object_wait_timeout_rcu intel_prepare_plane_fb intel_pin_and_fence_fb_obj > set_ftrace_filter

# Reset trace buffer:
echo 0 > trace

# Start tracing, trace for 20 seconds, Stop tracing:
echo 1 > tracing_on ; sleep 20 ; echo 0 > tracing_on

# Show what you traced:
more trace
# And store it to a file:
cat trace > ~/theTrace.txt
