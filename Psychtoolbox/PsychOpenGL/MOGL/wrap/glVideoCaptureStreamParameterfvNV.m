function glVideoCaptureStreamParameterfvNV( video_capture_slot, stream, pname, params )

% glVideoCaptureStreamParameterfvNV  Interface to OpenGL function glVideoCaptureStreamParameterfvNV
%
% usage:  glVideoCaptureStreamParameterfvNV( video_capture_slot, stream, pname, params )
%
% C function:  void glVideoCaptureStreamParameterfvNV(GLuint video_capture_slot, GLuint stream, GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVideoCaptureStreamParameterfvNV', video_capture_slot, stream, pname, single(params) );

return
