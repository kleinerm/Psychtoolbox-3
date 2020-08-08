function glStencilThenCoverStrokePathInstancedNV( numPaths, pathNameType, paths, pathBase, reference, mask, coverMode, transformType, transformValues )

% glStencilThenCoverStrokePathInstancedNV  Interface to OpenGL function glStencilThenCoverStrokePathInstancedNV
%
% usage:  glStencilThenCoverStrokePathInstancedNV( numPaths, pathNameType, paths, pathBase, reference, mask, coverMode, transformType, transformValues )
%
% C function:  void glStencilThenCoverStrokePathInstancedNV(GLsizei numPaths, GLenum pathNameType, const void* paths, GLuint pathBase, GLint reference, GLuint mask, GLenum coverMode, GLenum transformType, const GLfloat* transformValues)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glStencilThenCoverStrokePathInstancedNV', numPaths, pathNameType, paths, pathBase, reference, mask, coverMode, transformType, single(transformValues) );

return
