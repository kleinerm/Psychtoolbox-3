function values = glGetPixelMapfv( map )

% glGetPixelMapfv  Interface to OpenGL function glGetPixelMapfv
%
% usage:  values = glGetPixelMapfv( map )
%
% C function:  void glGetPixelMapfv(GLenum map, GLfloat* values)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

values = single(0);

moglcore( 'glGetPixelMapfv', map, values );

return
