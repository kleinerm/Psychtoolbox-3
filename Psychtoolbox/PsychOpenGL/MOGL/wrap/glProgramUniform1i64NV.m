function glProgramUniform1i64NV( program, location, x )

% glProgramUniform1i64NV  Interface to OpenGL function glProgramUniform1i64NV
%
% usage:  glProgramUniform1i64NV( program, location, x )
%
% C function:  void glProgramUniform1i64NV(GLuint program, GLint location, GLint64EXT x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform1i64NV', program, location, x );

return
