function glBlendFuncSeparate( srcRGB, dstRGB, srcAlpha, dstAlpha )

% glBlendFuncSeparate  Interface to OpenGL function glBlendFuncSeparate
%
% usage:  glBlendFuncSeparate( srcRGB, dstRGB, srcAlpha, dstAlpha )
%
% C function:  void glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glBlendFuncSeparate', srcRGB, dstRGB, srcAlpha, dstAlpha );

return
