function glProgramUniform4iEXT( program, location, v0, v1, v2, v3 )

% glProgramUniform4iEXT  Interface to OpenGL function glProgramUniform4iEXT
%
% usage:  glProgramUniform4iEXT( program, location, v0, v1, v2, v3 )
%
% C function:  void glProgramUniform4iEXT(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform4iEXT', program, location, v0, v1, v2, v3 );

return
