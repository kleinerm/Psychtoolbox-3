function glClearBufferiv( buffer, drawbuffer, value )

% glClearBufferiv  Interface to OpenGL function glClearBufferiv
%
% usage:  glClearBufferiv( buffer, drawbuffer, value )
%
% C function:  void glClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glClearBufferiv', buffer, drawbuffer, int32(value) );

return
