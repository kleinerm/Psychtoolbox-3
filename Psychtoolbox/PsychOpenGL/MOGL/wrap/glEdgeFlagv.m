function glEdgeFlagv( flag )

% glEdgeFlagv  Interface to OpenGL function glEdgeFlagv
%
% usage:  glEdgeFlagv( flag )
%
% C function:  void glEdgeFlagv(const GLboolean* flag)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glEdgeFlagv', uint8(flag) );

return
