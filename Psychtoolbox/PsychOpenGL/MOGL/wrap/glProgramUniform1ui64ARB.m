function glProgramUniform1ui64ARB( program, location, x )

% glProgramUniform1ui64ARB  Interface to OpenGL function glProgramUniform1ui64ARB
%
% usage:  glProgramUniform1ui64ARB( program, location, x )
%
% C function:  void glProgramUniform1ui64ARB(GLuint program, GLint location, GLuint64 x)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform1ui64ARB', program, location, uint64(x) );

return
