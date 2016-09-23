/*
*	Created:			15.06.14
*	Author:				LukeS
*	Description:		Точки кача
*	Info:				-
*/
#if defined _GH_included
	#endinput
#endif

#define _GH_included
#pragma library groundhold
#define GH_TICK				5
#define GH_DIR				"groundhold/"
#define GH_MASK_FILE			"point_%d.txt"
#define GH_PICKUP				1274
#define GH_LABEL_COLOR		0x331166FF
#define GH_ICON				30
#define MAX_GHS				32
#define GH_XP			    100
#define GH_MONEY			150
static GHClass[MAX_GHS],active_GH;
static GHSCP[MAX_GHS],GH_OWNER[MAX_GHS],GH_POWER[MAX_GHS],PlayersOnPoint[MAX_GHS];
stock LoadGHFromFile(filename[],GHid)
{
	new File:df = fopen(filename,io_read),
		k,
		c,
		name[128];
	if(df)
	{
		while(fread(df,stmp))
		{
			if(!CheckFileLine(stmp)) continue;
			// parse
			if((k = strfind(stmp,"=")) == -1) continue;
			
			if(!strcmp(stmp,"Coords",true,k))
			{
				c = 0;
				ftmp[0] = floatstr(strtok(stmp[k + 1],c,','));
				ftmp[1] = floatstr(strtok(stmp[k + 1],c,','));
				ftmp[2] = floatstr(strtok(stmp[k + 1],c,','));
				continue;
			}
			if(!strcmp(stmp,"Name",true,k))
			{
				strcat(name,stmp[k + 1]);
				continue;
			}
			if(!strcmp(stmp,"Class",true,k))
			{
				GHClass[GHid]=strval(stmp[k + 1]);
				continue;
			}
		}
		fclose(df);
		GH_OWNER[GHid]=-1;
		GH_POWER[GHid]=0;
		PlayersOnPoint[GHid]=0;
		active_GH++;
		// create pickup
		GHSCP[GHid]= CreateStreamedCheckpoint(ftmp[0],ftmp[1],ftmp[2],5);
		
		// create 3D text

		format(stmp,sizeof(stmp),"Точка Кача:%s\nКласс:%d\nПрибыль\n$:%d\nXP:%d",name,GHClass[GHid],GH_MONEY*GHClass[GHid]*5,GH_XP*GHClass[GHid]*5);
		Create3DTextLabel(stmp,0x33CCFFFF,ftmp[0],ftmp[1],(ftmp[2] + 1.0),50.0,0,1);
		// create icon
#if defined _streamer_icons_included
		CreateStreamedIcon(GH_ICON,0x0,ftmp[0],ftmp[1],ftmp[2]);
#endif
	}
}




//================================
// Obligatory
//================================
GH_OnGameModeInit()
{
	
	new i = 0;
	format(stmp,sizeof(stmp),MODE_DIR GH_DIR GH_MASK_FILE,i);
	while(fexist(stmp))
	{
		LoadGHFromFile(stmp,i);
		// up
		i++;
		// again
		format(stmp,sizeof(stmp),MODE_DIR GH_DIR GH_MASK_FILE,i);
	}	
	// data
#if defined PRINT_STATS_DATA
	printf("Groundholds: %d",i);
#endif
	print("Groundholds loaded.");
}

