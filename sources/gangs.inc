/*
*	Created:			15.06.10
*	Author:				009(LukeS Modified)
*	Description:		Банды. Создание, удаление, принятие в банду, изгнание из банды, зоны банд, захват зон, войны за зону/деньги/распад.
*/

/* TODO

 
+reinforce Timer and notifications


при оффлайне берется тикет реинфорса, при онлайне считается сколько защитников онлайн*/

#if defined _gangs_included
	#endinput
#endif

#define _gangs_included
#pragma library gangs

// --------------------------------------------------
// defines
// --------------------------------------------------
// gangs
#define GANGS_DIR				"gangs/"
#define GANGS_MASK_FILE			"%s.txt"
#define GANGS_NEUTRAL_COLOR		0x333333AA
#define MAX_REINFORCE_TIMERS    10
#define MAX_GANG_NAME			24
#define MAX_GANG_WELCOME		128
#define MAX_GANGS				MAX_PLAYERS
#define MAX_GANG_MEMBERS		16
#define MAX_GANG_COLOR_NAME		15
#define INVALID_GANG_ID			-1
#define GANG_COST				100000
#define GANG_SCORE_RATE			10
#define GANG_CAPITAL_ICON		19
// zones
#define GANG_ZONES_REB			8
#define	XP_PORTION				200
#define MONEY_PORTION			500
#define GANG_ZONES_PER_CITY		GANG_ZONES_REB * GANG_ZONES_REB
#define GANG_ZONES_DIR			"gang zones/"
#define GANG_ZONES_MASK_FILE	"zone_%d.txt"
#define INVALID_GANG_ZONE_ID	-1
#define INCOME_TIME				20*60 //20 минут
#define KILLS_FOR_CAPTURE		5
#define MAX_UNLOGGED_ZONE_TIME	60 * 60 * 24 * 15 // (((((((60 - минута) * 60) - час) * 24) - день) * 15) - 15 дней)
// wars
#define GANG_WAR_SCORE_PRICE	100
#define ZONE_GANG_WAR_COST		10_000
#define MAX_GANG_WAR_SPAWNS		1
#define GANG_WAR_WEAPONS_STRING	"Tec9\nM4"
// check functions
#if !defined CheckFileLine
		#error CheckFileLine not found in script.
#endif
#if !defined strtok
	#error strtok not found in script.
#endif
#if !defined oGetPlayerMoney
	#error oGetPlayerMoney not found in script.
#endif
#if !defined ChangeStatByNameInt
	#error ChangeStatByNameInt not found in script.
#endif
#if !defined oGivePlayerMoney
	#error oGivePlayerMoney not found in script.
#endif
#if !defined oGetPlayerName
	#error oGivePlayerMoney not found in script.
#endif


// --------------------------------------------------
// enums
// --------------------------------------------------
enum
{
	GANGS_HELP_CMD,
	GANGS_CREATE_CMD,
	GANGS_DELETE_CMD,
	GANGS_COLORS_CMD,
	GANGS_COLOR_CMD,
	GANGS_WELCOME_CMD,
	GANGS_INVITE_CMD,
	GANGS_QUIT_CMD,
	GANGS_KICK_CMD,
	GANGS_MEMBERS_CMD,
	GANGS_STATS_CMD,
	GANGS_ZONE_HELP_CMD,
	GANGS_ZONE_INFO_CMD,
	GANGS_ZONE_CLAIM_CMD,
	GANGS_WAR_HELP_CMD,
	GANGS_WAR_ATTACK_CMD,
	GANGS_WAR_JOIN_CMD,
	GANGS_WAR_LEAVE_CMD,
	GANGS_WAR_START_CMD
};
enum
{
	GANGS_TEAM_TEXT
};
enum GangColorsInfo
{
	cName[MAX_GANG_COLOR_NAME],
	cColor
};
enum
{
	GANG_WAR_STATE_OFF,
	GANG_WAR_STATE_LINEUP,
	GANG_WAR_STATE_ON
};
enum
{
	GANG_WAR_SIDE_ATTACKER,
	GANG_WAR_SIDE_DEFENDER
};
enum
{
	GANG_WAR_PRICE_ZONE,
	GANG_WAR_PRICE_MONEY,
	GANG_WAR_PRICE_SCORE,
	GANG_WAR_PRICE_DELETE
};
enum
{
	GANG_END_LIDER_QUIT,
	GANG_END_LIDER_LEAVE,
	GANG_WAR_END_KILLED
};
 enum war_eff
{
a_id,
d_id,
z_id,
ascore,
dscore
};

// --------------------------------------------------
// statics
// --------------------------------------------------
static
	// gangs
	IsGangActive[MAX_GANGS char],
	GangName[MAX_GANGS][MAX_GANG_NAME],
	GangColor[MAX_GANGS],
	GangKillsAll[MAX_GANGS],
	GangKillsGangs[MAX_GANGS],
	GangScore[MAX_GANGS],
	GangWelcome[MAX_GANGS][MAX_GANG_WELCOME],
	GangReinforces[MAX_GANGS],
	GangCap[MAX_GANGS],
	GZones[MAX_GANGS],
	XPIncome[MAX_GANGS],
	MoneyIncome[MAX_GANGS],
	CapIcon[MAX_GANGS],
	//GZCount,
	// members
	IsGangMemberActive[MAX_GANGS][MAX_GANG_MEMBERS char],
	GangMemberName[MAX_GANGS][MAX_GANG_MEMBERS][MAX_PLAYER_NAME],
	GangMemberPlayerid[MAX_GANGS][MAX_GANG_MEMBERS];
// colors
static GangColors[][GangColorsInfo] = 
{
	{"Red",				0xAA3333AA},
	{"LightRed",			0xFF6347AA},
	{"Orange",			0xFF9900AA},
	{"Yellow",			0xFFFF00AA},
	{"Green",			0x33AA33AA},
	{"LightGreen",		0x9ACD32AA},
	{"Blue",				0x33AAFFAA},
	{"LightBlue",			0x33CCFFAA},
	{"White",			0xFFFFFFAA},
	{"Purple",			0xC2A2DAAA},
	{"Black",			0x000000AA},
	{"Crimson",			0xDC143CAA},
	{"Limegreen",		0x32CD32AA},
	{"Aqua",			0x00FFFFAA},
	{"Flblue",			0x6495EDAA},
	{"Magenta",			0xFF00FFAA},
	{"Gold",				0xB8860BAA},
	{"Pink",				0xFFC0CBAA},
	{"MidNighBlue",		0x191970AA},
	{"Indigo",			0x4B00B0AA},
	{"Ivory",			0xFFFF82AA},
	{"SeaGreen",			0x20B2AAAA},
	{"Maroon",			0x800000AA},
	{"Olive",			0x808000AA},
	{"Orangered",		0xFF4500AA},
	{"SeaGreen2",		0x2E8B57AA},
	{"SpringGreen",		0x00FF7FAA},
	{"Tomato",			0xFF6347AA},
	{"YellowGreen",		0x9ACD32AA},
	{"MediumAqua",		0x83BFBFAA},
	{"MediumMagenta",	0x8B008BAA}
};
// zones
static
	Float:GangZonesCoords[GANG_ZONES_PER_CITY * 3][4],
	GangZonesServerId[GANG_ZONES_PER_CITY * 3],
	GangZonesOwner[GANG_ZONES_PER_CITY * 3][MAX_GANG_NAME],
	GangZonesLastOwnerLogin[GANG_ZONES_PER_CITY * 3],
	GangZonesColor[GANG_ZONES_PER_CITY * 3],
	GangZonesAttacker[GANG_ZONES_PER_CITY * 3][MAX_GANG_NAME],
	GangZonesCapture[GANG_ZONES_PER_CITY * 3 char],
	RTimer[MAX_REINFORCE_TIMERS],
	RTimerDat[MAX_REINFORCE_TIMERS][war_eff],
	Text3D:TRDLABEL[MAX_3DTEXT_GLOBAL],
	zone_tick =INCOME_TIME,
	TactDat[GANG_ZONES_PER_CITY * 3][3];
	// citys coords (min x,min y,max x,max y)
static Float:City[3][4] = 
{
	{44.60,		-2892.90,		2997.00,		-768.00},	// Los Santos
	{-2997.40,	-1115.50,		-1213.90,		1659.60},		// San Fierro
	{869.40,		596.30,		2997.00,		2993.80}	// Las Venturas
};
// gang wars
static
	GangWarState[MAX_GANGS char],
	GangWarSide[MAX_GANGS char],
	GangWarWith[MAX_GANGS],
	GangWarPriceType[MAX_GANGS char],
	GangWarPrice[MAX_GANGS],
	GangWarMembersCount[MAX_GANGS char],
	IsGangMemberInWar[MAX_GANGS][MAX_GANG_MEMBERS char],
	GangWarQuest;
static Float:GangWarSpawn[2][MAX_GANG_WAR_SPAWNS][3] =
{
	{
		{0.0,0.0,0.0}
	},
	{
		{0.0,0.0,0.0}
	}
};

// --------------------------------------------------
// stocks
// --------------------------------------------------
stock LoadGang(name[])
{
	// find gang in already loaded
	new gangid = INVALID_GANG_ID;
	for(new i = 0;i < MAX_GANGS;i++)
	{
		if(!IsGangActive{i})
		{
			if(gangid == INVALID_GANG_ID) gangid = i;
			continue;
		}
		if(strcmp(name,GangName[i])) continue;
		
		return i;
	}
	
	GangCap[gangid]=-1;
	// build file path
	format(stmp,sizeof(stmp),MODE_DIR GANGS_DIR GANGS_MASK_FILE,name);
	// try load gang
	new File:df = fopen(stmp,io_read),
		k;
	if(df)
	{
		new mcount = 1;
		while(fread(df,stmp))
		{
			if(!CheckFileLine(stmp)) continue;
			// parse
			if((k = strfind(stmp,"=")) == -1) continue;
			
			if(!strcmp(stmp,"Name",true,k))
			{
				format(GangName[gangid],MAX_GANG_NAME,stmp[k + 1]);
				continue;
			}
			if(!strcmp(stmp,"Color",true,k))
			{
				GangColor[gangid] = strval(stmp[k + 1]);
				continue;
			}
			if(!strcmp(stmp,"KillsAll",true,k))
			{
				GangKillsAll[gangid] = strval(stmp[k + 1]);
				continue;
			}
			if(!strcmp(stmp,"KillsGangs",true,k))
			{
				GangKillsGangs[gangid] = strval(stmp[k + 1]);
				continue;
			}
			if(!strcmp(stmp,"Score",true,k))
			{
				GangScore[gangid] = strval(stmp[k + 1]);
				continue;
			}
			if(!strcmp(stmp,"Welcome",true,k))
			{
				format(GangWelcome[gangid],MAX_GANG_WELCOME,stmp[k + 1]);
				continue;
			}
			if(!strcmp(stmp,"Reinforces",true,k))
			{
				GangReinforces[gangid] = strval(stmp[k + 1]);
				continue;
			}
			if(!strcmp(stmp,"XPIncome",true,k))
			{
				XPIncome[gangid] = strval(stmp[k + 1]);
				continue;
			}
			if(!strcmp(stmp,"MoneyIncome",true,k))
			{
				MoneyIncome[gangid] = strval(stmp[k + 1]);
				continue;
			}
			if(!strcmp(stmp,"GZones",true,k))
			{
				GZones[gangid] = strval(stmp[k + 1]);
				continue;
			}
			if(!strcmp(stmp,"Capital",true,k))
			{
				GangCap[gangid] = strval(stmp[k + 1]);
				if(GangCap[gangid]!=-1)
				{
				ftmp[0]=GangZonesCoords[GangCap[gangid]][2]-((GangZonesCoords[GangCap[gangid]][2]-GangZonesCoords[GangCap[gangid]][0])/2);
				ftmp[1]=GangZonesCoords[GangCap[gangid]][3]-((GangZonesCoords[GangCap[gangid]][3]-GangZonesCoords[GangCap[gangid]][1])/2);
				CapIcon[gangid]=CreateStreamedIcon(GANG_CAPITAL_ICON,0x0,ftmp[0],ftmp[1],0.0);
				}
				continue;
			}
			// members
			if(!strcmp(stmp,"Leader",true,k))
			{
				format(GangMemberName[gangid][0],MAX_PLAYER_NAME,stmp[k + 1]);
				IsGangMemberActive[gangid]{0} = 1;
				continue;
			}
			if(!strcmp(stmp,"Member",true,k))
			{
				format(GangMemberName[gangid][mcount],MAX_PLAYER_NAME,stmp[k + 1]);
				IsGangMemberActive[gangid]{mcount} = 1;
				
				mcount++;
				continue;
			}
		}
		fclose(df);
		// stat
		IsGangActive{gangid} = 1;
		for(new i = mcount;i < MAX_GANG_MEMBERS;i++) IsGangMemberActive[gangid]{i} = 0;
		//build
		// zone info
		for(new i = 0;i < sizeof(GangZonesOwner);i++)
		{
			if(strcmp(GangZonesOwner[i],GangName[ itmp[0] ])) continue;
			// owner
			new dt[6];
			getdate(dt[0],dt[1],dt[2]);
			gettime(dt[3],dt[4],dt[5]);
			GangZonesLastOwnerLogin[i] = mktime(dt[3],dt[4],dt[5],dt[2],dt[1],dt[0]);
			// save zone data
			SaveGangZone(i);
		}
		return gangid;
	}
	// not find file
	return INVALID_GANG_ID;
}

