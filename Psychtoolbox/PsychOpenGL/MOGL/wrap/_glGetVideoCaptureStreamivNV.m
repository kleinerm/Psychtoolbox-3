function params = glGetVideoCaptureStreamivNV( video_capture_slot, stream, pname )

% glGetVideoCaptureStreamivNV  Interface to OpenGL function glGetVideoCaptureStreamivNV
%
% usage:  params = glGetVideoCaptureStreamivNV( video_capture_slot, stream, pname )
%
% C function:  void glGetVideoCaptureStreamivNV(GLuint video_capture_slot, GLuint stream, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetVideoCaptureStreamivNV', video_capture_slot, stream, pname, params );

return
