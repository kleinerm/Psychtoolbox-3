function glTexCoord4d( s, t, r, q )

% glTexCoord4d  Interface to OpenGL function glTexCoord4d
%
% usage:  glTexCoord4d( s, t, r, q )
%
% C function:  void glTexCoord4d(GLdouble s, GLdouble t, GLdouble r, GLdouble q)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord4d', s, t, r, q );

return
