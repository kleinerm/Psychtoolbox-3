function RaspberryPiGPIODemo
% RaspberryPiGPIODemo - Show basic use of GPIO's on RaspberryPi.
%
% Demos full access to the Pi GPIOs when running as root,
% slightly more limited access when running as a regular user,
% and also the different setup steps required.
%
% Accessing the GPIOs as non-root requires the user account
% to be a member of the Unix group 'gpio'. PsychLinuxConfiguration
% should take care of adding your account if you let it do that
% setup for you. The 'gpio' command line utility allows to configure
% aspects of the GPIOs which your code can't control as non-root,
% therefore you'll see multiple system() callouts in this script
% to that helper utility. If the RPiGPIOMex mex file doesn't work
% then you will need to install the 'wiringPi' package to provide
% the required libwiringPi runtime library.
%
% Shows digital i/o by flashing/alternating the two
% LEDs on the RPi 2B, Red power, and green status.
%
% Shows how to efficiently wait for a rising edge
% trigger on Broadcom pin 17, a real GPIO pin on the
% connector. To avoid the need to actually connect
% a switch to the connector, uses internal programmable
% pullup and pulldown resistors to simulate external
% trigger input to the pin.
%
% Please note that the RPiGPIOMex file is just meant as
% a temporary stop-gap solution to get things going, until
% PTB gains a universal I/O driver. Development and support
% of RPiGPIOMex may cease at a future point in time, it may
% even get removed from the distro! Plan accordingly, maybe
% use a M-File wrapper around it to simplify transition to a
% future better I/O driver.
%
% All pin numbers used with RPiGPIOMex are Broadcom GPIO pin
% numbers, not pin numbers on the connector or wiringPi pin numbers!
%
% A nice translation table between connector pins and Broadcom GPIO
% pins can be found on the following website: https://pinout.xyz
%

% History:
% 26-Jun-2016 mk  Written.
% 22-Jul-2016 mk  Clarify pin numbering more, reference https://pinout.xyz

% Get Pi revision. We only handle > 1 at the moment:
revision = RPiGPIOMex

if revision < 2
  error('Sorry, RaspberryPi models before revision 2 are not yet supported. ');
end

% Broadcom GPIO pin numbers for the led's on the RPi 2B and
% maybe later models? First gen had different pin numbers, hence
% the 2nd gen check above:
redled = 35;
greenled = 47;

% Decouple OS from LEDs so we are alone in the driver seat:
% Not needed for real GPIO pins on the connector...
system('echo none > /sys/class/leds/led0/trigger');
system('echo none > /sys/class/leds/led1/trigger');

% Set pin 17 to input with reception of rising edge triggers.
% This will fire an interrupt if a rising edge is detected.
% The RPiGPIOMex(5, pin, timeout) command can efficiently wait
% for such triggers that are reported via hardware interrupts:
system('gpio edge 17 rising');

% Pull the input pin down to logical low via internal pulldown resistor,
% to have a good starting point for simulating a trigger by using
% the internal pullup resistor to create a low->high transition.
% All gpio commands except 'edge' and 'export' operate on wiringPi
% logical pin numbers, not on Broadcom GPIO pin numbers. As we only
% use Broadcom numbering, we need to use the -g flag for most commands
% to tell 'gpio' to use the Broadcom numbering:
system('gpio -g mode 17 down');

% List all pins which are exported for use by non-root applications,
% and their configuration:
system('gpio exports')

% Are we effectively running as root?
if geteuid ~= 0
  % Nope. Need some help from our powerful command line friend:
  % Call gpio command line utility to export pins of
  % red led and green led for use by us without need
  % to be root. Set them to output mode.
  %
  % Note: This does not actually work for the builtin LEDs!
  % It only works for the real GPIO pins, so these lines are
  % for illustration only and you need to run 'sudo octave'
  % to control the LEDs.
  system(sprintf('gpio export %i out', redled));
  system(sprintf('gpio export %i out', greenled));
else
  % Yes. Can setup stuff ourselves. Switch pins to output mode:
  RPiGPIOMex(3, redled, 1);
  RPiGPIOMex(3, greenled, 1);
end

% Remember current LED state for restore at end:
oldRed = RPiGPIOMex(0, redled)
oldGreen = RPiGPIOMex(0, greenled)

% Alternate between red and green led for 10 seconds:
timeout = GetSecs + 10;
while GetSecs < timeout
  RPiGPIOMex(1, redled, 1);
  RPiGPIOMex(1, greenled, 0);
  WaitSecs(0.5);
  RPiGPIOMex(1, redled, 0);
  RPiGPIOMex(1, greenled, 1);
  WaitSecs(0.5);
end

% Restore original LED states:
RPiGPIOMex(1, redled, oldRed);
RPiGPIOMex(1, greenled, oldGreen);

% Restore OS control of leds, displaying disc activity
% and state of power supply and system health:
system('echo mmc0 > /sys/class/leds/led0/trigger');
system('echo input > /sys/class/leds/led1/trigger');

% Test for efficiently waiting for reception of a trigger
% signal on an input pin.
if geteuid == 0
  % Switch pin 17 to input:
  RPiGPIOMex(3, 17, 0);

  % Pull it low:
  RPiGPIOMex(4, 17, -1);
  statelow = RPiGPIOMex(0, 17)

  % Pull it high to simulate an external rising edge trigger:
  RPiGPIOMex(4, 17, +1);
  statehi = RPiGPIOMex(0, 17)

  % Pull it low again:
  RPiGPIOMex(4, 17, -1);
  statelow = RPiGPIOMex(0, 17)

  % Wait for a rising edge trigger for up to 10 seconds:
  % First wait will usually complete immediately due to
  % some recorded low->high transition during port setup.
  trigger1 = RPiGPIOMex(5, 17, 10000)
  % 2nd and 3rd waits will usually actually wait for a
  % real trigger, or simply time out after 10 seconds:
  trigger2 = RPiGPIOMex(5, 17, 10000)
  trigger3 = RPiGPIOMex(5, 17, 10000)

  % Should be high after trigger:
  posthi = RPiGPIOMex(0, 17)

  % Tristate it:
  RPiGPIOMex(4, 17, 0);
  statetri = RPiGPIOMex(0, 17)
else
  % Wait for a rising edge trigger for up to 10 seconds:
  % First wait will usually complete immediately due to
  % some recorded low->high transition during port setup.
  trigger1 = RPiGPIOMex(5, 17, 10000)

  % Wait for a rising edge trigger for up to 10 seconds:
  % 2nd wait will usually actually wait for a real trigger,
  % or simply time out after 10 seconds:
  trigger2 = RPiGPIOMex(5, 17, 10000)

  % Should be high after trigger:
  posthi = RPiGPIOMex(0, 17)
end

return;
