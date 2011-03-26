function glDepthFunc( func )

% glDepthFunc  Interface to OpenGL function glDepthFunc
%
% usage:  glDepthFunc( func )
%
% C function:  void glDepthFunc(GLenum func)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glDepthFunc', func );

return
