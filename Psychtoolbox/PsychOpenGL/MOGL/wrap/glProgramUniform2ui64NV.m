function glProgramUniform2ui64NV( program, location, x, y )

% glProgramUniform2ui64NV  Interface to OpenGL function glProgramUniform2ui64NV
%
% usage:  glProgramUniform2ui64NV( program, location, x, y )
%
% C function:  void glProgramUniform2ui64NV(GLuint program, GLint location, GLuint64EXT x, GLuint64EXT y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform2ui64NV', program, location, x, y );

return
