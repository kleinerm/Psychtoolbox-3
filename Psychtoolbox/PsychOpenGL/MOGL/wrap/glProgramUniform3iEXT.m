function glProgramUniform3iEXT( program, location, v0, v1, v2 )

% glProgramUniform3iEXT  Interface to OpenGL function glProgramUniform3iEXT
%
% usage:  glProgramUniform3iEXT( program, location, v0, v1, v2 )
%
% C function:  void glProgramUniform3iEXT(GLuint program, GLint location, GLint v0, GLint v1, GLint v2)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform3iEXT', program, location, v0, v1, v2 );

return
