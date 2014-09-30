function glSetFenceAPPLE( fence )

% glSetFenceAPPLE  Interface to OpenGL function glSetFenceAPPLE
%
% usage:  glSetFenceAPPLE( fence )
%
% C function:  void glSetFenceAPPLE(GLuint fence)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSetFenceAPPLE', fence );

return
