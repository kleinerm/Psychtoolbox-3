function el=EyelinkInitDefaults(window)

% initialize eyelink defaults and control code structure
%
% USAGE: el=EyelinkInitDefaults([window])
%
%       window is optional windowPtr.
%       If set, pixel coordinates are send to eyetracker

% and fill it with some sensible values
% note that these values are only used by the m-file
% versions of dotrackersetup and dodriftcorrect.
%
% 02-06-01	fwc created, as suggested by John Palmer.
%				added also all control codes and defaults
% 17-10-02	fwc added event types
% 26-11-02  fwc&emp added PC support
% 11-01-04  fwc OS X changes
% 22-06-06  fwc further OSX changes
% 31-10-06  mk  Unified keyname mapping and such...

el=[];

% eyelink computer check
el.computer = computer;
%if ~(strcmp(el.computer,'PCWIN')==1 | strcmp(el.computer,'MAC2')==1 | strcmp(el.computer,'MAC')==1)
if ~(IsOSX | IsWin)
    disp([el.computer,' is not a supported computer type!']);
    return;
end

% Enable unified keyname -> keycode mapping for all operating systems:
KbName('UnifyKeyNames');

% eye codes
el.LEFT_EYE=0;
el.RIGHT_EYE=1;
el.BINOCULAR=2;

% eyelink connection states
el.notconnected=0;
el.connected=1;
el.dummyconnected=-1;
el.broadcastconnected=2;

if exist('window', 'var') & ~isempty(window)
    el.window=window;
    el.backgroundcolour = WhiteIndex(el.window);
    el.foregroundcolour = BlackIndex(el.window);

    % eyelink('initwindow', el.window); % just to make sure something is set

    rect=Screen(el.window,'Rect');
    if Eyelink('IsConnected') ~= el.notconnected
        Eyelink('Command', 'screen_pixel_coords = %d %d %d %d',rect(1),rect(2),rect(3)-1,rect(4)-1);
    end
else
    el.window=[];
end

% set some more global info parameters
el.targetdisplaysound='EyelinkTargetBeep';
el.calibrationfailedsound='EyelinkErrorBeep';
el.calibrationsuccesssound='EyelinkSuccessBeep';
el.targetbeep=1;  % sound a beep when a target is presented
el.allowlocaltrigger=1; % allow user to trigger him or herself
el.allowlocalcontrol=1; % allow control from subject-computer
el.mousetriggersdriftcorr=0; % 1=allow mouse to trigger drift correction (fwc trick)
el.quitkey=KbName('ESCAPE'); % when pressed in combination with modifier key
% forces getkeyforeyelink to return 'TERMINATE_KEY' !

%if strcmp(el.computer,'PCWIN')==1
%    el.modifierkey=KbName('alt');
%elseif strcmp(el.computer,'MAC2')==1
%    el.modifierkey=KbName('apple');
%elseif strcmp(el.computer,'MAC')==1

% Modifier key is always LeftGUI due to unified keyname mapping:
el.modifierkey=KbName('LeftGUI');

%else
%    disp([el.computer,' not a supported computer type!']);
%    return;
%end

el.waitformodereadytime=500;
el.calibrationtargetsize=2;  % size of calibration target as percentage of screen
el.calibrationtargetwidth=.75; % width of calibration target's border as percentage of screen

el.getkeyrepeat=1/5; % "sample time" for eyelinkgetkey
el.getkeytime=-1; % stores last time eyelinkgetkey was used

[keyIsDown,secs,el.lastKeyCodes] = KbCheck;

% keyCodes for EyelinkGetKey
% if strcmp(el.computer,'MAC')==1 % OSX
    el.uparrow=KbName('UpArrow');
    el.downarrow=KbName('DownArrow');
    el.rightarrow=KbName('RightArrow');
    el.leftarrow=KbName('LeftArrow');
    el.pageup=KbName('PageUp');
    el.pagedown=KbName('PageDown');
    el.return=KbName('Return');
    el.escape=KbName('ESCAPE');
    el.space=KbName('space');
    el.backspace=KbName('DELETE'); % is this delete backspace?
    if IsOSX
    % OS-X supports a separate keycode for the Enter key:
        el.enter=KbName('ENTER');
    else
        % M$-Windows and GNU/Linux don't have a separate code for Enter,
        % so we will map it to the 'Return' key:
        el.enter=el.return;
    end
    el.keysCached=1;
% else
%     el.keysCached=0;
% end

% since we do not actually remove keypresses the matlab buffer gets filled
% up quickly. Hence we type:
warning off MATLAB:namelengthmaxexceeded

% eyelink Tracker state bit: AND with flag word to test functionality

el.IN_DISCONNECT_MODE=16384;   	% disconnected
el.IN_UNKNOWN_MODE=0;    		% mode fits no class (i.e setup menu)
el.IN_IDLE_MODE=1;    			% off-line
el.IN_SETUP_MODE=2;   			% setup or cal/val/dcorr
el.IN_RECORD_MODE=4;    		% data flowing
el.IN_TARGET_MODE=8;    		% some mode that needs fixation targets
el.IN_DRIFTCORR_MODE=16;   		% drift correction
el.IN_IMAGE_MODE=32;   			% image-display mode
el.IN_USER_MENU=64;				% user menu
el.IN_PLAYBACK_MODE=256;		% tracker sending playback data

% eyelink key values
el.JUNK_KEY=1;		% return code for untranslatable key
el.TERMINATE_KEY=hex2dec('7FFF'); % return code for program exit/breakout key
el.CURS_UP=hex2dec('4800');
el.CURS_DOWN=hex2dec('5000');
el.CURS_LEFT=hex2dec('4B00');
el.CURS_RIGHT=hex2dec('4D00');
el.ESC_KEY=hex2dec('001B');
el.ENTER_KEY=hex2dec('000D');
el.PAGE_UP=hex2dec('4900');
el.PAGE_DOWN=hex2dec('5100');
el.SPACE_BAR=32;

% other eyelink values

el.KB_PRESS=10; % Eyelink.h
el.MISSING=-32768; % eyedata.h
el.MISSING_DATA=-32768;

% LINK RETURN CODES
el.NO_REPLY=1000; % no reply yet (for polling test)

% return codes for trial result
el.DONE_TRIAL=0;
el.TRIAL_OK=0;
el.REPEAT_TRIAL=1;
el.SKIP_TRIAL=2;
el.ABORT_EXPT=3;

el.TRIAL_ERROR=-1; 	% Bad trial: no data, etc.


% EVENT types
el.SAMPLE_TYPE=200;

el.STARTPARSE=1; % 	/* these only have time and eye data */
el.ENDPARSE=2;
el.BREAKPARSE=10;

el.STARTBLINK=3;    % /* EYE DATA: contents determined by evt_data */
el.ENDBLINK=4;   	% /* and by "read" data item */
el.STARTSACC=5;		% /* all use IEVENT format */
el.ENDSACC=6;
el.STARTFIX=7;
el.ENDFIX=8;
el.FIXUPDATE=9;

el.STARTSAMPLES=15;  	%/* start of events in block *//* control events: all put data into */
el.ENDSAMPLES=16;  		%/* end of samples in block *//* the EDF_FILE or ILINKDATA status  */
el.STARTEVENTS=17; 		% /* start of events in block */
el.ENDEVENTS=18;  		%/* end of events in block */

el.MESSAGEEVENT=24;  % /* user-definable text or data */
el.BUTTONEVENT=25;  %/* button state change */
el.INPUTEVENT=28;  % /* change of input port */





% el % uncomment to show contents of this default eyelink structure
