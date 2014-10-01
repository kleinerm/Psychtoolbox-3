function glClearDepthfOES( depth )

% glClearDepthfOES  Interface to OpenGL function glClearDepthfOES
%
% usage:  glClearDepthfOES( depth )
%
% C function:  void glClearDepthfOES(GLclampf depth)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glClearDepthfOES', depth );

return
