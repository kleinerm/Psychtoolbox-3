function glUniform4iv( location, count, value )

% glUniform4iv  Interface to OpenGL function glUniform4iv
%
% usage:  glUniform4iv( location, count, value )
%
% C function:  void glUniform4iv(GLint location, GLsizei count, const GLint* value)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform4iv', location, count, int32(value) );

return
