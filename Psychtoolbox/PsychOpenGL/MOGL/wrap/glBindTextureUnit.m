function glBindTextureUnit( unit, texture )

% glBindTextureUnit  Interface to OpenGL function glBindTextureUnit
%
% usage:  glBindTextureUnit( unit, texture )
%
% C function:  void glBindTextureUnit(GLuint unit, GLuint texture)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glBindTextureUnit', unit, texture );

return
