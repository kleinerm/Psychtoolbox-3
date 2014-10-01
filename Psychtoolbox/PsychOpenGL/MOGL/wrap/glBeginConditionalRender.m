function glBeginConditionalRender( id, mode )

% glBeginConditionalRender  Interface to OpenGL function glBeginConditionalRender
%
% usage:  glBeginConditionalRender( id, mode )
%
% C function:  void glBeginConditionalRender(GLuint id, GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glBeginConditionalRender', id, mode );

return
