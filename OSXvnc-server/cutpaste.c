/*
 * cutpaste.c - routines to deal with cut & paste buffers / selection.
 */

/*
 *  OSXvnc Copyright (C) 2001 Dan McGuirk <mcguirk@incompleteness.net>.
 *  Original Xvnc code Copyright (C) 1999 AT&T Laboratories Cambridge.  
 *  All Rights Reserved.
 *
 *  This is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 *  USA.
 */

#include <Cocoa/Cocoa.h>

#include <stdio.h>
#include "rfb.h"

int pasteBoardLastChangeCount=-1;

/*
 * rfbSetXCutText sets the cut buffer to be the given string.  We also clear
 * the primary selection.  Ideally we'd like to set it to the same thing, but I
 * can't work out how to do that without some kind of helper X client.
 */

void
rfbSetXCutText(char *str, int len)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSString *clientCutText = [[NSString alloc] initWithCString:str length:len];

    [[NSPasteboard generalPasteboard] declareTypes:[NSArray arrayWithObject:NSStringPboardType] owner:nil];
    [[NSPasteboard generalPasteboard] setString:clientCutText forType:NSStringPboardType];

    pasteBoardLastChangeCount = [[NSPasteboard generalPasteboard] changeCount]; // Don't need to send it back
    [pool release];
}


void rfbCheckForPasteboardChange()
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    // REDSTONE
    // First Let's see if we have new info on the pasteboard - if so we'll send that first, then send the FBU
    if (pasteBoardLastChangeCount != [[NSPasteboard generalPasteboard] changeCount]) {
        const char *pbString = [[[NSPasteboard generalPasteboard] stringForType:NSStringPboardType] cString];
        int length = 0;

        if (pbString)
            length = strlen(pbString);

        rfbSendServerCutText((char *) pbString, length);
        pasteBoardLastChangeCount = [[NSPasteboard generalPasteboard] changeCount];
    }
    
    [pool release];
}
