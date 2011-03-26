function glUniform3iv( location, count, value )

% glUniform3iv  Interface to OpenGL function glUniform3iv
%
% usage:  glUniform3iv( location, count, value )
%
% C function:  void glUniform3iv(GLint location, GLsizei count, const GLint* value)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform3iv', location, count, int32(value) );

return
