function glProgramUniform4fEXT( program, location, v0, v1, v2, v3 )

% glProgramUniform4fEXT  Interface to OpenGL function glProgramUniform4fEXT
%
% usage:  glProgramUniform4fEXT( program, location, v0, v1, v2, v3 )
%
% C function:  void glProgramUniform4fEXT(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform4fEXT', program, location, v0, v1, v2, v3 );

return
