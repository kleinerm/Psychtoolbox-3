function glTexCoord2d( s, t )

% glTexCoord2d  Interface to OpenGL function glTexCoord2d
%
% usage:  glTexCoord2d( s, t )
%
% C function:  void glTexCoord2d(GLdouble s, GLdouble t)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2d', s, t );

return
