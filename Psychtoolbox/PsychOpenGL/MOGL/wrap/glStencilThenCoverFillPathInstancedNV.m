function glStencilThenCoverFillPathInstancedNV( numPaths, pathNameType, paths, pathBase, fillMode, mask, coverMode, transformType, transformValues )

% glStencilThenCoverFillPathInstancedNV  Interface to OpenGL function glStencilThenCoverFillPathInstancedNV
%
% usage:  glStencilThenCoverFillPathInstancedNV( numPaths, pathNameType, paths, pathBase, fillMode, mask, coverMode, transformType, transformValues )
%
% C function:  void glStencilThenCoverFillPathInstancedNV(GLsizei numPaths, GLenum pathNameType, const void* paths, GLuint pathBase, GLenum fillMode, GLuint mask, GLenum coverMode, GLenum transformType, const GLfloat* transformValues)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glStencilThenCoverFillPathInstancedNV', numPaths, pathNameType, paths, pathBase, fillMode, mask, coverMode, transformType, single(transformValues) );

return
