/*
  PsychToolbox2/Source/Common/Base/PsychVersioning.c		
    
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	This file should compile on all platforms.
    

  HISTORY:
  3/13/03  awi		Created. 
  
  DESCRIPTION:
  
  
 
  
*/


#include "Psych.h"


/*
    PsychGetBuildDate()

-This function gurantees that the date stamp stays constant thoughout the compile, though the __DATE__ macro might do that anyway.  Descriptions of __DATE__ which I can find do not specify if it does, and it could depend on the implementation.

-It All references to the date macro should be made through PsychGetBuildDate() because then we can refresh any time macro references within all
source files by recompiling only this one file.  Otherwise, we would either do an incremental build, which would not update the date, or a 
a clean build, which recompiles all files and takes forever.  
*/
char *PsychGetBuildDate(void)
{
    static char buildDate[]=__DATE__;
    
    return(buildDate);
}

/*
    PsychGetBuildTime()

-This function gurantees that the time stamp stays constant thoughout the compile, though the __TIME__ macro might do that anyway.  Descriptions __TIME__ which I can find do not specify if it does, and it could depend on the implementation.

-It All references to the time macro should be made through PsychGetBuildDate() because then we can refresh any time macro references within all
source files by recompiling only this one file.  Otherwise, we would either do an incremental build, which would not update the date, or a 
a clean build, which recompiles all files and takes forever.

*/
char *PsychGetBuildTime(void)
{
    static char buildTime[]=__TIME__;
    
    return(buildTime);
}

/*
    PsychGetBuildNumber()
    
    Returns a unique number with each build.  
*/
int PsychGetBuildNumber(void)
{
    int year, month, day, hour, minute, second;
    const char *buildDateStr, *buildTimeStr;     //"Feb 12 1996"  //"23:59:01"
    char monthStr[10];   //gcc just gives 3-letter abbr. but we don't know about all compilers".  
    char *monthNames[]={"Jan", "Feb", "Mar", "Apr", "May",  "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"}; 
    static int buildNumber;
    static psych_bool firstTime=TRUE;
    
    
    if(firstTime){
        //first scan
        buildDateStr=PsychGetBuildDate();
        buildTimeStr=PsychGetBuildTime();
        sscanf(buildTimeStr,"%d:%d:%d",&hour,&minute,&second);  //make sure this works with numbers beginning with "0".  
        sscanf(buildDateStr,"%s %d %d", monthStr, &day, &year);
        for(month=0;month< 12;month++){
            if(!strncmp(monthNames[month], monthStr,3)) 
                break;
            else if(month==11)
                PsychErrorExitMsg(PsychError_internal, "failed to identify the macro constant specifying the month");
        } 
        buildNumber=second + minute*60 + hour *60*60 + day*24*60*60 + month*31*24*60*60 + (year - 2003)*12*31*24*60*60; 
        firstTime=FALSE;
    }
    return(buildNumber);
 
}

/*
    PsychGetVersionString()
    
    The version string has form major.minor.build.  
*/
char *PsychGetVersionString(void)
{
    #define MAX_VERSION_STRING_LENGTH 100
    static char versionString[MAX_VERSION_STRING_LENGTH];
    static psych_bool firstTime=TRUE;
    int buildNumber;
    
    if(firstTime){
        buildNumber=PsychGetBuildNumber();
        sprintf(versionString, "%d.%d.%d.%d", PSYCHTOOLBOX_MAJOR_VERSION, PSYCHTOOLBOX_MINOR_VERSION,  PSYCHTOOLBOX_POINT_VERSION, buildNumber);
        firstTime=FALSE;
    }
    return(versionString);
}


/* PsychGetMajorVersionNumber()

	Get the major version number of the Psychtoolbox.  For the time being we just read it from a 
	macro within the PsychSource but eventually it should be read from the Contents.m file at run time because
	that follows the policy of having one canonical define from which all references are derived. That's better
	than keeping multiple copies in sync, one in contents .m and one in the source.  
*/
int PsychGetMajorVersionNumber(void)
{
	return(PSYCHTOOLBOX_MAJOR_VERSION);
}


/* PsychGetMinorVersionNumber()

	Get the minor version number of the Psychtoolbox.  See  PsychGetMajorVersionNumber. 
*/
int PsychGetMinorVersionNumber(void)
{
	return(PSYCHTOOLBOX_MINOR_VERSION);
}


/* PsychGetPointVersionNumber()

	Get the minor version number of the Psychtoolbox.  See  PsychGetMajorVersionNumber. 
*/
int PsychGetPointVersionNumber(void)
{
	return(PSYCHTOOLBOX_POINT_VERSION);
}


/* PsychSetModuleAuthor()

	Information about Psychtoolbox authors, such as their names and email addresss, is returned by the module command "version", along with 
	version numbers.  For example, "Screen('Version')".  For all Psychtoolbox mex files there is one canonical source for this information,
	the file MODULEVersion.c shared between all mex files which provide the "Version" command.  Becausue not all Psychtoolbox authors 
	are authors on all modules, we provide a mechanism to enable reporting of particular authors for particular modules.  From within
	the function PsychModuleInit in your module source call PsychSetModuleAuthor() once for each author which you want to register.  
	once for each author.  PsychSetModuleAuthor identifies authors by their initials, which you can find find in MODULEVersion.c.   
*/	
void PsychSetModuleAuthor(char *initials)
{
	

}


