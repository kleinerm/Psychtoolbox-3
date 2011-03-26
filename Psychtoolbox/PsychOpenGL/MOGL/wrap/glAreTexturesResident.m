function [ r, residences ] = glAreTexturesResident( n, textures )

% glAreTexturesResident  Interface to glAreTexturesResident
% 
% usage:  [ r, residences ] = glAreTexturesResident( n, textures )
%         [ r, residences ] = glAreTexturesResident( textures )
%
% C function:  GLboolean glAreTexturesResident(GLsizei n, const GLuint* textures, GLboolean* residences)

% 21-Dec-2005 -- created (moglgen)

% ---allocate---
% ---protected---

if nargin==1,
    n=numel(textures),
elseif nargin~=2,
    error('invalid number of arguments');
end

residences = uint8(zeros(n,1));
r = moglcore( 'glAreTexturesResident', n, uint32(textures), residences );

return


% ---autocode---
%
% function [ r, residences ] = glAreTexturesResident( n, textures )
% 
% % glAreTexturesResident  Interface to OpenGL function glAreTexturesResident
% %
% % usage:  [ r, residences ] = glAreTexturesResident( n, textures )
% %
% % C function:  GLboolean glAreTexturesResident(GLsizei n, const GLuint* textures, GLboolean* residences)
% 
% % 05-Mar-2006 -- created (generated automatically from header files)
% 
% % ---allocate---
% 
% if nargin~=2,
%     error('invalid number of arguments');
% end
% 
% residences = uint8(0);
% 
% r = moglcore( 'glAreTexturesResident', n, uint32(textures), residences );
% 
% return
%
% ---skip---
