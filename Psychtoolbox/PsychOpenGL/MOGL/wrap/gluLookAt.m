function gluLookAt( eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ )

% gluLookAt  Interface to OpenGL function gluLookAt
%
% usage:  gluLookAt( eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ )
%
% C function:  void gluLookAt(GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ, GLdouble centerX, GLdouble centerY, GLdouble centerZ, GLdouble upX, GLdouble upY, GLdouble upZ)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'gluLookAt', eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ );

return
