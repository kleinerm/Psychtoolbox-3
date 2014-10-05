function glProgramUniform4ui64NV( program, location, x, y, z, w )

% glProgramUniform4ui64NV  Interface to OpenGL function glProgramUniform4ui64NV
%
% usage:  glProgramUniform4ui64NV( program, location, x, y, z, w )
%
% C function:  void glProgramUniform4ui64NV(GLuint program, GLint location, GLuint64EXT x, GLuint64EXT y, GLuint64EXT z, GLuint64EXT w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform4ui64NV', program, location, x, y, z, w );

return
