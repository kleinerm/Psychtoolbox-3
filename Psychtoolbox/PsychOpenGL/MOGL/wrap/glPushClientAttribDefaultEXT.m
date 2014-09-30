function glPushClientAttribDefaultEXT( mask )

% glPushClientAttribDefaultEXT  Interface to OpenGL function glPushClientAttribDefaultEXT
%
% usage:  glPushClientAttribDefaultEXT( mask )
%
% C function:  void glPushClientAttribDefaultEXT(GLbitfield mask)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glPushClientAttribDefaultEXT', mask );

return