stock SaveGang(gangid)
{
	// build file path
	format(stmp,sizeof(stmp),MODE_DIR GANGS_DIR GANGS_MASK_FILE,GangName[gangid]);
	// open
	new File:df = fopen(stmp,io_write);
	// write
	format(stmp,sizeof(stmp),"Name=%s\r\n",GangName[gangid]);
	fwrite(df,stmp);
	format(stmp,sizeof(stmp),"Color=%d\r\n",GangColor[gangid]);
	fwrite(df,stmp);
	format(stmp,sizeof(stmp),"KillsAll=%d\r\n",GangKillsAll[gangid]);
	fwrite(df,stmp);
	format(stmp,sizeof(stmp),"KillsGangs=%d\r\n",GangKillsGangs[gangid]);
	fwrite(df,stmp);
	format(stmp,sizeof(stmp),"Score=%d\r\n",GangScore[gangid]);
	fwrite(df,stmp);
	format(stmp,sizeof(stmp),"Welcome=%s\r\n",GangWelcome[gangid]);
	fwrite(df,stmp);
	format(stmp,sizeof(stmp),"Reinforces=%d\r\n",GangReinforces[gangid]);
	fwrite(df,stmp);
	format(stmp,sizeof(stmp),"XPIncome=%d\r\n",XPIncome[gangid]);
	fwrite(df,stmp);
	format(stmp,sizeof(stmp),"MoneyIncome=%d\r\n",MoneyIncome[gangid]);
	fwrite(df,stmp);
	format(stmp,sizeof(stmp),"Capital=%d\r\n",GangCap[gangid]);
	fwrite(df,stmp);
	format(stmp,sizeof(stmp),"GZones=%d\r\n",GZones[gangid]);
	fwrite(df,stmp);
	format(stmp,sizeof(stmp),"Leader=%s\r\n",GangMemberName[gangid][0]);
	fwrite(df,stmp);
	for(new i = 1;i < MAX_GANG_MEMBERS;i++)
	{
		if(!IsGangMemberActive[gangid]{i}) continue;
		format(stmp,sizeof(stmp),"Member=%s\r\n",GangMemberName[gangid][i]);
		fwrite(df,stmp);
	}
	// close
	fclose(df);
}

stock GangCharCheck(string[])
{
	for(new i = 0;i < strlen(string);i++)
	{
		switch(string[i])
		{
			case 'a'..'z': continue;
			case 'A'..'Z': continue;
			case '0'..'9': continue;
			case '_','[',']': continue;
		}
		return 0;
	}
	return 1;
}

stock GangDelete(gangid)
{
	// null players data
	DestroyStreamedIcon(CapIcon[gangid]);
	for(new i = 0;i < MAX_GANG_MEMBERS;i++)
	{
		if(!IsGangMemberActive[gangid]{i}) continue;
		if(GangMemberPlayerid[gangid][i] == INVALID_PLAYER_ID) continue;
		
		SetPVarInt(GangMemberPlayerid[gangid][i],"Gang",INVALID_GANG_ID);
		SetPlayerColor(GangMemberPlayerid[gangid][i],PLAYERS_COLOR);
	}
	// null data
	IsGangActive{gangid} = 0;
	// delete file
	format(stmp,sizeof(stmp),MODE_DIR GANGS_DIR GANGS_MASK_FILE,GangName[gangid]);
	fremove(stmp);
	// null gang zones info
	for(new i = 0;i < sizeof(GangZonesOwner);i++)
	{
		if(strcmp(GangZonesOwner[i],GangName[gangid])) continue;
		// owner
		format(GangZonesOwner[i],MAX_GANG_NAME,"Unknown");
		GangZonesColor[i] = COLOR_GREY;
		if(!GangZonesCapture{i}) GangZoneShowForAll(GangZonesServerId[i],GangZonesColor[i]);
		else 
		{
			GangZoneShowForAll(GangZonesServerId[i],GangZonesColor[i]);
			GangZoneFlashForAll(GangZonesServerId[i],COLOR_RED);
		}
		// save zone data
		SaveGangZone(i);
	}
}

stock LoadGangZone(zoneid)
{
	// build file path
	format(stmp,sizeof(stmp),MODE_DIR GANG_ZONES_DIR GANG_ZONES_MASK_FILE,zoneid);
	// try load gang zone
	new File:df = fopen(stmp,io_read),
		k;
	if(df)
	{
		while(fread(df,stmp))
		{
			if(!CheckFileLine(stmp)) continue;
			// parse
			if((k = strfind(stmp,"=")) == -1) continue;
			
			if(!strcmp(stmp,"Owner",true,k))
			{
				format(GangZonesOwner[zoneid],MAX_GANG_NAME,stmp[k + 1]);
				continue;
			}
			if(!strcmp(stmp,"Color",true,k))
			{
				GangZonesColor[zoneid] = strval(stmp[k + 1]);
				continue;
			}
			if(!strcmp(stmp,"LastOwnerLogin",true,k))
			{
				GangZonesLastOwnerLogin[zoneid] = strval(stmp[k + 1]);
				continue;
			}
			if(!strcmp(stmp,"TactDat",true,k))
			{
				k=k+1;
				TactDat[zoneid][0] = strval(strtok(stmp,k,','));
				TactDat[zoneid][1] = strval(strtok(stmp,k,','));
				TactDat[zoneid][2] = strval(strtok(stmp,k,','));
				continue;
			}
		}
		fclose(df);
	}
	else
	{
		//return -1;
		format(GangZonesOwner[zoneid],MAX_GANG_NAME,"Unknown");
		GangZonesColor[zoneid] = PLAYERS_COLOR;
		GangZonesLastOwnerLogin[zoneid] = 0;
		TactDat[zoneid][0]=random(5);				//By CoolLans :D
		itmp[2]=5-TactDat[zoneid][0];
		TactDat[zoneid][1]=random(itmp[2]); 
		TactDat[zoneid][2]=(6-(TactDat[zoneid][0]+TactDat[zoneid][1]));
	}
}

stock SaveGangZone(zoneid)
{
	// build file path
	format(stmp,sizeof(stmp),MODE_DIR GANG_ZONES_DIR GANG_ZONES_MASK_FILE,zoneid);
	// open
	new File:df = fopen(stmp,io_write);
	// write
	format(stmp,sizeof(stmp),"Owner=%s\r\n",GangZonesOwner[zoneid]);
	fwrite(df,stmp);
	format(stmp,sizeof(stmp),"Color=%d\r\n",GangZonesColor[zoneid]);
	fwrite(df,stmp);
	format(stmp,sizeof(stmp),"LastOwnerLogin=%d\r\n",GangZonesLastOwnerLogin[zoneid]);
	fwrite(df,stmp);
	format(stmp,sizeof(stmp),"TactDat=%d,%d,%d\r\n",TactDat[zoneid][0],TactDat[zoneid][1],TactDat[zoneid][2]);
	fwrite(df,stmp);
	// close
	fclose(df);
}

stock GetPlayerGangZone(playerid)
{
	static Float:pos[3];
	GetPlayerPos(playerid,pos[0],pos[1],pos[2]);
	for(new i = 0;i < sizeof(GangZonesCoords);i++)
	{
		if(GangZonesCoords[i][0] > pos[0]) continue;
		if(GangZonesCoords[i][1] > pos[1]) continue;
		if(GangZonesCoords[i][2] < pos[0]) continue;
		if(GangZonesCoords[i][3] < pos[1]) continue;
		return i;
	}
	return INVALID_GANG_ZONE_ID;
}

