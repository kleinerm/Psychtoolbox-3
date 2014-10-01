function glUniform1ui( location, v0 )

% glUniform1ui  Interface to OpenGL function glUniform1ui
%
% usage:  glUniform1ui( location, v0 )
%
% C function:  void glUniform1ui(GLint location, GLuint v0)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glUniform1ui', location, v0 );

return
