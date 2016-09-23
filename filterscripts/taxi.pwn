/*
*	Created:			21.08.12
*	Author:				009
*	Last Modifed:		-
*	Description:		Controllable NPC's taxi filterscript
*/

main() {}

// --------------------------------------------------
// includes
// --------------------------------------------------
#include <a_samp>
#include <cnpc>
#include <dijkstra>

// --------------------------------------------------
// defines
// --------------------------------------------------
#define VERSION "0.1 alpha"

#if !defined MAX_STRING
	#define MAX_STRING 128
#endif

#define MAX_TAXI    1
#define MAX_PATH    500

#define MESSAGES_COLOR 0xFFFFFFFF

// --------------------------------------------------
// enums
// --------------------------------------------------
enum taxi_info
{
	name[MAX_PLAYER_NAME],
	skin,
	Float:pos[4],
	cnpcid,
	vehicleid
};

// --------------------------------------------------
// news
// --------------------------------------------------
new strtmp[MAX_STRING],
	taxi_spawn[MAX_TAXI][taxi_info] =
	{
	    {"First_taxi",61,{-2011.4387,-41.8610,35.1650,353.0915},-1,-1}
	},
	Float:taxi_path[MAX_TAXI][MAX_PATH][3],
	taxi_pathlen[MAX_TAXI],
	taxi_pathstep[MAX_TAXI];

// --------------------------------------------------
// forwards
// --------------------------------------------------


// --------------------------------------------------
// publics
// --------------------------------------------------
public OnFilterScriptInit()
{
	// подгружаем поиск путей
	dijkstra_init();
	// создаём таксистов
	for(new i = 0;i < MAX_TAXI;i++)
	{
	    taxi_spawn[i][vehicleid] = CreateVehicle(420,taxi_spawn[i][pos][0],taxi_spawn[i][pos][1],taxi_spawn[i][pos][2],taxi_spawn[i][pos][3],-1,-1,0);
	    taxi_spawn[i][cnpcid] = FindLastFreeSlot();
	    CreateNPC(taxi_spawn[i][cnpcid],taxi_spawn[i][name]);
	    SetSpawnInfo(taxi_spawn[i][cnpcid],0,taxi_spawn[i][skin],taxi_spawn[i][pos][0],taxi_spawn[i][pos][1],taxi_spawn[i][pos][2],taxi_spawn[i][pos][3],0,0,0,0,0,0);
	    SpawnNPC(taxi_spawn[i][cnpcid]);
	    SetNPCPos(taxi_spawn[i][cnpcid],taxi_spawn[i][pos][0],taxi_spawn[i][pos][1],taxi_spawn[i][pos][2]);
	    SetNPCFacingAngle(taxi_spawn[i][cnpcid],taxi_spawn[i][pos][3]);
	    SetNPCVelocity(taxi_spawn[i][cnpcid],0.0,0.0,0.0);
	    PutNPCInVehicle(taxi_spawn[i][cnpcid],taxi_spawn[i][vehicleid],0);
	}
	
	print("Controllable NPC's taxi " VERSION " loaded.");
}

public OnFilterScriptExit()
{
	// выгружаем поиск путей
	dijkstra_exit();
	// удаляем таксистов
	for(new i = 0;i < MAX_TAXI;i++)
	{
		DestroyNPC(taxi_spawn[i][cnpcid]);
		DestroyVehicle(taxi_spawn[i][vehicleid]);
	}
	
	print("Controllable NPC's taxi " VERSION " unloaded.");
}

public OnPlayerConnect(playerid)
{

}

public OnPlayerDisconnect(playerid, reason)
{
    
}

public OnNPCGetDamage(npcid,playerid,Float:health_loss)
{

	return 1;
}

public OnNPCMovingComplete(npcid)
{
    for(new i = 0;i < MAX_TAXI;i++)
	{
	    if(taxi_spawn[i][cnpcid] != npcid) continue;

     	taxi_pathstep[i]++;
     	if(taxi_pathstep[i] == taxi_pathlen[i]) SetPlayerChatBubble(npcid,"Приехали",0xFFFFFFFF,20.0,2500);
     	else NPC_DriveTo(taxi_spawn[i][cnpcid],taxi_path[i][taxi_pathstep[i]][0],taxi_path[i][taxi_pathstep[i]][1],taxi_path[i][taxi_pathstep[i]][2] + 1.0,30.0,0);
	}
    
}

public OnNPCDeath(npcid,killerid,reason)
{

}

public OnPlayerCommandText(playerid, cmdtext[])
{
	new cmd[20],
		idx;

	cmd = strtok(cmdtext,idx);

	if(!strcmp(cmd,"/start",true))
	{
	    
	    return 1;
	}
	return 0;
}

public OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ)
{
	if(!IsPlayerInAnyVehicle(playerid)) return;
	
	for(new i = 0;i < MAX_TAXI;i++)
	{
	    if(!IsPlayerInVehicle(playerid,taxi_spawn[i][vehicleid])) continue;
	    
	    new Float:cpos[3];
	    new startt = GetTickCount();
		taxi_pathlen[i] = MAX_PATH;
		GetPlayerPos(playerid,cpos[0],cpos[1],cpos[2]);
		CalculatePath(cpos[0],cpos[1],cpos[2],fX,fY,fZ,taxi_path[i],taxi_pathlen[i]);
		new endt = GetTickCount();
		
		if(!taxi_pathlen[i])
		{
		    SetPlayerChatBubble(taxi_spawn[i][cnpcid],"Я не знаю пути туда",0xFFFFFFFF,20.0,2500);
		}
		else
		{
	    	taxi_pathstep[i] = 0;
	    	NPC_DriveTo(taxi_spawn[i][cnpcid],taxi_path[i][taxi_pathstep[i]][0],taxi_path[i][taxi_pathstep[i]][1],taxi_path[i][taxi_pathstep[i]][2] + 1.0,30.0,0);
	    }

		format(strtmp,sizeof(strtmp),"path [%d] created in %d msec",taxi_pathlen[i],(endt - startt));
		SendClientMessage(playerid,0xFF0000,strtmp);
	}
}

// --------------------------------------------------
// stocks
// --------------------------------------------------
stock strtok(const string[], &index)
{
	new length = strlen(string);
	while ((index < length) && (string[index] <= ' '))
	{
		index++;
	}

	new offset = index;
	new result[20];
	while ((index < length) && (string[index] > ' ') && ((index - offset) < (sizeof(result) - 1)))
	{
		result[index - offset] = string[index];
		index++;
	}
	result[index - offset] = EOS;
	return result;
}