stock GangWarComplete(winnergang,losegang,reason)
{
	switch(reason)
	{
		case GANG_END_LIDER_QUIT,GANG_END_LIDER_LEAVE:
		{
			// show msg
			if(reason == GANG_END_LIDER_QUIT) format(stmp,sizeof(stmp),"%s vs %s не состоится по причине выхода из игры лидера банды %s",GangName[winnergang],GangName[losegang],GangName[losegang]);
			else format(stmp,sizeof(stmp),"%s vs %s не состоится по причине выхода из банды лидера банды %s",GangName[winnergang],GangName[losegang],GangName[losegang]);
			if(IsGangActive{losegang})
			{
				// score price
				GangScore[losegang] -= GANG_WAR_SCORE_PRICE * GANG_SCORE_RATE;
				// members
				for(new i = 0;i < MAX_GANG_MEMBERS;i++)
				{
					if(!IsGangMemberActive[losegang]{i}) continue;
					if(GangMemberPlayerid[losegang][i] == INVALID_PLAYER_ID) continue;
					
					SendClientMessage(GangMemberPlayerid[losegang][i],GangColor[losegang],stmp);
					
					if(IsGangMemberInWar[losegang]{i} == 0) continue;
					IsGangMemberInWar[losegang]{i} = 0;
					GangWarMembersCount{losegang}--;
					// dead null stat
					SetPlayerQuest(GangMemberPlayerid[losegang][i],INVALID_QUEST_ID);
					if(GangWarState{losegang} == GANG_WAR_STATE_ON)
					{
						SetPlayerVirtualWorld(GangMemberPlayerid[losegang][i],0);
						SpawnPlayer(GangMemberPlayerid[losegang][i]);
					}
				}
			}
			if(IsGangActive{winnergang})
			{
				// score price
				GangScore[winnergang] += GANG_WAR_SCORE_PRICE * GANG_SCORE_RATE;
				// members
				for(new i = 0;i < MAX_GANG_MEMBERS;i++)
				{
					if(!IsGangMemberActive[winnergang]{i}) continue;
					if(GangMemberPlayerid[winnergang][i] == INVALID_PLAYER_ID) continue;
					
					SendClientMessage(GangMemberPlayerid[losegang][i],GangColor[losegang],stmp);
					
					if(IsGangMemberInWar[winnergang]{i} == 0) continue;
					IsGangMemberInWar[winnergang]{i} = 0;
					GangWarMembersCount{winnergang}--;
					// dead null stat
					SetPlayerQuest(GangMemberPlayerid[winnergang][i],INVALID_QUEST_ID);
					if(GangWarState{winnergang} == GANG_WAR_STATE_ON)
					{
						SetPlayerVirtualWorld(GangMemberPlayerid[winnergang][i],0);
						SpawnPlayer(GangMemberPlayerid[winnergang][i]);
					}
				}
			}
		}
		case GANG_WAR_END_KILLED:
		{
			if(GangWarSide[winnergang] == GANG_WAR_SIDE_ATTACKER) itmp[0] = winnergang;
			else itmp[0] = losegang;
			// give price
			switch(GangWarPriceType{ itmp[0] })
			{
				case GANG_WAR_PRICE_ZONE:
				{
					if(itmp[0] == winnergang) // win attacker
					{
						itmp[1] = GangWarPrice[ itmp[0] ];
						// zone captured
						format(GangZonesOwner[ itmp[1] ],MAX_GANG_NAME,GangName[ itmp[0] ]);
						GangZonesColor[ itmp[1] ] = GangColor[ itmp[0] ];
						GangZonesCapture{ itmp[1] } = 0;
						GangZoneShowForAll(GangZonesServerId[ itmp[1] ],GangZonesColor[ itmp[1] ]);
					}
					else // win defender
					{
						oGivePlayerMoney(GangMemberPlayerid[winnergang][0],ZONE_GANG_WAR_COST);
						oGivePlayerMoney(GangMemberPlayerid[losegang][0],-ZONE_GANG_WAR_COST);
					}
				}
				case GANG_WAR_PRICE_MONEY:
				{
					oGivePlayerMoney(GangMemberPlayerid[winnergang][0],GangWarPrice[ itmp[0] ]);
					oGivePlayerMoney(GangMemberPlayerid[losegang][0],-GangWarPrice[ itmp[0] ]);
				}
				case GANG_WAR_PRICE_SCORE:
				{
					GangScore[winnergang] += GangWarPrice[ itmp[0] ];
					GangScore[losegang] -= GangWarPrice[ itmp[0] ];
				}
				case GANG_WAR_PRICE_DELETE: 
				{
					// delete gang
					GangDelete(losegang);
				}
			}
			// show msg
			format(stmp,sizeof(stmp),"%s win %s",GangName[winnergang],GangName[losegang]);
			if(IsGangActive{losegang})
			{
				// score price
				GangScore[losegang] -= GANG_WAR_SCORE_PRICE * GANG_SCORE_RATE;
				// members
				for(new i = 0;i < MAX_GANG_MEMBERS;i++)
				{
					if(!IsGangMemberActive[losegang]{i}) continue;
					if(GangMemberPlayerid[losegang][i] == INVALID_PLAYER_ID) continue;
					
					SendClientMessage(GangMemberPlayerid[losegang][i],GangColor[losegang],stmp);
					
					if(IsGangMemberInWar[losegang]{i} == 0) continue;
					IsGangMemberInWar[losegang]{i} = 0;
					GangWarMembersCount{losegang}--;
					// dead null stat
					SetPlayerQuest(GangMemberPlayerid[losegang][i],INVALID_QUEST_ID);
					if(GangWarState{losegang} == GANG_WAR_STATE_ON)
					{
						SetPlayerVirtualWorld(GangMemberPlayerid[losegang][i],0);
						SpawnPlayer(GangMemberPlayerid[losegang][i]);
					}
				}
			}
			if(IsGangActive{winnergang})
			{
				// score price
				GangScore[winnergang] += GANG_WAR_SCORE_PRICE * GANG_SCORE_RATE;
				// members
				for(new i = 0;i < MAX_GANG_MEMBERS;i++)
				{
					if(!IsGangMemberActive[winnergang]{i}) continue;
					if(GangMemberPlayerid[winnergang][i] == INVALID_PLAYER_ID) continue;
					
					SendClientMessage(GangMemberPlayerid[losegang][i],GangColor[losegang],stmp);
					
					if(IsGangMemberInWar[winnergang]{i} == 0) continue;
					IsGangMemberInWar[winnergang]{i} = 0;
					GangWarMembersCount{winnergang}--;
					// dead null stat
					SetPlayerQuest(GangMemberPlayerid[winnergang][i],INVALID_QUEST_ID);
					if(GangWarState{winnergang} == GANG_WAR_STATE_ON)
					{
						SetPlayerVirtualWorld(GangMemberPlayerid[winnergang][i],0);
						SpawnPlayer(GangMemberPlayerid[winnergang][i]);
					}
				}
			}
		}
	}
}

stock GetGangWarPriceName(price)
{
	static name[30];
	name = "Unknown";
	switch(price)
	{
		case GANG_WAR_PRICE_ZONE: name = "Зона банды";
		case GANG_WAR_PRICE_MONEY: name = "Деньги";
		case GANG_WAR_PRICE_SCORE: name = "Очки банды";
		case GANG_WAR_PRICE_DELETE: name = "Удаление банды";
	}
	return name;
}

stock oGetPlayerGangName(playerid)
{
	static name[MAX_GANG_NAME];
	if((itmp[0] = GetPVarInt(playerid,"Gang")) == INVALID_GANG_ID) name = "Нет банды";
	else name = GangName[ itmp[0] ];
	return name;
}

// --------------------------------------------------
// forwards
// --------------------------------------------------


// --------------------------------------------------
// publics
// --------------------------------------------------


// --------------------------------------------------
// Obligatory functions
// --------------------------------------------------
Gangs_OnGameModeInit()
{
	//build invalid ids
	for(new i = 0; i<MAX_GANGS;i++)
		 {
			 for(new z = 0; z<MAX_GANG_MEMBERS;z++)
				 {
				 GangMemberPlayerid[i][z] = INVALID_PLAYER_ID;
				
				 }
		
		 }
	
	// build gang zones
	// create coords
	new gcount,
		Float:add[2],
		Float:current[2];
	for(new i = 0;i < 3;i++)
	{
		add[0] = (City[i][2] - City[i][0]) / GANG_ZONES_REB;
		add[1] = (City[i][3] - City[i][1]) / GANG_ZONES_REB;
		current[0] = City[i][0];
		current[1] = City[i][1];
		for(new a = 0;a < GANG_ZONES_REB;a++)
		{
			current[0] = City[i][0] + (add[0] * a);
			for(new b = 0;b < GANG_ZONES_REB;b++)
			{
				current[1] = City[i][1] + (add[1] * b);
				GangZonesCoords[gcount][0] = current[0];
				GangZonesCoords[gcount][1] = current[1];
				GangZonesCoords[gcount][2] = current[0] + add[0];
				GangZonesCoords[gcount][3] = current[1] + add[1];
				gcount++;
			}
		}
	}
	// create zones
	for(new i = 0;i < gcount;i++)
	{
		GangZonesServerId[i] = GangZoneCreate(GangZonesCoords[i][0],GangZonesCoords[i][1],GangZonesCoords[i][2],GangZonesCoords[i][3]);
		// read zone data
		LoadGangZone(i);
	}
	// gang war quest reg
	GangWarQuest = RegisterQuest();
	// data
	//GZCount=gcount;
#if defined PRINT_STATS_DATA
	printf("Max gangs: %d",MAX_GANGS);
	printf("Max gang members: %d",MAX_GANG_MEMBERS);
	printf("Gang cost: %d",GANG_COST);
	printf("Gang zones: %d",gcount);
	printf("Gang zones per city: %d",GANG_ZONES_PER_CITY);
#endif
	print("Gangs loaded.");
}

Gangs_OnGameModeExit()
{
	// save zones
	for(new i = 0;i < sizeof(GangZonesServerId);i++) SaveGangZone(i);	
}

Gangs_Text(playerid,textid,string[])
{
	switch(textid)
	{
		case GANGS_TEAM_TEXT:
		{
			// check player gang
			if((itmp[0] = GetPVarInt(playerid,"Gang")) == INVALID_GANG_ID) return 0;
			format(stmp,sizeof(stmp),"%s банде: %s",oGetPlayerName(playerid),string);
			// send for all gang
			for(new i = 0;i < MAX_GANG_MEMBERS;i++)
			{
				if(!IsGangMemberActive[ itmp[0] ]{i}) continue;
				if(GangMemberPlayerid[ itmp[0] ][i] == INVALID_PLAYER_ID) continue;
				
				SendClientMessage(GangMemberPlayerid[ itmp[0] ][i],GangColor[ itmp[0] ],stmp);
			}
			return 1;
		}
	}
	return 0;
}

