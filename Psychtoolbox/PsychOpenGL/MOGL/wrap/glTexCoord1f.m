function glTexCoord1f( s )

% glTexCoord1f  Interface to OpenGL function glTexCoord1f
%
% usage:  glTexCoord1f( s )
%
% C function:  void glTexCoord1f(GLfloat s)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord1f', s );

return
