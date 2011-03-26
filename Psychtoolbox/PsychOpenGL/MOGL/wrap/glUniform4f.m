function glUniform4f( location, v0, v1, v2, v3 )

% glUniform4f  Interface to OpenGL function glUniform4f
%
% usage:  glUniform4f( location, v0, v1, v2, v3 )
%
% C function:  void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glUniform4f', location, v0, v1, v2, v3 );

return
