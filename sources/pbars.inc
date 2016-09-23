/*
*	Created:			02.11.12
*	Updated:			02.11.12
*	Author:				LukeS
*	Description:		ПрогрессБары
*	TODO:				-
*/
#if defined _pbars_included
	#endinput
#endif

#if !defined _progress_included
		#error could not locate progress.inc file, please check your server includes
	#endif

#define _pbars_included


//===================================
//Statics
//===================================
static PlayerBar:XPBar[MAX_PLAYERS char];


