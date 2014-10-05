function glPathStencilFuncNV( func, ref, mask )

% glPathStencilFuncNV  Interface to OpenGL function glPathStencilFuncNV
%
% usage:  glPathStencilFuncNV( func, ref, mask )
%
% C function:  void glPathStencilFuncNV(GLenum func, GLint ref, GLuint mask)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPathStencilFuncNV', func, ref, mask );

return
