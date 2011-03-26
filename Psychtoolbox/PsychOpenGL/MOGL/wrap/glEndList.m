function glEndList

% glEndList  Interface to OpenGL function glEndList
%
% usage:  glEndList
%
% C function:  void glEndList(void)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

moglcore( 'glEndList' );

return
