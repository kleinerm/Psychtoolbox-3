function glDepthRange( zNear, zFar )

% glDepthRange  Interface to OpenGL function glDepthRange
%
% usage:  glDepthRange( zNear, zFar )
%
% C function:  void glDepthRange(GLclampd zNear, GLclampd zFar)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDepthRange', zNear, zFar );

return
