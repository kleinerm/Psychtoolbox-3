/* monadometricsparser.C - Experimental parser for Monado metrics files.
 *
 * This program reads messages from a file, parses and prints them:
 *
 * 0. gcc -I. *.c *.C -o monadometricsparser
 * 1. mkfifo /tmp/monado.protobuf
 * 2. rm /run/user/1000/monado_comp_ipc; XRT_METRICS_FILE=/tmp/monado.protobuf XRT_COMPOSITOR_FORCE_GPU_INDEX=1 monado-service
 * 3. ./monadometricsparser /tmp/monado.protobuf
 *
 * This code is originally based on the Nanopb simple example from
 * https://github.com/nanopb/nanopb/tree/master/examples/simple
 *
 * Copyright of the original example is in the LICENSE_nanopb.txt file.
 *
 * The code derived here for higly experimental use with Psychtoolbox is
 * Copyright 2023 Mario Kleiner, licensed under MIT license.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <pb_decode.h>
#include <pb_common.h>
#include "monado_metrics.pb.h"

/* This function reads manually the first tag from the stream and finds the
 * corresponding message type. It doesn't yet decode the actual message.
 *
 * Returns a pointer to the MsgType_fields array, as an identifier for the
 * message type. Returns null if the tag is of unknown type or an error occurs.
 */
static const pb_msgdesc_t* decode_unionmessage_type(pb_istream_t *stream)
{
    pb_wire_type_t wire_type;
    uint32_t tag;
    bool eof;

    while (pb_decode_tag(stream, &wire_type, &tag, &eof)) {
        if (wire_type == PB_WT_STRING) {
            pb_field_iter_t iter;
            if (pb_field_iter_begin(&iter, monado_metrics_Record_fields, NULL) &&
                pb_field_iter_find(&iter, tag))
            {
                /* Found our field. */
                return iter.submsg_desc;
            }
        }

        /* Wasn't our field.. */
        if (pb_skip_field(stream, wire_type))
            printf("SKIPPED %d\n", wire_type);
        else
            printf("FAILSKIPPED %d\n", wire_type);
    }

    return NULL;
}

static bool decode_unionmessage_contents(pb_istream_t *stream, const pb_msgdesc_t *messagetype, void *dest_struct)
{
    pb_istream_t substream;
    bool status;

    if (!pb_make_string_substream(stream, &substream))
        return false;

    status = pb_decode(&substream, messagetype, dest_struct);
    pb_close_string_substream(stream, &substream);

    return status;
}

static bool callback(pb_istream_t *stream, uint8_t *buf, size_t count)
{
    FILE *file = (FILE*) stream->state;
    bool status;

    if (buf == NULL) {
        while (count-- && fgetc(file) != EOF);
        return count == 0;
    }

    status = (fread(buf, 1, count, file) == count);

    if (feof(file))
        stream->bytes_left = 0;

    return status;
}

uint64_t oldSessionId = 0;
uint64_t lastUsedClientFrame = 0;       // Last used client frame for XR device display.
uint64_t waitedForClientFrame = 0;      // We wait for 1st actual XR device present of this client frame.
uint64_t waitedTargetTimeNsecs = 0;     // Target (desired) present time of the waitedForClientFrame / waitedForSystemFrame.
uint64_t nextClientFrame = 0;           // Id of our most recently submitted client frame.
uint64_t nextTargetTimeNsecs = 0;       // Target (desired) present time of the waitedForClientFrame / waitedForSystemFrame.

uint64_t waitedForSystemFrame = 0;      // We wait on present of this system frame.
uint64_t onsetTimeNsecs = 0;            // Actual (pageflipped) present time of the waitedForClientFrame/waitedForSystemFrame.

int verbosity = 3;

