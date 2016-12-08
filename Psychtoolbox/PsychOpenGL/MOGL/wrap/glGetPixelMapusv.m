function values = glGetPixelMapusv( map )

% glGetPixelMapusv  Interface to OpenGL function glGetPixelMapusv
%
% usage:  values = glGetPixelMapusv( map )
%
% C function:  void glGetPixelMapusv(GLenum map, GLushort* values)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

% a hack to find out how many values are returned
values = uint16(repmat(intmax('uint16'),[ 32 1 ]));
moglcore( 'glGetPixelMapusv', map, values );
values = values(find(values ~= intmax('uint16'))); %#ok<FNDSB>
return
% ---skip---
