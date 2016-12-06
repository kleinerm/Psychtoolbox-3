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
values = uint32(repmat(intmax('uint32'),[ 32 1 ]));
moglcore( 'glGetPixelMapuiv', map, values );
values = values(find(values ~= intmax('uint32'))); %#ok<FNDSB>
return
% ---skip---
