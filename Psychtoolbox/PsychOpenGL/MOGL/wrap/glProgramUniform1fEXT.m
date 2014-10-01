function glProgramUniform1fEXT( program, location, v0 )

% glProgramUniform1fEXT  Interface to OpenGL function glProgramUniform1fEXT
%
% usage:  glProgramUniform1fEXT( program, location, v0 )
%
% C function:  void glProgramUniform1fEXT(GLuint program, GLint location, GLfloat v0)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform1fEXT', program, location, v0 );

return
