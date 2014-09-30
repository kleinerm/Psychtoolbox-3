function glMultiDrawArraysIndirectCountARB( mode, indirect, drawcount, maxdrawcount, stride )

% glMultiDrawArraysIndirectCountARB  Interface to OpenGL function glMultiDrawArraysIndirectCountARB
%
% usage:  glMultiDrawArraysIndirectCountARB( mode, indirect, drawcount, maxdrawcount, stride )
%
% C function:  void glMultiDrawArraysIndirectCountARB(GLenum mode, GLintptr indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMultiDrawArraysIndirectCountARB', mode, indirect, drawcount, maxdrawcount, stride );

return
