function glBufferPageCommitmentARB( target, offset, size, commit )

% glBufferPageCommitmentARB  Interface to OpenGL function glBufferPageCommitmentARB
%
% usage:  glBufferPageCommitmentARB( target, offset, size, commit )
%
% C function:  void glBufferPageCommitmentARB(GLenum target, GLintptr offset, GLsizeiptr size, GLboolean commit)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glBufferPageCommitmentARB', target, offset, size, commit );

return
