function params = glGetFloatv( pname )

% glGetFloatv  Interface to glGetFloatv
% 
% usage:  params = glGetFloatv( pname )
%
% C function:  void glGetFloatv(GLenum pname, GLfloat* params)

% 21-Dec-2005 -- created (moglgen)

% ---allocate---
% ---protected---
% ---skip---

if nargin~=1,
    error('invalid number of arguments');
end

params = single(repmat(NaN,[ 32 1 ]));
moglcore( 'glGetFloatv', pname, params );
params = double(params);
params = params(find(~isnan(params))); %#ok<FNDSB>

return
