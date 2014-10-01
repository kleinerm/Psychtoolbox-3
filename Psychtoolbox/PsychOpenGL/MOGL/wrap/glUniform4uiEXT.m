function glUniform4uiEXT( location, v0, v1, v2, v3 )

% glUniform4uiEXT  Interface to OpenGL function glUniform4uiEXT
%
% usage:  glUniform4uiEXT( location, v0, v1, v2, v3 )
%
% C function:  void glUniform4uiEXT(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glUniform4uiEXT', location, v0, v1, v2, v3 );

return
