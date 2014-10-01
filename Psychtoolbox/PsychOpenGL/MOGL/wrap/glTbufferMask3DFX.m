function glTbufferMask3DFX( mask )

% glTbufferMask3DFX  Interface to OpenGL function glTbufferMask3DFX
%
% usage:  glTbufferMask3DFX( mask )
%
% C function:  void glTbufferMask3DFX(GLuint mask)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTbufferMask3DFX', mask );

return
