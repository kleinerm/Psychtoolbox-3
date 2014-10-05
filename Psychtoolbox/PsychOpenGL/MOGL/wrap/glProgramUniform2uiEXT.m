function glProgramUniform2uiEXT( program, location, v0, v1 )

% glProgramUniform2uiEXT  Interface to OpenGL function glProgramUniform2uiEXT
%
% usage:  glProgramUniform2uiEXT( program, location, v0, v1 )
%
% C function:  void glProgramUniform2uiEXT(GLuint program, GLint location, GLuint v0, GLuint v1)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform2uiEXT', program, location, v0, v1 );

return
