function glBlendFuncSeparateIndexedAMD( buf, srcRGB, dstRGB, srcAlpha, dstAlpha )

% glBlendFuncSeparateIndexedAMD  Interface to OpenGL function glBlendFuncSeparateIndexedAMD
%
% usage:  glBlendFuncSeparateIndexedAMD( buf, srcRGB, dstRGB, srcAlpha, dstAlpha )
%
% C function:  void glBlendFuncSeparateIndexedAMD(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glBlendFuncSeparateIndexedAMD', buf, srcRGB, dstRGB, srcAlpha, dstAlpha );

return
