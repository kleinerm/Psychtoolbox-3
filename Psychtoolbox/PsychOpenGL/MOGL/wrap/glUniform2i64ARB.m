function glUniform2i64ARB( location, x, y )

% glUniform2i64ARB  Interface to OpenGL function glUniform2i64ARB
%
% usage:  glUniform2i64ARB( location, x, y )
%
% C function:  void glUniform2i64ARB(GLint location, GLint64 x, GLint64 y)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform2i64ARB', location, int64(x), int64(y) );

return
