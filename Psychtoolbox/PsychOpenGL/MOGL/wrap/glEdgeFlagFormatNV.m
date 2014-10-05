function glEdgeFlagFormatNV( stride )

% glEdgeFlagFormatNV  Interface to OpenGL function glEdgeFlagFormatNV
%
% usage:  glEdgeFlagFormatNV( stride )
%
% C function:  void glEdgeFlagFormatNV(GLsizei stride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glEdgeFlagFormatNV', stride );

return
