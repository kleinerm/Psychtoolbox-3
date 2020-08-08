function glLoadIdentityDeformationMapSGIX( mask )

% glLoadIdentityDeformationMapSGIX  Interface to OpenGL function glLoadIdentityDeformationMapSGIX
%
% usage:  glLoadIdentityDeformationMapSGIX( mask )
%
% C function:  void glLoadIdentityDeformationMapSGIX(GLbitfield mask)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glLoadIdentityDeformationMapSGIX', mask );

return
