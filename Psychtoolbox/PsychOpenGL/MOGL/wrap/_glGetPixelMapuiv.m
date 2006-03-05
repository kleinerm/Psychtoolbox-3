function values = glGetPixelMapuiv( map )

% glGetPixelMapuiv  Interface to OpenGL function glGetPixelMapuiv
%
% usage:  values = glGetPixelMapuiv( map )
%
% C function:  void glGetPixelMapuiv(GLenum map, GLuint* values)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

values = uint32(0);

moglcore( 'glGetPixelMapuiv', map, values );

return
