function glUniform1f( location, v0 )

% glUniform1f  Interface to OpenGL function glUniform1f
%
% usage:  glUniform1f( location, v0 )
%
% C function:  void glUniform1f(GLint location, GLfloat v0)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glUniform1f', location, v0 );

return
