function glInsertEventMarkerEXT( length, marker )

% glInsertEventMarkerEXT  Interface to OpenGL function glInsertEventMarkerEXT
%
% usage:  glInsertEventMarkerEXT( length, marker )
%
% C function:  void glInsertEventMarkerEXT(GLsizei length, const GLchar* marker)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glInsertEventMarkerEXT', length, uint8(marker) );

return
