function glIndexFuncEXT( func, ref )

% glIndexFuncEXT  Interface to OpenGL function glIndexFuncEXT
%
% usage:  glIndexFuncEXT( func, ref )
%
% C function:  void glIndexFuncEXT(GLenum func, GLclampf ref)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glIndexFuncEXT', func, ref );

return
