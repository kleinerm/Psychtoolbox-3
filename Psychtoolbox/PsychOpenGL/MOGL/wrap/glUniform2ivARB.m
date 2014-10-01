function glUniform2ivARB( location, count, value )

% glUniform2ivARB  Interface to OpenGL function glUniform2ivARB
%
% usage:  glUniform2ivARB( location, count, value )
%
% C function:  void glUniform2ivARB(GLint location, GLsizei count, const GLint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform2ivARB', location, count, int32(value) );

return
