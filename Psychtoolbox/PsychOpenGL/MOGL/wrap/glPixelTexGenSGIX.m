function glPixelTexGenSGIX( mode )

% glPixelTexGenSGIX  Interface to OpenGL function glPixelTexGenSGIX
%
% usage:  glPixelTexGenSGIX( mode )
%
% C function:  void glPixelTexGenSGIX(GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glPixelTexGenSGIX', mode );

return
