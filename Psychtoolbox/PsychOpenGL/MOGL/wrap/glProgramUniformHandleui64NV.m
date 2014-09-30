function glProgramUniformHandleui64NV( program, location, value )

% glProgramUniformHandleui64NV  Interface to OpenGL function glProgramUniformHandleui64NV
%
% usage:  glProgramUniformHandleui64NV( program, location, value )
%
% C function:  void glProgramUniformHandleui64NV(GLuint program, GLint location, GLuint64 value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniformHandleui64NV', program, location, uint64(value) );

return
