function glStencilFillPathNV( path, fillMode, mask )

% glStencilFillPathNV  Interface to OpenGL function glStencilFillPathNV
%
% usage:  glStencilFillPathNV( path, fillMode, mask )
%
% C function:  void glStencilFillPathNV(GLuint path, GLenum fillMode, GLuint mask)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glStencilFillPathNV', path, fillMode, mask );

return
