function glProgramUniform1ui64NV( program, location, x )

% glProgramUniform1ui64NV  Interface to OpenGL function glProgramUniform1ui64NV
%
% usage:  glProgramUniform1ui64NV( program, location, x )
%
% C function:  void glProgramUniform1ui64NV(GLuint program, GLint location, GLuint64EXT x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform1ui64NV', program, location, x );

return
