function glFinishFenceAPPLE( fence )

% glFinishFenceAPPLE  Interface to OpenGL function glFinishFenceAPPLE
%
% usage:  glFinishFenceAPPLE( fence )
%
% C function:  void glFinishFenceAPPLE(GLuint fence)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glFinishFenceAPPLE', fence );

return
