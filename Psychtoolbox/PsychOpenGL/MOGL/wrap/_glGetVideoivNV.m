function params = glGetVideoivNV( video_slot, pname )

% glGetVideoivNV  Interface to OpenGL function glGetVideoivNV
%
% usage:  params = glGetVideoivNV( video_slot, pname )
%
% C function:  void glGetVideoivNV(GLuint video_slot, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetVideoivNV', video_slot, pname, params );

return
