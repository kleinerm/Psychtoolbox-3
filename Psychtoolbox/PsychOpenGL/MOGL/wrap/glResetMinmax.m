function glResetMinmax( target )

% glResetMinmax  Interface to OpenGL function glResetMinmax
%
% usage:  glResetMinmax( target )
%
% C function:  void glResetMinmax(GLenum target)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glResetMinmax', target );

return
