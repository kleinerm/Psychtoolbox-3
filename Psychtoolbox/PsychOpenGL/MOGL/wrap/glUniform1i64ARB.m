function glUniform1i64ARB( location, x )

% glUniform1i64ARB  Interface to OpenGL function glUniform1i64ARB
%
% usage:  glUniform1i64ARB( location, x )
%
% C function:  void glUniform1i64ARB(GLint location, GLint64 x)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glUniform1i64ARB', location, int64(x) );

return
