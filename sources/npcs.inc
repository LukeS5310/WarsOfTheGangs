/*
*	Created:			18.05.2012
*	Author:				LukeS
*	Description:		NPC handler
*/

#if defined _npcs_included
	#endinput
#endif

#define _npcs_included
#pragma library npcs
//new Float:ajust;
forward NPC_AIUpdate();
forward StartTour(npcid,playerid);
// --------------------------------------------------
// Defines
// --------------------------------------------------
#define for_npcs(%1) for(new %1 = SLOTS;%1 >= LastSpawnedNPC;%1--)
#define NPCS_DIR		"npcs/"
#define NPCS_MASK_FILE  "NPC_%d.cnpc"
#define BARON_SPOTS     "Baron_Spots.txt"
#define RESPAWN_DELAY	60
// --------------------------------------------------
// enums
// --------------------------------------------------
enum npcinfo
{
Float:x,
Float:y,
Float:z,
Float:angle,
weapon,
skin,
Float:LOS,		//Sight range
beh,
respawn
};
enum pointinfo 
{
    Float:point_x,
    Float:point_y,
    Float:point_z
};
// ------------------------------------
// Statics
// ------------------------------------
static NPC[MAX_NPCS][npcinfo];
static NPC_NAME[MAX_NPCS][MAX_PLAYER_NAME];
/* static Float:Taxi[][pointinfo]=
{
	{2457.2097,-1658.6517,13.3047},
	{2448.6133,-1658.6912,13.3047},
	{2435.0947,-1658.7539,13.3828},
	{2371.2141,-1656.3547,13.3828},
	{2345.5479,-1656.5760,13.3759},
	{2326.0461,-1653.9047,13.7240}

}; */
static LastSpawnedNPC,SLOTS;
static NPC_BEH[MAX_NPCS char];
static following[MAX_PLAYERS];
//static PatrolStep[MAX_NPCS char];
// ------------------------------------------
// enums
// ------------------------------------------
enum
{
BEHAIVOR_SPECIAL,
BEHAIVOR_ENEMY,
BEHAIVOR_NEUTRAL,
BEHAIVOR_FRIEND,
BEHAIVOR_HELIDRIVER,
BEHAIVOR_SCARY
};
enum
{
NPC_STATE_BATTLE,
NPC_STATE_PASSIVE,
NPC_STATE_WANDERING
};
// ----------------------------------------
// Obligatory functions
// ----------------------------------------
Npcs_OnGameModeInit()
{
SLOTS=FindLastFreeSlot();
ToogleVisualDeath(1);
SetTimer("NPC_AIUpdate",100,1);
//loading NPCs from files
new i=0;
format(stmp,sizeof(stmp),MODE_DIR NPCS_DIR NPCS_MASK_FILE,i);
while(fexist(stmp))
	{
		LoadNPCFromFile(stmp);
		// up
		i++;
		// again
		format(stmp,sizeof(stmp),MODE_DIR NPCS_DIR NPCS_MASK_FILE,i);
	}
return 1;

}
LoadNPCFromFile(filename[])
{
new File:df = fopen(filename,io_read),
		k,
		c,
		npcid=FindLastFreeSlot();
if(df)
	{
		while(fread(df,stmp))
		{
			if(!CheckFileLine(stmp)) continue;
			if((k = strfind(stmp,"=")) == -1) continue;
			
			if(!strcmp(stmp,"Name",true,k))
			{
				strcat(NPC_NAME[npcid],stmp[k + 1]);
				continue;
			}
			
			if(!strcmp(stmp,"Coords",true,k))
			{
				c = 0;
				NPC[npcid][x] = floatstr(strtok(stmp[k + 1],c,','));
				NPC[npcid][y] = floatstr(strtok(stmp[k + 1],c,','));
				NPC[npcid][z] = floatstr(strtok(stmp[k + 1],c,','));
				NPC[npcid][angle] = floatstr(strtok(stmp[k + 1],c,','));
				continue;
			}
			
			
			if(!strcmp(stmp,"Weapon",true,k))
			{
				NPC[npcid][weapon] = strval(stmp[k + 1]);
				continue;
			}
			
			if(!strcmp(stmp,"Skin",true,k))
			{
				NPC[npcid][skin] = strval(stmp[k + 1]);
				continue;
			}
			
			if(!strcmp(stmp,"LOS",true,k))
			{
				NPC[npcid][LOS] = floatstr(stmp[k + 1]);
				continue;
			}
			
			if(!strcmp(stmp,"Behaivor",true,k))
			{
				NPC[npcid][beh] = strval(stmp[k + 1]);
				continue;
			}
			
		}
		fclose(df);
	
	}
CreateNPC(npcid,NPC_NAME[npcid]);
LastSpawnedNPC=npcid;
SetNPCSkin(npcid,NPC[npcid][skin]);
switch(NPC[npcid][beh])
		{
			case 0:
				{
				SetNPCBehaivor(npcid,BEHAIVOR_SPECIAL);
				new Text3D:lbl= Create3DTextLabel("Выдача снаряжения, нажимте 'N' для вазимодействия",COLOR_WHITE,0.0,0.0,0.0,40.0,1);
				Attach3DTextLabelToPlayer(lbl,npcid,0.0,0.0,0.7);
				goto final;
				}
			case 1:
				{
				SetNPCBehaivor(npcid,BEHAIVOR_ENEMY);
				goto final;
				}
			case 2:
				{
				SetNPCBehaivor(npcid,BEHAIVOR_NEUTRAL);
				goto final;
				}
			case 3:
				{
				SetNPCBehaivor(npcid,BEHAIVOR_FRIEND);
				goto final;
				}
			case 4:
				{
				SetNPCBehaivor(npcid,BEHAIVOR_HELIDRIVER);
				goto final;
				}
			case 5:
				{
				SetNPCBehaivor(npcid,BEHAIVOR_SCARY);
				goto final;
				}
		}
final:
//SpawnNPC(npcid);
printf("LastSpawnedNPC is %d",LastSpawnedNPC);
//printf("Parsing Complete ID:%d Name:%s Coords: x=%f y=%f z=%f angle=%f Weapon:%d Skin:%d LOS:%f BEH:%d",npcid,NPC_NAME[npcid],NPC[npcid][x],NPC[npcid][y],NPC[npcid][z],NPC[npcid][angle],NPC[npcid][weapon],NPC[npcid][skin],NPC[npcid][LOS],NPC[npcid][beh]);
}
NPC_Tick()
{
for_npcs(i)
	{
	if(!IsValidNPC(i)) continue;
	if(NPC[i][respawn]==-1) continue; //npc spawned
	if(NPC[i][respawn]==0)
		{
		SpawnNPC(i);
		NPC[i][respawn]=-1;
		printf("NPC[%d] (RE)SPAWNED",i);
		}
	else NPC[i][respawn]--;
	}
return 1;
}
// --------------------------------------------------
// CNPC Callbacks
// --------------------------------------------------
public OnNPCSpawn(npcid)
{
SetNPCPos(npcid,NPC[npcid][x],NPC[npcid][y],NPC[npcid][z]);
SetNPCFacingAngle(npcid,NPC[npcid][angle]);
SetNPCWeapon(npcid,NPC[npcid][weapon]);
if(GetNPCBehaivor(npcid)==BEHAIVOR_HELIDRIVER)
	{
		PutNPCInVehicle(npcid,heli,0);


	}
return 1;
}
public OnNPCDeath(npcid,killerid,reason)
{
SetNPCPos(npcid,0.0,0.0,0.0);
NPC[npcid][respawn]=RESPAWN_DELAY;
return 1;
}
public OnNPCMovingComplete(npcid)
{

return 1;
}
public OnRecordingPlaybackEnd(npcid,reason)
{
return 1;
}
public OnNPCGetDamage(npcid,playerid,Float:health_loss,bodypart)
{

return 1;
}

