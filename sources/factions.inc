/*
*	Created:			18.05.2012
*	Author:				LukeS
*	Description:		�������
*/

#if defined _factions_included
	#endinput
#endif

#define _factions_included
#pragma library factions
// --------------------------------------------------
// Defines
// --------------------------------------------------
#define FACTIONS_TICK 						1200
#define HELICHECK							60
#define REHIDE								60
#define BOUNTY_FILE							"bounty_1.txt"
// --------------------------------------------------
// Numbers,strings etc...
// --------------------------------------------------
new ThievCheck,
	carwanted,
	chasing,
	//heli,
	tick,
	bounty,
	bool:IsHunting;

static cars[]=
{
	412,
	534,
	535,
	536,
	566,
	567,
	575,
	576,
	448,
	461,
	462,
	463,
	468,
	471,
	481,
	509,
	510,
	521,
	522,
	523,
	581,
	586,
	402,
	411,
	415,
	429,
	451,
	475,
	477,
	494,
	496,
	502,
	503,
	506,
	541,
	558,
	559,
	565,
	587,
	589,
	602,
	603,
	407,
	416,
	420,
	427,
	431,
	432,
	433,
	437,
	438,
	490,
	528,
	544,
	596,
	597,
	598,
	599,
	601
};
static awards[][]=
{
"Hotring Racer", "Bloodring Banger","XP Bonus","Large XP Bonus","Extra $$$","Large Extra$$$","No cops","Car repair"
};
static	neon[MAX_PLAYERS char],
		neon2[MAX_PLAYERS char],
		VehNeon[MAX_VEHICLES char],
		option1[MAX_PLAYERS char],
		option2[MAX_PLAYERS char],
		option3[MAX_PLAYERS char],
		HasSupplied[MAX_PLAYERS char];
//new WLevel[MAX_PLAYERS char];
//enums
enum
{
SETMAINCOP,
SETCOP,
COPBOSS,
COP,
COPS_CMD,
ARREST_CMD,
COPSHELP_CMD,
REASON_DEAD,
REASON_ARREST,
HANDSUP_CMD,
SUSPECT_CMD,
SETMAINTHIEF,
SETTHIEF,
THIEF,
THIEFBOSS,
THIEVES_CMD,
THIEFHELP_CMD,
CARWANTED_CMD,
SETMAINRACER,
SETRACER,
RACER,
RACERBOSS,
RACERHELP_CMD,
RACERS_CMD,
SETMAINFBI,
SETFBI,
FBIBOSS,
FBI,
FBIS_CMD,
HELI_CMD,
FBIHELP_CMD,
WANTED_CMD
};
//=====================================
//forwards
//=====================================
forward RandomizeCar();

