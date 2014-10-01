function glProgramUniform1iEXT( program, location, v0 )

% glProgramUniform1iEXT  Interface to OpenGL function glProgramUniform1iEXT
%
% usage:  glProgramUniform1iEXT( program, location, v0 )
%
% C function:  void glProgramUniform1iEXT(GLuint program, GLint location, GLint v0)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform1iEXT', program, location, v0 );

return
