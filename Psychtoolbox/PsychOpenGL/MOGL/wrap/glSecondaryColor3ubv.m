function glSecondaryColor3ubv( v )

% glSecondaryColor3ubv  Interface to OpenGL function glSecondaryColor3ubv
%
% usage:  glSecondaryColor3ubv( v )
%
% C function:  void glSecondaryColor3ubv(const GLubyte* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3ubv', uint8(v) );

return
