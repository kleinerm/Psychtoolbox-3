function glStencilOp( fail, zfail, zpass )

% glStencilOp  Interface to OpenGL function glStencilOp
%
% usage:  glStencilOp( fail, zfail, zpass )
%
% C function:  void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glStencilOp', fail, zfail, zpass );

return
