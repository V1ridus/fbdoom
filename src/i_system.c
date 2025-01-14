// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: m_bbox.c,v 1.1 1997/02/03 22:45:10 b1 Exp $";


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdarg.h>
#include <sys/time.h>

#ifndef PONGO
#else
#include "pongo.h"
#endif

#include "doomdef.h"
#include "m_misc.h"
#include "i_video.h"
#include "i_sound.h"

#include "d_net.h"
#include "g_game.h"

#ifdef __GNUG__
#pragma implementation "i_system.h"
#endif
#include "i_system.h"




int	mb_used = 6;


void
I_Tactile
( int	on,
  int	off,
  int	total )
{
  // UNUSED.
  on = off = total = 0;
}

ticcmd_t	emptycmd;
ticcmd_t*	I_BaseTiccmd(void)
{
    return &emptycmd;
}


int  I_GetHeapSize (void)
{
    return mb_used*1024*1024;
}

byte* I_ZoneBase (int*	size)
{
    *size = mb_used*1024*1024;
    byte* memory = (byte *) malloc (*size);
    memset(memory, 0, *size);
    return memory;
}



//
// I_GetTime
// returns time in 1/70th second tics
//
int  I_GetTime (void)
{
    struct timeval	tp;
    struct timezone	tzp;
    int			newtics;
    static int		basetime=0;
  
  #ifndef PONGO

    gettimeofday(&tp, &tzp);
    if (!basetime)
	basetime = tp.tv_sec;
    newtics = (tp.tv_sec-basetime)*TICRATE + tp.tv_usec*TICRATE/1000000;
    return newtics;
#else

    uint64_t cur_ticks = get_ticks();
    uint64_t cur_micro_ticks = (cur_ticks * 100 )/ 2407; // TODO PONGO, check this
    uint32_t secs = cur_micro_ticks / 1000000LLU;
    uint32_t micro_secs = cur_micro_ticks % 1000000LLU;
    if (!basetime) {
        basetime = secs;
    }

    newtics = (secs - basetime)*TICRATE + micro_secs*TICRATE/1000000;

    return newtics;
#endif

}



//
// I_Init
//
void I_Init (void)
{
    I_InitSound();
    I_InitMusic();
    //  I_InitGraphics();
}

//
// I_Quit
//
void I_Quit (void)
{
    D_QuitNetGame ();
    I_ShutdownSound();
    I_ShutdownMusic();
    M_SaveDefaults ();
    I_ShutdownGraphics();
    #ifndef PONGO
        exit(0); 
    #else 
        printf(0); // Hack to crash back to pongoterm
    #endif 
}

void I_WaitVBL(int count)
{
#ifdef SGI
    sginap(1);                                           
#else
#ifdef SUN
    sleep(0);
#else
    usleep (count * (1000000/70) );                                
#endif
#endif
}

void I_BeginRead(void)
{
}

void I_EndRead(void)
{
}

byte*	I_AllocLow(int length)
{
    byte*	mem;
        
    mem = (byte *)malloc (length);
    memset (mem,0,length);
    return mem;
}


//
// I_Error
//
extern boolean demorecording;

void I_Error (char *fmt, ...)
{
    //va_list	args;

    printf("Error: ");

    // Log message
    //printf("%s", fmt);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt,args); 
    va_end(args); 
    printf("\n");

    // Message first.
    //va_start (argptr,error);
    //fprintf (stderr, "Error: ");
    //vfprintf (stderr,error,argptr);
    //fprintf (stderr, "\n");
    //va_end (argptr);

    //fflush( stderr );



    // Shutdown. Here might be other errors.
    if (demorecording)
	G_CheckDemoStatus();

    D_QuitNetGame ();
    I_ShutdownGraphics();
    
    //exit(-1); TODO Pongo
}
