function glUniformHandleui64NV( location, value )

% glUniformHandleui64NV  Interface to OpenGL function glUniformHandleui64NV
%
% usage:  glUniformHandleui64NV( location, value )
%
% C function:  void glUniformHandleui64NV(GLint location, GLuint64 value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glUniformHandleui64NV', location, uint64(value) );

return
