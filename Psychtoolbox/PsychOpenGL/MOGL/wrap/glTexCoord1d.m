function glTexCoord1d( s )

% glTexCoord1d  Interface to OpenGL function glTexCoord1d
%
% usage:  glTexCoord1d( s )
%
% C function:  void glTexCoord1d(GLdouble s)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord1d', s );

return
