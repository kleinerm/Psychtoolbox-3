% % compatability notes
% %
% with the version released in July 2010, comes a small backward compatibility issue.
% To enable new functionality, much to the toolbox changed under the hood.
% Most important and most often requested feature has been added, namely
% the eye on the subject PC. Note that it is presently being shown without the
% yellow eye cursor.
% This requires a little different ordering of initializations, setting defaults and
% opening windows. Please check the supplied demo's to see how this is done.
% To make everything work how it used to do ,
% you'll need to add a parameter to EyelinkInit.
% So, use: 
% EyelinkInit(0,1);
% 
% USAGE: [result dummy]=EyelinkInit([dummy=0][enableCallbacks=0])
%  
%   Initialize Eyelink system and connection. Optional arguments:
%  
%   dummy: Omit, or set to 0 to attempt real initialization,
%  
%   enableCallbacks: Omit, or set to 0 for operation without callbacks and
%                    without display of eye camera images on the Subject PC.
%                    
% With these set, things should work as they used to (but you'll loose the eye-image!).
%
% Frans Cornelissen,
% Santa Barbara, 29th July 2010
