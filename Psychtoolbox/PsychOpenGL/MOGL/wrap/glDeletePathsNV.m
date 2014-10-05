function glDeletePathsNV( path, range )

% glDeletePathsNV  Interface to OpenGL function glDeletePathsNV
%
% usage:  glDeletePathsNV( path, range )
%
% C function:  void glDeletePathsNV(GLuint path, GLsizei range)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeletePathsNV', path, range );

return
