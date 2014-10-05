function glClampColor( target, clamp )

% glClampColor  Interface to OpenGL function glClampColor
%
% usage:  glClampColor( target, clamp )
%
% C function:  void glClampColor(GLenum target, GLenum clamp)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glClampColor', target, clamp );

return
