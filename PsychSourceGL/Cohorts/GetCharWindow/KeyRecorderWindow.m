//  Created by Allen Ingling on Fri Jun 18 2004.
//  Copyright (c) 2004 New York University. All rights reserved.

	
#import			"KeyRecorderWindow.h"

@implementation KeyRecorderWindow


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


- (void)enableKeypressCollection
{
	enableFlag=TRUE;
}


- (void)disableKeypressCollection
{
	enableFlag=FALSE;
}


- (void)dealloc
{
	if(characterList != NULL)
		[characterList release];
	[super dealloc];
}

/*
- (void)mouseDown:(NSEvent *)theEvent
{
	int x;
	
	x=100;

}
*/


- (void)keyDown:(NSEvent *)theEvent
{
	NSString			*keyCharacter;
	NSNumber			*timestamp, *tickCount, *keyCode; 
	NSDictionary		*tempEntry, *modifierFlags;
	unsigned int		rawEventModifierFlags;
	unsigned short		keyCodeUShort;


	if(enableFlag){
		tickCount=[NSNumber numberWithDouble:(double)TickCount()];
		keyCharacter=[theEvent characters];
		keyCodeUShort=[theEvent keyCode];
		keyCode=[NSNumber numberWithUnsignedShort:keyCodeUShort];
		//key modifier flag constants are defined by Cocoa.h, inclusion of which in a  mex file is barred by conflicts.  So we transfer flags back to the 
		//mex file as key-value CF array rathern than in a numeric value.  
		//Note: if we cared about economizing on memory usage, then we could retain the packed flag value and generate the 
		//dictionary from that only when accessed from outside, such as via ReadNextChar.
		//Note: also include the tickcount.
		rawEventModifierFlags=[theEvent modifierFlags];
		modifierFlags=[NSDictionary dictionaryWithObjectsAndKeys: 
							[NSNumber numberWithBool:(BOOL)((rawEventModifierFlags & NSAlphaShiftKeyMask) ? YES : NO)], @"NSAlphaShiftKeyMask", 
							[NSNumber numberWithBool:(BOOL)((rawEventModifierFlags & NSShiftKeyMask) ? YES : NO)], @"NSShiftKeyMask", 
							[NSNumber numberWithBool:(BOOL)((rawEventModifierFlags & NSControlKeyMask) ? YES : NO)], @"NSControlKeyMask", 
							[NSNumber numberWithBool:(BOOL)((rawEventModifierFlags & NSAlternateKeyMask) ? YES : NO)], @"NSAlternateKeyMask", 
							[NSNumber numberWithBool:(BOOL)((rawEventModifierFlags & NSCommandKeyMask) ? YES : NO)], @"NSCommandKeyMask", 
							[NSNumber numberWithBool:(BOOL)((rawEventModifierFlags & NSNumericPadKeyMask) ? YES : NO)], @"NSNumericPadKeyMask", 
							[NSNumber numberWithBool:(BOOL)((rawEventModifierFlags & NSHelpKeyMask) ? YES : NO)], @"NSHelpKeyMask", 
							[NSNumber numberWithBool:(BOOL)((rawEventModifierFlags & NSFunctionKeyMask) ? YES : NO)], @"NSFunctionKeyMask",
							nil];
		timestamp=[NSNumber numberWithDouble:(double)[theEvent timestamp]];
		tempEntry=[NSDictionary dictionaryWithObjectsAndKeys: 
							keyCharacter, @"character",
							timestamp, @"time",
							modifierFlags, @"modifierFlags",
							tickCount, @"tickCount",
							keyCode, @"keyCode",
							nil];
		[characterList addObject:tempEntry];	
	}
	
}


- (NSDictionary*)readNextChar
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


- (NSDictionary*)peekNextChar
{
	NSDictionary	*charDictionary;

	//return autoreleased and remove from the queue the first character.
	if([characterList count] == 0)
		return(NULL);
	else{
		charDictionary=[NSDictionary dictionaryWithDictionary:[characterList objectAtIndex:0]];
		return(charDictionary);
	}
}


- (NSArray*)readCharList
{
	NSArray		*tempCharacterList;
	
	tempCharacterList= [NSArray arrayWithArray:characterList];
	[characterList removeAllObjects];
	return(tempCharacterList);

}


- (NSArray*)peekCharList
{
	NSArray		*tempCharacterList;

	tempCharacterList= [NSArray arrayWithArray:characterList];
	return(tempCharacterList);
}


- (void)clearCharList
{
	[characterList removeAllObjects];
}


- (int)getCharListLength
{
	return((int)[characterList count]);
}


@end
