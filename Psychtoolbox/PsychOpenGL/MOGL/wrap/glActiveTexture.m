function glActiveTexture( texture )

% glActiveTexture  Interface to OpenGL function glActiveTexture
%
% usage:  glActiveTexture( texture )
%
% C function:  void glActiveTexture(GLenum texture)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glActiveTexture', texture );

return
