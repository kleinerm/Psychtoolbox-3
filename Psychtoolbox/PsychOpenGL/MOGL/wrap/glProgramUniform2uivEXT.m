function glProgramUniform2uivEXT( program, location, count, value )

% glProgramUniform2uivEXT  Interface to OpenGL function glProgramUniform2uivEXT
%
% usage:  glProgramUniform2uivEXT( program, location, count, value )
%
% C function:  void glProgramUniform2uivEXT(GLuint program, GLint location, GLsizei count, const GLuint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform2uivEXT', program, location, count, uint32(value) );

return
