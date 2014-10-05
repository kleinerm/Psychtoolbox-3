function glProgramUniform2i64NV( program, location, x, y )

% glProgramUniform2i64NV  Interface to OpenGL function glProgramUniform2i64NV
%
% usage:  glProgramUniform2i64NV( program, location, x, y )
%
% C function:  void glProgramUniform2i64NV(GLuint program, GLint location, GLint64EXT x, GLint64EXT y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform2i64NV', program, location, x, y );

return
