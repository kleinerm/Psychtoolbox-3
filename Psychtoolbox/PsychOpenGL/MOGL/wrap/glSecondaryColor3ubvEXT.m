function glSecondaryColor3ubvEXT( v )

% glSecondaryColor3ubvEXT  Interface to OpenGL function glSecondaryColor3ubvEXT
%
% usage:  glSecondaryColor3ubvEXT( v )
%
% C function:  void glSecondaryColor3ubvEXT(const GLubyte* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3ubvEXT', uint8(v) );

return
