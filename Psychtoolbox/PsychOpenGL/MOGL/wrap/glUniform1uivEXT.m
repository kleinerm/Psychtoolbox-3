function glUniform1uivEXT( location, count, value )

% glUniform1uivEXT  Interface to OpenGL function glUniform1uivEXT
%
% usage:  glUniform1uivEXT( location, count, value )
%
% C function:  void glUniform1uivEXT(GLint location, GLsizei count, const GLuint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform1uivEXT', location, count, uint32(value) );

return
