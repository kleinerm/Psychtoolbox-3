function params = glGetBooleanv( pname )

% glGetBooleanv  Interface to glGetBooleanv
% 
% usage:  params = glGetBooleanv( pname )
%
% C function:  void glGetBooleanv(GLenum pname, GLboolean* params)

% 21-Dec-2005 -- created (moglgen)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

params = uint8(0);
moglcore( 'glGetBooleanv', pname, params );

return


% ---autocode---
%
% function params = glGetBooleanv( pname )
% 
% % glGetBooleanv  Interface to OpenGL function glGetBooleanv
% %
% % usage:  params = glGetBooleanv( pname )
% %
% % C function:  void glGetBooleanv(GLenum pname, GLboolean* params)
% 
% % 05-Mar-2006 -- created (generated automatically from header files)
% 
% % ---allocate---
% 
% if nargin~=1,
%     error('invalid number of arguments');
% end
% 
% params = uint8(0);
% 
% moglcore( 'glGetBooleanv', pname, params );
% 
% return
%
% ---skip---
