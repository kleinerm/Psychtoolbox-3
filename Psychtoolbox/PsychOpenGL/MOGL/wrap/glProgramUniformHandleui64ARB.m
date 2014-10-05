function glProgramUniformHandleui64ARB( program, location, value )

% glProgramUniformHandleui64ARB  Interface to OpenGL function glProgramUniformHandleui64ARB
%
% usage:  glProgramUniformHandleui64ARB( program, location, value )
%
% C function:  void glProgramUniformHandleui64ARB(GLuint program, GLint location, GLuint64 value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniformHandleui64ARB', program, location, uint64(value) );

return