GH_OPESC(playerid,checkpointid)
{
#pragma unused playerid 
#pragma unused checkpointid
// for(itmp[0]=0;itmp[0]<active_GH;itmp[0]++)
// {
// if(checkpointid==GHSCP[itmp[0]])
	// {
	// if(GH_OWNER[itmp[0]]!=-1&&GH_OWNER[itmp[0]]!=playerid)
		// {
		// //SendClientMessage(GH_OWNER[itmp[0]],COLOR_LIGHTRED,"Кто то еще присутствует на точке, избавьтесь от конкурентов!");
		// SendClientMessage(playerid,COLOR_LIGHTRED,"Кто то еще присутствует на точке, избавьтесь от конкурентов!");
		
		// }
	// If(GH_OWNER[
		// {
		// GH_OWNER[itmp[0]]=playerid;
		// GH_POWER[itmp[0]]=0;
		// SendClientMessage(playerid,COLOR_LIGHTRED,"Вы захватили точку");
		// }
	// }


//}
return 1;
}
GH_Tick()///5 SEC
{
//return 1;

for(new i=0;i<active_GH;i++)
	{
	UpdateGround(i);
	//Pay(itmp[0]);
	}
return 1;
}
UpdateGround(gid)
{
PlayersOnPoint[gid]=0;
if(!IsPlayerConnected(GH_OWNER[gid])) GH_OWNER[gid]=-1;
GetStreamedCheckpointPos(GHSCP[gid],ftmp[0],ftmp[1],ftmp[2]);
//for(new i=0;i<=GetPlayersCount();i++)
foreach(new i : Player)
	{
	if(IsPlayerNPC(i)) continue;
	if(!IsPlayerConnected(i)) continue;
	if(IsPlayerInRangeOfPoint(i,5.0,ftmp[0],ftmp[1],ftmp[2])&&GH_OWNER[gid]==-1)
		{
		GH_OWNER[gid]=i;
		SendClientMessage(GH_OWNER[gid],COLOR_LIGHTRED,"Вы Захватили точку!");
		
		GH_POWER[gid]=0;
		}
	if(!IsPlayerInRangeOfPoint(i,5.0,ftmp[0],ftmp[1],ftmp[2])&& GH_OWNER[gid]==i)
		{
		SendClientMessage(GH_OWNER[gid],COLOR_LIGHTRED,"Вы потеряли точку!");
		print("lost");
		GH_OWNER[gid]=-1;
		
		}
	if(GH_POWER[gid]<5) GH_POWER[gid]++;
	if(IsPlayerInRangeOfPoint(i,5.0,ftmp[0],ftmp[1],ftmp[2])&& GH_OWNER[gid]!=i)SendClientMessage(i,COLOR_LIGHTRED,"На точке кто то еще, зачистите точку!");
	if(IsPlayerInRangeOfPoint(i,5.0,ftmp[0],ftmp[1],ftmp[2])) PlayersOnPoint[gid]++;
	}
if(GH_OWNER[gid]!=-1)	
		{
		if(PlayersOnPoint[gid]>1) return SendClientMessage(GH_OWNER[gid],COLOR_LIGHTRED,"Точка оспаривается, ничего не выплачено!");
		oGivePlayerMoney(GH_OWNER[gid],GH_MONEY*GHClass[gid]*GH_POWER[gid]);
		GivePlayerXP(GH_OWNER[gid],GH_XP*GHClass[gid]*GH_POWER[gid]);
		format(stmp,sizeof(stmp),"За удержание точки вы получили %d $ и %d XP",GH_MONEY*GHClass[gid]*GH_POWER[gid],GH_XP*GHClass[gid]*GH_POWER[gid]);
		SendClientMessage(GH_OWNER[gid],COLOR_LIGHTRED,stmp);
		format(stmp,sizeof(stmp),"payed %d $ and %d xp to id %d",GH_MONEY*GHClass[gid]*GH_POWER[gid],GH_XP*GHClass[gid]*GH_POWER[gid],GH_OWNER[gid]);
		print(stmp);
		}
return 1;
}
// Pay(gid)
// {
// if(GH_OWNER[gid]==-1) return 1;
// format(stmp,sizeof(stmp),"payed %d $ and %d xp to id %d",GH_MONEY*GHClass[gid]*GH_POWER[gid],GH_XP*GHClass[gid]*GH_POWER[gid],GH_OWNER[gid]);
// print(stmp);
// if(PlayersOnPoint[gid]>1) return SendClientMessage(GH_OWNER[gid],COLOR_LIGHTRED,"Точка оспаривается, ничего не выплачено!");
// oGivePlayerMoney(GH_OWNER[gid],GH_MONEY*GHClass[gid]*GH_POWER[gid]);
// GivePlayerXP(GH_OWNER[gid],GH_XP*GHClass[gid]*GH_POWER[gid]);
// format(stmp,sizeof(stmp),"За удержание точки вы получили %d $ и %d XP",GH_MONEY*GHClass[gid]*GH_POWER[gid],GH_XP*GHClass[gid]*GH_POWER[gid]);
// SendClientMessage(GH_OWNER[gid],COLOR_LIGHTRED,stmp);
// return 1;
// }