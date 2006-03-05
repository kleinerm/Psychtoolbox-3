function params = glGetIntegerv( pname )

% glGetIntegerv  Interface to OpenGL function glGetIntegerv
%
% usage:  params = glGetIntegerv( pname )
%
% C function:  void glGetIntegerv(GLenum pname, GLint* params)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

% a hack to find out how many values are returned
f=glGetFloatv(pname);

params=int32(zeros(size(f)));
moglcore( 'glGetIntegerv', pname, params );

return


% ---autocode---
%
% function params = glGetIntegerv( pname )
% 
% % glGetIntegerv  Interface to OpenGL function glGetIntegerv
% %
% % usage:  params = glGetIntegerv( pname )
% %
% % C function:  void glGetIntegerv(GLenum pname, GLint* params)
% 
% % 05-Mar-2006 -- created (generated automatically from header files)
% 
% % ---allocate---
% 
% if nargin~=1,
%     error('invalid number of arguments');
% end
% 
% params = int32(0);
% 
% moglcore( 'glGetIntegerv', pname, params );
% 
% return
%
