function glProgramUniform4i64NV( program, location, x, y, z, w )

% glProgramUniform4i64NV  Interface to OpenGL function glProgramUniform4i64NV
%
% usage:  glProgramUniform4i64NV( program, location, x, y, z, w )
%
% C function:  void glProgramUniform4i64NV(GLuint program, GLint location, GLint64EXT x, GLint64EXT y, GLint64EXT z, GLint64EXT w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform4i64NV', program, location, x, y, z, w );

return