Factions_Command(playerid,cmdid,params[])
{
	switch(cmdid)
	{
		case SETMAINCOP:
		{
			if(GetPVarInt(playerid,"AdminLevel") == 0) return SendClientMessage(playerid,COLOR_WHITE,"������ ������ � ���������� ����� ��������� ���� �������!");
			new id;
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"������ �������������: /setmaincop id");
			id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"����� �������");
			SetPVarInt(id,"Work",COPBOSS);
			format(stmp,sizeof(stmp),"%s �������� ������ %s ������ �����",oGetPlayerName(playerid),oGetPlayerName(id));
			SendClientMessageToAll(COLOR_WHITE,stmp);
			return 1;

		}
		case SETCOP:
		{
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"������ �������������: /setcop id");
			new id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"����� �������");
			if(GetPVarInt(playerid,"Work")==COPBOSS)
			{
			SetPVarInt(id,"work",COP);
			format(stmp,sizeof(stmp),"����� ����� %s ���� �� ������ ������ %s",oGetPlayerName(playerid),oGetPlayerName(id));
			SendClientMessageToAll(COLOR_WHITE,stmp);
			}
			else SendClientMessage(playerid,COLOR_ORANGE,"�� �� ����� �����!");
			return 1;
		}
		case COPS_CMD:
		{
		SendClientMessage(playerid,COLOR_WHITE,"������� ������");
		//for(new i = 0;i <= GetPlayersCount();i++)
		foreach(new i : Player)
		{
		//new str[100];
		//if(!IsPlayerConnected(i)) continue;
		if(IsPlayerNPC(i)) continue;
		if(GetPVarInt(i,"Work") == COPBOSS)
		{
		format(stmp,sizeof(stmp),"%s - ������� ���\n",oGetPlayerName(i));
		SendClientMessage(playerid,COLOR_BLUE,stmp);
		}
		if(GetPVarInt(i,"Work") == COP)
		{
		format(stmp,sizeof(stmp),"%s\n",oGetPlayerName(i));
		SendClientMessage(playerid,COLOR_BLUE,stmp);
		}
		}
		return 1;
		}
		case ARREST_CMD:
		{
			itmp[0]=GetPVarInt(playerid,"Work");
			if(itmp[0]== COPBOSS || itmp[0]== COP || itmp[0]== FBIBOSS || itmp[0]== FBI)
			{
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"������ �������������: /arrest id");
			new id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"����� �������");
			if(GetPlayerWantedLevel(id)==0) return SendClientMessage(playerid,COLOR_BLUE,"����� �� � �������");
			if(GetDistanceBetweenPlayers(playerid,id)>10) return SendClientMessage(playerid,COLOR_BLUE,"�� ������ �� ��������������");
			if(GetPlayerSpecialAction(id) != SPECIAL_ACTION_HANDSUP) return SendClientMessage(playerid,COLOR_BLUE,"����� �� ������� ���������!");
			//SetPlayerSpecialAction(id,SPECIAL_ACTION_CUFFED);
			SendClientMessage(playerid,COLOR_BLUE,"������������� ���������");
			award(playerid,GetPlayerWantedLevel(id),REASON_ARREST);
			format(stmp,sizeof(stmp),"������� ����� ����� �� ��������� � ������� %d $",-1000*GetPlayerWantedLevel(id));
			oGivePlayerMoney(id,-1000*GetPlayerWantedLevel(id));
			//WLevel{id}=0;
			SetPlayerWantedLevel(id,0);
			SendClientMessage(id,COLOR_BLUE,stmp);
			}
			else return SendClientMessage(playerid,COLOR_BLUE,"�� �� �����������/���!");
			return 1;
		}
		case COPSHELP_CMD:
		{
		ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"������ �� ��������",
            "�������\n\
			���� ����� ���� �� ����, � ���� ������������ ���� ������ �������\n\
			��� ���� ������� �������, ��� ���� ������� �� ���������� �����������\n\
			�������:\n\
			/setmaincop id (��� ������� � �������) - ��������� ��. ����\n\
			/setcop id(��� ����� �������) - ������ ������ �� ������ �����������\n\
			/cops - ���������� ����������� ������\n\
			/wanted - c����� ������������\n\
			/arrest id - ���������� �����������(������ �������)\n\
			/suspect id(������ ��� �����) - ������ � ������ �� ������"
            ,"��","");
		return 1;
		}
		case HANDSUP_CMD:
		{
		SetPlayerSpecialAction(playerid,SPECIAL_ACTION_HANDSUP);
		return 1;
		
		}
		case SUSPECT_CMD:
		{
		if(GetPVarInt(playerid,"Work") == COPBOSS ||GetPVarInt(playerid,"Work") == FBIBOSS) 
		{
		if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"������ �������������: /arrest id");
		new id=strval(params);
		if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"����� �������");
		new k=0;
		id=strval(strtok(params,k));
		new WL=strval(strtok(params,k));
		SetPlayerWantedLevel(id,WL);
		format(stmp,sizeof(stmp),"%s ������� � ������ ������ %s �� �������: %s (������� �������:%d)",oGetPlayerName(playerid),oGetPlayerName(id),params[k],WL);
		PlayCrimeReportForPlayer(playerid, id, 3);
		PlayCrimeReportForPlayer(id, id, 3);
		SendClientMessageToAll(COLOR_BLUE,stmp);
		}
		else return SendClientMessage(playerid,COLOR_BLUE,"������ ����� ������� � ��� ����� �������� � ������!");
		return 1;
		}
		case SETMAINTHIEF:
		{
			if(GetPVarInt(playerid,"AdminLevel") == 0) return SendClientMessage(playerid,COLOR_WHITE,"������ ������ � ���������� ����� ��������� ���� �������!");
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"������ �������������: /setmainthief id");
			new id;
			id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"����� �������");
			SetPVarInt(id,"Work",THIEFBOSS);
			format(stmp,sizeof(stmp),"%s �������� ������ %s ������ ���������",oGetPlayerName(playerid),oGetPlayerName(id));
			SendClientMessageToAll(COLOR_WHITE,stmp);
			SetPlayerMapIcon(playerid,99,1572.0670,-1477.8331,13.3571,55,0,1);
			return 1;

		}
		case SETTHIEF:
		{
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"������ �������������: /setthief id");
			new id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"����� �������");
			if(GetPVarInt(playerid,"Work")==THIEFBOSS)
			{
			SetPVarInt(id,"Work",THIEF);
			format(stmp,sizeof(stmp),"����� ��������� %s ���� �� ������ ������ %s",oGetPlayerName(playerid),oGetPlayerName(id));
			SendClientMessageToAll(COLOR_WHITE,stmp);
			SetPlayerMapIcon(playerid,99,1572.0670,-1477.8331,13.3571,55,0,1);
			}
			else SendClientMessage(playerid,COLOR_ORANGE,"�� �� ����� �����!");
			return 1;
		}
		case THIEVES_CMD:
		{
		SendClientMessage(playerid,COLOR_WHITE,"�������� ������");
		//for(new i = 0;i <= GetPlayersCount();i++)
		foreach(new i : Player)
		{
		//new str[100];
		//if(!IsPlayerConnected(i)) continue;
		if(IsPlayerNPC(i)) continue;
		if(GetPVarInt(i,"Work") == THIEFBOSS)
		{
		format(stmp,sizeof(stmp),"%s - ������� �������\n",oGetPlayerName(i));
		SendClientMessage(playerid,COLOR_BLUE,stmp);
		}
		if(GetPVarInt(i,"Work") == THIEF)
		{
		format(stmp,sizeof(stmp),"%s\n",oGetPlayerName(i));
		SendClientMessage(playerid,COLOR_BLUE,stmp);
		}
		}
		return 1;
		}
		case THIEFHELP_CMD:
		{
		ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"������ �� ��������",
            "��������\n\
			��������� �������� ���������, � ������� ���������� ������������ ������� ����������\n\
			������ ��������� - ��������� ���� ��������� � ������ ����� �� �������\n\
			�������:\n\
			/setmainthief id (��� ������� � �������) - ��������� ����� ���������\n\
			/setthief id(��� ����� �������) - ������ ������ �� ������ ���������\n\
			/thieves - ���������� ��������� ������"
            ,"��","");
		return 1;
		}
		case CARWANTED_CMD:
		{
		format(stmp,sizeof(stmp),"�� ������ ������ ��������� : %s",oGetVehicleName(carwanted));
		ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"��������� ���������",stmp,"��","");
		return 1;
		}
		case WANTED_CMD:
		{
		format(stmp,sizeof(stmp),"�����			������� �������\n");
		//for(new i=0;i<=GetPlayersCount();i++)
		foreach(new i : Player)
			{
			if(!IsPlayerConnected(i)) continue;
			itmp[1]=GetPlayerWantedLevel(i);
			if(itmp[1]>0)
				{
				//print("FOUND ONE");
				format(stmp,sizeof(stmp),"%s%s(%d)			%d\n",stmp,oGetPlayerName(i),i,itmp[1]);
				
				}
			
			}
		ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"� �������",stmp,"��","");
		return 1;
		}
		case SETMAINRACER:
		{
			if(GetPVarInt(playerid,"AdminLevel") == 0) return SendClientMessage(playerid,COLOR_WHITE,"������ ������ � ���������� ����� ��������� ���� �������!");
			new id;
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"������ �������������: /setmainracer id");
			id=strval(params);
			
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"����� �������");
			SetPVarInt(id,"Work",RACERBOSS);
			format(stmp,sizeof(stmp),"%s �������� ������ %s ������ ������� ��������",oGetPlayerName(playerid),oGetPlayerName(id));
			SendClientMessageToAll(COLOR_WHITE,stmp);
			return 1;

		}
		case SETRACER:
		{
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"������ �������������: /setracer id");
			new id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"����� �������");
			if(GetPVarInt(playerid,"Work")==RACERBOSS)
			{
			SetPVarInt(id,"Work",RACER);
			format(stmp,sizeof(stmp),"����� ������� �������� %s ���� �� ������ ������ %s",oGetPlayerName(playerid),oGetPlayerName(id));
			SendClientMessageToAll(COLOR_WHITE,stmp);
			}
			else SendClientMessage(playerid,COLOR_ORANGE,"�� �� ����� ��������!");
			return 1;
		}
		case RACERHELP_CMD:
		{
		ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"������ �� ��������",
            "������� �������:\n\
			������� ������� ����������� �������� � ��������������� ��������.\n\
			��� ����� ������ ���� ��������� ���������� �� ����, � ��� �� ������������� �������� ���.\n\
			��� �� ��� �������� �������������� ������ �� �����.\n\
			�������:\n\
			/setmainracer id (��� ������� � �������) - ��������� ����� ��������\n\
			/setracer id(��� ����� ������� ��������) - ������ ������ �� ������ ��������\n\
			/racers - ���������� �������� ������"
            ,"��","");
		return 1;
		}
		case RACERS_CMD:
		{
		SendClientMessage(playerid,COLOR_WHITE,"������� ������");
		//for(new i = 0;i <= GetPlayersCount();i++)
		foreach(new i : Player)
		{
		//new str[100];
		//if(!IsPlayerConnected(i)) continue;
		if(IsPlayerNPC(i)) continue;
		if(GetPVarInt(i,"Work") == RACERBOSS)
		{
		format(stmp,sizeof(stmp),"%s - ����� ������� ��������\n",oGetPlayerName(i));
		SendClientMessage(playerid,COLOR_BLUE,stmp);
		}
		if(GetPVarInt(i,"Work") == RACER)
		{
		format(stmp,sizeof(stmp),"%s\n",oGetPlayerName(i));
		SendClientMessage(playerid,COLOR_BLUE,stmp);
		}
		}
		return 1;
		}
		case SETMAINFBI:
		{
			if(GetPVarInt(playerid,"AdminLevel") == 0) return SendClientMessage(playerid,COLOR_WHITE,"������ ������ � ���������� ����� ��������� ���� �������!");
			new id;
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"������ �������������: /setmainfbi id");
			id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"����� �������");
			SetPVarInt(id,"Work",FBIBOSS);
			format(stmp,sizeof(stmp),"%s �������� ������ %s ������ FBI",oGetPlayerName(playerid),oGetPlayerName(id));
			SendClientMessageToAll(COLOR_WHITE,stmp);
			return 1;

		}
		case SETFBI:
		{
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"������ �������������: /setFBI id");
			new id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"����� �������");
			if(GetPVarInt(playerid,"Work")==FBIBOSS)
			{
			SetPVarInt(id,"work",FBI);
			format(stmp,sizeof(stmp),"����� ��� %s ���� �� ������ ������ %s",oGetPlayerName(playerid),oGetPlayerName(id));
			SendClientMessageToAll(COLOR_WHITE,stmp);
			}
			else SendClientMessage(playerid,COLOR_ORANGE,"�� �� ����� �����!");
			return 1;
		}
		case FBIS_CMD:
		{
		SendClientMessage(playerid,COLOR_WHITE,"��� ������");
		//for(new i = 0;i <= GetPlayersCount();i++)
		foreach(new i : Player)
		{
		//new stmp[100];
		if(!IsPlayerConnected(i)) continue;
		if(IsPlayerNPC(i)) continue;
		if(GetPVarInt(i,"Work") == FBIBOSS)
		{
		format(stmp,sizeof(stmp),"%s - ����� ���\n",oGetPlayerName(i));
		SendClientMessage(playerid,COLOR_BLUE,stmp);
		}
		if(GetPVarInt(i,"Work") == FBI)
		{
		format(stmp,sizeof(stmp),"%s\n",oGetPlayerName(i));
		SendClientMessage(playerid,COLOR_BLUE,stmp);
		}
		}
		return 1;
		}
		case HELI_CMD:
		{
			itmp[0]=GetPVarInt(playerid,"Work");
			if(itmp[0]== FBIBOSS || itmp[0]== FBI)
			{
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"������ �������������: /heli id");
			new id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"����� �������");
			if(GetPlayerWantedLevel(id)==0) return SendClientMessage(playerid,COLOR_BLUE,"����� �� � �������");
			//if(GetDistanceBetweenPlayers(playerid,id)>10) return SendClientMessage(playerid,COLOR_BLUE,"�� ������ �� ��������������");
			//if(GetPlayerSpecialAction(id) != SPECIAL_ACTION_HANDSUP) return SendClientMessage(playerid,COLOR_BLUE,"����� �� ������� ���������!");
			//SetPlayerSpecialAction(id,SPECIAL_ACTION_CUFFED);
			chasing=id;
			FBI_HeliUpdate(id);
			SendClientMessage(playerid,COLOR_BLUE,"�������� ����� �������������");
			//award(playerid,GetPlayerWantedLevel(id),REASON_ARREST);
			//format(stmp,sizeof(stmp),"������� ����� ����� �� ��������� � ������� %d $",-1000*GetPlayerWantedLevel(id));
			//GivePlayerMoney(id,-1000*GetPlayerWantedLevel(id));
			//WLevel{id}=0;
			//SetPlayerWantedLevel(id,0);
			//SendClientMessage(id,COLOR_BLUE,stmp);
			}
			else return SendClientMessage(playerid,COLOR_BLUE,"�� �� ���!");
			return 1;
		}
		case FBIHELP_CMD:
		{
		ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"������ �� ��������",
            "���:\n\
			��� ������� �� ������� ������������\n\
			��� ����� �������� �������� ��������, ������� ����� �������� �������� ��� ��������������� �����������\n\
			�������:\n\
			/setmainfbi id (��� ������� � �������) - ��������� ����� ���\n\
			/setfbi id(��� ����� ���) - ������ ������ �� ������\n\
			/wanted - ������ ������������\n\
			/fbis - ���������� ������� ��� ������\n\
			/arrest id - ���������� �����������(������ �������)\n\
			/suspect id(������ ��� �����) - ������ � ������ �� ������"
            ,"��","");
		return 1;
		}
		
	}
	return 0;
}
Factions_OnPlayerDeath(playerid, killerid, reason)
{
#pragma unused reason
if(killerid!=INVALID_PLAYER_ID)
{
	new WL=GetPlayerWantedLevel(playerid);
	if(chasing==playerid) chasing=-1;
	if(GetPVarInt(killerid,"Work") == COP || GetPVarInt(killerid,"Work") ==COPBOSS)
	{
	if(WL>0)
		{
		award(killerid,WL,REASON_DEAD);
		//WLevel{playerid}=0;
		SetPlayerWantedLevel(playerid,0);
		}
	else SendClientMessage(killerid,COLOR_BLUE,"�� ����� �����������!");
	}
	if(GetPVarInt(killerid,"Work") == FBI || GetPVarInt(killerid,"Work") ==FBIBOSS)
	{
	if(WL>0)
		{
		if(WL>4)
		{
		award(killerid,WL,REASON_DEAD);
		//WLevel{playerid}=0;
		}
		else SendClientMessage(killerid,COLOR_BLUE,"����� ���� ���������� �������� ���������, ���� ������ ��������� �����������");
		SetPlayerWantedLevel(playerid,0);
		}
	else SendClientMessage(killerid,COLOR_BLUE,"�� ����� �����������!");
	}

	else
	{
		//new i;
		//WLevel{killerid}++;
		SetPlayerWantedLevel(killerid,GetPlayerWantedLevel(killerid)+1);
		SendClientMessage(killerid,COLOR_BLUE,"��� ����������� �������,����� ������� ������� /handsup");
		
		format(stmp,sizeof(stmp),"%s ������������� �� �������� %s",oGetPlayerName(killerid),oGetPlayerName(playerid));
		//for(i = 0;i <= GetPlayersCount();i++)
		foreach(new i : Player)
		{
		if(!IsPlayerConnected(i)) continue;
		if(IsPlayerNPC(i)) continue;
		itmp[0]=GetPVarInt(i,"Work");
		if(itmp[0] == COP || itmp[0] ==COPBOSS || itmp[0] == FBI || itmp[0] ==FBIBOSS)
		{
		SendClientMessage(i,COLOR_BLUE,stmp);
		PlayCrimeReportForPlayer(i, killerid, 3);
		}
		}
		
		
		
		
		PlayCrimeReportForPlayer(killerid, killerid, 3);
		
	}
}
SetPlayerWantedLevel(playerid,0);
return 1;
}
Factions_OnPlayerSpawn(playerid)
{
	new k;
	new wrk =GetPVarInt(playerid,"Work");
	HasSupplied[playerid]=0;
	if(wrk==THIEF || wrk== THIEFBOSS) k=1;
	switch(k)
	{
		case 1:
		{
		//nothing to do here :D
		}
	
	}
	return 1;// make thieves and others have their chekpoints and missions
}
Factions_OPESC(playerid,checkpointid)
{
//format(stmp,sizeof(stmp),"Chid = %d thievch = %d carwant= %d ",checkpointid,ThievCheck,carwanted);
//SendClientMessage(playerid,COLOR_BLUE,stmp);

if(checkpointid == ThievCheck)
{
//if(IsPlayerInAnyVehicle(playerid)) return 1;
if(GetVehicleModel(GetPlayerVehicleID(playerid))== carwanted)
	{
	new moneygive=random(5000)+7000;
	new xpgive=random(2000)*(GetPVarInt(playerid,"Level")+1);
	oGivePlayerMoney(playerid,moneygive);
	GivePlayerXP(playerid,xpgive);
	format(stmp,sizeof(stmp),"������� �� �����! ��� ���� ������� - %d $ � %d �����",moneygive,xpgive);
	SendClientMessage(playerid,COLOR_BLUE,stmp);
	SetVehicleToRespawn(GetPlayerVehicleID(playerid));
	RandomizeCar();
	}
}
return 1;
}
Factions_OnGameModeInit()
{

//SetActorInvulnerable(bounty,0);
//ApplyActorAnimation(bounty, "ped", "WEAPON_crouch", 4.1, 0, 0, 0, 0, 0);
//heli=AddStaticVehicle(497,0.0,0.0,-100.0,0.0,0,1);
chasing=-1;
ThievCheck=CreateStreamedCheckpoint(1572.0670,-1477.8331,13.3571,5);
//SetTimer("RandomizeCar",1200000,true);
RandomizeCar();
print("Factons Loaded.");
}
Factions_OnSavePlayerData(playerid,File:datafile,reason)
{
		#pragma unused reason
		format(stmp,sizeof(stmp),"Work=%d\r\n",GetPVarInt(playerid,"Work"));
		fwrite(datafile,stmp);

}
Factions_OnLoadPlayerData(playerid,datastr[],separatorpos)
{
if(!strcmp(datastr,"Work",true,separatorpos)) 
	{
		SetPVarInt(playerid,"Work",strval(datastr[separatorpos + 1]));
	}
new wrk=GetPVarInt(playerid,"Work");
if(wrk == THIEF|| wrk== THIEFBOSS)
{
SetPlayerMapIcon(playerid,99,1572.0670,-1477.8331,13.3571,55,0,1);
}
}
award(killerid,WL,reason)
{
		new moneygive,xpgive;
		if(reason == REASON_ARREST)
		{
		moneygive=150*(10*WL);
		xpgive=1500*(10*WL);
		oGivePlayerMoney(killerid,moneygive);
		GivePlayerXP(killerid,xpgive);
		format(stmp,sizeof(stmp),"�� ���������� ����������� � ������� ������� %d ���� ������� - %d $ � %d �����",WL,moneygive,xpgive);
		}
		else
		{
		moneygive=100*(10*WL);
		xpgive=1000*(10*WL);
		oGivePlayerMoney(killerid,moneygive);
		GivePlayerXP(killerid,xpgive);
		format(stmp,sizeof(stmp),"�� ��������� ����������� � ������� ������� %d ���� ������� - %d $ � %d �����",WL,moneygive,xpgive);
		
		}
		SendClientMessage(killerid,COLOR_BLUE,stmp);
		return 1;

}
Factions_tick()
{
if(tick%REHIDE==0)
{
//remove that
if(IsHunting==true) ReHide();

}
if(tick%HELICHECK==0 && chasing!=-1)
{
//printf("chasing is %d",chasing);
FBI_HeliUpdate(chasing);
}
if(tick==FACTIONS_TICK)
	{
	if(IsNight()) BountyUpdate();
	
	RandomizeCar();
	tick=0;
	}
tick+=5;
//printf("%d",tick);
//printf("factions tick:%d CHASING :%d",tick,chasing);
return 1;
}
BountyUpdate()
{
SendClientMessageToAll(COLOR_LIGHTRED,"� ����� Angel Pine �������� ����������, ������� ��� � �������� �������");
if(IsHunting==true) return 1;
IsHunting=true;
bounty=CreateActor(162,2467.3535,-1632.1931,13.4001,173.9677);

//invull
SetActorInvulnerable(bounty,0); 
return 1;
}
ReHide()
{
	//SendClientMessageToAll(COLOR_WHITE,"REHIDDEN");
	new File:df = fopen(BOUNTY_FILE ,io_read);
	if(df)
	{
		// count of spawns
		new counts;
		while(fread(df,stmp)) counts++;
		// set on start
		fseek(df);
		// randomize spawn
		counts = random(counts);
		// go
		while(fread(df,stmp))
		{
			counts--;
			if(counts > 0) continue;
			
			new c;
			ftmp[0] = floatstr(strtok(stmp,c,','));
			ftmp[1] = floatstr(strtok(stmp,c,','));
			ftmp[2] = floatstr(strtok(stmp,c,','));
			break;
		}
		SetActorPos(bounty,ftmp[0],ftmp[1],ftmp[2]);
		ApplyActorAnimation(bounty, "ped", "WEAPON_crouch", 4.1, 1, 0, 0, 0, 0);
		fclose(df);
	}
	else print("[ERROR] not found bounty spawns file");
	return 1;
}
Factions_OPGDA(playerid, damaged_actorid, Float: amount, weaponid, bodypart)
{
#pragma unused amount,bodypart,weaponid
if(damaged_actorid==bounty)
	{
	format(stmp,sizeof(stmp),"%s ���� ����, ������� - 10 000$",oGetPlayerName(playerid));
	SendClientMessageToAll(COLOR_WHITE,stmp);
	oGivePlayerMoney(playerid,10000);
	DestroyActor(bounty);
	IsHunting=false;
	}
return 1;
}

