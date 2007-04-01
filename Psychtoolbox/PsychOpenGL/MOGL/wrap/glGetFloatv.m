function params = glGetFloatv( pname )

% glGetFloatv  Interface to glGetFloatv
% 
% usage:  params = glGetFloatv( pname )
%
% C function:  void glGetFloatv(GLenum pname, GLfloat* params)

% 21-Dec-2005 -- created (moglgen)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

params = moglsingle(repmat(NaN,[ 32 1 ]));
moglcore( 'glGetFloatv', pname, params );
params = mogldouble(params);
params = params(find(~isnan(params)));

return


% ---autocode---
%
% function params = glGetFloatv( pname )
% 
% % glGetFloatv  Interface to OpenGL function glGetFloatv
% %
% % usage:  params = glGetFloatv( pname )
% %
% % C function:  void glGetFloatv(GLenum pname, GLfloat* params)
% 
% % 05-Mar-2006 -- created (generated automatically from header files)
% 
% % ---allocate---
% 
% if nargin~=1,
%     error('invalid number of arguments');
% end
% 
% params = moglsingle(0);
% 
% moglcore( 'glGetFloatv', pname, params );
% 
% return
%
