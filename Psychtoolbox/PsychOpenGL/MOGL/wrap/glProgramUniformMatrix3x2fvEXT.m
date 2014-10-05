function glProgramUniformMatrix3x2fvEXT( program, location, count, transpose, value )

% glProgramUniformMatrix3x2fvEXT  Interface to OpenGL function glProgramUniformMatrix3x2fvEXT
%
% usage:  glProgramUniformMatrix3x2fvEXT( program, location, count, transpose, value )
%
% C function:  void glProgramUniformMatrix3x2fvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniformMatrix3x2fvEXT', program, location, count, transpose, single(value) );

return
