function glProgramUniformui64vNV( program, location, count, value )

% glProgramUniformui64vNV  Interface to OpenGL function glProgramUniformui64vNV
%
% usage:  glProgramUniformui64vNV( program, location, count, value )
%
% C function:  void glProgramUniformui64vNV(GLuint program, GLint location, GLsizei count, const GLuint64EXT* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniformui64vNV', program, location, count, uint64(value) );

return
