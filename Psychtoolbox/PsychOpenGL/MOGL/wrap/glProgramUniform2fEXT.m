function glProgramUniform2fEXT( program, location, v0, v1 )

% glProgramUniform2fEXT  Interface to OpenGL function glProgramUniform2fEXT
%
% usage:  glProgramUniform2fEXT( program, location, v0, v1 )
%
% C function:  void glProgramUniform2fEXT(GLuint program, GLint location, GLfloat v0, GLfloat v1)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform2fEXT', program, location, v0, v1 );

return
