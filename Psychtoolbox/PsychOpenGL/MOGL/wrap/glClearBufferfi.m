function glClearBufferfi( buffer, drawbuffer, depth, stencil )

% glClearBufferfi  Interface to OpenGL function glClearBufferfi
%
% usage:  glClearBufferfi( buffer, drawbuffer, depth, stencil )
%
% C function:  void glClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glClearBufferfi', buffer, drawbuffer, depth, stencil );

return
