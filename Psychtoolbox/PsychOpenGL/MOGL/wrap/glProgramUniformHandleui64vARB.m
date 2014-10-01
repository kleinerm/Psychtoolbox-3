function glProgramUniformHandleui64vARB( program, location, count, values )

% glProgramUniformHandleui64vARB  Interface to OpenGL function glProgramUniformHandleui64vARB
%
% usage:  glProgramUniformHandleui64vARB( program, location, count, values )
%
% C function:  void glProgramUniformHandleui64vARB(GLuint program, GLint location, GLsizei count, const GLuint64* values)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniformHandleui64vARB', program, location, count, uint64(values) );

return
