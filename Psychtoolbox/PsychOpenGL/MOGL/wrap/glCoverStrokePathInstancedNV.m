function glCoverStrokePathInstancedNV( numPaths, pathNameType, paths, pathBase, coverMode, transformType, transformValues )

% glCoverStrokePathInstancedNV  Interface to OpenGL function glCoverStrokePathInstancedNV
%
% usage:  glCoverStrokePathInstancedNV( numPaths, pathNameType, paths, pathBase, coverMode, transformType, transformValues )
%
% C function:  void glCoverStrokePathInstancedNV(GLsizei numPaths, GLenum pathNameType, const void* paths, GLuint pathBase, GLenum coverMode, GLenum transformType, const GLfloat* transformValues)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glCoverStrokePathInstancedNV', numPaths, pathNameType, paths, pathBase, coverMode, transformType, single(transformValues) );

return
