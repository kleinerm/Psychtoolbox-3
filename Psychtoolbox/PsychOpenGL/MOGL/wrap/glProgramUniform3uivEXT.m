function glProgramUniform3uivEXT( program, location, count, value )

% glProgramUniform3uivEXT  Interface to OpenGL function glProgramUniform3uivEXT
%
% usage:  glProgramUniform3uivEXT( program, location, count, value )
%
% C function:  void glProgramUniform3uivEXT(GLuint program, GLint location, GLsizei count, const GLuint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform3uivEXT', program, location, count, uint32(value) );

return
