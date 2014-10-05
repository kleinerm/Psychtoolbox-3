function glProgramUniform3i64NV( program, location, x, y, z )

% glProgramUniform3i64NV  Interface to OpenGL function glProgramUniform3i64NV
%
% usage:  glProgramUniform3i64NV( program, location, x, y, z )
%
% C function:  void glProgramUniform3i64NV(GLuint program, GLint location, GLint64EXT x, GLint64EXT y, GLint64EXT z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform3i64NV', program, location, x, y, z );

return
