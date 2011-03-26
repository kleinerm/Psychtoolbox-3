function glUniform2fv( location, count, value )

% glUniform2fv  Interface to OpenGL function glUniform2fv
%
% usage:  glUniform2fv( location, count, value )
%
% C function:  void glUniform2fv(GLint location, GLsizei count, const GLfloat* value)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform2fv', location, count, single(value) );

return
