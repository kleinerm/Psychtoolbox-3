function glSecondaryColor3sv( v )

% glSecondaryColor3sv  Interface to OpenGL function glSecondaryColor3sv
%
% usage:  glSecondaryColor3sv( v )
%
% C function:  void glSecondaryColor3sv(const GLshort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3sv', int16(v) );

return
