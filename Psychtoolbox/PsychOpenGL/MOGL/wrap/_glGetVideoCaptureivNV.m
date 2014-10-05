function params = glGetVideoCaptureivNV( video_capture_slot, pname )

% glGetVideoCaptureivNV  Interface to OpenGL function glGetVideoCaptureivNV
%
% usage:  params = glGetVideoCaptureivNV( video_capture_slot, pname )
%
% C function:  void glGetVideoCaptureivNV(GLuint video_capture_slot, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetVideoCaptureivNV', video_capture_slot, pname, params );

return
