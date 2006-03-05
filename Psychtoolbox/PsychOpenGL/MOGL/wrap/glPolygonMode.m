function glPolygonMode( face, mode )

% glPolygonMode  Interface to OpenGL function glPolygonMode
%
% usage:  glPolygonMode( face, mode )
%
% C function:  void glPolygonMode(GLenum face, GLenum mode)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPolygonMode', face, mode );

return
