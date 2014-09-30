function glActiveTextureARB( texture )

% glActiveTextureARB  Interface to OpenGL function glActiveTextureARB
%
% usage:  glActiveTextureARB( texture )
%
% C function:  void glActiveTextureARB(GLenum texture)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glActiveTextureARB', texture );

return
