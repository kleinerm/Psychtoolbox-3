function data = glGetBooleani_v( target, index )

% glGetBooleani_v  Interface to OpenGL function glGetBooleani_v
%
% usage:  data = glGetBooleani_v( target, index )
%
% C function:  void glGetBooleani_v(GLenum target, GLuint index, GLboolean* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

data = uint8(0);

moglcore( 'glGetBooleani_v', target, index, data );

return
