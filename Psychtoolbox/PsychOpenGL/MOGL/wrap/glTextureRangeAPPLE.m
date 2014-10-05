function glTextureRangeAPPLE( target, length, pointer )

% glTextureRangeAPPLE  Interface to OpenGL function glTextureRangeAPPLE
%
% usage:  glTextureRangeAPPLE( target, length, pointer )
%
% C function:  void glTextureRangeAPPLE(GLenum target, GLsizei length, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTextureRangeAPPLE', target, length, pointer );

return
