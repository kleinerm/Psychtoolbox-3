function glFrontFace( mode )

% glFrontFace  Interface to OpenGL function glFrontFace
%
% usage:  glFrontFace( mode )
%
% C function:  void glFrontFace(GLenum mode)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glFrontFace', mode );

return
