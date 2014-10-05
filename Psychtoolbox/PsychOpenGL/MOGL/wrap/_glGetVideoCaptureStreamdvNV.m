function params = glGetVideoCaptureStreamdvNV( video_capture_slot, stream, pname )

% glGetVideoCaptureStreamdvNV  Interface to OpenGL function glGetVideoCaptureStreamdvNV
%
% usage:  params = glGetVideoCaptureStreamdvNV( video_capture_slot, stream, pname )
%
% C function:  void glGetVideoCaptureStreamdvNV(GLuint video_capture_slot, GLuint stream, GLenum pname, GLdouble* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = double(0);

moglcore( 'glGetVideoCaptureStreamdvNV', video_capture_slot, stream, pname, params );

return
