function glDeformSGIX( mask )

% glDeformSGIX  Interface to OpenGL function glDeformSGIX
%
% usage:  glDeformSGIX( mask )
%
% C function:  void glDeformSGIX(GLbitfield mask)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glDeformSGIX', mask );

return
