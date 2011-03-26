function glBlendEquation( mode )

% glBlendEquation  Interface to OpenGL function glBlendEquation
%
% usage:  glBlendEquation( mode )
%
% C function:  void glBlendEquation(GLenum mode)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glBlendEquation', mode );

return
