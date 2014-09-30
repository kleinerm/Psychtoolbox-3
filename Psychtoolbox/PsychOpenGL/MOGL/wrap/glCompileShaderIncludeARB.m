function glCompileShaderIncludeARB( shader, count, const, length )

% glCompileShaderIncludeARB  Interface to OpenGL function glCompileShaderIncludeARB
%
% usage:  glCompileShaderIncludeARB( shader, count, const, length )
%
% C function:  void glCompileShaderIncludeARB(GLuint shader, GLsizei count, const GLchar* const, const GLint* length)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glCompileShaderIncludeARB', shader, count, uint8(const), int32(length) );

return
