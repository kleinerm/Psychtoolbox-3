function XOrgConfSelector(sdir)
% XOrgConfSelector([sdir]) - Select a X11 configuration file to apply.
%
% By default configuration files are chosen from a PsychtoolboxConfigDir
% subfolder, where XOrgConfCreator creates and stores them by default.
%
% You can override the choice of the directory for config files by
% providing the optional 'sdir' argument with the path to a different
% folder.
%
% The chosen configuration file is copied into the systems
% /etc/X11/xorg.conf.d/ folder for the X-Server to pick it up and apply it
% during the next logout->login cycle or restart.
%

% History:
% 04-Nov-2015  mk  Created.
% 25-Apr-2016  mk  Add troubleshooting tips in case somebody needs them.
% 02-Feb-2024  mk  Add error handling and troubleshooting tips in case of error.

  if nargin < 1 || isempty(sdir)
    sdir = PsychtoolboxConfigDir ('XorgConfs');
  end

  conffiles = {};
  candfiles = dir(sdir);
  for i=1:length(candfiles)
    if ~candfiles(i).isdir
      conffiles{end+1} = candfiles(i).name;
    end
  end

  fprintf('You can choose from the following X11 configuration files:\n\n');
  for i=1:length(conffiles)
    fprintf('%i) %s\n', i, conffiles{i});
  end
  fprintf('\n');
  answer = [];
  while isempty(answer) || ~ismember(answer, 0:length(conffiles))
    answer = input('Which one should I use? (Choose 0 to remove a current active file): ');
  end
  fprintf('\n\n');

  updated = 0;

  if answer == 0
    % Remove existing file:
    if exist('/etc/X11/xorg.conf.d/90-ptbxorg.conf')
      delete('/etc/X11/xorg.conf.d/90-ptbxorg.conf');
      if exist('/etc/X11/xorg.conf.d/90-ptbxorg.conf')
        fprintf('Delete operation failed. See warning message above this line.\nPlease try to fix the error and retry.\n');
        fprintf('Running the command PsychLinuxConfiguration might help, if you forgot to do it\n');
        fprintf('after installing Psychtoolbox from Debian, Ubuntu or NeuroDebian.\n\n');
        fprintf('Failed to remove the configuration file from the X11 config folder under:\n');
      else
        fprintf('Removed the old existing xorg.conf file from the X11 config folder under:\n');
        updated = 1;
      end
    else
      fprintf('There isn''t any old configuration file to remove in the X11 config folder under:\n');
    end
  else
    % Copy selected file into the X11 config directory:
    [rc, err] = copyfile([sdir conffiles{answer}], '/etc/X11/xorg.conf.d/90-ptbxorg.conf');
    if rc
      fprintf('Copied the xorg.conf file into the X11 config folder under:\n');
      updated = 1;
    else
      if isempty(err)
        err = 'See system error message above this line';
      else
        err = ['System error message is: ' err];
      end
      fprintf('Copy operation failed. %s.\nPlease try to fix the error and retry.\n', err);
      fprintf('Running the command PsychLinuxConfiguration might help, if you forgot to do it\n');
      fprintf('after installing Psychtoolbox from Debian, Ubuntu or NeuroDebian.\n\n');
      fprintf('Failed to copy the xorg.conf file into the X11 config folder under:\n');
    end
  end

  fprintf('/etc/X11/xorg.conf.d/90-ptbxorg.conf\n\n');

  if updated
    fprintf('Troubleshooting tip in case of emergency:\n\n');
    fprintf('In case something goes seriously wrong and you end up with a non-functioning GUI,\n');
    fprintf('you can always press(*) ''CTRL+ALT+F3'', which provides you with a text console. Then login\n');
    fprintf('there, run octave or matlab from the text console and call XOrgConfSelector to remove\n');
    fprintf('the troublesome config file. Or type this: rm /etc/X11/xorg.conf.d/90-ptbxorg.conf\n\n');
    fprintf('(*)Apple computer users may need to press ''FN+CTRL+ALT+F3'' instead of CTRL+ALT+F3\n\n');
    fprintf('Now please logout and login again for the new configuration to take effect.\n\n');
  end
end