public RandomizeCar()
{
		carwanted=cars[random(sizeof(cars))];
		format(stmp,sizeof(stmp),"��� ����� ������ ����� %s ���� �� � ��� � �������� �������",oGetVehicleName(carwanted));
		//for(new i = 0;i <= GetPlayersCount();i++)
		foreach(new i : Player)
		{
		if(!IsPlayerConnected(i)) continue;
		if(IsPlayerNPC(i)) continue;
		new wrk =GetPVarInt(i,"Work");
		if(wrk==THIEF || wrk== THIEFBOSS)
		{
		SendClientMessage(i,COLOR_BLUE,stmp);
		}
		}
		return 1;
}
Factions_VehInteract(playerid)
{
new wrk =GetPVarInt(playerid,"Work");
new skill = GetPVarInt(playerid,"DriverSkill");
new listitems[256];
listitems="�������� �����\n��������� ������";
if(skill>=200) strcat(listitems, "\n�������/������� �����");
if(wrk==RACER || wrk == RACERBOSS)
	{
		if(skill>=400) strcat(listitems, "\n���������� ������\n��������� ����(1000$)");
		if(skill>=500) strcat(listitems, "\n���������� �����(5000$)");
	}
ShowPlayerDialog(playerid,DIALOG_INCAR,DIALOG_STYLE_LIST,"������",listitems," �������"," ������");
return 1;
}
Factions_Dialog(playerid,dialogid,response,listitem,inputtext[])
{
//#pragma unused inputtext
if(!response) return 1;
itmp[0]=GetPlayerVehicleID(playerid);
itmp[1]=GetVehicleModel(itmp[0]);
switch(dialogid)
	{
	case DIALOG_INCAR:
		{
			switch(listitem)
				{
				case 0:
					{
					Player_Command(playerid,PLAYER_RADIO_CMD,"");
					return 1;
					}
				case 1:
					{
					ShowPlayerDialog(playerid,DIALOG_PLATE,DIALOG_STYLE_INPUT,"��������� ����","������� �������� �����","���������","������ ");
					return 1;
					}
				case 2:
					{
					new engine,lights,alarm,doors,bonnet,boot,objective;
					GetVehicleParamsEx(itmp[0],engine,lights,alarm,doors,bonnet,boot,objective);
					SetVehicleParamsEx(itmp[0],engine,lights,alarm,true,bonnet,boot,objective);
					SetVehicleParamsForPlayer(itmp[0],playerid,false,objective);
					return 1;
					}
				case 3:
					{
					GetVehicleHealth(itmp[0],ftmp[0]);
					printf("%f current health",ftmp[0]);
					ftmp[0]=ftmp[0]+random(5)*(GetPVarInt(playerid,"DriverSkill")/10);
					printf("%f after repairs",ftmp[0]);
					if(ftmp[0]>1000.0) ftmp[0]=1000.0;
					SetVehicleHealth(itmp[0],ftmp[0]);
					format(stmp,sizeof(stmp),"��������� �������� ��������������, ������� ��������� ~ %0.f",ftmp[0]/10);
					SendClientMessage(playerid,COLOR_LIGHTGREEN,stmp);
					return 1;
					}
				case 4:
					{
					//VehicleAddNeon(itmp[0],color);
					if(oGetPlayerMoney(playerid)<1000) return SendClientMessage(playerid,COLOR_SPRINGGREEN,"� ��� ������������ �����!");
					oGivePlayerMoney(playerid,-1000);
					ShowPlayerDialog(playerid,DIALOG_NEON,DIALOG_STYLE_LIST,"�������� ����","�����\n�������\n�������\n������\n�������\n�����","�������"," ������");
					}
				case 5:
					{
					new skill = GetPVarInt(playerid,"DriverSkill");
					if(oGetPlayerMoney(playerid)<5000) return SendClientMessage(playerid,COLOR_SPRINGGREEN,"� ��� ������������ �����!");
					oGivePlayerMoney(playerid,-5000);
					if(skill<=500)
						{
						AddVehicleComponent(itmp[0],1009);
						SendClientMessage(playerid,COLOR_SPRINGGREEN,"��� ����� �������� ��� ���������� 2� �����!");
						}
					if(skill>=750&& skill<900)
						{
						AddVehicleComponent(itmp[0],1008);
						SendClientMessage(playerid,COLOR_SPRINGGREEN,"��� ����� �������� ��� ���������� 5� �����!");
						}
					if(skill>900)
						{
						AddVehicleComponent(itmp[0],1010);
						SendClientMessage(playerid,COLOR_SPRINGGREEN,"��� ����� �������� ��� ���������� 10� �����!");
						}
					return 1;
					}
			
				}
			return 1;
		}
	case DIALOG_PLATE:
		{
			Player_Command(playerid,PLAYER_SAVECAR_CMD,inputtext);
			return 1;
		}
	case DIALOG_WIN:
		{
		switch(listitem)
			{
			case 0:
				{
				Award_Player(playerid,option1[playerid]);
				printf("AWARD FIRED type is %d",option1[playerid]);
				printf("AWARD FIRED type2 is %d",option2[playerid]);
				printf("AWARD FIRED type3 is %d",option3[playerid]);
				return 1;
				}
			case 1:
				{
				Award_Player(playerid,option2[playerid]);
				return 1;
				}
			case 2:
				{
				Award_Player(playerid,option3[playerid]);
				return 1;
				}
			}
		return 1;
		}
	case DIALOG_NEON:
		{
		new neoncolor;
		switch(listitem)
			{
				case 0: 
					neoncolor=18648; //blue
				case 1: 
					neoncolor=18647; //red
				case 2: 
					neoncolor=18649; //green
				case 3: 
					neoncolor=18650; //yellow
				case 4: 
					neoncolor=18651; //pink
				case 5: 
					neoncolor=18652; //white
			}
		if(neon{playerid}!=0 && neon2{playerid}!=0)
			{
			DestroyObject(neon{playerid});
			DestroyObject(neon2{playerid});
			VehNeon{itmp[0]}=0;
			}
		VehNeon{itmp[0]}=playerid;	
		neon{playerid}=CreateObject(neoncolor,0,0,0,0,0,0,283);
		neon2{playerid}=CreateObject(neoncolor,0,0,0,0,0,0,283);
		switch(itmp[1])
			{
				case 558: //URANUS HAHA
				{
					AttachObjectToVehicle(neon{playerid},itmp[0],0.8,0,-0.48,0,0,0);
					AttachObjectToVehicle(neon2{playerid},itmp[0],-0.8,0,-0.48,0,0,0);
				}
				case 560: //SULTAN
				{
					AttachObjectToVehicle(neon{playerid},itmp[0],0.82,0,-0.50,0,0,0);
					AttachObjectToVehicle(neon2{playerid},itmp[0],-0.82,0,-0.50,0,0,0);
					print("do a sultan way");
				}
				case 559: //JESTER
				{
					AttachObjectToVehicle(neon{playerid},itmp[0],0.82,0,-0.50,0,0,0);
					AttachObjectToVehicle(neon2{playerid},itmp[0],-0.82,0,-0.50,0,0,0);
				}
				case 561: //STRATUM
				{
					AttachObjectToVehicle(neon{playerid},itmp[0],0.8,0,-0.65,0,0,0);
					AttachObjectToVehicle(neon2{playerid},itmp[0],-0.8,0,-0.65,0,0,0);
				}
				case 562: //ELEGY
				{
					AttachObjectToVehicle(neon{playerid},itmp[0],0.82,0,-0.50,0,0,0);
					AttachObjectToVehicle(neon2{playerid},itmp[0],-0.82,0,-0.50,0,0,0);
				}
				case 565:
				{
					AttachObjectToVehicle(neon{playerid},itmp[0],0.82,0,-0.50,0,0,0);
					AttachObjectToVehicle(neon2{playerid},itmp[0],-0.82,0,-0.50,0,0,0);
				}
				default:
				{
					AttachObjectToVehicle(neon{playerid},itmp[0],0.8,0,-0.60,0,0,0);
					AttachObjectToVehicle(neon2{playerid},itmp[0],-0.8,0,-0.60,0,0,0);
				}
			}
		return 1;
		}
	}
return 0;
}
Factions_OnPlayerDisconnect(playerid)
{
if(neon{playerid}!=0 && neon2{playerid}!=0)
	{
	DestroyObject(neon{playerid});
	DestroyObject(neon2{playerid});
	}
neon{playerid}=0;
neon2{playerid}=0;
return 1;
}
Factions_OnVehicleSpawn(vehicleid) // ��� - ����� ��������� ����� ��� ������� ����� ���� ������
{
#pragma unused vehicleid
//if(neon{VehNeon{vehicleid}}!=0 && neon2{VehNeon{vehicleid}}!=0)
	//{
	//DestroyObject(neon{VehNeon{vehicleid}});
	//DestroyObject(neon2{VehNeon{vehicleid}});
	//}
//neon{VehNeon{vehicleid}}=0;
//neon2{VehNeon{vehicleid}}=0;

return 1;
}
Factions_OnPlayerWinsRace(playerid,place)
{
new wrk=GetPVarInt(playerid,"Work");
if(wrk==RACER||wrk==RACERBOSS)
	{
	option1[playerid]=random(sizeof(awards));
	option2[playerid]=random(sizeof(awards));
	option3[playerid]=random(sizeof(awards));
	SetPlayerWantedLevel(playerid,random(3));
	format(stmp,sizeof(stmp),"%s\n",awards[option1[playerid]]);
	if(place<=2)
		{
			strcat(stmp,awards[option2[playerid]]);
			strcat(stmp,"\n");
		}
	if(place<2) strcat(stmp,awards[option3[playerid]]);
	ShowPlayerDialog(playerid,DIALOG_WIN,DIALOG_STYLE_LIST,"������ � �����",stmp,"�������"," ������");
	}
return 1;
}
Factions_OPCTD(playerid,Text:clickedid)
{
#pragma unused playerid,clickedid
// well... lets go the other way
return 1;
}
FBI_HeliUpdate(playerid)
{
if(!IsPlayerConnected(playerid))
{
chasing=-1;
return 0;
}
GetPlayerPos(playerid,ftmp[1],ftmp[2],ftmp[3]);
format(stmp,sizeof(stmp),"|��������|:%s ������� � ������ %s, ������������ �� ",oGetPlayerName(playerid),GetXYZZoneName(ftmp[1],ftmp[2],ftmp[3]));
if(IsPlayerInAnyVehicle(playerid)) GetVehicleZAngle(GetPlayerVehicleID(playerid),ftmp[0]);
//printf("Rotation is %f",ftmp[0]);

else GetPlayerFacingAngle(playerid,ftmp[0]);
if((ftmp[0]>315.0 && ftmp[0]<361.0)||(ftmp[0]>0.0 && ftmp[0]<=45.0)) strcat(stmp,"�����.");
if(ftmp[0]>45.0 && ftmp[0]<=135.0) strcat(stmp,"������.");
if(ftmp[0]>135.0 && ftmp[0]<=225.0) strcat(stmp,"��.");
if(ftmp[0]>225.0 && ftmp[0]<=315.0) strcat(stmp,"�����.");
//for(new i = 0;i <= GetPlayersCount();i++)
foreach(new i : Player)
		{
		//if(!IsPlayerConnected(i)) continue;
		if(IsPlayerNPC(i)) continue;
		itmp[0]=GetPVarInt(i,"Work");
		if(itmp[0] == COP || itmp[0] ==COPBOSS || itmp[0] == FBI || itmp[0] ==FBIBOSS)
		{
		SendClientMessage(i,COLOR_BLUE,stmp);
		PlayCrimeReportForPlayer(i, playerid, 3);
		}
		}
return 1;
}
IsPlayerAtSupply(playerid)
{
if(IsPlayerInRangeOfPoint(playerid,2.5,251.2976,68.0977,1003.6406)) return 1;
if(IsPlayerInRangeOfPoint(playerid,2.5,233.0436,166.3443,1003.0234)) return 1;
if(IsPlayerInRangeOfPoint(playerid,2.5,-1616.1479,679.9449,7.1901)) return 1;
return 0;
}
Factions_SupplyPlayer(playerid)
{
if(HasSupplied[playerid]==1) return SendClientMessage(playerid,COLOR_BLUE,"�� ��� �������� ���� �����");
itmp[0]=GetPVarInt(playerid,"Work");
if(itmp[0]==COP)
	{
		oGivePlayerWeapon(playerid,3,1); //�������
		oGivePlayerWeapon(playerid,22,150); //��������
		SendClientMessage(playerid,COLOR_BLUE,"��� ��� ����� ����������. �������� �������!");
	
	}
if(itmp[0]==COPBOSS)
	{
		oGivePlayerWeapon(playerid,3,1); //�������
		oGivePlayerWeapon(playerid,22,150); //��������
		oGivePlayerWeapon(playerid,25,25); //��������
		SendClientMessage(playerid,COLOR_BLUE,"��� ��� ����� ����������. �������� ����, ���!");
	
	}
if(itmp[0]==FBI)
	{
		oGivePlayerWeapon(playerid,23,100); //�������� � ����������
		oGivePlayerWeapon(playerid,29,150); //���
		SetPlayerArmour(playerid,100); //����������
		SendClientMessage(playerid,COLOR_BLUE,"��� ��� ����� ����������. ������� �� ����, ���!");
	
	}
if(itmp[0]==FBIBOSS)
	{
		oGivePlayerWeapon(playerid,24,120); //����
		oGivePlayerWeapon(playerid,29,150); //���
		SetPlayerArmour(playerid,100); //����������
		oGivePlayerWeapon(playerid,27,50); //������ ��������
		SendClientMessage(playerid,COLOR_BLUE,"��� ��� ����� ����������. ������� �����, ���!");
	
	}
else return SendClientMessage(playerid,COLOR_BLUE,"��� ���� ���� �������,���?");
HasSupplied[playerid]=1;
return 1;
}
Award_Player(playerid,option)
{
switch(option)
	{
		case 0: //hotring racer
			{
			AddStaticVehicle(502,0.0,0.0,0.0,0.0,random(128),random(128));
			SendClientMessage(playerid,COLOR_LIGHTGREEN,"��� Hotring ��������� �� �������� ����� ��������");
			return 1;
			}
		case 1: //bloodring banger
			{
			AddStaticVehicle(504,0.0,0.0,0.0,0.0,random(128),random(128));
			SendClientMessage(playerid,COLOR_LIGHTGREEN,"��� Bloodring ��������� �� �������� ����� ��������");
			return 1;			
			}
		case 2: //XP bonus
			{
			itmp[0]=(1+GetPVarInt(playerid,"Level"))*100;
			GivePlayerXP(playerid,itmp[0]);
			format(stmp,sizeof(stmp),"�� �������� %d ��������� �����",itmp[0]);
			SendClientMessage(playerid,COLOR_LIGHTGREEN,stmp);
			return 1;
			}
		case 3: //large XP bonus
			{
			itmp[0]=(1+GetPVarInt(playerid,"Level"))*1000;
			GivePlayerXP(playerid,itmp[0]);
			format(stmp,sizeof(stmp),"�� �������� %d ��������� �����",itmp[0]);
			SendClientMessage(playerid,COLOR_LIGHTGREEN,stmp);
			return 1;			
			}
		case 4://$$$
			{
			itmp[0]=(1+GetPVarInt(playerid,"Level"))*150;
			oGivePlayerMoney(playerid,itmp[0]);
			format(stmp,sizeof(stmp),"�� �������� %d �������� �����",itmp[0]);
			SendClientMessage(playerid,COLOR_LIGHTGREEN,stmp);
			return 1;
			}
		case 5://large$$$
			{
			itmp[0]=(1+GetPVarInt(playerid,"Level"))*1500;
			oGivePlayerMoney(playerid,itmp[0]);
			format(stmp,sizeof(stmp),"�� �������� %d �������� �����",itmp[0]);
			SendClientMessage(playerid,COLOR_LIGHTGREEN,stmp);
			return 1;
			}
		case 6: //no cops triforce
			{
			SetPlayerWantedLevel(playerid,0);
			SendClientMessage(playerid,COLOR_LIGHTGREEN,"���� ������� �� ���!");
			return 1;
			}
		case 7: //car repair
			{
			RepairVehicle(GetPlayerVehicleID(playerid));
			return 1;
			}
	}

return 1;
}