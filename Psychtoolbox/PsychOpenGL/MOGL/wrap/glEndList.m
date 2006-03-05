function glEndList

% glEndList  Interface to OpenGL function glEndList
%
% usage:  glEndList
%
% C function:  void glEndList(void)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

moglcore( 'glEndList' );

return
