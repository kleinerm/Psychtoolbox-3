function glProgramUniform4uiEXT( program, location, v0, v1, v2, v3 )

% glProgramUniform4uiEXT  Interface to OpenGL function glProgramUniform4uiEXT
%
% usage:  glProgramUniform4uiEXT( program, location, v0, v1, v2, v3 )
%
% C function:  void glProgramUniform4uiEXT(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform4uiEXT', program, location, v0, v1, v2, v3 );

return
