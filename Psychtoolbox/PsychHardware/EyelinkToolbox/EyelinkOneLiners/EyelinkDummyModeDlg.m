function runindummymode=EyelinkDummyModeDlg

% puts up a dlg asking to run in dummy mode

%
%	HISTORY
%	27-11-02 fwc	created it as screen windows does not support 'dialog' function
%   280606  fwc        changed name for OSX version

ButtonName=questdlg('Run in dummy mode?', ...
                         'Eyelink not connected', ...
                         'Yes','Quit','Quit');
   
     switch ButtonName,
        case 'Yes', 
         disp('Running program in dummy mode');
		 runindummymode=1;
       case 'Quit',
         disp('Goodbye......')
		 runindummymode=0;
      end % switch
