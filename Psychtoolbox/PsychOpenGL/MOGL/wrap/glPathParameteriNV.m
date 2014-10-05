function glPathParameteriNV( path, pname, value )

% glPathParameteriNV  Interface to OpenGL function glPathParameteriNV
%
% usage:  glPathParameteriNV( path, pname, value )
%
% C function:  void glPathParameteriNV(GLuint path, GLenum pname, GLint value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPathParameteriNV', path, pname, value );

return
