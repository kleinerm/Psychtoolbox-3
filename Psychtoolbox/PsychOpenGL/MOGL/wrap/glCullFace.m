function glCullFace( mode )

% glCullFace  Interface to OpenGL function glCullFace
%
% usage:  glCullFace( mode )
%
% C function:  void glCullFace(GLenum mode)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glCullFace', mode );

return
