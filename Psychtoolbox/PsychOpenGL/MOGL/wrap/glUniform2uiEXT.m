function glUniform2uiEXT( location, v0, v1 )

% glUniform2uiEXT  Interface to OpenGL function glUniform2uiEXT
%
% usage:  glUniform2uiEXT( location, v0, v1 )
%
% C function:  void glUniform2uiEXT(GLint location, GLuint v0, GLuint v1)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform2uiEXT', location, v0, v1 );

return
