function [ r, dataOut ] = gluScaleImage( format, wIn, hIn, typeIn, dataIn, wOut, hOut, typeOut )

% gluScaleImage  Interface to OpenGL function gluScaleImage
%
% usage:  [ r, dataOut ] = gluScaleImage( format, wIn, hIn, typeIn, dataIn, wOut, hOut, typeOut )
%
% C function:  GLint gluScaleImage(GLenum format, GLsizei wIn, GLsizei hIn, GLenum typeIn, const void* dataIn, GLsizei wOut, GLsizei hOut, GLenum typeOut, GLvoid* dataOut)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=8,
    error('invalid number of arguments');
end

dataOut = (0);

r = moglcore( 'gluScaleImage', format, wIn, hIn, typeIn, dataIn, wOut, hOut, typeOut, dataOut );

return
