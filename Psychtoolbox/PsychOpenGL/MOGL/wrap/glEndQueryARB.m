function glEndQueryARB( target )

% glEndQueryARB  Interface to OpenGL function glEndQueryARB
%
% usage:  glEndQueryARB( target )
%
% C function:  void glEndQueryARB(GLenum target)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glEndQueryARB', target );

return
