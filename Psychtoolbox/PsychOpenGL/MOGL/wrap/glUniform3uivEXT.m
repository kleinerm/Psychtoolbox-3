function glUniform3uivEXT( location, count, value )

% glUniform3uivEXT  Interface to OpenGL function glUniform3uivEXT
%
% usage:  glUniform3uivEXT( location, count, value )
%
% C function:  void glUniform3uivEXT(GLint location, GLsizei count, const GLuint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform3uivEXT', location, count, uint32(value) );

return
