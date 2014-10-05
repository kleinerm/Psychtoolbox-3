function glUniform2uiv( location, count, value )

% glUniform2uiv  Interface to OpenGL function glUniform2uiv
%
% usage:  glUniform2uiv( location, count, value )
%
% C function:  void glUniform2uiv(GLint location, GLsizei count, const GLuint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform2uiv', location, count, uint32(value) );

return
