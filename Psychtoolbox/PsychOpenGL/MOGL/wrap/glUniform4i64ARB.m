function glUniform4i64ARB( location, x, y, z, w )

% glUniform4i64ARB  Interface to OpenGL function glUniform4i64ARB
%
% usage:  glUniform4i64ARB( location, x, y, z, w )
%
% C function:  void glUniform4i64ARB(GLint location, GLint64 x, GLint64 y, GLint64 z, GLint64 w)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glUniform4i64ARB', location, int64(x), int64(y), int64(z), int64(w) );

return
