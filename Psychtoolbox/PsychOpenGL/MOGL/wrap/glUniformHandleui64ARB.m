function glUniformHandleui64ARB( location, value )

% glUniformHandleui64ARB  Interface to OpenGL function glUniformHandleui64ARB
%
% usage:  glUniformHandleui64ARB( location, value )
%
% C function:  void glUniformHandleui64ARB(GLint location, GLuint64 value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glUniformHandleui64ARB', location, uint64(value) );

return
