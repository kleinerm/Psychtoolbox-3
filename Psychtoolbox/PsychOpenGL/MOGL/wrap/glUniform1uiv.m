function glUniform1uiv( location, count, value )

% glUniform1uiv  Interface to OpenGL function glUniform1uiv
%
% usage:  glUniform1uiv( location, count, value )
%
% C function:  void glUniform1uiv(GLint location, GLsizei count, const GLuint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform1uiv', location, count, uint32(value) );

return
