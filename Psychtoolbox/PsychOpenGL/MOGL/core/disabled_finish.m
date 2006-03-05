function finish

% FINISH  Clean up after MOGL toolbox before quitting MATLAB
% 
% finish

% 26-Nov-2005 -- created (RFM)

% make sure 'update' process is running
if glmSetUpdate(1)==0,
	warning('unable to restart update process');
	pause;
end

return