//------------------------------------------------
// Standard Callback Overrides
//------------------------------------------------
Npcs_OnPlayerEnterVehicle(playerid,vehicleid,ispass)
{
#pragma unused ispass
if(following[playerid]!=-1)
{
new seat=NPC_GetFreeSeats(following[playerid],vehicleid);
if(seat!=-1)
	{
	PutNPCInVehicle(following[playerid],vehicleid,seat);
	}
}
return 1;
}
Npcs_OnPlayerExitVehicle(playerid,vehicleid)
{
if(following[playerid]!=-1)
{
#pragma unused vehicleid
//new nid=following[playerid];
//SpawnNPC(nid);
//SetNPCPos(nid,NPC[nid][x],NPC[nid][y],NPC[nid][z]);
}
return 1;
}

// -----------------------------------------------
// AI Stuff
// -----------------------------------------------
public NPC_AIUpdate()
{
for_npcs(i)
	{
	if(!IsValidNPC(i)) continue;
	UpdateAIForNPC(i);
	//printf("%d is a valid NPC",i);
	}

return 1;
}
NPC_FollowPlayer(npcid,playerid)
{
new Float:vx,Float:vy,Float:vz;
GetPlayerPos(playerid,ftmp[1],ftmp[2],ftmp[3]);
GetPlayerVelocity(playerid,vx,vy,vz);
if(vx>0)
	{
	ftmp[1]=ftmp[1]-1.0;
	}
if(vx<0)
	{
	ftmp[1]=ftmp[1]+1.0;
	}
if(vy>0)
	{
	ftmp[2]=ftmp[2]-1.0;
	}
if(vy<0)
	{
	ftmp[2]=ftmp[2]+1.0;
	}
if(vy==0 && vx==0) return 1;
if(GetDistanceBetweenPlayers(npcid,playerid)<5.0) NPC_WalkTo(npcid,ftmp[1],ftmp[2],ftmp[3],0);
else NPC_RunTo(npcid,ftmp[1],ftmp[2],ftmp[3],0);
//print("check passed");
return 1;
}
SetNPCBehaivor(npcid,behaivor)
{
NPC_BEH{npcid}=behaivor;
return 1;
}
GetNPCBehaivor(npcid)
{
return NPC_BEH{npcid};

}
//patrol(npcid)
//{
//new i=PatrolStep{npcid};
//NPC_WalkTo(npcid,Point[i][point_x],Point[i][point_y],Point[i][point_z],0);
//PatrolStep{npcid}++;
//if(PatrolStep{npcid}==5) PatrolStep{npcid}=0;
//return 1;
//}
//public StartTour(npcid,playerid)
//{
//new i=PatrolStep{npcid};
//SendClientMessage(playerid,COLOR_WHITE,"Водитель: ну привет!");
//NPC_DriveTo(npcid,Taxi[i][point_x],Taxi[i][point_y],Taxi[i][point_z],20.0,0);
//PatrolStep{npcid}=1;
//ContinueTour(npcid);
//return 1;
//}
//ContinueTour(npcid)
//{
//if(PatrolStep{npcid}==6)
//{
 //PatrolStep{npcid}=0;
 //return 1;
