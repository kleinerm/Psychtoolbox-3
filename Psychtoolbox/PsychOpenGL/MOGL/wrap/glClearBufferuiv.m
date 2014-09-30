function glClearBufferuiv( buffer, drawbuffer, value )

% glClearBufferuiv  Interface to OpenGL function glClearBufferuiv
%
% usage:  glClearBufferuiv( buffer, drawbuffer, value )
%
% C function:  void glClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glClearBufferuiv', buffer, drawbuffer, uint32(value) );

return
