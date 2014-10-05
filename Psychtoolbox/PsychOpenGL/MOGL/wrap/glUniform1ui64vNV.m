function glUniform1ui64vNV( location, count, value )

% glUniform1ui64vNV  Interface to OpenGL function glUniform1ui64vNV
%
% usage:  glUniform1ui64vNV( location, count, value )
%
% C function:  void glUniform1ui64vNV(GLint location, GLsizei count, const GLuint64EXT* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform1ui64vNV', location, count, uint64(value) );

return
