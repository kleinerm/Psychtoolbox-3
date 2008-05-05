function [scal] = fineadjust(scal)

if exist('commandwindow') %#ok<EXIST>
    commandwindow;
end

pause(.5);

% open window and check whether that window has the same resolution as the
% one stored in the calibration data file
[scal.windowPtr,rect] = Screen('OpenWindow', scal.screenNumber); % windowPtr: psychtoolbox reference to the active drawing window, % rect: resolution of the active drawing window in pixels [x(1) y(1) x(end) y(end)]
if sum(rect-scal.rect)~=0,
    error('The resolution of the monitor has changed since the initialization of the spatial calibration.')
end

scal = manual_calibrate(scal); % this function will update the XCALIBDOTS en YCALIBDOTS
evstring = ['save ' PsychtoolboxConfigDir('tmp') 'currentCalib[' num2str(scal.screenNumber) '].mat'];
eval(evstring);
Screen('CloseAll');
ShowCursor;
