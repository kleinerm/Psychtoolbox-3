function glStringMarkerGREMEDY( len, string )

% glStringMarkerGREMEDY  Interface to OpenGL function glStringMarkerGREMEDY
%
% usage:  glStringMarkerGREMEDY( len, string )
%
% C function:  void glStringMarkerGREMEDY(GLsizei len, const void* string)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glStringMarkerGREMEDY', len, string );

return
