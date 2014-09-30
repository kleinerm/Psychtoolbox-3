function glStencilStrokePathInstancedNV( numPaths, pathNameType, paths, pathBase, reference, mask, transformType, transformValues )

% glStencilStrokePathInstancedNV  Interface to OpenGL function glStencilStrokePathInstancedNV
%
% usage:  glStencilStrokePathInstancedNV( numPaths, pathNameType, paths, pathBase, reference, mask, transformType, transformValues )
%
% C function:  void glStencilStrokePathInstancedNV(GLsizei numPaths, GLenum pathNameType, const void* paths, GLuint pathBase, GLint reference, GLuint mask, GLenum transformType, const GLfloat* transformValues)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glStencilStrokePathInstancedNV', numPaths, pathNameType, paths, pathBase, reference, mask, transformType, single(transformValues) );

return
