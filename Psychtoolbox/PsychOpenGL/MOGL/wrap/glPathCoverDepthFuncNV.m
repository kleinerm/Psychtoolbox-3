function glPathCoverDepthFuncNV( func )

% glPathCoverDepthFuncNV  Interface to OpenGL function glPathCoverDepthFuncNV
%
% usage:  glPathCoverDepthFuncNV( func )
%
% C function:  void glPathCoverDepthFuncNV(GLenum func)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glPathCoverDepthFuncNV', func );

return
