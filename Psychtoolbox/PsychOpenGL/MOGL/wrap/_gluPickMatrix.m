function viewport = gluPickMatrix( x, y, delX, delY )

% gluPickMatrix  Interface to OpenGL function gluPickMatrix
%
% usage:  viewport = gluPickMatrix( x, y, delX, delY )
%
% C function:  void gluPickMatrix(GLdouble x, GLdouble y, GLdouble delX, GLdouble delY, GLint* viewport)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

viewport = int32(0);

moglcore( 'gluPickMatrix', x, y, delX, delY, viewport );

return
