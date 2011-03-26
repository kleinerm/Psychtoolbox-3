function glAlphaFunc( func, ref )

% glAlphaFunc  Interface to OpenGL function glAlphaFunc
%
% usage:  glAlphaFunc( func, ref )
%
% C function:  void glAlphaFunc(GLenum func, GLclampf ref)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glAlphaFunc', func, ref );

return
