function glBindVideoCaptureStreamTextureNV( video_capture_slot, stream, frame_region, target, texture )

% glBindVideoCaptureStreamTextureNV  Interface to OpenGL function glBindVideoCaptureStreamTextureNV
%
% usage:  glBindVideoCaptureStreamTextureNV( video_capture_slot, stream, frame_region, target, texture )
%
% C function:  void glBindVideoCaptureStreamTextureNV(GLuint video_capture_slot, GLuint stream, GLenum frame_region, GLenum target, GLuint texture)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glBindVideoCaptureStreamTextureNV', video_capture_slot, stream, frame_region, target, texture );

return
