function glUniform4uiv( location, count, value )

% glUniform4uiv  Interface to OpenGL function glUniform4uiv
%
% usage:  glUniform4uiv( location, count, value )
%
% C function:  void glUniform4uiv(GLint location, GLsizei count, const GLuint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform4uiv', location, count, uint32(value) );

return
