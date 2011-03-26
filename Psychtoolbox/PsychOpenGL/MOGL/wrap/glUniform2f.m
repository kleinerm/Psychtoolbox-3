function glUniform2f( location, v0, v1 )

% glUniform2f  Interface to OpenGL function glUniform2f
%
% usage:  glUniform2f( location, v0, v1 )
%
% C function:  void glUniform2f(GLint location, GLfloat v0, GLfloat v1)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform2f', location, v0, v1 );

return
