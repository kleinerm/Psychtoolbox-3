function glUniformHandleui64vNV( location, count, value )

% glUniformHandleui64vNV  Interface to OpenGL function glUniformHandleui64vNV
%
% usage:  glUniformHandleui64vNV( location, count, value )
%
% C function:  void glUniformHandleui64vNV(GLint location, GLsizei count, const GLuint64* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniformHandleui64vNV', location, count, uint64(value) );

return
