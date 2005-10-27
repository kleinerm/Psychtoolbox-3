//  Created by Allen Ingling on Fri Jun 18 2004.
//  Copyright (c) 2004 New York University. All rights reserved.

	
#import			"KeyRecorderWindow.h"

@implementation KeyRecorderWindow


- (void)enableGather
{
	enableFlag=TRUE;
}

- (void)disableGather
{
	enableFlag=FALSE;
}


- (id)init
{
    if (self = [super init]) {
		enableFlag=FALSE;
		characterList=[[NSMutableArray alloc] initWithCapacity:1];
	}
    return(self);
}


- (id)initWithContentRect:(NSRect)contentRect styleMask:(unsigned int)styleMask backing:(NSBackingStoreType)backingType defer:(BOOL)flag
{
    if (self = [super initWithContentRect:contentRect styleMask:styleMask backing:backingType defer:flag]) {
		enableFlag=FALSE;
		characterList=[[NSMutableArray alloc] initWithCapacity:1];
	}
    return(self);

}



- (void)dealloc
{
	if(characterList != NULL)
		[characterList release];
	[super dealloc];
}

- (void)keyDown:(NSEvent *)theEvent
{
	NSString			*keyCharacter;
	NSNumber			*timestamp;
	NSDictionary		*tempEntry;

	if(enableFlag){
		keyCharacter=[theEvent characters];
		timestamp=[NSNumber numberWithDouble:[theEvent timestamp]];
		tempEntry=[NSDictionary dictionaryWithObjects:[NSArray arrayWithObjects: keyCharacter, timestamp, nil] 
								forKeys:[NSArray arrayWithObjects:@"character", @"time", nil]];
		[characterList addObject:tempEntry];	
	}
	
}



- (NSDictionary*)getNextChar;
{
	NSDictionary	*charDictionary;

	//return autoreleased and remove from the queue the first character.
	if([characterList count] == 0)
		return(NULL);
	else{
		charDictionary=[NSDictionary dictionaryWithDictionary:[characterList objectAtIndex:0]];
		[characterList removeObjectAtIndex:0];
		return(charDictionary);
	}
}


- (void)clearQueue;
{
	[characterList removeAllObjects];
}


@end
