function glProgramUniform3uiEXT( program, location, v0, v1, v2 )

% glProgramUniform3uiEXT  Interface to OpenGL function glProgramUniform3uiEXT
%
% usage:  glProgramUniform3uiEXT( program, location, v0, v1, v2 )
%
% C function:  void glProgramUniform3uiEXT(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform3uiEXT', program, location, v0, v1, v2 );

return
