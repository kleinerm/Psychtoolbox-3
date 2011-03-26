function mask = glGetPolygonStipple

% glGetPolygonStipple  Interface to OpenGL function glGetPolygonStipple
%
% usage:  mask = glGetPolygonStipple
%
% C function:  void glGetPolygonStipple(GLubyte* mask)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=0,
    error('invalid number of arguments');
end

mask = uint8(zeros(128,1));
moglcore( 'glGetPolygonStipple', mask );

return
% ---skip---
