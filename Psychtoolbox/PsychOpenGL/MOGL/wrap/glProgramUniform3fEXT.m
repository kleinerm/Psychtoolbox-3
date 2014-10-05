function glProgramUniform3fEXT( program, location, v0, v1, v2 )

% glProgramUniform3fEXT  Interface to OpenGL function glProgramUniform3fEXT
%
% usage:  glProgramUniform3fEXT( program, location, v0, v1, v2 )
%
% C function:  void glProgramUniform3fEXT(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform3fEXT', program, location, v0, v1, v2 );

return
