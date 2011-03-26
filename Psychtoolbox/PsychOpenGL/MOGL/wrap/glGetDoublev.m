function params = glGetDoublev( pname )

% glGetDoublev  Interface to glGetDoublev
% 
% usage:  params = glGetDoublev( pname )
%
% C function:  void glGetDoublev(GLenum pname, GLdouble* params)

% 21-Dec-2005 -- created (moglgen)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

params = double(repmat(NaN,[ 32 1 ]));
moglcore( 'glGetDoublev', pname, params );
params = params(find(~isnan(params)));

return


% ---autocode---
%
% function params = glGetDoublev( pname )
% 
% % glGetDoublev  Interface to OpenGL function glGetDoublev
% %
% % usage:  params = glGetDoublev( pname )
% %
% % C function:  void glGetDoublev(GLenum pname, GLdouble* params)
% 
% % 05-Mar-2006 -- created (generated automatically from header files)
% 
% % ---allocate---
% 
% if nargin~=1,
%     error('invalid number of arguments');
% end
% 
% params = double(0);
% 
% moglcore( 'glGetDoublev', pname, params );
% 
% return
%
% ---skip---
