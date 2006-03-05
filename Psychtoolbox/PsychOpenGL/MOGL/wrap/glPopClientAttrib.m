function glPopClientAttrib

% glPopClientAttrib  Interface to OpenGL function glPopClientAttrib
%
% usage:  glPopClientAttrib
%
% C function:  void glPopClientAttrib(void)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

moglcore( 'glPopClientAttrib' );

return
