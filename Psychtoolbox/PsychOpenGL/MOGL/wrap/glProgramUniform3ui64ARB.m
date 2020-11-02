function glProgramUniform3ui64ARB( program, location, x, y, z )

% glProgramUniform3ui64ARB  Interface to OpenGL function glProgramUniform3ui64ARB
%
% usage:  glProgramUniform3ui64ARB( program, location, x, y, z )
%
% C function:  void glProgramUniform3ui64ARB(GLuint program, GLint location, GLuint64 x, GLuint64 y, GLuint64 z)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform3ui64ARB', program, location, uint64(x), uint64(y), uint64(z) );

return
