function glUniform1ui64ARB( location, x )

% glUniform1ui64ARB  Interface to OpenGL function glUniform1ui64ARB
%
% usage:  glUniform1ui64ARB( location, x )
%
% C function:  void glUniform1ui64ARB(GLint location, GLuint64 x)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glUniform1ui64ARB', location, uint64(x) );

return
