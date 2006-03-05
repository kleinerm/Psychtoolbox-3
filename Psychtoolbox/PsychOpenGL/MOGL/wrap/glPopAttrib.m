function glPopAttrib

% glPopAttrib  Interface to OpenGL function glPopAttrib
%
% usage:  glPopAttrib
%
% C function:  void glPopAttrib(void)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

moglcore( 'glPopAttrib' );

return
