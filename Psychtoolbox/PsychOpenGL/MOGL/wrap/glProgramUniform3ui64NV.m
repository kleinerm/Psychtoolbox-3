function glProgramUniform3ui64NV( program, location, x, y, z )

% glProgramUniform3ui64NV  Interface to OpenGL function glProgramUniform3ui64NV
%
% usage:  glProgramUniform3ui64NV( program, location, x, y, z )
%
% C function:  void glProgramUniform3ui64NV(GLuint program, GLint location, GLuint64EXT x, GLuint64EXT y, GLuint64EXT z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform3ui64NV', program, location, x, y, z );

return
