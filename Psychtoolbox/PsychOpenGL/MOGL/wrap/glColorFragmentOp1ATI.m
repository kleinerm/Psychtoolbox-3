function glColorFragmentOp1ATI( op, dst, dstMask, dstMod, arg1, arg1Rep, arg1Mod )

% glColorFragmentOp1ATI  Interface to OpenGL function glColorFragmentOp1ATI
%
% usage:  glColorFragmentOp1ATI( op, dst, dstMask, dstMod, arg1, arg1Rep, arg1Mod )
%
% C function:  void glColorFragmentOp1ATI(GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glColorFragmentOp1ATI', op, dst, dstMask, dstMod, arg1, arg1Rep, arg1Mod );

return
