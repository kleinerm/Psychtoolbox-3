function values = glGetPixelMapuiv( map )

% glGetPixelMapuiv  Interface to OpenGL function glGetPixelMapuiv
%
% usage:  values = glGetPixelMapuiv( map )
%
% C function:  void glGetPixelMapuiv(GLenum map, GLuint* values)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

% a hack to find out how many values are returned
f=glGetPixelMapfv(map);

values = uint32(zeros(size(f)));
moglcore( 'glGetPixelMapuiv', map, values );

return
% ---skip---
