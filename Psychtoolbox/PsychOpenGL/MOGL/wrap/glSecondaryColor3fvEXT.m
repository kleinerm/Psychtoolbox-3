function glSecondaryColor3fvEXT( v )

% glSecondaryColor3fvEXT  Interface to OpenGL function glSecondaryColor3fvEXT
%
% usage:  glSecondaryColor3fvEXT( v )
%
% C function:  void glSecondaryColor3fvEXT(const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3fvEXT', single(v) );

return
