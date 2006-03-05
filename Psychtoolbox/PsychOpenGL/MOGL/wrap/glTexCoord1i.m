function glTexCoord1i( s )

% glTexCoord1i  Interface to OpenGL function glTexCoord1i
%
% usage:  glTexCoord1i( s )
%
% C function:  void glTexCoord1i(GLint s)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord1i', s );

return
