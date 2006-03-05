function values = glGetMinmax( target, reset, format, type )

% glGetMinmax  Interface to OpenGL function glGetMinmax
%
% usage:  values = glGetMinmax( target, reset, format, type )
%
% C function:  void glGetMinmax(GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid* values)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

values = (0);

moglcore( 'glGetMinmax', target, reset, format, type, values );

return
