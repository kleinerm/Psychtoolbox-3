function params = glGetVideouivNV( video_slot, pname )

% glGetVideouivNV  Interface to OpenGL function glGetVideouivNV
%
% usage:  params = glGetVideouivNV( video_slot, pname )
%
% C function:  void glGetVideouivNV(GLuint video_slot, GLenum pname, GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint32(0);

moglcore( 'glGetVideouivNV', video_slot, pname, params );

return
