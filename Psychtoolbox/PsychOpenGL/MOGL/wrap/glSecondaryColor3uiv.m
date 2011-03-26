function glSecondaryColor3uiv( v )

% glSecondaryColor3uiv  Interface to OpenGL function glSecondaryColor3uiv
%
% usage:  glSecondaryColor3uiv( v )
%
% C function:  void glSecondaryColor3uiv(const GLuint* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3uiv', uint32(v) );

return
