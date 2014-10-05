function glProgramUniform1uivEXT( program, location, count, value )

% glProgramUniform1uivEXT  Interface to OpenGL function glProgramUniform1uivEXT
%
% usage:  glProgramUniform1uivEXT( program, location, count, value )
%
% C function:  void glProgramUniform1uivEXT(GLuint program, GLint location, GLsizei count, const GLuint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform1uivEXT', program, location, count, uint32(value) );

return
