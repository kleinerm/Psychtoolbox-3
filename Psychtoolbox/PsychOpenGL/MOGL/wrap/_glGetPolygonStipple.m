function mask = glGetPolygonStipple

% glGetPolygonStipple  Interface to OpenGL function glGetPolygonStipple
%
% usage:  mask = glGetPolygonStipple
%
% C function:  void glGetPolygonStipple(GLubyte* mask)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=0,
    error('invalid number of arguments');
end

mask = uint8(0);

moglcore( 'glGetPolygonStipple', mask );

return
