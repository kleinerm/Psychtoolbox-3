function glBlendColor( red, green, blue, alpha )

% glBlendColor  Interface to OpenGL function glBlendColor
%
% usage:  glBlendColor( red, green, blue, alpha )
%
% C function:  void glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glBlendColor', red, green, blue, alpha );

return
