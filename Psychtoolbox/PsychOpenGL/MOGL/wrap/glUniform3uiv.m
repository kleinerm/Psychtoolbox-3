function glUniform3uiv( location, count, value )

% glUniform3uiv  Interface to OpenGL function glUniform3uiv
%
% usage:  glUniform3uiv( location, count, value )
%
% C function:  void glUniform3uiv(GLint location, GLsizei count, const GLuint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform3uiv', location, count, uint32(value) );

return
