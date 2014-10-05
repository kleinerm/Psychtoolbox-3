function glClipControl( origin, depth )

% glClipControl  Interface to OpenGL function glClipControl
%
% usage:  glClipControl( origin, depth )
%
% C function:  void glClipControl(GLenum origin, GLenum depth)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glClipControl', origin, depth );

return
