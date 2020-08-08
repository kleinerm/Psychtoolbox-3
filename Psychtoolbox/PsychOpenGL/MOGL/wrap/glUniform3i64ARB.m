function glUniform3i64ARB( location, x, y, z )

% glUniform3i64ARB  Interface to OpenGL function glUniform3i64ARB
%
% usage:  glUniform3i64ARB( location, x, y, z )
%
% C function:  void glUniform3i64ARB(GLint location, GLint64 x, GLint64 y, GLint64 z)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glUniform3i64ARB', location, int64(x), int64(y), int64(z) );

return
