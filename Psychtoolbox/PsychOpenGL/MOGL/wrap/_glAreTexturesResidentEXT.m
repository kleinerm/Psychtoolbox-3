function [ r, residences ] = glAreTexturesResidentEXT( n, textures )

% glAreTexturesResidentEXT  Interface to OpenGL function glAreTexturesResidentEXT
%
% usage:  [ r, residences ] = glAreTexturesResidentEXT( n, textures )
%
% C function:  GLboolean glAreTexturesResidentEXT(GLsizei n, const GLuint* textures, GLboolean* residences)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

residences = uint8(0);

r = moglcore( 'glAreTexturesResidentEXT', n, uint32(textures), residences );

return
