function glClampColorARB( target, clamp )

% glClampColorARB  Interface to OpenGL function glClampColorARB
%
% usage:  glClampColorARB( target, clamp )
%
% C function:  void glClampColorARB(GLenum target, GLenum clamp)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glClampColorARB', target, clamp );

return
