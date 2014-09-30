function glSyncTextureINTEL( texture )

% glSyncTextureINTEL  Interface to OpenGL function glSyncTextureINTEL
%
% usage:  glSyncTextureINTEL( texture )
%
% C function:  void glSyncTextureINTEL(GLuint texture)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSyncTextureINTEL', texture );

return
