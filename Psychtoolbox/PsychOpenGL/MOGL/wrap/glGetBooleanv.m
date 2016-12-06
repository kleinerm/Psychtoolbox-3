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

params = uint8([0,0]);
moglcore( 'glGetBooleanv', pname, params );
params = params(1:end-1);
return
