function glProgramUniform1uiEXT( program, location, v0 )

% glProgramUniform1uiEXT  Interface to OpenGL function glProgramUniform1uiEXT
%
% usage:  glProgramUniform1uiEXT( program, location, v0 )
%
% C function:  void glProgramUniform1uiEXT(GLuint program, GLint location, GLuint v0)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform1uiEXT', program, location, v0 );

return