Gangs_Command(playerid,cmdid,params[])
{
	new idx;
	switch(cmdid)
	{
		case GANGS_HELP_CMD:
		{
			stmp[0] = 0;
			strcat(stmp,"Вы можете создать свою банду либо вступить в уже существующую\nЧтобы создать свою банду вы должны иметь нужное количество денег (" #GANG_COST "$)\n");
			strcat(stmp,"Для вступления в банду требуется просто найти лидера и попросить принять.\nБанды могут захватывать зоны чтобы получать очки банды, которые нужны для лидерства\n");
			strcat(stmp,"Для захвата зоны нужно продержаться на ней указанное время, понеся минимальные потери и по возможности убив наибольшее количество врагов\n");
			strcat(stmp,"Команды:\nКнопка 'H' вне машины - меню банды\n/g create - создать банду\n/g delete - удалить банду\n/g colors - цвета банд\n/g color - изменить цвет банды\n/g invite - пригласить в банду\n/g kick - выгнать из банды\n/g quit - выйти из банды\n/g welcome - изменить приветствие\n/g members - члены банды\n/g stats - статистика банды");
			ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,MODE_NAME " -> Банды",stmp,"Ок","");
			return 1;
		}
		case GANGS_CREATE_CMD:
		{
			// check quest
			if(GetPlayerQuest(playerid) != INVALID_QUEST_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы выполняете задание");
			// check player gang
			if(GetPVarInt(playerid,"Gang") != INVALID_GANG_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы уже в банде");
			// check moneys
			if(oGetPlayerMoney(playerid) < GANG_COST) return SendClientMessage(playerid,SYSTEM_COLOR,"У вас не хватает денег.Требуется " #GANG_COST "$");
			// get color name
			strtoktmp = strtok(params,idx);
			// find color
			itmp[0] = -1;
			for(new i = 0;i < sizeof(GangColors);i++)
			{
				if(strcmp(GangColors[i][cName],strtoktmp,true)) continue;
				itmp[0] = GangColors[i][cColor];
				break;
			}
			if(itmp[0] == -1) return SendClientMessage(playerid,SYSTEM_COLOR,"Не верный цвет, введите /g colors для просмотра списка цветов");
			// get name
			strtoktmp = strtok(params,idx);
			// check name
			itmp[1] = strlen(strtoktmp);
			if((itmp[1] < 3) || (itmp[1] > MAX_GANG_NAME)) return SendClientMessage(playerid,SYSTEM_COLOR,"Не верная длинна названия банды, должна быть от 3 до " #MAX_GANG_NAME);
			if(!GangCharCheck(strtoktmp)) return SendClientMessage(playerid,SYSTEM_COLOR,"Используйте в названии банды только a-z , A-Z , 0-9 , _ , [ , ]");
			if(!strcmp("Unknown",strtoktmp)) return SendClientMessage(playerid,SYSTEM_COLOR,"Название банды Unknown недопустимо");
			// check exist
			format(stmp,sizeof(stmp),MODE_DIR GANGS_DIR GANGS_MASK_FILE,strtoktmp);
			if(fexist(stmp)) return SendClientMessage(playerid,SYSTEM_COLOR,"Банда с таким именем уже существует");
			// find free slot
			itmp[1] = INVALID_GANG_ID;
			for(new i = 0;i < MAX_GANGS;i++)
			{
				if(IsGangActive{i}) continue;
				itmp[1] = i;
				break;
			}
			if(itmp[1] == INVALID_GANG_ID) return print("[ERROR] cant find free slot for gang");
			// set data
			IsGangActive{ itmp[1] } = 1;
			format(GangName[ itmp[1] ],MAX_GANG_NAME,strtoktmp);
			GangColor[ itmp[1] ] = itmp[0];
			GangKillsAll[ itmp[1] ] = 0;
			GangKillsGangs[ itmp[1] ] = 0;
			GangScore[ itmp[1] ] = 0;
			GangCap[ itmp[1] ] = -1;
			GZones[itmp[1]] = 0;
			XPIncome[itmp[1]] = 0;
			MoneyIncome[itmp[1]]=0;
			GangReinforces[itmp[1]]=0;
			format(GangWelcome[ itmp[1] ],MAX_GANG_WELCOME,"Добро пожаловать в %s! (вы можете сменить приветствие командой /g welcome)",strtoktmp);
			format(GangMemberName[ itmp[1] ][0],MAX_PLAYER_NAME,oGetPlayerName(playerid));
			IsGangMemberActive[ itmp[1] ]{0} = 1;
			GangMemberPlayerid[ itmp[1] ][0] = playerid;
			for(new i = 1;i < MAX_GANG_MEMBERS;i++) IsGangMemberActive[ itmp[1] ]{i} = 0;
			// save file
			SaveGang(itmp[1]);
			// set data for player
			SetPVarInt(playerid,"Gang",itmp[1]);
			oGivePlayerMoney(playerid,-GANG_COST);
			SetPlayerColor(playerid,itmp[0]);
			SendClientMessage(playerid,itmp[0],GangWelcome[ itmp[1] ]);
			// result
			SendClientMessage(playerid,SYSTEM_COLOR,"Банда успешно создана");
			return 1;
		}
		case GANGS_DELETE_CMD:
		{
			// check quest
			if(GetPlayerQuest(playerid) != INVALID_QUEST_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы выполняете задание");
			// check player gang
			if((itmp[0] = GetPVarInt(playerid,"Gang")) == INVALID_GANG_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не в банде");
			// check lider
			if(GangMemberPlayerid[ itmp[0] ][0] != playerid) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не лидер банды");
			// check gang war
			if(GangWarState{ itmp[0] } != GANG_WAR_STATE_OFF) GangWarComplete(GangWarWith[ itmp[0] ],itmp[0],GANG_END_LIDER_LEAVE);
			// delete gang
			GangDelete(itmp[0]);
			// result
			SendClientMessage(playerid,SYSTEM_COLOR,"Банда удалена");
			return 1;
		}
		case GANGS_COLORS_CMD:
		{
			for(new i = 0;i < sizeof(GangColors);i++) SendClientMessage(playerid,GangColors[i][cColor],GangColors[i][cName]);
			return SendClientMessage(playerid,SYSTEM_COLOR,"Page Up/Down для просмотра всего списка");
		}
		case GANGS_COLOR_CMD:
		{
			// check quest
			if(GetPlayerQuest(playerid) != INVALID_QUEST_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы выполняете задание");
			// check player gang
			if((itmp[0] = GetPVarInt(playerid,"Gang")) == INVALID_GANG_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не в банде");
			// check lider
			if(GangMemberPlayerid[ itmp[0] ][0] != playerid) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не лидер банды");
			// get color name
			strtoktmp = strtok(params,idx);
			// find color
			itmp[1] = -1;
			for(new i = 0;i < sizeof(GangColors);i++)
			{
				if(strcmp(GangColors[i][cName],strtoktmp)) continue;
				itmp[1] = GangColors[i][cColor];
				break;
			}
			if(itmp[1] == -1) return SendClientMessage(playerid,SYSTEM_COLOR,"Не верный цвет, введите /g colors для просмотра списка цветов");
			// set data
			GangColor[ itmp[0] ] = itmp[1];
			for(new i = 0;i < MAX_GANG_MEMBERS;i++)
			{
				if(!IsGangMemberActive[ itmp[0] ]{i}) continue;
				if(GangMemberPlayerid[ itmp[0] ][i] == INVALID_PLAYER_ID) continue;
				
				SetPlayerColor(GangMemberPlayerid[ itmp[0] ][i],itmp[1]);
			}
			// change gang zones info
			for(new i = 0;i < sizeof(GangZonesOwner);i++)
			{
				if(strcmp(GangZonesOwner[i],GangName[ itmp[0] ])) continue;
				// owner
				GangZonesColor[i] = itmp[1];
				if(!GangZonesCapture{i}) GangZoneShowForAll(GangZonesServerId[i],GangZonesColor[i]);
				else 
				{
					GangZoneShowForAll(GangZonesServerId[i],GangZonesColor[i]);
					GangZoneFlashForAll(GangZonesServerId[i],COLOR_RED);
				}
				// save zone data
				SaveGangZone(i);
			}
			// result
			SendClientMessage(playerid,SYSTEM_COLOR,"Цвет банды изменён");
			return 1;
		}
		case GANGS_WELCOME_CMD:
		{
			// check quest
			if(GetPlayerQuest(playerid) != INVALID_QUEST_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы выполняете задание");
			// check player gang
			if((itmp[0] = GetPVarInt(playerid,"Gang")) == INVALID_GANG_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не в банде");
			format(GangWelcome[ itmp[0] ],MAX_GANG_WELCOME,"%s (Изменено %s)",params,oGetPlayerName(playerid));
			SendClientMessage(playerid,GangColor[ itmp[0] ],GangWelcome[ itmp[0] ]);
			// result
			SendClientMessage(playerid,SYSTEM_COLOR,"Приветствие банды изменено");
			return 1;
		}
		case GANGS_INVITE_CMD:
		{
			// check quest
			if(GetPlayerQuest(playerid) != INVALID_QUEST_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы выполняете задание");
			// check player gang
			if((itmp[0] = GetPVarInt(playerid,"Gang")) == INVALID_GANG_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не в банде");
			// check lider
			if(GangMemberPlayerid[ itmp[0] ][0] != playerid) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не лидер банды");
			// get player id
			itmp[1] = strval(strtok(params,idx));
			// check online
			if(!IsPlayerConnected(itmp[1])) return SendClientMessage(playerid,SYSTEM_COLOR,"Не верный id игрока");
			// check quest
			if(GetPlayerQuest(itmp[1]) != INVALID_QUEST_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Игрок на задании");
			// check gang
			if(GetPVarInt(itmp[1],"Gang") != INVALID_GANG_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Игрок уже в банде");
			// send invite
			SetPVarInt(itmp[1],"GangInvite",itmp[0]);
			format(stmp,sizeof(stmp),"%s приглашает вас в банду %s",oGetPlayerName(playerid),GangName[ itmp[0] ]);
			ShowPlayerDialog(itmp[1],DIALOG_GANG_INVITE,DIALOG_STYLE_MSGBOX,"Приглашение",stmp,"Согласиться","Отказаться");
			format(stmp,sizeof(stmp),"Вы пригласили %s в банду",oGetPlayerName(itmp[1]));
			SendClientMessage(playerid,SYSTEM_COLOR,stmp);
			return 1;
		}
		case GANGS_QUIT_CMD:
		{
			// check quest
			if(GetPlayerQuest(playerid) != INVALID_QUEST_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы выполняете задание");
			// check player gang
			if((itmp[0] = GetPVarInt(playerid,"Gang")) == INVALID_GANG_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не в банде");
			// check gang war
			if(GangWarState{ itmp[0] } != GANG_WAR_STATE_OFF)
			{
				// check lider
				if(GangMemberPlayerid[ itmp[0] ][0] == playerid) GangWarComplete(GangWarWith[ itmp[0] ],itmp[0],GANG_END_LIDER_LEAVE);
			}
			// leave gang
			new count[2];
			for(new i = 0;i < MAX_GANG_MEMBERS;i++)
			{
				if(!IsGangMemberActive[ itmp[0] ]{i}) continue;
				if(GangMemberPlayerid[ itmp[0] ][i] == playerid) 
				{
					IsGangMemberActive[ itmp[0] ]{i} = 0;
					GangMemberPlayerid[ itmp[0] ][i] = INVALID_PLAYER_ID;
					continue;
				}
				if(GangMemberPlayerid[ itmp[0] ][i] != INVALID_PLAYER_ID) count[0]++;
				count[1]++;
			}
			// if all offline
			if(count[0] == 0) 
			{
				SaveGang(itmp[0]);
				IsGangActive{ itmp[0] } = 0;
			}
			// if gang null
			if(count[1] == 0) GangDelete(itmp[0]);
			// set data
			SetPVarInt(playerid,"Gang",INVALID_GANG_ID);
			SetPlayerColor(playerid,PLAYERS_COLOR);
			return SendClientMessage(playerid,SYSTEM_COLOR,"Вы покинули банду");
		}
		case GANGS_KICK_CMD:
		{
			// check quest
			if(GetPlayerQuest(playerid) != INVALID_QUEST_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы выполняете задание");
			// check player gang
			if((itmp[0] = GetPVarInt(playerid,"Gang")) == INVALID_GANG_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не в банде");
			// check lider
			if(GangMemberPlayerid[ itmp[0] ][0] != playerid) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не лидер банды");
			// get name
			strtoktmp = strtok(params,idx);
			// check name
			if(strlen(strtoktmp) < 3) return SendClientMessage(playerid,SYSTEM_COLOR,"Не верный ник");
			// leave gang
			itmp[1] = -2;
			new count[2];
			for(new i = 0;i < MAX_GANG_MEMBERS;i++)
			{
				if(!IsGangMemberActive[ itmp[0] ]{i}) continue;
				if(!strcmp(strtoktmp,GangMemberName[ itmp[0] ][i]))
				{
					itmp[1] = GangMemberPlayerid[ itmp[0] ][i];
					IsGangMemberActive[ itmp[0] ]{i} = 0;
					GangMemberPlayerid[ itmp[0] ][i] = INVALID_PLAYER_ID;
					continue;
				}
				if(GangMemberPlayerid[ itmp[0] ][i] != INVALID_PLAYER_ID) count[0]++;
				count[1]++;
			}
			// check find
			if(itmp[1] == -2) return SendClientMessage(playerid,SYSTEM_COLOR,"Члена банды с таким ником не найдено");
			// check gang war
			if(GangWarState{ itmp[0] } != GANG_WAR_STATE_OFF)
			{
				// check lider
				if(GangMemberPlayerid[ itmp[0] ][0] == itmp[1]) GangWarComplete(GangWarWith[ itmp[0] ],itmp[0],GANG_END_LIDER_LEAVE);
			}
			// if all offline
			if(count[0] == 0) 
			{
				SaveGang(itmp[0]);
				IsGangActive{ itmp[0] } = 0;
			}
			// if gang null
			if(count[1] == 0) GangDelete(itmp[0]);
			// set data
			if(itmp[1] != INVALID_PLAYER_ID)
			{
				SetPVarInt(itmp[1],"Gang",INVALID_GANG_ID);
				SetPlayerColor(itmp[1],PLAYERS_COLOR);
				SendClientMessage(itmp[1],SYSTEM_COLOR,"Вы изгнаны из банды");
			}
			format(stmp,sizeof(stmp),"Вы изгнали %s из банды",strtoktmp);
			return SendClientMessage(playerid,SYSTEM_COLOR,stmp);
		}
		case GANGS_MEMBERS_CMD:
		{
			// check player gang
			if((itmp[0] = GetPVarInt(playerid,"Gang")) == INVALID_GANG_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не в банде");
			// null str
			stmp[0] = 0;
			// build string
			for(new i = 0;i < MAX_GANG_MEMBERS;i++)
			{
				if(!IsGangMemberActive[ itmp[0] ]{i}) continue;
				format(stmp,sizeof(stmp),"%s%s\n",stmp,GangMemberName[ itmp[0] ][i]);
			}
			// show
			ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"Члены банды",stmp,"Ок","");
			return 1;
		}
		case GANGS_STATS_CMD:
		{
			// check player gang
			if((itmp[0] = GetPVarInt(playerid,"Gang")) == INVALID_GANG_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не в банде");
			// null str
			stmp[0] = 0;
			// build string
			format(stmp,sizeof(stmp),"Название:\t\t\t%s\n",GangName[ itmp[0] ]);
			format(stmp,sizeof(stmp),"%sВсего убийств:\t\t%d\n",stmp,GangKillsAll[ itmp[0] ]);
			format(stmp,sizeof(stmp),"%sУбийств членов банд:\t%d\n",stmp,GangKillsGangs[ itmp[0] ]);
			format(stmp,sizeof(stmp),"%sОчки банды:\t\t%d\n",stmp,GangScore[ itmp[0] ]);
			format(stmp,sizeof(stmp),"%sПриветствие банды:\t%s\n",stmp,GangWelcome[ itmp[0] ]);
			format(stmp,sizeof(stmp),"%sДоступно укреплений:\t%d\n",stmp,GangReinforces[itmp[0]]);
			//printf("Debug %d gzones=%d GZCount=%d ",GZones[itmp[0]]/(GZCount/100),GZones[itmp[0]],GZCount);
			//ftmp[0]=GZCount/100;
			//ftmp[1]=GZones[itmp[0]]/ftmp[0];
			format(stmp,sizeof(stmp),"%sТерриторий под контролем:\t%d\n",stmp,GZones[itmp[0]]);
			// show
			ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"Статистика банды",stmp,"Ок","");
			return 1;
		}
		case GANGS_ZONE_HELP_CMD:
		{
			stmp[0] = 0;
			strcat(stmp,"Каждая банда может захватывать зоны, за захват даются очки банды\n");
			strcat(stmp,"Чтобы захватить зону надо продержаться в зоне определенное время, стараться не нести потери и убивать как можно больше врагов\n");
			strcat(stmp,"Зоны имеют три критерия ценности:\n");
			strcat(stmp,"Информационный - чем больше уровень этой характеристики тем больше опыта дает зона управляющей банде\n");
			strcat(stmp,"Финансовый - эта характеристика отвечает за количество денег которое достается банде-владельцу с зоны\n");
			strcat(stmp,"Стратегический - влияет на продолжительность таймера захвата(если куплено и активировано укрепление, то бонус составляет до 5 часов)\n");
			strcat(stmp,"Столица - Лидер банды может выбрать любую зону столицей, это даст удвоение всей прибыли и невозможность захвата столицы, пока под контролем есть другие зоны,\n");
			strcat(stmp,"если зон не осталось то захват столицы приведет к уничтожению вашей банды полностью\n");
			strcat(stmp,"Команды:\n/gz info - информация о зоне в которой находитесь\n/gz claim - захват зоны");
			ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,MODE_NAME " -> Зоны банд",stmp,"Ок","");
			return 1;
		}
		case GANGS_ZONE_INFO_CMD:
		{
			// check zone
			if((itmp[0] = GetPlayerGangZone(playerid)) == INVALID_GANG_ZONE_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не в зоне");
			// null str
			stmp[0] = 0;
			// build string

			format(stmp,sizeof(stmp),"Зона номер:\t\t%d\n",itmp[0]);
			format(stmp,sizeof(stmp),"%sБанда владелец:\t%s\n",stmp,GangZonesOwner[ itmp[0] ]);
			format(stmp,sizeof(stmp),"%sЦенность:\n",stmp);
			format(stmp,sizeof(stmp),"%sИнформационная %s %d/5\n",stmp,DrawPBar(TactDat[itmp[0]][0],5),TactDat[itmp[0]][0]);
			format(stmp,sizeof(stmp),"%sФинансовая     %s %d/5\n",stmp,DrawPBar(TactDat[itmp[0]][1],5),TactDat[itmp[0]][1]);
			format(stmp,sizeof(stmp),"%sCтратегическая %s %d/5\n",stmp,DrawPBar(TactDat[itmp[0]][2],5),TactDat[itmp[0]][2]);
			// show
			ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"Информация о зоне",stmp,"Ок","");
			return 1;
		}
		case GANGS_WAR_HELP_CMD:
		{
			stmp[0] = 0;
			strcat(stmp,"Каждая банда может обьявить войну другой банде, такое \"официальное\" обьявление войны даёт гарантию\nчто заявленая ставка будет передана победителю\n");
			strcat(stmp,"Чтобы обьявить войну лидеры обоих банд должны быть онлайн и заявленые условия боя реальны\n");
			strcat(stmp,"Команды:\n/gw attack - обьявить войну банде\n/gw join - принять участие в войне\n/gw quit - отменить участие в войне");
			ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,MODE_NAME " -> Войны банд",stmp,"Ок","");
			return 1;
		}
		case GANGS_WAR_ATTACK_CMD:
		{
			// check quest
			new lid;
			if(GetPlayerQuest(playerid) != INVALID_QUEST_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы выполняете задание");
			// check player gang
			if((itmp[0] = GetPVarInt(playerid,"Gang")) == INVALID_GANG_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не в банде");
			// check lider
			if(GangMemberPlayerid[ itmp[0] ][0] != playerid) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не лидер банды");
			// get gang name
			strtoktmp = strtok(params,idx);
			// check name
			if(strlen(strtoktmp) < 3) return SendClientMessage(playerid,SYSTEM_COLOR,"Не верное название банды");
			// find def gang
			for(new i = 0;i < MAX_GANGS;i++)
			{
				if(strcmp(strtoktmp,GangName[i])) continue;
				// check lider online
				if((itmp[1] = GangMemberPlayerid[i][0]) == INVALID_PLAYER_ID)
				{
				return SendClientMessage(playerid,SYSTEM_COLOR,"Лидер банды не в сети");
				}
				lid=i;
				// get war params
				GangWarPriceType{ itmp[0] } = strval(strtok(params,idx));
				GangWarPrice[ itmp[0] ] = strval(strtok(params,idx));
				// check war params
				switch(GangWarPriceType{ itmp[0] })
				{
					case GANG_WAR_PRICE_ZONE:
					{
						// check zone id
						if((GangWarPrice[ itmp[0] ] < 0) || (GangWarPrice[ itmp[0] ] > GANG_ZONES_PER_CITY * 3)) return SendClientMessage(playerid,SYSTEM_COLOR,"Не верная зона");
						// check zone own
						if(strcmp(GangZonesOwner[ GangWarPrice[ itmp[0] ] ],GangName[i])) return SendClientMessage(playerid,SYSTEM_COLOR,"Зона-приз не принадлежит атакованой банде");
						// check money
						if(oGetPlayerMoney(playerid) < ZONE_GANG_WAR_COST) return SendClientMessage(playerid,SYSTEM_COLOR,"Вам требуется иметь " #ZONE_GANG_WAR_COST " для войны за зону");
					}
					case GANG_WAR_PRICE_MONEY:
					{
						// check money
						if(oGetPlayerMoney(playerid) < GangWarPrice[ itmp[0] ]) return SendClientMessage(playerid,SYSTEM_COLOR,"У вас нет столько денег");
						// check money
						if(oGetPlayerMoney(itmp[1]) < GangWarPrice[ itmp[0] ]) return SendClientMessage(playerid,SYSTEM_COLOR,"У лидера атакованой банды нет столько денег");
					}
					case GANG_WAR_PRICE_SCORE:
					{
						// check score
						if(GangScore[ itmp[0] ] < GangWarPrice[ itmp[0] ]) return SendClientMessage(playerid,SYSTEM_COLOR,"У вашей банды недостаточно очков банды");
						// check score
						if(GangScore[i] < GangWarPrice[ itmp[0] ]) return SendClientMessage(playerid,SYSTEM_COLOR,"У атакованой банды недостаточно очков банды");
					}
					case GANG_WAR_PRICE_DELETE: {}
					default: return SendClientMessage(playerid,SYSTEM_COLOR,"Неизвестный тип ставки");
				}
				// set war data
				GangWarState{ itmp[0] } = GANG_WAR_STATE_LINEUP;
				GangWarState{i} = GANG_WAR_STATE_LINEUP;
				GangWarSide{ itmp[0] } = GANG_WAR_SIDE_ATTACKER;
				GangWarSide{i} = GANG_WAR_SIDE_DEFENDER;
				GangWarWith[ itmp[0] ] = i;
				GangWarWith[i] = itmp[0];
				GangWarMembersCount{ itmp[0] } = 0;
				GangWarMembersCount{i} = 0;
				for(new j = 0;j < MAX_GANG_MEMBERS;j++)
				{
					IsGangMemberInWar[ itmp[0] ]{j} = 0;
					IsGangMemberInWar[i]{j} = 0;
				}
				// send info for def-lider
				// null str
				stmp[0] = 0;
				// build string
				format(stmp,sizeof(stmp),"Банда \"%s\" обьявила вам войну, ставка %s (%d)",GangName[ itmp[0] ],GetGangWarPriceName(GangWarPriceType{ itmp[0] }),GangWarPrice[ itmp[0] ]);
				ShowPlayerDialog(lid,DIALOG_GANG_WAR,DIALOG_STYLE_MSGBOX,"Война!",stmp,"Согласиться","Отказаться");
				return 1;
			}
			SendClientMessage(playerid,SYSTEM_COLOR,"Не верное название банды");
			return 1;
		}
		case GANGS_WAR_JOIN_CMD:
		{
			// check quest
			if(GetPlayerQuest(playerid) != INVALID_QUEST_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы выполняете задание");
			// check player gang
			if((itmp[0] = GetPVarInt(playerid,"Gang")) == INVALID_GANG_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не в банде");
			// check gang war
			if(GangWarState{ itmp[0] } != GANG_WAR_STATE_LINEUP) return SendClientMessage(playerid,SYSTEM_COLOR,"В данный момент нет войны");
			// check already exist
			for(new i = 0;i < MAX_GANG_MEMBERS;i++)
			{
				if(!IsGangMemberActive[ itmp[0] ]{i}) continue;
				if(GangMemberPlayerid[ itmp[0] ][i] != playerid) continue;
				if(IsGangMemberInWar[ itmp[0] ]{i} == 1) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы уже записались на войну");
				IsGangMemberInWar[ itmp[0] ]{i} = 1;
				GangWarMembersCount{ itmp[0] }++;
				format(stmp,sizeof(stmp),"%s записался на участие в войне.Записано %d человек",oGetPlayerName(playerid),GangWarMembersCount{ itmp[0] });
				for(new j = 0;j < MAX_GANG_MEMBERS;j++)
				{
					if(!IsGangMemberActive[ itmp[0] ]{j}) continue;
					if(GangMemberPlayerid[ itmp[0] ][j] == INVALID_PLAYER_ID) continue;
					
					SendClientMessage(GangMemberPlayerid[ itmp[0] ][j],GangColor[ itmp[0] ],stmp);
				}
				SetPlayerQuest(playerid,GangWarQuest);
				return 1;
			}
			print("[ERROR] gang war join -> player not find in gang list");
			return 1;
		}
		case GANGS_ZONE_CLAIM_CMD:
		{
		if(GetPlayerVirtualWorld(playerid) == STUNT_VIRTUAL_WORLD) return SendClientMessage(playerid,COLOR_RED,"Захват из Стант-мира невозможен");
		new atks,defs,defid,tid = GetFreeRTimer();
		if(tid == -1) return SendClientMessage(playerid,COLOR_RED,"превышен лимит сессий захвата");

		if((itmp[2] = GetPlayerGangZone(playerid)) == INVALID_GANG_ZONE_ID) return SendClientMessage(playerid,COLOR_RED,"Вы не в зоне для захвата");
		if((itmp[1] = GetPVarInt(playerid,"Gang")) == INVALID_GANG_ID) return 0;
		if(strcmp(GangZonesOwner[ itmp[2] ],GangName[ itmp[1] ]) == 0) return SendClientMessage(playerid,COLOR_RED,"Эта зона уже ваша");
		if(GangZonesCapture{ itmp[2] } == 1) return SendClientMessage(playerid,COLOR_RED,"Эта зона уже захватывается!");
		if(strcmp(GangZonesOwner[ itmp[2] ],"Unknown"))
			{
				itmp[0]=LoadGang(GangZonesOwner[ itmp[2] ]);
				if(GangCap[itmp[0]]!=-1)
				{
				if(GZones[itmp[0]]>1 && GangCap[itmp[0]]==itmp[2]) return SendClientMessage(playerid,COLOR_RED,"Столица не может быть захвачена пока есть другие незахваченые зоны этой банды!");
				else if(GangCap[itmp[0]]==itmp[2])
					{
					format(stmp,sizeof(stmp),"Атакована столица банды %s! Атакующий :%s",GangZonesOwner[ itmp[2] ],GangName[ GetPVarInt(playerid,"Gang")]);
					SendClientMessageToAll(COLOR_YELLOW,stmp);
					}
				}
			}
		GetPlayerPos(playerid,ftmp[0],ftmp[1],ftmp[2]);
		GangZonesCapture{ itmp[2] } = 1;
		//printf("DEBUG:SRVID is %d itmp2 is %d",GangZonesServerId[ itmp[2] ],itmp[2]);
		GangZoneFlashForAll(GangZonesServerId[ itmp[2] ],COLOR_RED);
		//GangZoneFlashForPlayer(playerid,GangZonesServerId[ itmp[2] ],COLOR_RED);
		format(stmp,sizeof(stmp),"Атакована зона %d в районе : %s! Владелец: %s Атакующий :%s",itmp[2],GetXYZZoneName(ftmp[0],ftmp[1],ftmp[2]) ,GangZonesOwner[ itmp[2] ],GangName[ GetPVarInt(playerid,"Gang")]);
		SendClientMessageToAll(COLOR_YELLOW,stmp);
		//считаем количество атакующих и защищающихся онлайн
		//for(new i = 0;i <= GetPlayersCount();i++)
		foreach(new i : Player)
			{
				//if(!IsPlayerConnected(i)) continue;
				if(IsPlayerNPC(i)) continue;
				if(GetPVarInt(i,"Gang")==INVALID_GANG_ID) continue;
				if(GetPVarInt(i,"Gang")==GetPVarInt(playerid,"Gang")) atks++;
				if(strcmp(GangZonesOwner[ itmp[2] ],GangName[ GetPVarInt(i,"Gang") ]) == 0) defs++;
			}
		printf("TTL atk %d def %d",atks,defs);
		
		//calculate reinforce
		if(defs == 0) 
			{
				if(!strcmp(GangZonesOwner[ itmp[2] ],"Unknown"))
					{
					RTimer[tid] = 5; // ничейная территория
					defid=-1;
					goto end;
					}
				else defid=LoadGang(GangZonesOwner[ itmp[2] ]);
				if(GangReinforces[defid] >0)
				{
					RTimer[tid] = 25200+(3600*TactDat[itmp[2]][2]); // реинфорс тикет активен
					GangReinforces[defid]--;
					SaveGang(defid);
				}
				else RTimer[tid] = 10; // никого онлайн, нет тикетов
				end:
			}
		else RTimer[tid] = 300 *(1+((defs*15)/100)) +(120*TactDat[itmp[2]][2]); //каждый активный защитник увеличивает время реинфорса на 15%
		//RTimer[tid]=RTimer[tid]*(1+((10*TactDat[itmp[2]][2])/100)); //Стратегический бонус
		GangZonesAttacker[ itmp[2] ]=GangName[ GetPVarInt(playerid,"Gang")];
		//RTimer[GetFreeRTimer()] =((10*(defs+1))/(atks+1));
		RTimerDat[tid][a_id] = GetPVarInt(playerid,"Gang");
		RTimerDat[tid][d_id] =defid;
		RTimerDat[tid][z_id] =itmp[2];
		RTimerDat[tid][ascore] = 1;
		RTimerDat[tid][dscore] =0;
		format(stmp,sizeof(stmp),"Зона %d оспаривается\n До заврешения войны за территорию: %s",itmp[2],ConvertSecs(RTimer[tid]));
		TRDLABEL[900+tid]=Create3DTextLabel(stmp,GangColor[ RTimerDat[tid][a_id]],ftmp[0],ftmp[1],(ftmp[2] + 1.0),200.0,0,0);
		printf("%d",RTimer[tid]);
		return 1;
		}
		case GANGS_WAR_LEAVE_CMD:
		{
			// check quest
			if(GetPlayerQuest(playerid) != GangWarQuest) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не участник войны");
			// check player gang
			if((itmp[0] = GetPVarInt(playerid,"Gang")) == INVALID_GANG_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не в банде");
			// check gang war
			if(GangWarState{ itmp[0] } != GANG_WAR_STATE_LINEUP) return SendClientMessage(playerid,SYSTEM_COLOR,"В данный момент нет войны");
			// check already exist
			for(new i = 0;i < MAX_GANG_MEMBERS;i++)
			{
				if(!IsGangMemberActive[ itmp[0] ]{i}) continue;
				if(GangMemberPlayerid[ itmp[0] ][i] != playerid) continue;
				if(IsGangMemberInWar[ itmp[0] ]{i} == 0) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не записаны на войну");
				IsGangMemberInWar[ itmp[0] ]{i} = 0;
				GangWarMembersCount{ itmp[0] }--;
				format(stmp,sizeof(stmp),"%s вышел из участия в войне.Записано %d человек",oGetPlayerName(playerid),GangWarMembersCount{ itmp[0] });
				for(new j = 0;j < MAX_GANG_MEMBERS;j++)
				{
					if(!IsGangMemberActive[ itmp[0] ]{j}) continue;
					if(GangMemberPlayerid[ itmp[0] ][j] == INVALID_PLAYER_ID) continue;
					
					SendClientMessage(GangMemberPlayerid[ itmp[0] ][j],GangColor[ itmp[0] ],stmp);
				}
				SetPlayerQuest(playerid,INVALID_QUEST_ID);
				return 1;
			}
			print("[ERROR] gang war leave -> player not find in gang list");
			return 1;
		}
		case GANGS_WAR_START_CMD:
		{
			// check quest
			if(GetPlayerQuest(playerid) != GangWarQuest) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не участник войны");
			// check player gang
			if((itmp[0] = GetPVarInt(playerid,"Gang")) == INVALID_GANG_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не в банде");
			// check lider
			if(GangMemberPlayerid[ itmp[0] ][0] != playerid) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы не лидер банды");
			// check gang war
			if(GangWarState{ itmp[0] } != GANG_WAR_STATE_LINEUP) return SendClientMessage(playerid,SYSTEM_COLOR,"В данный момент нет войны");
			// check members count
			if(GangWarMembersCount{ itmp[0] } != GangWarMembersCount{ GangWarWith[ itmp[0] ] }) return SendClientMessage(playerid,SYSTEM_COLOR,"Не равное количество участников");
			// start
			itmp[1] = GangWarWith[ itmp[0] ];
			GangWarState[ itmp[0] ] = GANG_WAR_STATE_ON;
			GangWarState[ itmp[1] ] = GANG_WAR_STATE_ON;
			// members teleport
			format(stmp,sizeof(stmp),"%s vs %s (%dx%d)",GangName[ itmp[0] ],GangName[ itmp[1] ],GangWarMembersCount{ itmp[0] },GangWarMembersCount{ itmp[1] });
			for(new g = 0;g < 2;g++)
			{
				for(new i = 0;i < MAX_GANG_MEMBERS;i++)
				{
					if(!IsGangMemberActive[ itmp[g] ]{i}) continue;
					if(GangMemberPlayerid[ itmp[g] ][i] == INVALID_PLAYER_ID) continue;
					if(IsGangMemberInWar[ itmp[g] ]{i} == 0) continue;
					// teleport
					itmp[2] = random(MAX_GANG_WAR_SPAWNS);
					SetPlayerPos(GangMemberPlayerid[ itmp[g] ][i],GangWarSpawn[g][ itmp[2] ][0],GangWarSpawn[g][ itmp[2] ][1],GangWarSpawn[g][ itmp[2] ][2]);
					SetPlayerVirtualWorld(GangMemberPlayerid[ itmp[g] ][i],GANG_WAR_VIRTUAL_WORLD + itmp[0]);
					// stat
					oSetPlayerHealth(GangMemberPlayerid[ itmp[g] ][i],100.0);
					oSetPlayerArmour(GangMemberPlayerid[ itmp[g] ][i],100.0);
					// show weapon dialog
					ShowPlayerDialog(GangMemberPlayerid[ itmp[g] ][i],DIALOG_GANG_WAR_WEAPONS,DIALOG_STYLE_LIST,"Оружие",GANG_WAR_WEAPONS_STRING,"Взять","Готово");
					// send msg
					SendClientMessage(GangMemberPlayerid[ itmp[g] ][i],GangColor[ itmp[g] ],stmp);
				}
			}
			return 1;
		}
	}
	return 0;
}

Gangs_OnPlayerConnect(playerid)
{
	SetPVarInt(playerid,"Gang",INVALID_GANG_ID);
	SetPVarInt(playerid,"GangInvite",INVALID_GANG_ID);
}

Gangs_OnPlayerSpawn(playerid)
{
	for(new i = 0;i < sizeof(GangZonesServerId);i++) 
	{
		if(!GangZonesCapture{i}) GangZoneShowForPlayer(playerid,GangZonesServerId[i],GangZonesColor[i]);
		else 
		{
			GangZoneShowForPlayer(playerid,GangZonesServerId[i],GangZonesColor[i]);
			GangZoneFlashForPlayer(playerid,GangZonesServerId[i],COLOR_RED);
		}
	}
	// check quest
	if(GetPlayerQuest(playerid) != INVALID_QUEST_ID) return;
	// check gang
	if((itmp[0] = GetPVarInt(playerid,"Gang")) != INVALID_GANG_ID) SetPlayerColor(playerid,GangColor[ itmp[0] ]);
}

Gangs_OnPlayerDeath(playerid,killerid,reason)
{
	#pragma unused reason
	// check gang war
	/* if((GetPlayerQuest(playerid) == GangWarQuest) && (GetPlayerQuest(killerid) == GangWarQuest))
	{
		// check killer gang
		if((itmp[0] = GetPVarInt(killerid,"Gang")) == INVALID_GANG_ID) return;
		// check player gang
		if((itmp[1] = GetPVarInt(playerid,"Gang")) == INVALID_GANG_ID) return;
		if((GangWarState[ itmp[0] ] == GANG_WAR_STATE_ON) && (GangWarState[ itmp[1] ] == GANG_WAR_STATE_ON))
		{
			static mem[2];
			mem[0] = -1;
			mem[1] = -1;
			// find killer
			for(new i = 0;i < MAX_GANG_MEMBERS;i++)
			{
				if(!IsGangMemberActive[ itmp[0] ]{i}) continue;
				if(GangMemberPlayerid[ itmp[0] ][i] != killerid) continue;
				if(IsGangMemberInWar[ itmp[0] ]{i} == 0) continue;
				
				mem[0] = i;
				break;
			}
			// find dead
			for(new i = 0;i < MAX_GANG_MEMBERS;i++)
			{
				if(!IsGangMemberActive[ itmp[1] ]{i}) continue;
				if(GangMemberPlayerid[ itmp[1] ][i] != playerid) continue;
				if(IsGangMemberInWar[ itmp[1] ]{i} == 0) continue;
				
				mem[1] = i;
				break;
			}
			// check finded
			if((mem[0] != -1) && (mem[1] != -1))
			{
				// players in war
				IsGangMemberInWar[ itmp[1] ]{ mem[1] } = 0;
				GangWarMembersCount{ itmp[1] }--;
				// dead null stat
				SetPlayerQuest(playerid,INVALID_QUEST_ID);
				SetPlayerVirtualWorld(playerid,0);
				// check end
				if(GangWarMembersCount{ itmp[1] } == 0)
				{
					// gang war end
					if(itmp[0] != itmp[1]) GangWarComplete(itmp[0],itmp[1],GANG_WAR_END_KILLED);
					else GangWarComplete(GangWarWith[ itmp[1] ],itmp[1],GANG_WAR_END_KILLED);
					// end work
					return;
				}
			}
		}
	} */
	// check quest
	if(GetPlayerQuest(playerid) != INVALID_QUEST_ID) return;
	if(GetPlayerQuest(killerid) != INVALID_QUEST_ID) return;
	// check killer gang
	if((itmp[0] = GetPVarInt(killerid,"Gang")) == INVALID_GANG_ID) return;
	GangKillsAll[ itmp[0] ]++;
	// check player gang
	if((itmp[1] = GetPVarInt(playerid,"Gang")) == INVALID_GANG_ID) return;
	// check team kill
	if(itmp[0] == itmp[1])
	{
		format(stmp,sizeof(stmp),"%s убил союзника %s",oGetPlayerName(killerid),oGetPlayerName(playerid));
		// send for all gang
		for(new i = 0;i < MAX_GANG_MEMBERS;i++)
		{
			if(!IsGangMemberActive[ itmp[1] ]{i}) continue;
			if(GangMemberPlayerid[ itmp[1] ][i] == INVALID_PLAYER_ID) continue;
			
			SendClientMessage(GangMemberPlayerid[ itmp[1] ][i],GangColor[ itmp[1] ],stmp);
		}
		return;
	}
	// rate
	if(GangScore[ itmp[1] ] > GangScore[ itmp[0] ]) ftmp[0] = 1.5;
	else if(GangScore[ itmp[1] ] == GangScore[ itmp[0] ]) ftmp[0] = 1.0;
	else if(GangScore[ itmp[1] ] < GangScore[ itmp[0] ]) ftmp[0] = 0.25;
	// add score
	GangScore[ itmp[0] ] += floatround(GANG_SCORE_RATE * ftmp[0]);
	// add gang kill
	GangKillsGangs[ itmp[0] ] ++;
	// find zone
	if((itmp[2] = GetPlayerGangZone(playerid)) == INVALID_GANG_ZONE_ID) return;
	// check player
	
	if((strcmp(GangZonesOwner[ itmp[2] ],GangName[ itmp[1] ]) == 0) || (strcmp(GangZonesOwner[ itmp[2] ],"Unknown") == 0))
	{
		// killed owner of zone
		
		if(!strcmp(GangZonesAttacker[ itmp[2] ],GangName[ itmp[0] ]))
		{
			itmp[3]=FindTimerBySides(itmp[0],itmp[1]);
			if(itmp[3]==-1) return;
			RTimerDat[itmp[3]][ascore]++;
		/*else GangZonesCapture{ itmp[2] } = 0;
		if(GangZonesCapture{ itmp[2] } == KILLS_FOR_CAPTURE)
		{
			// zone captured
			format(GangZonesOwner[ itmp[2] ],MAX_GANG_NAME,GangName[ itmp[0] ]);
			GangZonesColor[ itmp[2] ] = GangColor[ itmp[0] ];
			GangZonesCapture{ itmp[2] } = 0;
			GangZoneShowForAll(GangZonesServerId[ itmp[2] ],GangZonesColor[ itmp[2] ]);
			// attacker bonus
			GangScore[ itmp[0] ] += floatround((GANG_SCORE_RATE * 10) * ftmp[0]);*/
		}
		
		/* else if(GangZonesCapture{ itmp[2] } == 1) GangZoneFlashForAll(GangZonesServerId[ itmp[2] ],COLOR_RED);
		else if(GangZonesCapture{ itmp[2] } == 0) GangZoneShowForAll(GangZonesServerId[ itmp[2] ],GangZonesColor[ itmp[2] ]);
		// attacker
		format(GangZonesAttacker[ itmp[2] ],MAX_GANG_NAME,GangName[ itmp[0] ]);  */
	}
	else if(!strcmp(GangZonesAttacker[ itmp[2] ],GangName[ itmp[1] ]))
	{				
		itmp[3]=FindTimerBySides(itmp[1],itmp[0]);
		if(itmp[3]==-1) return;
		//killed attacker
		print("attacker down");
		RTimerDat[itmp[3]][dscore]++;
/* 		new dt[6];
		getdate(dt[0],dt[1],dt[2]);
		gettime(dt[3],dt[4],dt[5]);
		itmp[3] = mktime(dt[3],dt[4],dt[5],dt[2],dt[1],dt[0]);
		if((itmp[3] - GangZonesLastOwnerLogin[ itmp[2] ]) > MAX_UNLOGGED_ZONE_TIME)
		{
			// killed owner of zone
			if(!strcmp(GangZonesAttacker[ itmp[2] ],GangName[ itmp[0] ])) GangZonesCapture{ itmp[2] }++;
			else GangZonesCapture{ itmp[2] } = 0;
			if(GangZonesCapture{ itmp[2] } == KILLS_FOR_CAPTURE)
			{
				// zone captured
				format(GangZonesOwner[ itmp[2] ],MAX_GANG_NAME,GangName[ itmp[0] ]);
				GangZonesColor[ itmp[2] ] = GangColor[ itmp[0] ];
				GangZonesCapture{ itmp[2] } = 0;
				GangZoneShowForAll(GangZonesServerId[ itmp[2] ],GangZonesColor[ itmp[2] ]);
				// attacker bonus
				GangScore[ itmp[0] ] += floatround((GANG_SCORE_RATE * 10) * ftmp[0]);
			}
			else if(GangZonesCapture{ itmp[2] } == 1) GangZoneFlashForAll(GangZonesServerId[ itmp[2] ],COLOR_RED);
			else if(GangZonesCapture{ itmp[2] } == 0) GangZoneShowForAll(GangZonesServerId[ itmp[2] ],GangZonesColor[ itmp[2] ]);
			// attacker
			format(GangZonesAttacker[ itmp[2] ],MAX_GANG_NAME,GangName[ itmp[0] ]);
		} */
	}
}
Gangs_OnPlayerDisconnect(playerid,reason)
{
#pragma unused playerid, reason
//#pragma unused reason
return 1;
}
Gangs_Dialog(playerid,dialogid,response,listitem,inputtext[])
{
	//#pragma unused listitem
	#pragma unused inputtext
	switch(dialogid)
	{
		case DIALOG_GANG_INVITE:
		{
			if(response)
			{
				itmp[0] = GetPVarInt(playerid,"GangInvite");
				if(itmp[0] == INVALID_GANG_ID) return 1;
				// find free slot
				new mid = -1;
				for(new i = 0;i < MAX_GANG_MEMBERS;i++)
				{
					if(IsGangMemberActive[ itmp[0] ]{i}) continue;
					mid = i;
				}
				// check slot
				if(mid == -1)
				{
					SendClientMessage(playerid,SYSTEM_COLOR,"В банде нет мест");
					return 1;
				}
				// set data for gang
				IsGangMemberActive[ itmp[0] ]{mid} = 1;
				format(GangMemberName[ itmp[0] ][mid],MAX_PLAYER_NAME,oGetPlayerName(playerid));
				GangMemberPlayerid[ itmp[0] ][mid] = playerid;
				// set data for player
				SetPVarInt(playerid,"Gang",itmp[0]);
				SetPlayerColor(playerid,GangColor[ itmp[0] ]);
				SendClientMessage(playerid,GangColor[ itmp[0] ],GangWelcome[ itmp[0] ]);
			}
			else
			{
				itmp[0] = GetPVarInt(playerid,"GangInvite");
				if(itmp[0] == INVALID_GANG_ID) return 1;
				if(GangMemberPlayerid[ itmp[0] ][0] == INVALID_PLAYER_ID) return 1;
				format(stmp,sizeof(stmp),"%s отказался вступать в банду",oGetPlayerName(playerid));
				SendClientMessage(GangMemberPlayerid[ itmp[0] ][0],SYSTEM_COLOR,stmp);
			}
			return 1;
		}
		case DIALOG_GANG:
		{
			if(!response) return 1;
			switch(listitem)
			{
				case 0: //stats
				{
				Gangs_Command(playerid,GANGS_STATS_CMD," ");
				return 1;
				}
				case 1: // gz info
				{
				Gangs_Command(playerid,GANGS_ZONE_INFO_CMD," ");
				return 1;
				}
				case 2://members
				{
				Gangs_Command(playerid,GANGS_MEMBERS_CMD," ");
				return 1;
				}
				case 3://leave
				{
				Gangs_Command(playerid,GANGS_QUIT_CMD," ");
				return 1;
				}
				case 4: //claim
				{
				Gangs_Command(playerid,GANGS_ZONE_CLAIM_CMD," ");
				return 1;
				}
				case 5: //leader - make capital
				{
				ShowPlayerDialog(playerid,DIALOG_GANG_CAPITAL,DIALOG_STYLE_MSGBOX,"{FF0000}ВНИМАНИЕ!","Ваша столица не может быть захвачена, пока остаются другие территории под вашим контролем,\n но если их не осталось, удачный захват вашей столицы уничтожит вашу банду. Продолжить?","Да","Нет");
				//Rebuild_tact(); //DEBUG ONLY - DO NOT ENABLE!
				return 1;
				}
				case 6: //leader - buy reinforce
				{
				if(oGetPlayerMoney(playerid)<1000000) return SendClientMessage(playerid,COLOR_SPRINGGREEN,"У вас недостаточно денег!");
				oGivePlayerMoney(playerid,-1000000);
				GangReinforces[GetPVarInt(playerid,"Gang")]++;
				format(stmp,sizeof(stmp),"Вы приобрели укрепление,укреплений доступно у вашей банды : %d",GangReinforces[GetPVarInt(playerid,"Gang")]);
				SendClientMessage(playerid,SYSTEM_COLOR,stmp);
				return 1;
				}
				
			}
			return 1;
		}
		case DIALOG_GANG_CAPITAL:
		{
		if(!response) return 1;
		itmp[1]=GetPVarInt(playerid,"Gang");
		if((itmp[2] = GetPlayerGangZone(playerid)) == INVALID_GANG_ZONE_ID) return SendClientMessage(playerid,COLOR_RED,"Вы не в зоне");
		if(strcmp(GangZonesOwner[ itmp[2] ],GangName[ itmp[1] ])) return SendClientMessage(playerid,COLOR_RED,"Эта зона не ваша");
		if(oGetPlayerMoney(playerid)<10000000) return SendClientMessage(playerid,COLOR_RED,"Недостаточно денег");
		oGivePlayerMoney(playerid,-10000000);
		GangCap[itmp[1]]=itmp[2];
		ftmp[0]=GangZonesCoords[itmp[2]][2]-((GangZonesCoords[itmp[2]][2]-GangZonesCoords[itmp[2]][0])/2);
		ftmp[1]=GangZonesCoords[itmp[2]][3]-((GangZonesCoords[itmp[2]][3]-GangZonesCoords[itmp[2]][1])/2);
		//SetPlayerMapIcon( playerid, 95, ftmp[0],ftmp[1],0.0, 19, 0, MAPICON_GLOBAL );
		CapIcon[itmp[1]]=CreateStreamedIcon(GANG_CAPITAL_ICON,0x0,ftmp[0],ftmp[1],0.0);
		return 1;
		}
	}
	return 1;
}
Gang_Menu(playerid)
{
	stmp="Инфо о банде\nИнфо о зоне\nЧлены банды\nПокинуть банду\nАтаковать территорию" ;
	if(GangMemberPlayerid[ GetPVarInt(playerid,"Gang") ][0] == playerid) strcat(stmp,"\nСделать территорию Столицей(10 000 000$)\nКупить 'Укрепление'(1 000 000$)");
	ShowPlayerDialog(playerid,DIALOG_GANG,DIALOG_STYLE_LIST,"Банда",stmp," Выбрать"," Отмена");
	return 1;
}
Gangs_OnSavePlayerData(playerid,File:datafile,reason)
{
	static gangid;
	gangid = GetPVarInt(playerid,"Gang");
	if(gangid != INVALID_GANG_ID)
	{
		format(stmp,sizeof(stmp),"Gang=%s\r\n",GangName[gangid]);
		fwrite(datafile,stmp);
		SaveGang(gangid);
		if(reason == SAVE_REASON_DISCONNECT)
		{
			// check gang war
			/* if(GangWarState{gangid} != GANG_WAR_STATE_OFF)
			{
				// check lider
				if(GangMemberPlayerid[gangid][0] == playerid) GangWarComplete(GangWarWith[gangid],gangid,GANG_END_LIDER_QUIT);
			} */
			new count;
			for(new i = 0;i < MAX_GANG_MEMBERS;i++)
			{
				if(!IsGangMemberActive[gangid]{i}) continue;
				if(GangMemberPlayerid[gangid][i] == playerid) GangMemberPlayerid[gangid][i] = INVALID_PLAYER_ID;
				if(GangMemberPlayerid[gangid][i] != INVALID_PLAYER_ID) count++;
			}
			if(count == 0) 
			{
				IsGangActive{gangid} = 0;
				
				
				
				
			}
		}
	}
	else
	{
		fwrite(datafile,"Gang=\r\n");
	}
	// save zones
	for(new i = 0;i < sizeof(GangZonesServerId);i++) SaveGangZone(i);
}

Gangs_OnLoadPlayerData(playerid,datastr[],separatorpos)
{
	if(!strcmp(datastr,"Gang",true,separatorpos)) 
	{
		if(strlen(datastr[separatorpos + 1]) > 0)
		{
			new gangid = LoadGang(datastr[separatorpos + 1]);
			new name[MAX_PLAYER_NAME];
			GetPlayerName(playerid,name,sizeof(name));
			SetPVarInt(playerid,"Gang",gangid);
			if(gangid != INVALID_GANG_ID)
			{
				for(new i = 0;i < MAX_GANG_MEMBERS;i++)
				{
					if(!IsGangMemberActive[gangid]{i}) continue;
					if(strcmp(name,GangMemberName[gangid][i])) continue;
					SendClientMessage(playerid,GangColor[gangid],GangWelcome[gangid]);
					//print("FOUND!");
					GangMemberPlayerid[gangid][i] = playerid;
					return;
					// gang member logged
					
				}
				SendClientMessage(playerid,GangColor[gangid],"Вас выгнали из банды");
				SetPVarInt(playerid,"Gang",INVALID_GANG_ID);
				return;
				
			}						
		}
		else SetPVarInt(playerid,"Gang",INVALID_GANG_ID);
	}
}
//GetGangColorForPlayer(playerid)
//{
//	new gangid = GetPVarInt(playerid,"Gang");
//	return GangColor[gangid];
//}

GetFreeRTimer()
{
for(new i=0; i<=MAX_REINFORCE_TIMERS;i++)
	{
	if(RTimer[i]!=0) continue;
	else return i;
	
	}
return -1;
} 
Gangs_Update()
{
//timer ticks
//format(stmp,sizeof(stmp),"DEBUG:%d SEC BEFORE UPDATE",zone_tick);
//SendClientMessageToAll(COLOR_WHITE,stmp);
//print(stmp);

if(zone_tick==0) 
{
	zone_tick=INCOME_TIME;
	Income();
	
}
zone_tick--;
//printf("%d",zone_tick);
for(new i=0; i<=MAX_REINFORCE_TIMERS;i++)
	{
	if(RTimer[i]==0) continue;
	if(RTimer[i]==1) SovChange(i);
	format(stmp,sizeof(stmp),"Зона %d оспаривается\n До заврешения войны за территорию: %s \nCтатус захвата:%s",RTimerDat[i][z_id],ConvertSecs(RTimer[i]),DrawStat(i));
	//print("timer FIRED");
	Update3DTextLabelText(TRDLABEL[900+i],GangColor[ RTimerDat[i][a_id]],stmp);
	RTimer[i]--;
	}
//printf("2:%d",zone_tick);
return 1;
}
SovChange(Timerid)
{
//print("attempting to change sovereignity..");
itmp[0]=RTimerDat[Timerid][z_id];
itmp[1]=RTimerDat[Timerid][a_id];
itmp[2]=RTimerDat[Timerid][d_id];
if(RTimerDat[Timerid][ascore]-RTimerDat[Timerid][dscore]>0)
	{
		//for(new i=0;i<=GetPlayersCount();i++)
		foreach(new i : Player)
			{
		//	if(!IsPlayerConnected(i)) continue;
			if(IsPlayerNPC(i)) continue;
			if(GetPlayerVirtualWorld(i) == STUNT_VIRTUAL_WORLD) continue;
			if((itmp[1]==GetPVarInt(i,"Gang")) && (GetPlayerGangZone(i)==itmp[0])) goto win;
			}
		format(stmp,sizeof(stmp),"Вы должны быть зоне захвата");
		SendClientMessageToAll(COLOR_YELLOW,stmp);
		goto lose;
	}
else 
{
format(stmp,sizeof(stmp),"Банда-защитник отстояла территорию");
SendClientMessageToAll(COLOR_YELLOW,stmp);
goto lose;
}
win:

format(GangZonesOwner[itmp[0]],MAX_GANG_NAME,GangName[ itmp[1] ]);
GangZonesColor[ itmp[0] ] = GangColor[ itmp[1] ];
format(stmp,sizeof(stmp),"Зона %d захвачена бандой:%s",itmp[0],GangName[itmp[1]]);
SendClientMessageToAll(COLOR_YELLOW,stmp);
XPIncome[itmp[1]]=XPIncome[itmp[1]]+XP_PORTION*TactDat[itmp[0]][0];
MoneyIncome[itmp[1]]=MoneyIncome[itmp[1]]+MONEY_PORTION*TactDat[itmp[0]][1];
GZones[itmp[1]]++;
if(itmp[2]!=-1)
{
GZones[itmp[2]]--;
if(GangCap[itmp[2]]==itmp[0])
	{
	format(stmp,sizeof(stmp),"Столица банды %s пала. Банда уничтожена",GangName[itmp[2]]);
	SendClientMessageToAll(COLOR_LIGHTRED,stmp);
	GangDelete(itmp[2]);
	goto final;
	}
XPIncome[itmp[2]]=XPIncome[itmp[2]]-XP_PORTION*TactDat[itmp[0]][0];
MoneyIncome[itmp[2]]=MoneyIncome[itmp[2]]-MONEY_PORTION*TactDat[itmp[0]][1];
SaveGang(itmp[2]);
final:
}
lose:
Delete3DTextLabel(TRDLABEL[900+Timerid]);
GangZonesCapture{ itmp[0] } = 0;
GangZoneShowForAll(GangZonesServerId[ itmp[0] ],GangZonesColor[ itmp[0] ]);
return 1;
}
ConvertSecs(secs)
{
itmp[1]=secs/60; //minutes
itmp[2]=itmp[1]/60; //hours
secs=secs-itmp[1]*60;
itmp[1]=itmp[1]-itmp[2]*60;
format(stmp,sizeof(stmp),"%d Часов %d Минут %d Секунд",itmp[2],itmp[1],secs);
return stmp;
}
DrawStat(tid)
{
itmp[0]=RTimerDat[tid][ascore]-RTimerDat[tid][dscore];
format(stmp,sizeof(stmp)," ");
if(itmp[0]>5) itmp[0]=5;
if(itmp[0]<-5) itmp[0]=-5;
for(new i=-5; i<=5;i++)
{
if(i==itmp[0])
	{
		strcat(stmp,"|");
		continue;
	}
strcat(stmp,"-");

}
format(stmp,sizeof(stmp),"D %s A",stmp);
return stmp;
}
FindTimerBySides(s1,s2)
{

for(new i=0; i<=MAX_REINFORCE_TIMERS;i++)
	{
	if(RTimer[i]==0) continue;
	else 
	{
		if(RTimerDat[i][a_id]==s1 && RTimerDat[i][d_id]==s2) return i;
	}
	}
print("nothing found");
return -1; 
}
stock Rebuild_tact()
{
itmp[0]=0;
while((itmp[1]=LoadGangZone(itmp[0]))!=-1)
	{
	TactDat[itmp[0]][0]=random(5);
	itmp[2]=5-TactDat[itmp[0]][0];
	TactDat[itmp[0]][1]=random(itmp[2]); //By CoolLans :D
	TactDat[itmp[0]][2]=(6-(TactDat[itmp[0]][0]+TactDat[itmp[0]][1]));
	SaveGangZone(itmp[0]);
	itmp[0]++;
	}
return SendClientMessageToAll(SYSTEM_COLOR,"PROCESSED");
}
Income()
{
//print("income");
//for(new i=0;i<=GetPlayersCount();i++)
foreach(new i : Player)
	{
				if(!IsPlayerConnected(i)) continue;
				if(IsPlayerNPC(i)) continue;
				itmp[1]=GetPVarInt(i,"Gang");
				if( itmp[1] == INVALID_GANG_ID) continue;
				if(GangCap[itmp[1]]!=-1)
				{
				//print("cap is fired");
				oGivePlayerMoney(i,MoneyIncome[itmp[1]]*2);
				GivePlayerXP(i,XPIncome[itmp[1]]*2);
				format(stmp,sizeof(stmp),"За владение столицей выплачена двойная прибыль с территорий XP:%d и %d $",XPIncome[itmp[1]]*2,MoneyIncome[itmp[1]]*2);
				}
				else
				{
				oGivePlayerMoney(i,MoneyIncome[itmp[1]]);
				GivePlayerXP(i,XPIncome[itmp[1]]);
				format(stmp,sizeof(stmp),"Выплачена прибыль с территорий XP:%d и %d $",XPIncome[itmp[1]],MoneyIncome[itmp[1]]);
				}
				//printf("cap is %d",GangCap[g]);
				SendClientMessage(i,GangColor[itmp[1]],stmp);
			
	}
return 1;
}