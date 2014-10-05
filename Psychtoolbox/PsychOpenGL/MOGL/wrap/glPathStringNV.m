function glPathStringNV( path, format, length, pathString )

% glPathStringNV  Interface to OpenGL function glPathStringNV
%
% usage:  glPathStringNV( path, format, length, pathString )
%
% C function:  void glPathStringNV(GLuint path, GLenum format, GLsizei length, const void* pathString)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glPathStringNV', path, format, length, pathString );

return
