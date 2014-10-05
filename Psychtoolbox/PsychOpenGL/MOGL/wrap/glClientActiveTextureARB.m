function glClientActiveTextureARB( texture )

% glClientActiveTextureARB  Interface to OpenGL function glClientActiveTextureARB
%
% usage:  glClientActiveTextureARB( texture )
%
% C function:  void glClientActiveTextureARB(GLenum texture)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glClientActiveTextureARB', texture );

return
