function glDisable( cap )

% glDisable  Interface to OpenGL function glDisable
%
% usage:  glDisable( cap )
%
% C function:  void glDisable(GLenum cap)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glDisable', cap );

return
