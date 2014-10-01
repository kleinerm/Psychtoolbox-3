function data = glGetColorTableEXT( target, format, type )

% glGetColorTableEXT  Interface to OpenGL function glGetColorTableEXT
%
% usage:  data = glGetColorTableEXT( target, format, type )
%
% C function:  void glGetColorTableEXT(GLenum target, GLenum format, GLenum type, void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

data = (0);

moglcore( 'glGetColorTableEXT', target, format, type, data );

return
