/*
	This file is part of Warzone 2100.
	Copyright (C) 1999-2004  Eidos Interactive
	Copyright (C) 2005-2020  Warzone 2100 Project

	Warzone 2100 is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Warzone 2100 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Warzone 2100; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/
/** @file
 *  Functions for the display/functionality of the Transporter
 */

#ifndef __INCLUDED_SRC_TRANSPORTER_H__
#define __INCLUDED_SRC_TRANSPORTER_H__

#include "lib/widget/widget.h"

#include <cstdint>

#define IDTRANS_FORM			9000	//The Transporter base form
#define IDTRANS_CONTENTFORM		9003	//The Transporter Contents form
#define IDTRANS_DROIDS			9006	//The Droid base form
#define IDTRANS_LAUNCH			9010	//The Transporter Launch button
#define	IDTRANS_CAPACITY		9500	//The Transporter capacity label

//defines how much space is on the Transporter
#define TRANSPORTER_CAPACITY		10

/// how much cargo capacity a droid takes up
int transporterSpaceRequired(const DROID *psDroid);

//initialises Transporter variables
void initTransporters();
// Refresh the transporter screen.
bool intRefreshTransporter();
/*Add the Transporter Interface*/
bool intAddTransporter(DROID *psSelected, bool offWorld);
/* Remove the Transporter widgets from the screen */
void intRemoveTrans(bool skipIntModeReset = false);
void intRemoveTransNoAnim(bool skipIntModeReset = false);
/* Process return codes from the Transporter Screen*/
void intProcessTransporter(UDWORD id);

/*Adds a droid to the transporter, removing it from the world*/
void transporterAddDroid(DROID *psTransporter, DROID *psDroidToAdd);
void transporterRemoveDroid(DROID *psTransport, DROID *psDroid, QUEUE_MODE mode);
/*check to see if the droid can fit on the Transporter - return true if fits*/
bool checkTransporterSpace(DROID const *psTransporter, DROID const *psAssigned, bool mayFlash = true);

/**
 * True if psDroid's body type is allowed on this class of transporter: a DROID_TRANSPORTER
 * (cyborg transporter) only accepts cyborgs, a DROID_SUPERTRANSPORTER accepts anything.
 * This is a type-compatibility test only - it says nothing about remaining space.
 */
bool transporterAcceptsDroidType(DROID const *psTransporter, DROID const *psDroid);

/**
 * Picks the best transporter for psDroid to embark on, from psDroid's own player.
 * Considers only live, non-flying transporters with planned space remaining, where "planned"
 * subtracts droids already holding a DORDER_EMBARK order on that transporter.
 * Scored by reachable path distance from psDroid; maxSqDist caps the straight-line distance
 * considered. Returns nullptr if nothing qualifies.
 * Reads only synchronized game state - safe to call on every client.
 */
DROID *transporterFindBestForEmbark(DROID const *psDroid, int maxSqDist = INT32_MAX);

/**
 * If psTransporter has no room for psDroidToAdd, order psDroidToAdd to embark on the best
 * nearby transporter instead, and return true. Returns false if there is room, or if no
 * suitable alternative exists - the caller should then proceed with transporterAddDroid().
 * Only the game's own embark targets may be overridden this way, so it is up to the caller
 * to establish that the player did not pick psTransporter explicitly. Multiplayer only.
 */
bool transporterRedirectIfFull(DROID const *psTransporter, DROID *psDroidToAdd);

/*calculates how much space is remaining on the transporter - allows droids to take
up different amount depending on their body size - currently all are set to one!*/
int calcRemainingCapacity(const DROID *psTransporter);

bool transporterIsEmpty(const DROID *psTransporter);

/*launches the defined transporter to the offworld map*/
bool launchTransporter(DROID *psTransporter);

/*checks how long the transporter has been travelling to see if it should
have arrived - returns true when there*/
bool updateTransporter(DROID *psTransporter);

void intUpdateTransCapacity(WIDGET *psWidget, const W_CONTEXT *psContext);

/* Remove the Transporter Launch widget from the screen*/
void intRemoveTransporterLaunch();

//process the launch transporter button click
void processLaunchTransporter();

SDWORD	bobTransporterHeight();

/*This is used to display the transporter button and capacity when at the home base ONLY*/
bool intAddTransporterLaunch(DROID *psDroid);

/*called when a Transporter has arrived back at the LZ when sending droids to safety*/
void resetTransporter();

/* get time transporter launch button was pressed */
UDWORD transporterGetLaunchTime();

/*set the time for the Launch*/
void transporterSetLaunchTime(UDWORD time);

void flashMissionButton(UDWORD buttonID);
void stopMissionButtonFlash(UDWORD buttonID);
/*checks the order of the droid to see if its currently flying*/
bool transporterFlying(const DROID *psTransporter);
//initialise the flag to indicate the first transporter has arrived - set in startMission()
void initFirstTransporterFlag();

#endif // __INCLUDED_SRC_TRANSPORTER_H__
