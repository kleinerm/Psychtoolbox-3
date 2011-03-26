function glSecondaryColor3fv( v )

% glSecondaryColor3fv  Interface to OpenGL function glSecondaryColor3fv
%
% usage:  glSecondaryColor3fv( v )
%
% C function:  void glSecondaryColor3fv(const GLfloat* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3fv', single(v) );

return
