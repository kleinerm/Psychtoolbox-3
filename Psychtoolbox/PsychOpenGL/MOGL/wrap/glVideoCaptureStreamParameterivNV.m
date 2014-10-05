function glVideoCaptureStreamParameterivNV( video_capture_slot, stream, pname, params )

% glVideoCaptureStreamParameterivNV  Interface to OpenGL function glVideoCaptureStreamParameterivNV
%
% usage:  glVideoCaptureStreamParameterivNV( video_capture_slot, stream, pname, params )
%
% C function:  void glVideoCaptureStreamParameterivNV(GLuint video_capture_slot, GLuint stream, GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVideoCaptureStreamParameterivNV', video_capture_slot, stream, pname, int32(params) );

return
