function glUniform2iv( location, count, value )

% glUniform2iv  Interface to OpenGL function glUniform2iv
%
% usage:  glUniform2iv( location, count, value )
%
% C function:  void glUniform2iv(GLint location, GLsizei count, const GLint* value)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform2iv', location, count, int32(value) );

return
