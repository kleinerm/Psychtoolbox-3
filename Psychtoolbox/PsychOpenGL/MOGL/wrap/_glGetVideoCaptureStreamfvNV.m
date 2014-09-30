function params = glGetVideoCaptureStreamfvNV( video_capture_slot, stream, pname )

% glGetVideoCaptureStreamfvNV  Interface to OpenGL function glGetVideoCaptureStreamfvNV
%
% usage:  params = glGetVideoCaptureStreamfvNV( video_capture_slot, stream, pname )
%
% C function:  void glGetVideoCaptureStreamfvNV(GLuint video_capture_slot, GLuint stream, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetVideoCaptureStreamfvNV', video_capture_slot, stream, pname, params );

return
