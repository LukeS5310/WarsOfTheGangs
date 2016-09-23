/*
*	Created:			20.08.12
*	Author:				009 & OKStyle
*	Last Modifed:		-
*	Description:		Controllable NPC's missions filterscript
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
#define VERSION             "0.1 alpha"

#if !defined MAX_STRING
	#define MAX_STRING 128
#endif

#define MISSIONS_COUNT 3
#define MAX_PATH 100
#define MESSAGES_COLOR 0xFFFFFFFF

// --------------------------------------------------
// enums
// --------------------------------------------------
enum MissionInfo
{
	name[64],			// �������� ������
	pickupmodel,        // id ������ ������
	Float:start[3],		// ���������� ������ ������
	request,			// id ������ ������� ������ ���� �������� ��� ������� ����
	pickup				// id ���������� �� ������ ������
};
enum PlayerInfo
{
	progress[MISSIONS_COUNT],	// ���������� ������ (����� - 1 ��� 0)
	current_mission,			// ������� ������
	current_step,				// ������� ���� ������
	iscutscene,					// ���� - ������� ���� ��� ��������?
	timer						// ������, ��������� ��� ���������
};


// --------------------------------------------------
// news
// --------------------------------------------------
new strtmp[MAX_STRING],
	missions[MISSIONS_COUNT][MissionInfo] = {
		{"Let's Rock! Part 1",1254,{-1657.8768,-155.0978,14.1484},-1,-1},
		{"Let's Rock! Part 2",1254,{-1657.8768,-135.0978,14.1484},0,-1},
		{"Case delivery",1559,{2069.5586, -1766.5908, 13.5626},-1,-1}
	},
	players[MAX_PLAYERS][PlayerInfo],
	Text:Letterbox[2],
	player_mission_data[MAX_PLAYERS][MISSIONS_COUNT][20],
	Float:move_path[MAX_NPCS][MAX_PATH][3],
	move_pathlen[MAX_NPCS],
	move_pathstep[MAX_NPCS],
	move_lastrecalc[MAX_NPCS],
	Float:move_lastpos[MAX_NPCS][3];
new Float:ftmp[6];

// --------------------------------------------------
// forwards
// --------------------------------------------------
forward Mission_CutsceneStep(playerid);
forward Mission_InProgressStep(playerid);

// --------------------------------------------------
// publics
// --------------------------------------------------
public OnFilterScriptInit()
{
	// ���������� ����� �����
	dijkstra_init();
	// ��������� ������ ������ ������
	for(new i = 0;i < MISSIONS_COUNT;i++)
	{
		missions[i][pickup] = CreatePickup(missions[i][pickupmodel],22,missions[i][start][0],missions[i][start][1],missions[i][start][2]);
	}
	// ������� TextDraw
	Letterbox[0] = TextDrawCreate(0.0,0.0,"_");
	TextDrawUseBox(Letterbox[0],1);
	TextDrawBoxColor(Letterbox[0],0x000000ff);
	TextDrawLetterSize(Letterbox[0],0.0,12.2);	
	Letterbox[1] = TextDrawCreate(0.0,480.0,"_");
	TextDrawUseBox(Letterbox[1],1);
	TextDrawBoxColor(Letterbox[1],0x000000ff);
	TextDrawLetterSize(Letterbox[1],0.0,-15.0);
	// ��� ������ ������� ������ �������� (���� ������ �������� �� ����� ������)
	for(new playerid = 0;playerid < MAX_PLAYERS;playerid++)
	{
	    if(!IsPlayerConnected(playerid)) continue;
	    
	    OnPlayerConnect(playerid);
	}
	
	print("Controllable NPC's missions " VERSION " loaded.");
}

public OnFilterScriptExit()
{
	// ��������� ����� �����
	dijkstra_exit();
	// ������� ������ ������ ������
	for(new i = 0;i < MISSIONS_COUNT;i++) DestroyPickup(missions[i][pickup]);
	// ��� ������ ������� ������ �������� (���� ������ �������� �� ����� ������)
	for(new playerid = 0;playerid < MAX_PLAYERS;playerid++)
	{
	    if(!IsPlayerConnected(playerid)) continue;

	    OnPlayerDisconnect(playerid,0);
	}
	
	print("Controllable NPC's missions " VERSION " unloaded.");
}

public OnPlayerConnect(playerid)
{
	for(new i = 0;i < MISSIONS_COUNT;i++) players[playerid][progress][i] = 0;
	players[playerid][current_mission] = -1;
	players[playerid][current_step] = -1;
	players[playerid][timer] = -1;
}

public OnPlayerDisconnect(playerid, reason)
{
    for(new i = 0;i < MISSIONS_COUNT;i++) players[playerid][progress][i] = 0;
	Mission_Clear(playerid);
}

public OnPlayerPickUpPickup(playerid, pickupid)
{
	// ���� ������ ����� ������
	for(new i = 0;i < sizeof(missions);i++)
	{
		if(missions[i][pickup] == pickupid)
		{
			// ������ �������, ���������
			Mission_Start(i,playerid);
			// ������, �������� ����
			break;
		}
	}
	return 1;
}

public OnNPCGetDamage(npcid,playerid,Float:health_loss)
{

	return 1;
}

public OnNPCMovingComplete(npcid)
{
	move_pathstep[npcid]++;
    GetNPCVelocity(npcid,ftmp[0],ftmp[1],ftmp[2]);
	GetNPCPos(npcid,move_lastpos[npcid][0],move_lastpos[npcid][1],move_lastpos[npcid][2]);
	new Float:speed = (floatsqroot((ftmp[0]*ftmp[0]) + (ftmp[1]*ftmp[1]) + (ftmp[2]*ftmp[2])) * 5.0) + 40.0;
	new mstep = move_pathstep[npcid];
	NPC_DriveTo(npcid,move_path[npcid][mstep][0],move_path[npcid][mstep][1],move_path[npcid][mstep][2] + 1.0,speed,0);
}

public OnNPCDeath(npcid,killerid,reason)
{

}

public OnPlayerCommandText(playerid, cmdtext[])
{
	new cmd[20],
		idx;

	cmd = strtok(cmdtext,idx);

	if(!strcmp(cmd,"/mission",true))
	{
	    new i = strval(strtok(cmdtext,idx));
	    SetPlayerPos(playerid,missions[i][start][0],missions[i][start][1],missions[i][start][2]);
	    return 1;
	}
	return 0;
}

public Mission_CutsceneStep(playerid)
{
	// ��������� � �������� �� �����, ���� ��� (��������� ������ ><)
	if(!players[playerid][iscutscene]) return printf("[ERROR - missions.amx] Mission_CutsceneStep called for player not in cutscene");
	// ���������� ������� ������
	new missionid = players[playerid][current_mission];
	switch(missionid)
	{
		case 0: // Let's Rock! Part 1
		{
			// ���������� ������� ���� ��������
			switch(players[playerid][current_step])
			{
				case 0: // ������ ������
				{
				
					players[playerid][current_step]++;
					players[playerid][timer] = SetTimerEx("Mission_CutsceneStep",10000,0,"d",playerid);
				}
				case 1: // ����� ������
				{
				
					// ������������� ������ ��� ������ ������
					players[playerid][current_step] = 0;
					players[playerid][iscutscene] = 0;
					// ��������� ������
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",0,0,"d",playerid);
				}
			}
		}
		case 1: // Let's Rock! Part 2
		{
		
		}
		case 2: // Case delivery
		{
		    // ���������� ������� ���� ��������
			switch(players[playerid][current_step])
			{
				case 0: // ������ ������
				{
                    SetPlayerFacingAngle(playerid,-90.0);
				    TogglePlayerControllable(playerid,false);
					TextDrawShowForPlayer(playerid,Letterbox[0]);
					TextDrawShowForPlayer(playerid,Letterbox[1]);
					InterpolateCameraPos(playerid, 2069.5586, -1766.5908, 13.5626, 2073.5552, -1774.5889, 18.5572, 2000, CAMERA_MOVE);
					InterpolateCameraLookAt(playerid, 2069.5586, -1766.5908, 13.5626, 2069.5586, -1766.5908, 13.5626, 2000, CAMERA_MOVE);
					SetPlayerSpecialAction(playerid,SPECIAL_ACTION_USECELLPHONE);
					
					SendClientMessage(playerid,0x00FF00FF,"������, ���� ���� �������� ���� � ��������, �� ������ ������� ���");
					SendClientMessage(playerid,0x00FF00FF,"�� ���� ���������, ����� ����� � ���� �������");
					
					players[playerid][current_step]++;
					players[playerid][timer] = SetTimerEx("Mission_CutsceneStep",10000,0,"d",playerid);
				}
				case 1: // ����� ������
				{
				    SetPlayerSpecialAction(playerid,SPECIAL_ACTION_STOPUSECELLPHONE);
                    SetCameraBehindPlayer(playerid);
					TextDrawHideForPlayer(playerid,Letterbox[0]);
					TextDrawHideForPlayer(playerid,Letterbox[1]);
					TogglePlayerControllable(playerid,true);
					
					// ������������� ������ ��� ������ ������
					players[playerid][current_step] = 0;
					players[playerid][iscutscene] = 0;
					// ��������� ������
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",0,0,"d",playerid);
				}
			}
		    
		}
	}
	return 1;
}

public Mission_InProgressStep(playerid)
{
	// ��������� � �������� �� �����, ���� �� (��������� ������ ><)
	if(players[playerid][iscutscene]) return printf("[ERROR - missions.amx] Mission_InProgressStep called for player in cutscene");
	// ���������� ������� ������
	new missionid = players[playerid][current_mission];
	switch(missionid)
	{
		case 0: // Let's Rock! Part 1
		{
			// ���������� ������� ���� ����������
			switch(players[playerid][current_step])
			{
				case 0: // ������ ������
				{

				}
				case 1: // ����� ������
				{
					
				}
			}
		}
		case 1: // Let's Rock! Part 2
		{
		
		}
		case 2: // Case delivery
		{
		    // ���������� ������� ���� ��������
			switch(players[playerid][current_step])
			{
				case 0: // ������ ������
				{
		    		player_mission_data[playerid][missionid][0] = CreatePickup(1210, 23, 2788.7070, -1417.5576, 16.2500);
					player_mission_data[playerid][missionid][1] = CreateVehicle(468, 2792.5432, -1415.6924, 15.9153, 270.0, 0, 0, -1); // sanchez
					SetPlayerMapIcon(playerid, 98, 2788.7070, -1417.5576, 26.2500, 52, -1, MAPICON_GLOBAL);
					
					// ������������� ������
					players[playerid][current_step]++;
					// ������� ��������� ���
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",1000,1,"d",playerid);
				}
				case 1: // ������� ���� ����� �������� ����
				{
				    if(IsPlayerInRangeOfPoint(playerid,5.0,2788.7070, -1417.5576, 16.2500))
				    {
				        KillTimer(players[playerid][timer]);
				        // ���� ������, �������� ����� �������
				        DestroyPickup(player_mission_data[playerid][missionid][0]);
						RemovePlayerMapIcon(playerid, 98);
						TogglePlayerControllable(playerid,false);
						TextDrawShowForPlayer(playerid,Letterbox[0]);
						TextDrawShowForPlayer(playerid,Letterbox[1]);
						PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
						SetPlayerAttachedObject(playerid, 1, 1210, 5, 0.2989, 0.0939, 0.0270, -15.3, -108.0);
						InterpolateCameraPos(playerid, 2788.7070, -1417.5576, 16.2500, 2805.2500, -1419.2341, 19.2500, 2000, CAMERA_MOVE);
						InterpolateCameraLookAt(playerid, 2823.1003, -1418.5031, 16.2562, 2823.1003, -1418.5031, 16.2562, 2000, CAMERA_MOVE);
						SendClientMessage(playerid,0x00FF00FF,"����� ������...");
						
						// ������������� ������
						players[playerid][current_step]++;
						// ������� ��������� ���
						players[playerid][timer] = SetTimerEx("Mission_InProgressStep",5000,0,"d",playerid);
				    }
				}
				case 2: // ����� ����� �������, ��������� ������
				{
				    for(new i = 2;i < 6;i++)
				    {
					    player_mission_data[playerid][missionid][i] = FindLastFreeSlot();
					    format(strtmp,sizeof(strtmp),"MissionNPC_%d",player_mission_data[playerid][missionid][i]);
					    CreateNPC(player_mission_data[playerid][missionid][i],strtmp);
					    SetSpawnInfo(player_mission_data[playerid][missionid][i],0,126,0.0,0.0,0.0,0.0,0,0,0,0,0,0);
					    SpawnNPC(player_mission_data[playerid][missionid][i]);
				    }
				    player_mission_data[playerid][missionid][6] = CreateVehicle(507, 2834.1208,-1483.2860,11.9153,88.9492, 0, 0, -1);
				    PutNPCInVehicle(player_mission_data[playerid][missionid][2],player_mission_data[playerid][missionid][6],0);
				    SetNPCPos(player_mission_data[playerid][missionid][2],2834.1208,-1483.2860,11.9153);
				    SetNPCFacingAngle(player_mission_data[playerid][missionid][2],88.9492);
				    PutNPCInVehicle(player_mission_data[playerid][missionid][3],player_mission_data[playerid][missionid][6],1);
				    PutNPCInVehicle(player_mission_data[playerid][missionid][4],player_mission_data[playerid][missionid][6],2);
				    PutNPCInVehicle(player_mission_data[playerid][missionid][5],player_mission_data[playerid][missionid][6],3);

					SetPlayerCheckpoint(playerid,1459.1420,-1264.5731,13.3906,4.0);
				    SetCameraBehindPlayer(playerid);
					TextDrawHideForPlayer(playerid,Letterbox[0]);
					TextDrawHideForPlayer(playerid,Letterbox[1]);
					TogglePlayerControllable(playerid,true);
					
					// ������������� ������
					players[playerid][current_step]++;
					// ������� ��������� ���
					players[playerid][timer] = SetTimerEx("Mission_InProgressStep",1000,1,"d",playerid);
				}
				case 3: // ������, ���� �������� �� �������, ���� ����� ������� �� ����� ���������� - ������ ��������
				{
				    if(IsPlayerInRangeOfPoint(playerid,5.0,1459.1420,-1264.5731,13.3906))
				    {
				        KillTimer(players[playerid][timer]);
				        // ����� ������ ����� ����������, ������ ��������
				        DestroyVehicle(player_mission_data[playerid][missionid][1]);
					    for(new i = 2;i < 6;i++)
					    {
						    DestroyNPC(player_mission_data[playerid][missionid][i]);
					    }
					    DestroyVehicle(player_mission_data[playerid][missionid][6]);
				        GameTextForPlayer(playerid, "~g~~h~Mission Passed!", 3000, 3);
						PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
						GivePlayerMoney(playerid, 5000);
						DisablePlayerCheckpoint(playerid);
						RemovePlayerAttachedObject(playerid, 1);
				        
						// ������������� ������
						players[playerid][current_mission] = -1;
						players[playerid][current_step] = 0;
						players[playerid][iscutscene] = 0;
						players[playerid][progress][missionid] = 1;
						players[playerid][timer] = -1;
				    }
				    else
				    {
				        new npcid = player_mission_data[playerid][missionid][2];
				        // ��������� ������������� �����������
				        new chk = move_pathlen[npcid];
				        if((move_pathlen[npcid] == 0) || (!IsPlayerInRangeOfPoint(playerid,20.0,move_path[npcid][chk][0],move_path[npcid][chk][1],move_path[npcid][chk][2])))
				        {
				            GetPlayerPos(playerid,ftmp[3],ftmp[4],ftmp[5]);
					        GetNPCPos(npcid,ftmp[0],ftmp[1],ftmp[2]);
					        // ��������� ���� �� ����� �� ������
					        move_pathlen[npcid] = MAX_PATH;
					        CalculatePath(ftmp[0],ftmp[1],ftmp[2],ftmp[3],ftmp[4],ftmp[5],move_path[npcid],move_pathlen[npcid]);
					        if(move_pathlen[npcid] > 0)
					        {
					            GetPlayerVelocity(playerid,ftmp[0],ftmp[1],ftmp[2]);
					            new mstep = move_pathstep[npcid] = 0;
					            // check step
					            ftmp[0] = move_lastpos[npcid][0] - move_path[npcid][mstep][0];
					            ftmp[1] = move_lastpos[npcid][1] - move_path[npcid][mstep][1];
					            ftmp[2] = move_lastpos[npcid][2] - move_path[npcid][mstep][2];
					            new Float:d = floatsqroot((ftmp[0]*ftmp[0]) + (ftmp[1]*ftmp[1]) + (ftmp[2]*ftmp[2]));
					            if(d < 1.0) mstep = move_pathstep[npcid] = 1;
					            NPC_DriveTo(npcid,move_path[npcid][mstep][0],move_path[npcid][mstep][1],move_path[npcid][mstep][2] + 1.0,40.0,0);
					        }
					        else printf("[%d] cant find path",npcid);
					        // ��������� ����� �����������
					        move_lastrecalc[npcid] = GetTickCount();
						}
				    }
				}
			}
		}
	}
	return 1;
}

public OnPlayerDeath(playerid, killerid, reason)
{
	if(players[playerid][current_mission] != -1)
	{
	    GameTextForPlayer(playerid, "~g~~h~Mission Failed!", 3000, 3);
	    Mission_Clear(playerid);
	}
	return 1;
}

// --------------------------------------------------
// stocks
// --------------------------------------------------

stock Mission_Start(missionid,playerid)
{
	// ��������� ��� ���������� ������
	if(players[playerid][current_mission] != -1)
	{
		// ����� �� ����� ������ ������
		SendClientMessage(playerid,MESSAGES_COLOR,"� ���� � ��� ���� ����.");
		return 0;
	}
	// ��������� ���������� ������
	if(missions[missionid][request] != -1)
	{
		new requestid = missions[missionid][request];
		if(!players[playerid][progress][requestid])
		{
			// ����� �� ����� ������ ������
			SendClientMessage(playerid,MESSAGES_COLOR,"�� ������ ������ ��� ���� ������� ���.");
			return 0;
		}
	}
	// ������������� ������ ������
	players[playerid][current_mission] = missionid;
	players[playerid][current_step] = 0;
	players[playerid][iscutscene] = 1;
	// �������� ���������� ��������
	players[playerid][timer] = SetTimerEx("Mission_CutsceneStep",0,0,"d",playerid);
	return 1;
}

stock Mission_Clear(playerid)
{
    // ������� ������ ������
	new missionid = players[playerid][current_mission];
	switch(missionid)
	{
	    case 2: // Case delivery
		{
			switch(players[playerid][current_step])
			{
				case 1:
				{
		    		DestroyPickup(player_mission_data[playerid][missionid][0]);
					DestroyVehicle(player_mission_data[playerid][missionid][1]);
					RemovePlayerMapIcon(playerid, 98);
				}
				case 2:
				{
				    DestroyVehicle(player_mission_data[playerid][missionid][1]);
				    RemovePlayerAttachedObject(playerid, 1);
				}
				case 3:
				{
				    DestroyVehicle(player_mission_data[playerid][missionid][1]);
				    RemovePlayerAttachedObject(playerid, 1);
				    for(new i = 2;i < 6;i++)
				    {
					    DestroyNPC(player_mission_data[playerid][missionid][i]);
				    }
				    DestroyVehicle(player_mission_data[playerid][missionid][6]);
				    DisablePlayerCheckpoint(playerid);
				}
			}
		}
	}
	if(players[playerid][timer] != 0) KillTimer(players[playerid][timer]);
	players[playerid][current_mission] = -1;
	players[playerid][current_step] = -1;
}

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
