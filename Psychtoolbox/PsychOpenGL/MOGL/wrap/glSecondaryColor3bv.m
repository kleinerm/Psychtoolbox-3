function glSecondaryColor3bv( v )

% glSecondaryColor3bv  Interface to OpenGL function glSecondaryColor3bv
%
% usage:  glSecondaryColor3bv( v )
%
% C function:  void glSecondaryColor3bv(const GLbyte* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3bv', int8(v) );

return
