function params = glGetVideoi64vNV( video_slot, pname )

% glGetVideoi64vNV  Interface to OpenGL function glGetVideoi64vNV
%
% usage:  params = glGetVideoi64vNV( video_slot, pname )
%
% C function:  void glGetVideoi64vNV(GLuint video_slot, GLenum pname, GLint64EXT* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int64(0);

moglcore( 'glGetVideoi64vNV', video_slot, pname, params );

return
