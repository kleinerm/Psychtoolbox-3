function glDetachObjectARB( containerObj, attachedObj )

% glDetachObjectARB  Interface to OpenGL function glDetachObjectARB
%
% usage:  glDetachObjectARB( containerObj, attachedObj )
%
% C function:  void glDetachObjectARB(GLhandleARB containerObj, GLhandleARB attachedObj)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDetachObjectARB', containerObj, attachedObj );

return
