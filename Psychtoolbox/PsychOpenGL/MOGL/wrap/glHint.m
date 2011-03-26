function glHint( target, mode )

% glHint  Interface to OpenGL function glHint
%
% usage:  glHint( target, mode )
%
% C function:  void glHint(GLenum target, GLenum mode)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glHint', target, mode );

return
