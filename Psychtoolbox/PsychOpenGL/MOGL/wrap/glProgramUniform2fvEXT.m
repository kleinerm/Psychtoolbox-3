function glProgramUniform2fvEXT( program, location, count, value )

% glProgramUniform2fvEXT  Interface to OpenGL function glProgramUniform2fvEXT
%
% usage:  glProgramUniform2fvEXT( program, location, count, value )
%
% C function:  void glProgramUniform2fvEXT(GLuint program, GLint location, GLsizei count, const GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform2fvEXT', program, location, count, single(value) );

return
