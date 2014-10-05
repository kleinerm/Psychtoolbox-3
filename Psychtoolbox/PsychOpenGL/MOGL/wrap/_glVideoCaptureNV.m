function [ r, sequence_num, capture_time ] = glVideoCaptureNV( video_capture_slot )

% glVideoCaptureNV  Interface to OpenGL function glVideoCaptureNV
%
% usage:  [ r, sequence_num, capture_time ] = glVideoCaptureNV( video_capture_slot )
%
% C function:  GLenum glVideoCaptureNV(GLuint video_capture_slot, GLuint* sequence_num, GLuint64EXT* capture_time)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

sequence_num = uint32(0);
capture_time = uint64(0);

r = moglcore( 'glVideoCaptureNV', video_capture_slot, sequence_num, capture_time );

return
