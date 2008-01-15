function daq = DaqFind
% Syntax: DeviceIndex = DaqFind
%
% Purpose: To allow various Daq functions to run without explicitly being told
%          the index of the daq in PsychHID's enumeration.  Works iff there
%          is one device.
%
% History: 12/10/07   mpr   consolidated calls from other code
%
% see also DaqDeviceIndex and DaqReset

daq = DaqDeviceIndex;

if isempty(daq)
  error('No daq found');
elseif length(daq) > 1
  error('More than one daq found.  You must specify one of them.');
end

return;
