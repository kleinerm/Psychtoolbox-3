function glUniform1fv( location, count, value )

% glUniform1fv  Interface to OpenGL function glUniform1fv
%
% usage:  glUniform1fv( location, count, value )
%
% C function:  void glUniform1fv(GLint location, GLsizei count, const GLfloat* value)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform1fv', location, count, single(value) );

return
