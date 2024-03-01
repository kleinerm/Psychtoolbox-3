function EyelinkEraseCalTarget(el, rect)
% erase calibration target
%
% USAGE: EyelinkEraseCalTarget(el, rect)
%
%        el: eyelink default values
%        rect: rect that will be filled with background colour

warning('EyelinkToolbox:LegacyEraseCalTarget',['The function EyelinkEraseCalTarget() is deprecated. Please update your ', ...
    'script to use the current method for handling camera setup mode callbacks with PsychEyelinkDispatchCallback.m.']);
warning('off', 'EyelinkToolbox:LegacyEraseCalTarget');

if ~IsEmptyRect(rect)
    Screen( 'FillOval', el.window, el.backgroundcolour,  rect );
    Screen( 'Flip',  el.window);
end
