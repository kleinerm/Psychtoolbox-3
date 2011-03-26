function glEnable( cap )

% glEnable  Interface to OpenGL function glEnable
%
% usage:  glEnable( cap )
%
% C function:  void glEnable(GLenum cap)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glEnable', cap );

return
