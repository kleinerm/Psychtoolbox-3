function glAttachObjectARB( containerObj, obj )

% glAttachObjectARB  Interface to OpenGL function glAttachObjectARB
%
% usage:  glAttachObjectARB( containerObj, obj )
%
% C function:  void glAttachObjectARB(GLhandleARB containerObj, GLhandleARB obj)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glAttachObjectARB', containerObj, obj );

return
