function values = glGetMinmaxEXT( target, reset, format, type )

% glGetMinmaxEXT  Interface to OpenGL function glGetMinmaxEXT
%
% usage:  values = glGetMinmaxEXT( target, reset, format, type )
%
% C function:  void glGetMinmaxEXT(GLenum target, GLboolean reset, GLenum format, GLenum type, void* values)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

values = (0);

moglcore( 'glGetMinmaxEXT', target, reset, format, type, values );

return
