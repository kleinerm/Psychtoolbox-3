function glProgramUniform2iEXT( program, location, v0, v1 )

% glProgramUniform2iEXT  Interface to OpenGL function glProgramUniform2iEXT
%
% usage:  glProgramUniform2iEXT( program, location, v0, v1 )
%
% C function:  void glProgramUniform2iEXT(GLuint program, GLint location, GLint v0, GLint v1)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform2iEXT', program, location, v0, v1 );

return
