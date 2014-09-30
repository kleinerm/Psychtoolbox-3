function glProgramUniformHandleui64vNV( program, location, count, values )

% glProgramUniformHandleui64vNV  Interface to OpenGL function glProgramUniformHandleui64vNV
%
% usage:  glProgramUniformHandleui64vNV( program, location, count, values )
%
% C function:  void glProgramUniformHandleui64vNV(GLuint program, GLint location, GLsizei count, const GLuint64* values)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniformHandleui64vNV', program, location, count, uint64(values) );

return
