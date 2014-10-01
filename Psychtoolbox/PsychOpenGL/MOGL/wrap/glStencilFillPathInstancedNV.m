function glStencilFillPathInstancedNV( numPaths, pathNameType, paths, pathBase, fillMode, mask, transformType, transformValues )

% glStencilFillPathInstancedNV  Interface to OpenGL function glStencilFillPathInstancedNV
%
% usage:  glStencilFillPathInstancedNV( numPaths, pathNameType, paths, pathBase, fillMode, mask, transformType, transformValues )
%
% C function:  void glStencilFillPathInstancedNV(GLsizei numPaths, GLenum pathNameType, const void* paths, GLuint pathBase, GLenum fillMode, GLuint mask, GLenum transformType, const GLfloat* transformValues)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glStencilFillPathInstancedNV', numPaths, pathNameType, paths, pathBase, fillMode, mask, transformType, single(transformValues) );

return
