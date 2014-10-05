function glPushGroupMarkerEXT( length, marker )

% glPushGroupMarkerEXT  Interface to OpenGL function glPushGroupMarkerEXT
%
% usage:  glPushGroupMarkerEXT( length, marker )
%
% C function:  void glPushGroupMarkerEXT(GLsizei length, const GLchar* marker)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPushGroupMarkerEXT', length, uint8(marker) );

return
