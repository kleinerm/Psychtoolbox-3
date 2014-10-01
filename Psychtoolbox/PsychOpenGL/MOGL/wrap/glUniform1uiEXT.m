function glUniform1uiEXT( location, v0 )

% glUniform1uiEXT  Interface to OpenGL function glUniform1uiEXT
%
% usage:  glUniform1uiEXT( location, v0 )
%
% C function:  void glUniform1uiEXT(GLint location, GLuint v0)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glUniform1uiEXT', location, v0 );

return
