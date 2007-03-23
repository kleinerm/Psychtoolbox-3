<?php
// parseptblog.php -- PHP parser for Psychtoolbox online registration log.
//
// Description: 
//
// This PHP script parses the Psychtoolbox online registration log,
// counts number of installations and other interesting numbers and
// outputs its results as HTML formatted text.
//
// It is supposed to be called from the Psychtoolbox Wiki to provide
// online statistics about PTB's real world use.
//
// Installation:
//
// 1. Change the $filename string to the full path and filename of the
//    Psychtoolbox online registration log file.
//
// 2. Copy this file into the "actions" subfolder of the Wiki installation.
//
// 3. Embed the following line of code into the appropriate Wiki page:
//    {{parseptblog}}
//
// 4. Done.
//
//
// History:
// 10/20/2006 Written. (MK)
// 11/03/2006 * Made more robust against corrupted lines in logfile.
//            * More detailled stats: Show distribution of Macintoshes.
//            * Improved HTML output formatting.

// Debug flag: If set to 1, outputs diagnostic output as well.
$debugmode = 0;

// Default filename for registration log file:
$filename = "/tmp/ptbregistrationlog.fixed";

if (file_exists($filename)===FALSE) {
   print "<br />The file $filename does not exist!<br />";
   return;
}

// Read logfile into $lines array:
$lines = file ($filename);

// Reset our counters:
$transactioncount = 0;
$totalcount = 0;
$betacount = 0;
$stablecount = 0;
$trunkcount = 0;
$unknowncount = 0;
$oldptb307count = 0;
$osxcount = 0;
$wincount = 0;
$linuxcount = 0;

$ppccount = 0;
$intelnativecount = 0;
$intelrosettacount = 0;

$intransaction = 0;
$linescount = 0;
$curline = 0;
$corruptcount = 0;

// For each input line $fl in array do:
foreach($lines as $fl){
  // print "$fl \n\n";

  // A new line has begun:
  $linescount++;

  // Check if input line contains a <MACID> --> Start of new transaction.
  $ls = strpos($fl, '<MACID>');
  if ($ls !== FALSE) {
    // <MACID> detected: Is this valid?
    if ($intransaction === 1) {
      // Invalid line! Tried to start a new transaction while old one still active
      // this must be a corrupted record in the logfile:
      if ($debugmode) printf("LOGPARSER-WARNING: Corrupt, unfinished transaction started at line $curline --> Skipped!<br />");

      // Increment counter of corrupt transactions:
      $corruptcount++;
    }

    // Reset processing to a sane state, so we can start a new transaction with the
    // current input line $fl:

    // Mark start of a new transaction:
    $intransaction=1;

    // Remember line number:
    $curline = $linescount;

    // Reset to empty transaction line:
    $ofl = '';
  }

  // Add current line to concatenated line:
  $ofl .= $fl;

  // End of line reached?
  $le = strpos($ofl, '</DATE>');
  if ($le === FALSE) {
    // Transaction line not complete. Just skip.
    // to next iteration.
  }
  else {
    // One transaction line finished. Do the stats.

    // This counts as one valid transaction:
    $transactioncount++;

    // Extract MACID as globally unique identifier of this user:
    $ls = strpos($ofl, '<MACID>');
    $le = strpos($ofl, '</MACID>');            
    $macid = substr($ofl, $ls, $le - $ls + 1);

    // Create an entry in the array of ids, using macid as key:
    $uniqueptbs[$macid]=$ofl;

    // Is this the very first entry?
    if ($transactioncount===1) {
      // Retrieve its date:
      $ls = strpos($ofl, '<DATE>');
      $le = strpos($ofl, '</DATE>');
      $firstdate = substr($ofl, $ls, $le - $ls);
    }

    // Reset to empty line:
    $ofl = '';

    // Transaction finished:
    $intransaction=0;    
  }
}

// Done with parsing the log file: Do the stats on all users.
foreach($uniqueptbs as $ofl) {
  // Increase total unique count:
  $totalcount++;

  // Reset assigned flag:
  $assigned = 0;

  // Now count by category:

  // Flavor:
  if (strpos($ofl, '<FLAVOR>beta</FLAVOR>')) { $assigned++ ; $betacount++; }
  if (strpos($ofl, '<FLAVOR>stable</FLAVOR>')) { $assigned++ ; $stablecount++; }
  if (strpos($ofl, '<FLAVOR>trunk</FLAVOR>')) { $assigned++ ; $trunkcount++; }
  if (strpos($ofl, '<FLAVOR>Psychtoolbox-3.0.7</FLAVOR>')) { $assigned++ ; $oldptb307count++; }
  if (strpos($ofl, '<FLAVOR>unknown</FLAVOR>')) { $assigned++ ; $unknowncount++; }

  // Operating system:
  if (strpos($ofl, '<OS>MacOS-X')) { $assigned++ ; $osxcount++; }
  if (strpos($ofl, '<OS>Windows')) { $assigned++ ; $wincount++; }
  if (strpos($ofl, '<OS>Linux')) { $assigned++ ; $linuxcount++; }

  // On MacOS-X by system architecture and type:
  if (strpos($ofl, '<OS>MacOS-X')) {
    if (strpos($ofl, '<CPUARCH>ppc')) {
      // It is a PowerPC Macintosh:
      $ppccount++;
    }
    else if (strpos($ofl, '<CPUARCH>i386')){
      // Intel Macintosh:
      $intelmaccount++;

      // What type?
      if (strpos($ofl, '<ENVARCH>MACI')){
	// IntelMac native:
	$intelnativecount++;
      }
      else {
	// Rosetta emulated:
	$intelrosettacount++;
      }
    }
    else {
      // Unclassified:
      $somemaccount++;
    }
  }

  if ($assigned < 2 && $debugmode > 0) print "LOGPARSER-WARNING: UNASSIGNED MACID: $ofl <br />";
  if ($assigned > 2 && $debugmode > 0) print "LOGPARSER-WARNING: DOUBLE-ASSIGNED MACID: $ofl <br />";
}


// Output the statistics as HTML formatted code: This is fed into the WikkaWikki engine:
print "<h3>";
print "<br /><br />Registered Psychtoolbox-3 installations as of " . date ("F d Y H:i:s.", filemtime($filename)) . "<br />";
print "<pre><br />";
print "Total number of downloads + updates     : $transactioncount<br />";
print "Earliest registration was at            : $firstdate<br />";

print "<br />";
print "Total number of unique installations    : $totalcount<br />";

print "<br />Breakdown by Psychtoolbox flavor:<br /><br />";
print "Number of 'beta'/'current' installations: $betacount<br />";
print "Number of 'stable' installations        : $stablecount<br />";
print "Number of 'trunk' installations         : $trunkcount<br />";
print "Number of unclassified installations    : $unknowncount<br />";
print "Number of Psychtoolbox-3.0.7 installs   : $oldptb307count<br />";

print "<br />Breakdown by host operating system:<br /><br />";
print "MacOS-X: $osxcount<br />";
print "Windows: $wincount<br />";
print "Linux  : $linuxcount<br />";

print "<br />For Macintosh - Breakdown by system architecture:<br /><br />";
print "PowerMac                     : $ppccount<br />";
print "IntelMac (Matlab w. Rosetta) : $intelrosettacount<br />";
print "IntelMac (Native Matlab beta): $intelnativecount<br />";
print "Unknown                      : $somemaccount<br />";
print "</pre></h3><pre>";
print "Parsed lines in registration log        : $linescount<br />";
print "Invalid (skipped) log entries           : $corruptcount<br />";
print "</pre>";

// Done.
?>
