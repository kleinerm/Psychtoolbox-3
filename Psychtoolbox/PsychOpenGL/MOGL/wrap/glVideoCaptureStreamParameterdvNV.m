function glVideoCaptureStreamParameterdvNV( video_capture_slot, stream, pname, params )

% glVideoCaptureStreamParameterdvNV  Interface to OpenGL function glVideoCaptureStreamParameterdvNV
%
% usage:  glVideoCaptureStreamParameterdvNV( video_capture_slot, stream, pname, params )
%
% C function:  void glVideoCaptureStreamParameterdvNV(GLuint video_capture_slot, GLuint stream, GLenum pname, const GLdouble* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVideoCaptureStreamParameterdvNV', video_capture_slot, stream, pname, double(params) );

return
