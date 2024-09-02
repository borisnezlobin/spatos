/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2012 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org

    This file written by Ryan C. Gordon (icculus@icculus.org)
*/
#include "SDL_config.h"

/* Output raw audio data to a file. */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "SDL_timer.h"
#include "SDL_audio.h"
#include "../SDL_audiomem.h"
#include "../SDL_audio_c.h"
#include "../SDL_audiodev_c.h"
#include "SDL_redoxaudio.h"

/* The tag name used by REDOX audio */
#define REDOXAUD_DRIVER_NAME         "redox"

/* Audio driver functions */
static int REDOXAUD_OpenAudio(_THIS, SDL_AudioSpec *spec);
static void REDOXAUD_WaitAudio(_THIS);
static void REDOXAUD_PlayAudio(_THIS);
static Uint8 *REDOXAUD_GetAudioBuf(_THIS);
static void REDOXAUD_CloseAudio(_THIS);

/* Audio driver bootstrap functions */
static int REDOXAUD_Available(void)
{
	return(1);
}

static void REDOXAUD_DeleteDevice(SDL_AudioDevice *device)
{
	SDL_free(device->hidden);
	SDL_free(device);
}

static SDL_AudioDevice *REDOXAUD_CreateDevice(int devindex)
{
	SDL_AudioDevice *this;
	const char *envr;

	/* Initialize all variables that we clean on shutdown */
	this = (SDL_AudioDevice *)SDL_malloc(sizeof(SDL_AudioDevice));
	if ( this ) {
		SDL_memset(this, 0, (sizeof *this));
		this->hidden = (struct SDL_PrivateAudioData *)
				SDL_malloc((sizeof *this->hidden));
	}
	if ( (this == NULL) || (this->hidden == NULL) ) {
		SDL_OutOfMemory();
		if ( this ) {
			SDL_free(this);
		}
		return(0);
	}
	SDL_memset(this->hidden, 0, (sizeof *this->hidden));

	this->hidden->output = -1;

	/* Set the function pointers */
	this->OpenAudio = REDOXAUD_OpenAudio;
	this->WaitAudio = REDOXAUD_WaitAudio;
	this->PlayAudio = REDOXAUD_PlayAudio;
	this->GetAudioBuf = REDOXAUD_GetAudioBuf;
	this->CloseAudio = REDOXAUD_CloseAudio;

	this->free = REDOXAUD_DeleteDevice;

	return this;
}

AudioBootStrap REDOXAUD_bootstrap = {
	REDOXAUD_DRIVER_NAME, "Redox audio",
	REDOXAUD_Available, REDOXAUD_CreateDevice
};

/* This function waits until it is possible to write a full sound buffer */
static void REDOXAUD_WaitAudio(_THIS)
{}

static void REDOXAUD_PlayAudio(_THIS)
{
	int written;

	/* Write the audio data */
	written = write(this->hidden->output,
                        this->hidden->mixbuf,
                        this->hidden->mixlen);

	/* If we couldn't write, assume fatal error for now */
	if ( (Uint32)written != this->hidden->mixlen ) {
		this->enabled = 0;
	}
#ifdef DEBUG_AUDIO
	fprintf(stderr, "Wrote %d bytes of audio data\n", written);
#endif
}

static Uint8 *REDOXAUD_GetAudioBuf(_THIS)
{
	return(this->hidden->mixbuf);
}

static void REDOXAUD_CloseAudio(_THIS)
{
	if ( this->hidden->mixbuf != NULL ) {
		SDL_FreeAudioMem(this->hidden->mixbuf);
		this->hidden->mixbuf = NULL;
	}
	if ( this->hidden->output >= 0 ) {
		close(this->hidden->output);
		this->hidden->output = -1;
	}
}

static int REDOXAUD_OpenAudio(_THIS, SDL_AudioSpec *spec)
{
	fprintf(stderr, "WARNING: You are using the SDL redox audio driver!\n");

	/* Open the audio device */
	this->hidden->output = open("audio:", O_WRONLY);
	if ( this->hidden->output < 0 ) {
		perror("failed to open audio:");
		return(-1);
	}

	spec->freq = 44100;
	spec->format = AUDIO_S16;
	spec->channels = 2;
	SDL_CalculateAudioSpec(spec);

	/* Allocate mixing buffer */
	this->hidden->mixlen = spec->size;
	this->hidden->mixbuf = (Uint8 *) SDL_AllocAudioMem(this->hidden->mixlen);
	if ( this->hidden->mixbuf == NULL ) {
		perror("failed to allocate audio memory");
		return(-1);
	}
	SDL_memset(this->hidden->mixbuf, spec->silence, spec->size);

	fprintf(stderr, "openaudio success\n");

	/* We're ready to rock and roll. :-) */
	return(0);
}
