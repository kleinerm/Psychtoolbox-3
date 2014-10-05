function glProgramUniformMatrix4x3fvEXT( program, location, count, transpose, value )

% glProgramUniformMatrix4x3fvEXT  Interface to OpenGL function glProgramUniformMatrix4x3fvEXT
%
% usage:  glProgramUniformMatrix4x3fvEXT( program, location, count, transpose, value )
%
% C function:  void glProgramUniformMatrix4x3fvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniformMatrix4x3fvEXT', program, location, count, transpose, single(value) );

return
