function glAlphaFragmentOp1ATI( op, dst, dstMod, arg1, arg1Rep, arg1Mod )

% glAlphaFragmentOp1ATI  Interface to OpenGL function glAlphaFragmentOp1ATI
%
% usage:  glAlphaFragmentOp1ATI( op, dst, dstMod, arg1, arg1Rep, arg1Mod )
%
% C function:  void glAlphaFragmentOp1ATI(GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glAlphaFragmentOp1ATI', op, dst, dstMod, arg1, arg1Rep, arg1Mod );

return
