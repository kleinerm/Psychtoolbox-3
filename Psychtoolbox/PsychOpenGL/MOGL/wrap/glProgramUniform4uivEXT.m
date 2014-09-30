function glProgramUniform4uivEXT( program, location, count, value )

% glProgramUniform4uivEXT  Interface to OpenGL function glProgramUniform4uivEXT
%
% usage:  glProgramUniform4uivEXT( program, location, count, value )
%
% C function:  void glProgramUniform4uivEXT(GLuint program, GLint location, GLsizei count, const GLuint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform4uivEXT', program, location, count, uint32(value) );

return
