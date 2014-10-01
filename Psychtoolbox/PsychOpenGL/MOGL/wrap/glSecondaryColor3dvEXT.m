function glSecondaryColor3dvEXT( v )

% glSecondaryColor3dvEXT  Interface to OpenGL function glSecondaryColor3dvEXT
%
% usage:  glSecondaryColor3dvEXT( v )
%
% C function:  void glSecondaryColor3dvEXT(const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3dvEXT', double(v) );

return
