function glBlendEquationEXT( mode )

% glBlendEquationEXT  Interface to OpenGL function glBlendEquationEXT
%
% usage:  glBlendEquationEXT( mode )
%
% C function:  void glBlendEquationEXT(GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glBlendEquationEXT', mode );

return
