function glUniform2ui64ARB( location, x, y )

% glUniform2ui64ARB  Interface to OpenGL function glUniform2ui64ARB
%
% usage:  glUniform2ui64ARB( location, x, y )
%
% C function:  void glUniform2ui64ARB(GLint location, GLuint64 x, GLuint64 y)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform2ui64ARB', location, uint64(x), uint64(y) );

return
