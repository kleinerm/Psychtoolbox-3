function glCoverFillPathInstancedNV( numPaths, pathNameType, paths, pathBase, coverMode, transformType, transformValues )

% glCoverFillPathInstancedNV  Interface to OpenGL function glCoverFillPathInstancedNV
%
% usage:  glCoverFillPathInstancedNV( numPaths, pathNameType, paths, pathBase, coverMode, transformType, transformValues )
%
% C function:  void glCoverFillPathInstancedNV(GLsizei numPaths, GLenum pathNameType, const void* paths, GLuint pathBase, GLenum coverMode, GLenum transformType, const GLfloat* transformValues)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glCoverFillPathInstancedNV', numPaths, pathNameType, paths, pathBase, coverMode, transformType, single(transformValues) );

return
