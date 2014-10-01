function glProgramUniformui64NV( program, location, value )

% glProgramUniformui64NV  Interface to OpenGL function glProgramUniformui64NV
%
% usage:  glProgramUniformui64NV( program, location, value )
%
% C function:  void glProgramUniformui64NV(GLuint program, GLint location, GLuint64EXT value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniformui64NV', program, location, value );

return
