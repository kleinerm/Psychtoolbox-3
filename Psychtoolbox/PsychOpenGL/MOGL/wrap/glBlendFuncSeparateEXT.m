function glBlendFuncSeparateEXT( sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha )

% glBlendFuncSeparateEXT  Interface to OpenGL function glBlendFuncSeparateEXT
%
% usage:  glBlendFuncSeparateEXT( sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha )
%
% C function:  void glBlendFuncSeparateEXT(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glBlendFuncSeparateEXT', sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha );

return