static bool executeMetricsCycle(pb_istream_t* pbstream)
{
    const pb_msgdesc_t *type;
    uint64_t dummysize;
    bool status;

    while (1) {
        status = false;

        // Decode and throw away varint with message size info:
        pb_decode_varint(pbstream, &dummysize);

        // Decode type of message:
        type = decode_unionmessage_type(pbstream);

        // Choose suitable decoder call:
        if (type == monado_metrics_Version_fields) {
            // Protocol version: We only support version 1.1 at the moment:
            monado_metrics_Version msg = {};

            status = decode_unionmessage_contents(pbstream, type, &msg);

            if (verbosity >= 3)
                printf("PsychOpenXRCore-INFO: Using Monado metrics protocol version: %d.%d\n", msg.major, msg.minor);

            if  ((msg.major != 1 || msg.minor != 1) && (verbosity >= 2))
                printf("PsychOpenXRCore-WARNING: Not required version 1.1. Expect possible severe trouble due to miscommunication Monado <-> PsychOpenXRCore!\n");
        }
        else if (type == monado_metrics_SystemFrame_fields) {
            // Something not so important. Eat up and ignore:
            monado_metrics_SystemFrame msg = {};

            status = decode_unionmessage_contents(pbstream, type, &msg);
            // printf("Got monado_metrics_SystemFrame\n");
        }
        else if (type == monado_metrics_SystemGpuInfo_fields) {
            // Something not so important. Eat up and ignore:
            monado_metrics_SystemGpuInfo msg = {};

            status = decode_unionmessage_contents(pbstream, type, &msg);
            //printf("Got monado_metrics_SystemGpuInfo\n");
        }
        else if (type == monado_metrics_SessionFrame_fields) {
            // MonadoXR compositor has received our latest submitted XR frame for future display:
            monado_metrics_SessionFrame msg = {};

            status = decode_unionmessage_contents(pbstream, type, &msg);

            if (verbosity >= 4)
                printf("PsychOpenXRCore-DEBUG: %s Received session frame:    session_id %ld :: session_frame_id %ld :: predicted_display_time_ns %ld :: display_time_ns %ld\n",
                       msg.discarded ? "Discarded" : "Got",  msg.session_id, msg.frame_id, msg.predicted_display_time_ns, msg.display_time_ns);

            if (verbosity >= 3)
                printf("PsychOpenXRCore-INFO: New frame %ld from client submitted for future initial present at desired_present_time_ns %ld\n", msg.frame_id, msg.display_time_ns);

            if (msg.session_id > oldSessionId) {
                if (verbosity >= 3)
                    printf("PsychOpenXRCore-INFO: New OpenXR session %ld started -- Resetting.\n", msg.session_id);

                oldSessionId = msg.session_id;
                waitedForClientFrame = 0;
                waitedTargetTimeNsecs = 0;
                nextClientFrame = 0;
                nextTargetTimeNsecs = 0;
                lastUsedClientFrame = 0;
                waitedForSystemFrame = 0;
                onsetTimeNsecs = 0;
            }

            if (waitedForClientFrame == 0) {
                waitedForClientFrame = msg.frame_id;
                waitedTargetTimeNsecs = msg.display_time_ns;
            }
            else if (msg.frame_id != waitedForClientFrame) {
                if (verbosity >= 3)
                    printf("PsychOpenXRCore-INFO: New frame %ld from client while old one %ld still pending initial present...\n", msg.frame_id, waitedForClientFrame);

                nextClientFrame = msg.frame_id;
                nextTargetTimeNsecs = msg.display_time_ns;
            }
            else {
                if (verbosity >= 2)
                    printf("PsychOpenXRCore-INFO: New frame %ld from client identical to old one?!?\n", msg.frame_id);
            }
        }
        else if (type == monado_metrics_Used_fields) {
            // Info about a submitted frame currently latched for presentation at next flip:
            monado_metrics_Used msg = {};

            status = decode_unionmessage_contents(pbstream, type, &msg);

            if (verbosity >= 4)
                printf("PsychOpenXRCore-DEBUG: Got monado_metrics_Used:      session_id %ld :: session_frame_id %ld :: system_frame_id %ld :: when %ld nsecs\n",
                       msg.session_id, msg.session_frame_id, msg.system_frame_id, msg.when_ns);

            if (msg.session_id > oldSessionId) {
                if (verbosity >= 3)
                    printf("PsychOpenXRCore-INFO: New OpenXR session %ld started -- Resetting.\n", msg.session_id);

                oldSessionId = msg.session_id;
                waitedForClientFrame = 0;
                waitedTargetTimeNsecs = 0;
                nextClientFrame = 0;
                nextTargetTimeNsecs = 0;
                lastUsedClientFrame = 0;
                waitedForSystemFrame = 0;
                onsetTimeNsecs = 0;
            }

            // Client frame picked for XR present cycle the 1st time?
            if (msg.session_frame_id > lastUsedClientFrame) {
                // Bump in frame id of used frame. New stimulus about to go up on the display.

                // The one we expect?
                if (msg.session_frame_id == waitedForClientFrame) {
                    // Yes.
                    if (verbosity >= 3)
                        printf("PsychOpenXRCore-INFO: New frame %ld from client about to do initial present in system frame %ld.\n",
                               msg.session_frame_id, msg.system_frame_id);
                }
                else if (msg.session_frame_id == nextClientFrame) {
                    if (verbosity >= 3)
                        printf("PsychOpenXRCore-INFO: Frame %ld from client about to do initial present in system frame %ld. Old client frame %ld dropped!\n",
                               msg.session_frame_id, msg.system_frame_id, waitedForClientFrame);

                    waitedForClientFrame = nextClientFrame;
                    nextClientFrame = 0;
                    waitedTargetTimeNsecs = nextTargetTimeNsecs;
                    nextTargetTimeNsecs = 0;
                }

                lastUsedClientFrame = msg.session_frame_id;
                waitedForSystemFrame = msg.system_frame_id;
            }
        }
        else if (type == monado_metrics_SystemPresentInfo_fields) {
            // New image present completed on the HMD, ie. kms pageflipped onto scanout and
            // reported by VK_GOOGLE_display_timing et al.:
            monado_metrics_SystemPresentInfo msg = {};

            status = decode_unionmessage_contents(pbstream, type, &msg);

            if (verbosity >= 4)
                printf("PsychOpenXRCore-DEBUG: Got monado_metrics_SystemPresentInfo: frame_id %ld :: desired_present_time_ns %ld vs. actual_present_time_ns %ld :: Delta %ld\n",
                       msg.frame_id, msg.desired_present_time_ns, msg.actual_present_time_ns, msg.actual_present_time_ns - msg.desired_present_time_ns);

            // Waited for initial present of a new client frame?
            if (msg.frame_id == waitedForSystemFrame) {
                // Yes, this is the first present of our waitedForClientFrame. Present completion!
                onsetTimeNsecs = msg.actual_present_time_ns;

                if (verbosity >= 3)
                    printf("PsychOpenXRCore-INFO: Frame %ld from client presented in system frame %ld at time %ld nsecs. Delay wrt. desired onset time %f msecs.\n",
                           waitedForClientFrame, waitedForSystemFrame, onsetTimeNsecs, (double) (onsetTimeNsecs - waitedTargetTimeNsecs) / 1e6);

                // Invalidate and update:
                waitedForClientFrame = nextClientFrame;
                nextClientFrame = 0;
                waitedTargetTimeNsecs = nextTargetTimeNsecs;
                nextTargetTimeNsecs = 0;
            }
        }

        // Trouble in decode?
        if (!status) {
            if (verbosity >= 1)
                printf("PsychOpenXRCore-ERROR: Metrics message decode failed: %s\n", PB_GET_ERROR(pbstream));

            return (false);
        }
    }

    // Success:
    return (true);
}

int main(int nargin, char* argv[])
{
    if (nargin < 2) {
        printf("Input filename missing!\n");
        return(1);
    }

    // Init: Open metrics fifo/pipe:
    FILE* infile = fopen(argv[1], "rb");
    if (!infile) {
        printf("Could not open file %s : %s\n", argv[1], strerror(errno));
        return(1);
    }

    // Turn it into a nanopb stream / init stream:
    pb_istream_t stream = { &callback, infile, SIZE_MAX };

    while (executeMetricsCycle(&stream)) {}

    // Shutdown: Close fifo:
    fclose(infile);

    return 0;
}
