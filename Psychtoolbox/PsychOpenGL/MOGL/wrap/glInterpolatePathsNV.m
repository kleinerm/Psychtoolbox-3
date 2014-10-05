function glInterpolatePathsNV( resultPath, pathA, pathB, weight )

% glInterpolatePathsNV  Interface to OpenGL function glInterpolatePathsNV
%
% usage:  glInterpolatePathsNV( resultPath, pathA, pathB, weight )
%
% C function:  void glInterpolatePathsNV(GLuint resultPath, GLuint pathA, GLuint pathB, GLfloat weight)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glInterpolatePathsNV', resultPath, pathA, pathB, weight );

return
