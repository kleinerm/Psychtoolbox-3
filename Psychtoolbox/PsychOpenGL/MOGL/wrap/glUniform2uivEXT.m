function glUniform2uivEXT( location, count, value )

% glUniform2uivEXT  Interface to OpenGL function glUniform2uivEXT
%
% usage:  glUniform2uivEXT( location, count, value )
%
% C function:  void glUniform2uivEXT(GLint location, GLsizei count, const GLuint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform2uivEXT', location, count, uint32(value) );

return
