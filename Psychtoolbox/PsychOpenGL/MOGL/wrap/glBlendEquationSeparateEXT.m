function glBlendEquationSeparateEXT( modeRGB, modeAlpha )

% glBlendEquationSeparateEXT  Interface to OpenGL function glBlendEquationSeparateEXT
%
% usage:  glBlendEquationSeparateEXT( modeRGB, modeAlpha )
%
% C function:  void glBlendEquationSeparateEXT(GLenum modeRGB, GLenum modeAlpha)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glBlendEquationSeparateEXT', modeRGB, modeAlpha );

return
