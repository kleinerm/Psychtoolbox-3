function glUniform4uivEXT( location, count, value )

% glUniform4uivEXT  Interface to OpenGL function glUniform4uivEXT
%
% usage:  glUniform4uivEXT( location, count, value )
%
% C function:  void glUniform4uivEXT(GLint location, GLsizei count, const GLuint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform4uivEXT', location, count, uint32(value) );

return
