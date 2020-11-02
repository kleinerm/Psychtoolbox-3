function glUniform4ui64ARB( location, x, y, z, w )

% glUniform4ui64ARB  Interface to OpenGL function glUniform4ui64ARB
%
% usage:  glUniform4ui64ARB( location, x, y, z, w )
%
% C function:  void glUniform4ui64ARB(GLint location, GLuint64 x, GLuint64 y, GLuint64 z, GLuint64 w)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glUniform4ui64ARB', location, uint64(x), uint64(y), uint64(z), uint64(w) );

return
