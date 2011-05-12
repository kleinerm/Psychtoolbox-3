function EyelinkUpdateDefaults(el)

% USAGE = EyelinkUpdateDefaults(el) 
%
% This function passes changes to the calibration defaults structure
% to the callback function. To be called after EyelinkInitDefaults if any
% changes are made to the output structure of that function.
%
% 
% el=EyelinkInitSRDefaults(window);
% el.backgroundColour = BlackIndex(window);
% EyelinkUpdateDefaults(el);
%
%
%

% 27-1-2011 NJ created 



% Window assigned?
%if ~isempty(el.window) & ~isempty(el.callback) %#ok<AND2>
if ~isempty(el.callback)
    % Yes. Assign it to our dispatch callback:
%     EyelinkDispatchCallback(el);
    PsychEyelinkDispatchCallback(el);
end


end