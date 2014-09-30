function glBlendFuncSeparatei( buf, srcRGB, dstRGB, srcAlpha, dstAlpha )

% glBlendFuncSeparatei  Interface to OpenGL function glBlendFuncSeparatei
%
% usage:  glBlendFuncSeparatei( buf, srcRGB, dstRGB, srcAlpha, dstAlpha )
%
% C function:  void glBlendFuncSeparatei(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glBlendFuncSeparatei', buf, srcRGB, dstRGB, srcAlpha, dstAlpha );

return
