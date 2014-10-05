function glBeginConditionalRenderNVX( id )

% glBeginConditionalRenderNVX  Interface to OpenGL function glBeginConditionalRenderNVX
%
% usage:  glBeginConditionalRenderNVX( id )
%
% C function:  void glBeginConditionalRenderNVX(GLuint id)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glBeginConditionalRenderNVX', id );

return
