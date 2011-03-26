function glPolygonMode( face, mode )

% glPolygonMode  Interface to OpenGL function glPolygonMode
%
% usage:  glPolygonMode( face, mode )
%
% C function:  void glPolygonMode(GLenum face, GLenum mode)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPolygonMode', face, mode );

return
