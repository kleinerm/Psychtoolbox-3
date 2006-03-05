function glPushClientAttrib( mask )

% glPushClientAttrib  Interface to OpenGL function glPushClientAttrib
%
% usage:  glPushClientAttrib( mask )
%
% C function:  void glPushClientAttrib(GLbitfield mask)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glPushClientAttrib', mask );

return
