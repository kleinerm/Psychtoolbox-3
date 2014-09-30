function glStencilFuncSeparateATI( frontfunc, backfunc, ref, mask )

% glStencilFuncSeparateATI  Interface to OpenGL function glStencilFuncSeparateATI
%
% usage:  glStencilFuncSeparateATI( frontfunc, backfunc, ref, mask )
%
% C function:  void glStencilFuncSeparateATI(GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glStencilFuncSeparateATI', frontfunc, backfunc, ref, mask );

return
