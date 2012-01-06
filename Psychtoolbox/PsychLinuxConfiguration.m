function PsychLinuxConfiguration
% PsychLinuxConfiguration -- Optimize setup of Linux system.
%
% This script modifies system settings and configuration files
% to optimize a Linux system for use with Psychtoolbox.
%
% Currently it modifies files to allow to run Octave or Matlab
% as regular non-root user, ie. without need for root login or
% the "sudo" command. It does so by changing file permissions
% and resource usage limits to allow a regular user application
% to switch to realtime scheduling, lock its memory, and to
% access special purpose hardware like GPUs, Bits+, Datapixx and
% other research equipment.
%
% Realtime optimizations are achieved by extending the
% /etc/security/limits.conf file with entries that allow
% all users to lock application memory into physical RAM,
% eliminating/minimizing interference from the VM subsystem,
% and to run with realtime priorities up to level 50.
%
% root-less hardware access is achieved by copying a special
% psychtoolbox.rules file into the /etc/udev/rules.d/ directory.
% This udev rules file contains rules to auto-detect certain
% hardware at bootup or when the hw is hot-plugged and to
% reconfigure this hw or access permission for root-less access
% by Psychtoolbox, and for optimal performance for the kind
% of typical PTB use cases.
%
% The script calls into the shell via "sudo" to achieve this
% setup task, which itself needs admin privileges to modify
% system files etc. "sudo" will prompt the user for his admin
% password to complete the tasks.
%

% History:
% 6.01.2012   mk  Written.

if ~IsLinux
  return;
end

% Setup of /etc/udev/rules.d/psychtoolbox.rules file, if needed:

% Assume no need to install or update:
needinstall = 0;
fprintf('\n\nLinux specific system setup for running Psychtoolbox as non-root user:\n');
fprintf('You need to be a user with administrative rights for this function to succeed.\n');
fprintf('If you don''t have administrator rights, or if you don''t trust this script to\n');
fprintf('tinker around with system settings, simply answer all questions with "n" for "No"\n');
fprintf('and then call a system administrator for help.\n\n');
fprintf('Checking if the Psychtoolbox udev rules file is installed and up to date.\n');
fprintf('This file will allow Psychtoolbox to access special research hardware equipment,\n');
fprintf('e.g., the Cambridge Research Systems Bits+ box, the Datapixx from VPixx, response\n');
fprintf('button boxes, and some special features of your graphics card, e.g., high precision\n');
fprintf('timestamping. You will be able to access this hardware without the need to run\n');
fprintf('Matlab or Octave as sudo root user.\n\n');

% Check if udev psychtoolbox.rules file exists:
if ~exist('/etc/udev/rules.d/psychtoolbox.rules', 'file')
  % No: Needs to be installed.
  needinstall = 1;
  fprintf('The udev rules file for Psychtoolbox is not installed on your system.\n');
  answer = input('Should i install it? [y/n] : ', 's');
else
  % Yes.
  fprintf('The udev rules file for Psychtoolbox is already installed on your system.\n');

  % Compare its modification date with the one in PTB:
  r = dir([PsychtoolboxRoot '/PsychBasic/psychtoolbox.rules']);
  i = dir('/etc/udev/rules.d/psychtoolbox.rules');

  if r.datenum > i.datenum
    needinstall = 2;
    fprintf('However, it seems to be outdated. I have a more recent version with me.\n');
    answer = input('Should i update it? [y/n] : ', 's');
  end
end

if needinstall && answer == 'y'
  fprintf('I will copy my most recent rules file to your system. Please enter\n');
  fprintf('now your system administrator password. You will not see any feedback.\n');
  drawnow;

  cmd = sprintf('sudo cp %s/PsychBasic/psychtoolbox.rules /etc/udev/rules.d/', PsychtoolboxRoot);
  [rc, msg] = system(cmd);
  if rc == 0
    fprintf('Success! You may need to reboot your machine for some changes to take effect.\n');
  else
    fprintf('Failed! The error message was: %s\n', msg);
  end
end

% Check if /etc/security/limits.conf has proper entries to allow memory locking
% and real-time scheduling:
fid = fopen('/etc/security/limits.conf');
if fid == -1
  fprintf('Could not open /etc/security/limits.conf for reading. Can not set it up, sorry!\n');
  return;
end

fprintf('\nChecking if /etc/security/limits.conf has entries which allow everyone to\n');
fprintf('make use of realtime scheduling and memory locking -- Needed for good timing.\n\n');

mlockok = 0;
rtpriook = 0;

while ~feof(fid)
  fl = fgetl(fid);
  fprintf('%s\n', fl);

  if fl == -1
    continue;
  end

  if ~isempty(strfind(fl, 'memlock')) && ~isempty(strfind(fl, 'unlimited')) && ~isempty(strfind(fl, '*')) && ~isempty(strfind(fl, '-'))
    mlockok = 1;
  end

  if ~isempty(strfind(fl, 'rtprio')) && ~isempty(strfind(fl, '50')) && ~isempty(strfind(fl, '*')) && ~isempty(strfind(fl, '-'))
    rtpriook = 1;
  end
end

% Done reading the file:
fclose(fid);

drawnow;

% ...and?
if ~(mlockok && rtpriook)
  fprintf('\n\nThe file seems to be missing some suitable setup lines.\n');
  answer = input('Should i add them for you? [y/n] : ', 's');
  if answer == 'y'
    fprintf('I will try to add config lines to your system. Please enter\n');
    fprintf('now your system administrator password. You will not see any feedback.\n');
    drawnow;

    % Set amount of lockable memory to unlimited for all users:
    [rc, msg] = system('sudo /bin/bash -c ''echo "*     -     memlock     unlimited" >> /etc/security/limits.conf''');
    if rc ~= 0
      fprintf('Failed! The error message was: %s\n', msg);
    end

    % Set allowable realtime priority for all users to 50:
    [rc2, msg] = system('sudo /bin/bash -c ''echo "*     -     rtprio      50" >> /etc/security/limits.conf''');
    if rc2 ~= 0
      fprintf('Failed! The error message was: %s\n', msg);
    end

    if (rc == 0) && (rc2 == 0)
      fprintf('\n\nSuccess! You should be able to use the Priority() command now without the need\n');
      fprintf('to run Matlab or Octave as sudo root user. However, you must log out and then\n');
      fprintf('login again once for the settings to take effect.\n\n');
    else
      fprintf('\n\nFailed! Maybe ask a system administrator for help?\n\n');
    end
  end
else
  fprintf('\n\nYour system is already setup for use of Priority().\n\n');
end

fprintf('Finished. Assuming nothing went wrong, your system should now be ready\n');
fprintf('for use with Psychtoolbox and without need to run as root user, ie. sudo root.\n');
fprintf('If you encounter problems, try rebooting the machine. Some of the settings only\n');
fprintf('become effective after a reboot.\n\n\n');

return;