//}
//new i=PatrolStep{npcid};
//NPC_DriveTo(npcid,Taxi[i][point_x],Taxi[i][point_y],Taxi[i][point_z],20.0,0);
//PatrolStep{npcid}++;
//return 1;
//}
UpdateAIForNPC(npcid)
{
switch(GetNPCBehaivor(npcid))
	{
		case BEHAIVOR_SPECIAL:
			{
			return 1;
			
			}
		case BEHAIVOR_ENEMY:
			{
			NPC_SearchDestroy(npcid);
			//print("UpdateAIForNPC fired");
			return 1;
			}
		case BEHAIVOR_FRIEND:
			{
			NPC_HangOut(npcid);
			return 1;
			}
		case BEHAIVOR_HELIDRIVER:
			{
			NPC_HeliChase(npcid);
			return 1;
			}
		case BEHAIVOR_SCARY:
			{
			NPC_Roam(npcid);
			return 1;
			}
	}
return 0;
}
NPC_Roam(npcid)
{
new closest=GetPlayerClosestPlayer(npcid);
//print("SearchDestroy fired");
if(GetDistanceBetweenPlayers(npcid,closest)<NPC[npcid][LOS] && GetDistanceBetweenPlayers(npcid,closest)!=-1.0)
{
WarpToSafe(npcid,closest);
}
return 1;
}
WarpToSafe(npcid,closest)
{
	start:
	new pointid=random(12);
	new File:df = fopen(MODE_DIR NPCS_DIR BARON_SPOTS,io_read),
		c;//,url[128];
	if(df)
	{
		while(fread(df,stmp))
		{
			if(!CheckFileLine(stmp)) continue;
			//printf("c is %d pointid is %d",c,pointid);
			if(c == pointid)
			{
				new k=0;
				c = 0;
				// url
				//strmid(url,stmp,0,strfind(stmp,","));
				// tune
				//format(url,sizeof(url),"%s",stmp);
				ftmp[0] = floatstr(strtok(stmp,k,','));
				ftmp[1] = floatstr(strtok(stmp,k,','));
				ftmp[2] = floatstr(strtok(stmp,k,','));
				ftmp[3] = floatstr(strtok(stmp,k,','));
				if(GetPlayerDistanceFromPoint(closest,ftmp[0],ftmp[1],ftmp[2])>20.0)
					{
					SetNPCPos(npcid,ftmp[0],ftmp[1],ftmp[2]);
					SetNPCFacingAngle(npcid,ftmp[3]);
					}
				else goto start;
			    //SendClientMessageToAll(COLOR_GREY,url);
				break;
			}
			else c++;
		}
		fclose(df);
	}
	else print("[ERROR] memories file not exist");
	return 1;

}
//NPC_ChasePlayer(npcid,playerid)
//{
//GetPlayerPos(playerid,ftmp[1],ftmp[2],ftmp[3]);
//NPC_DriveTo(npcid,ftmp[1],ftmp[2],ftmp[3],1);
//return 1;
//}
NPC_HangOut(npcid)
{
new closest=GetPlayerClosestPlayer(npcid);
if(GetDistanceBetweenPlayers(npcid,closest)<NPC[npcid][LOS] && GetDistanceBetweenPlayers(npcid,closest)!=-1.0)
{
following[closest]=npcid;
//printf("now following %d",following[closest]);
NPC_FollowPlayer(npcid,closest);
}
else 
{
	StopNPC(npcid);
	following[closest]=-1;
}
//print("hangout fired");
return 1;
}
NPC_SearchDestroy(npcid)
{
new closest=GetPlayerClosestPlayer(npcid);
//print("SearchDestroy fired");
if(GetDistanceBetweenPlayers(npcid,closest)<NPC[npcid][LOS] && GetDistanceBetweenPlayers(npcid,closest)!=-1.0)
{
NPC_Attack(npcid,closest);
}
else StopNPC(npcid);
//printf("attack Move not fired closest id %d distance %f",closest,GetDistanceBetweenPlayers(npcid,closest));
return 1;
}
NPC_Attack(npcid,playerid)
{
GetPlayerPos(playerid,ftmp[1],ftmp[2],ftmp[3]);
//ftmp[1]+=1.3;
//ftmp[3]+=0.7;
NPC_ShootAt(npcid,ftmp[1],ftmp[2],ftmp[3]);
return 1;
}
NPC_HeliChase(npcid)
{
if(chasing==-1) NPC_DriveTo(npcid,ftmp[0],ftmp[1],ftmp[2]+20.0,itmp[0],0);
GetPlayerPos(chasing,ftmp[0],ftmp[1],ftmp[2]);
ftmp[3]=GetPlayerDistanceFromPoint(npcid,ftmp[0],ftmp[1],ftmp[2]);
if(ftmp[3]<=30.0) itmp[0]=15;
if(ftmp[3]>30.0 && ftmp[3]<=40.0) itmp[0]=25;
if(ftmp[3]>40.0 && ftmp[3]<=50.0) itmp[0]=35;
if(ftmp[3]>50.0 && ftmp[3]<=100.0) itmp[0]=70;
if(ftmp[3]>100.0)  itmp[0]=120;
NPC_DriveTo(npcid,ftmp[0],ftmp[1],ftmp[2]+20.0,itmp[0],0);

return 1;
}
//===========================================
//stocks
//===========================================
stock NPC_GetFreeSeats(npcid,vehicleid)
{
#pragma unused npcid
new sid,seat[3];
seat[0]=1;
for(new i=0;i<GetPlayersCount();i++)
	{
	sid=GetPlayerVehicleSeat(i);
	if(GetPlayerVehicleID(i)!=vehicleid) continue;
	if(sid==128) continue;
	seat[sid]=1;
	}
for(new i=0;i<=3;i++)
	{
	//printf("seat %d is taken!",i);
	if(seat[i]==0) return i;
	
	}
print("no available seats!");
return -1;
}
stock GetPlayerClosestPlayer(playerid)
{
	new pid = INVALID_PLAYER_ID,
		Float:distance = 69000.0,
		Float:tmp[7];
	GetPlayerPos(playerid,tmp[0],tmp[1],tmp[2]);
	for(new i = 0;i < MAX_PLAYERS;i++)
	{
		if(i == playerid) continue;
		if(!IsPlayerConnected(i)) continue;
		if(IsPlayerNPC(i)) continue;
		GetPlayerPos(i,tmp[3],tmp[4],tmp[5]);
		tmp[6] = GetDistanceBetweenCoords(tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
		if(distance < tmp[6]) continue;
		distance = tmp[6];
		pid = i;
	}
	return pid;
}