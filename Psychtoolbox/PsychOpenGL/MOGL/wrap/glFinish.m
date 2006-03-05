function glFinish

% glFinish  Interface to OpenGL function glFinish
%
% usage:  glFinish
%
% C function:  void glFinish(void)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

moglcore( 'glFinish' );

return
