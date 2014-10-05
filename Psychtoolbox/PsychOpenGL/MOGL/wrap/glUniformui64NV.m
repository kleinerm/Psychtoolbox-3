function glUniformui64NV( location, value )

% glUniformui64NV  Interface to OpenGL function glUniformui64NV
%
% usage:  glUniformui64NV( location, value )
%
% C function:  void glUniformui64NV(GLint location, GLuint64EXT value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glUniformui64NV', location, value );

return
