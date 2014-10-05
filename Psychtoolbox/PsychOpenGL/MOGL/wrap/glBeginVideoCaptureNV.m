function glBeginVideoCaptureNV( video_capture_slot )

% glBeginVideoCaptureNV  Interface to OpenGL function glBeginVideoCaptureNV
%
% usage:  glBeginVideoCaptureNV( video_capture_slot )
%
% C function:  void glBeginVideoCaptureNV(GLuint video_capture_slot)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glBeginVideoCaptureNV', video_capture_slot );

return
