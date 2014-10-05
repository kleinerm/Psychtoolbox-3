function glUniform1ui64NV( location, x )

% glUniform1ui64NV  Interface to OpenGL function glUniform1ui64NV
%
% usage:  glUniform1ui64NV( location, x )
%
% C function:  void glUniform1ui64NV(GLint location, GLuint64EXT x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glUniform1ui64NV', location, x );

return
