function glUniform3ui64ARB( location, x, y, z )

% glUniform3ui64ARB  Interface to OpenGL function glUniform3ui64ARB
%
% usage:  glUniform3ui64ARB( location, x, y, z )
%
% C function:  void glUniform3ui64ARB(GLint location, GLuint64 x, GLuint64 y, GLuint64 z)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glUniform3ui64ARB', location, uint64(x), uint64(y), uint64(z) );

return
