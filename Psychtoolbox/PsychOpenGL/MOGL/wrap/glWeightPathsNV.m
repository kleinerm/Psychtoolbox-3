function glWeightPathsNV( resultPath, numPaths, paths, weights )

% glWeightPathsNV  Interface to OpenGL function glWeightPathsNV
%
% usage:  glWeightPathsNV( resultPath, numPaths, paths, weights )
%
% C function:  void glWeightPathsNV(GLuint resultPath, GLsizei numPaths, const GLuint* paths, const GLfloat* weights)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glWeightPathsNV', resultPath, numPaths, uint32(paths), single(weights) );

return
