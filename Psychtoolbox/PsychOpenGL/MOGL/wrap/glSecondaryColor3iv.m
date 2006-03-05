function glSecondaryColor3iv( v )

% glSecondaryColor3iv  Interface to OpenGL function glSecondaryColor3iv
%
% usage:  glSecondaryColor3iv( v )
%
% C function:  void glSecondaryColor3iv(const GLint* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3iv', int32(v) );

return
