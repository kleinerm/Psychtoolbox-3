function params = glGetVideoui64vNV( video_slot, pname )

% glGetVideoui64vNV  Interface to OpenGL function glGetVideoui64vNV
%
% usage:  params = glGetVideoui64vNV( video_slot, pname )
%
% C function:  void glGetVideoui64vNV(GLuint video_slot, GLenum pname, GLuint64EXT* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint64(0);

moglcore( 'glGetVideoui64vNV', video_slot, pname, params );

return
