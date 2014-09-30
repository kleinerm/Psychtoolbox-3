function glBindVideoCaptureStreamBufferNV( video_capture_slot, stream, frame_region, ARB )

% glBindVideoCaptureStreamBufferNV  Interface to OpenGL function glBindVideoCaptureStreamBufferNV
%
% usage:  glBindVideoCaptureStreamBufferNV( video_capture_slot, stream, frame_region, ARB )
%
% C function:  void glBindVideoCaptureStreamBufferNV(GLuint video_capture_slot, GLuint stream, GLenum frame_region, GLintptr ARB)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glBindVideoCaptureStreamBufferNV', video_capture_slot, stream, frame_region, ARB );

return
