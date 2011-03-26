function glUniform1iv( location, count, value )

% glUniform1iv  Interface to OpenGL function glUniform1iv
%
% usage:  glUniform1iv( location, count, value )
%
% C function:  void glUniform1iv(GLint location, GLsizei count, const GLint* value)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform1iv', location, count, int32(value) );

return
