function glPushClientAttrib( mask )

% glPushClientAttrib  Interface to OpenGL function glPushClientAttrib
%
% usage:  glPushClientAttrib( mask )
%
% C function:  void glPushClientAttrib(GLbitfield mask)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glPushClientAttrib', mask );

return
