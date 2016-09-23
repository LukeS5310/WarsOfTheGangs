/*
*	Created:			05.06.10
*	Author:				009(LukeS edit)
*	Description:		стриммер иконок
*/

#if defined _streamer_icons_included
	#endinput
#endif

#define _streamer_icons_included
#pragma library streamer_icons

// --------------------------------------------------
// defines
// --------------------------------------------------
#define MAX_STREAMED_ICONS		1000
#define ICONS_STREAMED_DIST		200.0
#define INVALID_ICON_ID			255
#define MAX_ICONS				100

// --------------------------------------------------
// statics
// --------------------------------------------------
static 
	IconsStatus[MAX_STREAMED_ICONS char],
	Float:IconsData[MAX_STREAMED_ICONS][3],
	IconsType[MAX_STREAMED_ICONS char],
	IconsColor[MAX_STREAMED_ICONS],
	PlayerShowed[MAX_PLAYERS][MAX_STREAMED_ICONS char],
	PlayerIconsSlots[MAX_PLAYERS][MAX_ICONS char],
	bool:IsStreaming[MAX_PLAYERS],
	MinIconId,
	IconsCount;

// --------------------------------------------------
// stocks
// --------------------------------------------------
stock DestroyStreamedIcon(iconid)
{
IconsStatus{iconid} = 1;
return 1;

}
stock UpdateStreamedIcon(type,iconid)
{
IconsType{iconid} = type;
return 1;
}
stock CreateStreamedIcon(type,color,Float:X,Float:Y,Float:Z)
{
	static id;
	id = MinIconId;
	if(id >= MAX_STREAMED_ICONS)
	{
		print("[ERROR] Limit of streamed icons");
		return 0;
	}
	//find in deleted icons
	for(new i;i<=id;i++)
		{
		if(IconsStatus{id} == 0) continue;
		id=i;
		print("found here!");
		}
	IconsStatus{id} = 0;
	IconsData[id][0] = X;
	IconsData[id][1] = Y;
	IconsData[id][2] = Z;
	IconsType{id} = type;
	IconsColor[id] = color;
	// min cp
	do MinIconId++;
	while((MinIconId < MAX_STREAMED_ICONS) && (IconsStatus{MinIconId} == 1));
	// count
	if(IconsCount < MinIconId) IconsCount = MinIconId;
	// ret id
	return id;
}

stock PlayerIconStream(playerid)
{
	for(new i = 0; i < IconsCount; i++)
	{
		if(IsPlayerInRangeOfPoint(playerid,ICONS_STREAMED_DIST,IconsData[i][0],IconsData[i][1],IconsData[i][2]))
		{
			if(PlayerShowed[playerid]{i} != INVALID_ICON_ID) continue;
			if(IconsStatus{i} == 1)
				{ 
				
				RemovePlayerMapIcon(playerid,PlayerShowed[playerid]{i});
				PlayerIconsSlots[playerid]{ PlayerShowed[playerid]{i} } = 0;
				PlayerShowed[playerid]{i} = INVALID_ICON_ID;
				
				continue; //changed here
				}
			for(new j = 1;j < MAX_ICONS;j++)
			{
				if(PlayerIconsSlots[playerid]{j} == 1) continue;
				
				PlayerIconsSlots[playerid]{j} = 1;
				PlayerShowed[playerid]{i} = j;
				SetPlayerMapIcon(playerid,j,IconsData[i][0],IconsData[i][1],IconsData[i][2],IconsType{i},IconsColor[i]);
				break;
			}
		}
		else if(PlayerShowed[playerid]{i} != INVALID_ICON_ID)
		{
			RemovePlayerMapIcon(playerid,PlayerShowed[playerid]{i});
			PlayerIconsSlots[playerid]{ PlayerShowed[playerid]{i} } = 0;
			PlayerShowed[playerid]{i} = INVALID_ICON_ID;
		}
	}
}

// --------------------------------------------------
// forwards
// --------------------------------------------------
// #if !defined ON_PLAYER_UPDATE_STREAMERS
// forward Streamer_Icons();
// #endif

// --------------------------------------------------
// publics
// --------------------------------------------------
#if !defined ON_PLAYER_UPDATE_STREAMERS
Streamer_Icons()
{
	//for(new playerid = 0;playerid <= GetPlayersCount();playerid++)
	foreach(new playerid : Player)
	{
		//if(!IsPlayerConnected(playerid)) continue;
		if(IsPlayerNPC(playerid)) continue;
		if(IsStreaming[playerid] == false) continue;
		PlayerIconStream(playerid);
	}
}
#endif

// --------------------------------------------------
// Obligatory functions
// --------------------------------------------------
StreamerIcon_OnGameModeInit()
{
	print("Streamer icons loaded.");
}
StreamerIcon_OPVWC(playerid,worldid)
{
//print("TOGGLED");
if(worldid == 0) IsStreaming[playerid] = true;
else IsStreaming[playerid] = false;
}
StreamerIcon_OnPlayerConnect(playerid)
{
	for(new i = 0;i < MAX_STREAMED_ICONS;i++) PlayerShowed[playerid]{i} = INVALID_ICON_ID;
	for(new i = 0;i < MAX_ICONS;i++) PlayerIconsSlots[playerid]{i} = 0;
}

StreamerIcon_OnPlayerDisconnect(playerid)
{
	for(new i = 0;i < MAX_STREAMED_ICONS;i++) PlayerShowed[playerid]{i} = INVALID_ICON_ID;
	for(new i = 0;i < MAX_ICONS;i++) PlayerIconsSlots[playerid]{i} = 0;
}

#if defined ON_PLAYER_UPDATE_STREAMERS
StreamerIcon_OnPlayerUpdate(playerid)
{
	PlayerIconStream(playerid);
}
#endif