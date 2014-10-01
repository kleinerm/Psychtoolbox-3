function glEndVideoCaptureNV( video_capture_slot )

% glEndVideoCaptureNV  Interface to OpenGL function glEndVideoCaptureNV
%
% usage:  glEndVideoCaptureNV( video_capture_slot )
%
% C function:  void glEndVideoCaptureNV(GLuint video_capture_slot)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glEndVideoCaptureNV', video_capture_slot );

return
