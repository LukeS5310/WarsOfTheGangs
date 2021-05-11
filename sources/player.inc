/*
*	Created:			21.04.10
*	Author:				009
*	Description:		Регистрация, авторизация, уровни, прокачка
*	Info:				Взято из Undead Attack 0.1 beta 4, изменено под WoG
*/

#if defined _player_included
	#endinput
#endif

#define _player_included
#pragma library player

// --------------------------------------------------
// defines
// --------------------------------------------------
#define PLAYERS_DIR				"players/"
#define PLAYERS_FILE_TYPE		".ini"
#define RADIO_FILE				"radio.txt"
#define START_HELP_FILE			"start_help.txt"
#define START_HELP_COLOR		0xFFFFFFAA
//#define REGENERATION_TIME		5
#define TIME_FOR_SAVE			120 //2 mins
#define HELP_TIME				10_000
#define XP_RATE					20
#define MAX_LEVEL				500
// --------------------------------------------------
// enums
// --------------------------------------------------
enum
{
	PLAYER_STAT_CMD,
	PLAYER_HELP_CMD,
	PLAYER_SAVECHAR_CMD,
	PLAYER_CHANGEPASS_CMD,
	PLAYER_KILL_CMD,
	PLAYER_CREDITS_CMD,
	PLAYER_GIVECASH_CMD,
	PLAYER_FLIP_CMD,
	PLAYER_UPGRADE_CMD,
	PLAYER_INFO_CMD,
	PLAYER_PM_CMD,
    PLAYER_ADMINS_CMD,
	PLAYER_SKYDIVE_CMD,
	PLAYER_PLATE_CMD,
	PLAYER_RADIO_CMD,
	PLAYER_SAVECAR_CMD,
	PLAYER_BEER_CMD
};
enum
{
	CHANGE_STAT_ADD,
	CHANGE_STAT_SET
};
enum
{
	SAVE_REASON_SAVECHAR,
	SAVE_REASON_AUTOSAVE,
	SAVE_REASON_DISCONNECT
};
enum
{
	STARTHELP_ACTION_TITLE,
	STARTHELP_ACTION_TEXT,
	STARTHELP_ACTION_INTERIOR,
	STARTHELP_ACTION_CAMERA_POS
};
// --------------------------------------------------
// news
// --------------------------------------------------

	//IsPlayerWelcome[MAX_PLAYERS char];
new col1[MAX_VEHICLES char],col2[MAX_VEHICLES char],vowner[MAX_VEHICLES char];
//new regtick;
new Text: lag;
new save_tick;
#if defined _progress_included
	static 	Bar:XPBar[MAX_PLAYERS],
			Text:lvl[MAX_PLAYERS];
#endif
// --------------------------------------------------
// forwards
// --------------------------------------------------
//forward Autosave();
forward StartHelpNextStep(playerid);
//forward Welcome(playerid,step);
// global
forward OnSavePlayerData(playerid,File:datafile,reason);
forward OnLoadPlayerData(playerid,datastr[],separatorpos);

// --------------------------------------------------
// publics
// --------------------------------------------------
Autosave()
{
	// autosave
	//for(new i = 0;i <= GetPlayersCount();i++)
	foreach(new i : Player)
	{
		//if(!IsPlayerConnected(i)) continue;
		//if(IsPlayerNPC(i)) continue;
		if(GetPVarInt(i,"Logged") != 1) continue;
		
		// save
		SavePlayer(i,SAVE_REASON_AUTOSAVE);
	}
}

public StartHelpNextStep(playerid)
{
	itmp[0] = GetPVarInt(playerid,"StartHelpStep") + 1;
	SetPVarInt(playerid,"StartHelpStep",itmp[0]);
	itmp[0] = ChangeStartHelpStep(playerid,itmp[0]);
	if(itmp[0] != -1) SetTimerEx("StartHelpNextStep",itmp[0],0,"d",playerid);
}

// --------------------------------------------------
// stocks
// --------------------------------------------------
stock SavePlayer(playerid,reason)
{
	if(GetPVarInt(playerid,"Logged") != 1) return;
	
	new name[MAX_PLAYER_NAME];
	GetPVarString(playerid,"Account",name,sizeof(name));
	
	format(stmp,sizeof(stmp),MODE_DIR PLAYERS_DIR "%s" PLAYERS_FILE_TYPE,name);
	
	new File:pfile = fopen(stmp,io_write);
	
	// player core stats
	GetPVarString(playerid,"Password",stmp,sizeof(stmp));
	format(stmp,sizeof(stmp),"Password=%s\r\n",stmp);
	fwrite(pfile,stmp);
	format(stmp,sizeof(stmp),"SeenIntro=%d\r\n",GetPVarInt(playerid,"SeenIntro"));
	fwrite(pfile,stmp);
	format(stmp,sizeof(stmp),"Level=%d\r\n",GetPVarInt(playerid,"Level"));
	fwrite(pfile,stmp);
	format(stmp,sizeof(stmp),"Kills=%d\r\n",GetPVarInt(playerid,"Kills"));
	fwrite(pfile,stmp);
	format(stmp,sizeof(stmp),"Deaths=%d\r\n",GetPVarInt(playerid,"Deaths"));
	fwrite(pfile,stmp);
	format(stmp,sizeof(stmp),"Money=%d\r\n",oGetPlayerMoney(playerid));
	fwrite(pfile,stmp);
	format(stmp,sizeof(stmp),"XP=%d\r\n",GetPVarInt(playerid,"XP"));
	fwrite(pfile,stmp);
	format(stmp,sizeof(stmp),"UpgradesPoints=%d\r\n",GetPVarInt(playerid,"UpgradesPoints"));
	fwrite(pfile,stmp);
	format(stmp,sizeof(stmp),"Regeneration=%.1f\r\n",GetPVarFloat(playerid,"Regeneration"));
	fwrite(pfile,stmp);
	format(stmp,sizeof(stmp),"MaxHealth=%.1f\r\n",GetPVarFloat(playerid,"MaxHealth"));
	fwrite(pfile,stmp);
	format(stmp,sizeof(stmp),"Armor=%.1f\r\n",GetPVarFloat(playerid,"Armor"));
	fwrite(pfile,stmp);
	format(stmp,sizeof(stmp),"Skin=%d\r\n",GetPVarInt(playerid,"Skin"));
	fwrite(pfile,stmp);
	format(stmp,sizeof(stmp),"DriverSkill=%d\r\n",GetPVarInt(playerid,"DriverSkill"));
	fwrite(pfile,stmp);
	format(stmp,sizeof(stmp),"FightStyle=%d\r\n",GetPVarInt(playerid,"FightStyle"));
	fwrite(pfile,stmp);
	format(stmp,sizeof(stmp),"SharpShooting=%d\r\n",GetPVarInt(playerid,"SharpShooting"));
	fwrite(pfile,stmp);
	stmp = "";
	GetPVarString(playerid,"sv_vehdata",stmp,sizeof(stmp));
	format(stmp,sizeof(stmp),"Vdata=%s\r\n",stmp);
	fwrite(pfile,stmp);
	stmp = "";
	GetPVarString(playerid,"sv_vehmod",stmp,sizeof(stmp));
	format(stmp,sizeof(stmp),"Vmod=%s\r\n",stmp);
	fwrite(pfile,stmp);

	
	// other systems
	OnSavePlayerData(playerid,pfile,reason);
	printf("%s[%d] saved successfully!",oGetPlayerName(playerid),playerid);
	
	fclose(pfile);
}

stock LoadPlayer(playerid,pass[])
{
	new name[MAX_PLAYER_NAME];
	GetPVarString(playerid,"Account",name,sizeof(name));
	
	format(stmp,sizeof(stmp),MODE_DIR PLAYERS_DIR "%s" PLAYERS_FILE_TYPE,name);
	
	new File:pfile = fopen(stmp,io_read);
	if(!pfile) return 0;
	
	while(fread(pfile,stmp))
	{
		if(!CheckFileLine(stmp)) continue;
		
		if((itmp[0] = strfind(stmp,"=")) == -1) continue;
		
		// player core stats
		if(!strcmp(stmp,"Password",true,itmp[0]))
		{
			if(!strcmp(stmp[itmp[0] + 1],pass,true))
			{
				SetPVarInt(playerid,"Logged",1);
				SetPVarString(playerid,"Password",pass);
				continue;
			}
			else return 0;
		}
		if(!strcmp(stmp,"SeenIntro",true,itmp[0])) 
		{					
			SetPVarInt(playerid,"SeenIntro",strval(stmp[itmp[0] + 1]));
			continue;
		}
		if(!strcmp(stmp,"Level",true,itmp[0])) 
		{					
			SetPVarInt(playerid,"Level",strval(stmp[itmp[0] + 1]));
			continue;
		}
		if(!strcmp(stmp,"Kills",true,itmp[0])) 
		{
			SetPVarInt(playerid,"Kills",strval(stmp[itmp[0] + 1]));
			continue;
		}
		if(!strcmp(stmp,"Deaths",true,itmp[0])) 
		{
			SetPVarInt(playerid,"Deaths",strval(stmp[itmp[0] + 1]));
			continue;
		}
		if(!strcmp(stmp,"Money",true,itmp[0])) 
		{
			oSetPlayerMoney(playerid,strval(stmp[itmp[0] + 1]));
			continue;
		}
		if(!strcmp(stmp,"XP",true,itmp[0])) 
		{
			SetPVarInt(playerid,"XP",strval(stmp[itmp[0] + 1]));
			continue;
		}
		if(!strcmp(stmp,"UpgradesPoints",true,itmp[0])) 
		{
			SetPVarInt(playerid,"UpgradesPoints",strval(stmp[itmp[0] + 1]));
			continue;
		}
		if(!strcmp(stmp,"Regeneration",true,itmp[0]))
		{
			SetPVarFloat(playerid,"Regeneration",floatstr(stmp[itmp[0] + 1]));
			continue;
		}
		if(!strcmp(stmp,"MaxHealth",true,itmp[0])) 
		{
			SetPVarFloat(playerid,"MaxHealth",floatstr(stmp[itmp[0] + 1]));
			continue;
		}
        if(!strcmp(stmp,"Armor",true,itmp[0])) 
		{
			SetPVarFloat(playerid,"Armor",floatstr(stmp[itmp[0] + 1]));
			continue;
		}
		if(!strcmp(stmp,"Skin",true,itmp[0]))
		{
			SetPVarInt(playerid,"Skin",strval(stmp[itmp[0] + 1]));
			continue;
		}
		if(!strcmp(stmp,"DriverSkill",true,itmp[0]))
		{
			SetPVarInt(playerid,"DriverSkill",strval(stmp[itmp[0] + 1]));
			continue;
		}
		if(!strcmp(stmp,"FightStyle",true,itmp[0]))
		{
			SetPVarInt(playerid,"FightStyle",strval(stmp[itmp[0] + 1]));
			continue;
		}
		if(!strcmp(stmp,"SharpShooting",true,itmp[0]))
		{
			SetPVarInt(playerid,"SharpShooting",strval(stmp[itmp[0] + 1]));
			continue;
		}
		if(!strcmp(stmp,"Vdata",true,itmp[0])) 
		{
		SetPVarString(playerid,"sv_vehdata",stmp[itmp[0] + 1]);
		new Float:fmcx,Float:fmcy,Float:fmcz,Float:fan,model,k=0,clr1,clr2,pjob;//бредовый код, поправить по возможности
		new plat[20];
		///GetPVarString(playerid,"sv_vehdata",dat,sizeof(dat));
		fmcx = floatstr(strtok(stmp,k,','));
		fmcy = floatstr(strtok(stmp,k,','));
		fmcz = floatstr(strtok(stmp,k,','));
		fan = floatstr(strtok(stmp,k,','));
		model = strval(strtok(stmp,k,','));
		clr1 = strval(strtok(stmp,k,','));
		clr2 = strval(strtok(stmp,k,','));
		pjob = strval(strtok(stmp,k,','));
		format(plat,sizeof(plat),"%s",strtok(stmp,k,','));
		SetPVarInt(playerid,"vid",-1);
		if(IsValidVehicleModel(model))
		{
			// create vehicle	
		SetPVarInt(playerid,"vid",CreateVehicle(model,fmcx,fmcy,fmcz,fan,clr1,clr2,-1));
		new vid=GetPVarInt(playerid,"vid");
		vowner{vid}=playerid;
		col1{vid}=clr1;
		col2{vid}=clr2;
		SetVehiclePaintJob(vid,pjob);
		SetVehicleNumberPlate(GetPVarInt(playerid,"vid"),plat);
		//SendClientMessageToAll(COLOR_WHITE,"СОЗДАНО");
		}
		continue;
		}
		if(!strcmp(stmp,"Vmod",true,itmp[0])) 
		{
		SetPVarString(playerid,"sv_vehmod",stmp[itmp[0] + 1]);
		for(new i=0;i<14;i++)
			{
			AddVehicleComponent(GetPVarInt(playerid,"vid"),strval(strtok(stmp,itmp[0],',')));
			
			}
		continue;
		}
		// other systems
		#if defined _progress_included
		XPBar[playerid]=CreateProgressBar(43.0,435.0,540.0,_,COLOR_ORANGE,CalculateXPForLevel(GetPVarInt(playerid,"Level")));
		lvl[playerid]=TextDrawCreate(320,435,"0");
		TextDrawFont(lvl[playerid],2);
		TextDrawLetterSize(lvl[playerid],0.5,1.5);
		TextDrawSetOutline(lvl[playerid],1);
		TextDrawAlignment(lvl[playerid],2);
		TextDrawColor(lvl[playerid],COLOR_SPRINGGREEN);
		#endif
		OnLoadPlayerData(playerid,stmp,itmp[0]);
	}
	
	fclose(pfile);
	return 1;
}
CreateSavedVehicle(playerid)
{
new Float:fmcx,Float:fmcy,Float:fmcz,Float:fan,model,k=0,clr1,clr2,pjob;
new dat[150],mod[100],plat[20];
GetPVarString(playerid,"sv_vehdata",dat,sizeof(dat));
fmcx = floatstr(strtok(dat,k,','));
fmcy = floatstr(strtok(dat,k,','));
fmcz = floatstr(strtok(dat,k,','));
fan = floatstr(strtok(dat,k,','));
model = strval(strtok(dat,k,','));
clr1 = strval(strtok(dat,k,','));
clr2 = strval(strtok(dat,k,','));
pjob = strval(strtok(dat,k,','));
format(plat,sizeof(plat),"%s",strtok(dat,k,','));
//SetPVarInt(playerid,"vid",-1);
DestroyVehicle(GetPVarInt(playerid,"vid"));
if(IsValidVehicleModel(model))
{
	// create vehicle	
SetPVarInt(playerid,"vid",CreateVehicle(model,fmcx,fmcy,fmcz,fan,clr1,clr2,-1));

new vid=GetPVarInt(playerid,"vid");
SetVehiclePaintJob(vid,pjob);
GetPVarString(playerid,"sv_vehmod",mod,sizeof(mod));
vowner{vid}=playerid;
col1{vid}=clr1;
col2{vid}=clr2;
SetVehicleNumberPlate(vid,plat);
for(new i=0;i<14;i++)
			{
			AddVehicleComponent(vid,strval(strtok(mod,itmp[0],',')));
			}
//SetVehicleToRespawn(vid);
//OnSavedCarResp(vid);
//SendClientMessageToAll(COLOR_WHITE,"СОЗДАНО");
}
return 1;
}
OnSavedCarResp(vehicleid)
{
SetVehiclePaintJob(vehicleid,GetVehiclePaintJob(vehicleid));
GetPVarString(vowner{vehicleid},"sv_vehmod",stmp,sizeof(stmp));
for(new i=0;i<14;i++)
			{
			AddVehicleComponent(vehicleid,strval(strtok(stmp,itmp[0],',')));
			}
return 1;
}

stock ChangeStatByNameInt(name[],stat[],data,type)
{
	static 
		ntmp[MAX_PLAYER_NAME],
		pid;
	pid = INVALID_PLAYER_ID;
	//for(new i = 0;i <= GetPlayersCount();i++)
	foreach(new i : Player)
	{
		GetPlayerName(i,ntmp,sizeof(ntmp));
		if(!strcmp(ntmp,name))
		{
			pid = i;
			break;
		}
	}
	if(pid != INVALID_PLAYER_ID)
	{
		switch(type)
		{
			case CHANGE_STAT_ADD:
			{
				SetPVarInt(pid,stat,GetPVarInt(pid,stat) + data);
				if(!strcmp(stat,"Money")) oGivePlayerMoney(pid,data);
				else if(!strcmp(stat,"XP"))
				{
					GivePlayerXP(pid,data);
				}
			}
			case CHANGE_STAT_SET: 
			{
				SetPVarInt(pid,stat,data);
				if(!strcmp(stat,"Money")) 
				{
					oSetPlayerMoney(pid,data);
				}
				else if(!strcmp(stat,"XP"))
				{
					SetPlayerXP(pid,data);
				}
			}
		}		
	}
	else
	{
		format(stmp,sizeof(stmp),MODE_DIR PLAYERS_DIR "%s" PLAYERS_FILE_TYPE,name);
		if(!fexist(stmp))
		{
		print("DEBUG:load unsuccesful! skipping...");
		return;
		}
	
		new File:pfile = fopen(stmp,io_read);
		new File:tfile = fopen("temp",io_write);
		
		while(fread(pfile,stmp))
		{
			if(strfind(stmp,stat) == -1) fwrite(tfile,stmp);
			else 
			{
				itmp[0] = strfind(stmp,"=");
				new ndata = strval(stmp[itmp[0] + 1]);
				switch(type)
				{
					case CHANGE_STAT_ADD: ndata += data;
					case CHANGE_STAT_SET: ndata = data;
				}				
				format(stmp,sizeof(stmp),"%s=%d\r\n",stat,ndata);
				fwrite(tfile,stmp);
			}
		}
		fclose(pfile);
		fclose(tfile);
		// rename
		format(stmp,sizeof(stmp),MODE_DIR PLAYERS_DIR "%s" PLAYERS_FILE_TYPE,name);
		tfile = fopen(stmp,io_write);
		pfile = fopen("temp",io_read);
		while(fread(pfile,stmp)) fwrite(tfile,stmp);
		fclose(pfile);
		fclose(tfile);
		// delete temp
		fremove("temp");
		print("DEBUG:load succesful!");
	}
}

stock SetPlayerLevel(playerid,level_set) // Устанавливает игроку уровень
{
	if(level_set > MAX_LEVEL) return 0;
	SetPVarInt(playerid,"XP",0);
	SetPVarInt(playerid,"Level",level_set);
	SetPlayerScore(playerid,GetPVarInt(playerid,"Level"));
	return 1;
}

stock GivePlayerLevel(playerid,level_give) // Даёт игроку уровень
{
	//printf("level give is %d",level_give);
	if(GetPVarInt(playerid,"Level") >= MAX_LEVEL) return 0;
	if(GetPVarInt(playerid,"Level") + level_give > MAX_LEVEL)
	{
		level_give = MAX_LEVEL - GetPVarInt(playerid,"Level");
		SendClientMessage(playerid,COLOR_BLUE,"Вы достигли максимального уровня!");
	}
	SetPVarInt(playerid,"XP",0);
	SetPVarInt(playerid,"Level",GetPVarInt(playerid,"Level")+level_give);
	SetPlayerScore(playerid,GetPVarInt(playerid,"Level"));
	OnPlayerLevelChange(playerid,GetPVarInt(playerid,"Level")-level_give,GetPVarInt(playerid,"Level"));
	return 1;
}

stock SetPlayerXP(playerid,xp_set) // Устанавливает опыт игроку
{
	new xp_all = (xp_set+GetPVarInt(playerid,"XP")); // помещаем весь опыт в переменную
	new xp_tolvl = PlayerXPToLevel(playerid); // помещаем кол-во опыта до уровня в переменную
	if(xp_all == xp_tolvl)
	{
		SetPVarInt(playerid,"XP",0);
		GivePlayerLevel(playerid,1);
	}
	else if(xp_all > xp_tolvl)
	{
		GivePlayerLevel(playerid,1);
		SetPlayerXP(playerid, (xp_set - xp_tolvl) ); // устанавливаем нужный опыт
	}
	else if(xp_all < 0)
	{
		new xp_temp = GetPVarInt(playerid,"XP"); // старый опыт игрока
		GivePlayerLevel(playerid,-1);
		SetPVarInt(playerid,"XP", PlayerXPToLevel(playerid) + (xp_temp + xp_set) ); // вычисляем кол-во опыта для уровня пониженного уровня
	}
	else if(xp_all < xp_tolvl)
	{
		SetPVarInt(playerid,"XP",xp_set);
	}
}

stock GivePlayerXP(playerid,xp_give)
{
	if(xp_give == 0) return 1;
	if(GetPVarInt(playerid,"Level") >= MAX_LEVEL) return 1;
	//new xp_all = (xp_give+GetPVarInt(playerid,"XP")); // помещаем весь опыт в переменную
	new xp_tolvl = PlayerXPToLevel(playerid); // помещаем кол-во опыта до уровня в переменную
	if(xp_give == xp_tolvl)
	{
		SetPVarInt(playerid,"XP",0);
		GivePlayerLevel(playerid,1);
	}
	else if(xp_give > xp_tolvl)
	{
		GivePlayerLevel(playerid,1);
		SetPlayerXP(playerid, (xp_give - xp_tolvl) ); // устанавливаем нужный опыт
	}
	else if(xp_give < 0)
	{
		new xp_temp = GetPVarInt(playerid,"XP"); // старый опыт игрока
		GivePlayerLevel(playerid,-1);
		SetPVarInt(playerid,"XP", PlayerXPToLevel(playerid) + (xp_temp + xp_give) ); // вычисляем кол-во опыта для пониженного уровня
	}
	else if(xp_give < xp_tolvl)
	{
		SetPVarInt(playerid,"XP",GetPVarInt(playerid,"XP")+xp_give);
	}
	#if defined _progress_included
	SetProgressBarValue(XPBar[playerid],GetPVarInt(playerid,"XP") );
	UpdateProgressBar(XPBar[playerid],playerid);
	#endif
	return 1;
}

stock OnPlayerLevelChange(playerid,old_level,new_level)
{
	if(new_level >= MAX_LEVEL) SendClientMessage(playerid,COLOR_BLUE,"УРА! Вы достигли максимального уровня!");
	SetPVarInt(playerid,"UpgradesPoints",GetPVarInt(playerid,"UpgradesPoints") + (new_level-old_level));
	if(new_level%5==0)
		{
			format(stmp,sizeof(stmp),"Вы достигли %d уровня, у вас есть %d неиспользованных очков прокачки, распределите их в личном меню или командой /upgrade",new_level,GetPVarInt(playerid,"UpgradesPoints"));
			SendClientMessage(playerid,COLOR_SPRINGGREEN,stmp);
		}
	#if defined _progress_included
	SetProgressBarMaxValue(XPBar[playerid], CalculateXPForLevel(new_level));
	UpdateProgressBar(XPBar[playerid],playerid);
	format(stmp,sizeof(stmp),"%d",GetPVarInt(playerid,"Level"));
	TextDrawSetString(lvl[playerid],stmp);
	#endif
	return 1;
}

stock PlayerXPToLevel(playerid)// Узнаёт сколько осталось игроку набрать опыта до следующего уровня
{
	return (CalculateXPForLevel(GetPVarInt(playerid,"Level")) - GetPVarInt(playerid,"XP"));
}

stock CalculateXPForLevel(level)
{
	return floatround((level * level) * XP_RATE);
}
/*
stock CalculateLevelForXP(xp)
{
	return floatround(floatsqroot((xp / XP_RATE)));
}*/

stock ChangeStartHelpStep(playerid,step)
{
	static line[256];
	new File:datafile = fopen(MODE_DIR START_HELP_FILE,io_read);
	if(datafile)
	{
		// find line by step
		for(new i = 0;i <= step;i++) 
		{
			if(!fread(datafile,line))
			{
				// file end - complete help show
				fclose(datafile);
				TogglePlayerSpectating(playerid,false);
				return -1;
			}
			else if(!CheckFileLine(line)) i--;
		}
		// check
		if(!CheckFileLine(line)) return -1;
		// parse
		static idx;
		idx = 0;
		// time of action 
		itmp[0] = strval(strtok(line,idx,','));
		// type of action
		itmp[1] = strval(strtok(line,idx,','));
		// next by type
		switch(itmp[1])
		{
			case STARTHELP_ACTION_TITLE:
			{
				GameTextForPlayer(playerid,line[idx],5000,1);
			}
			case STARTHELP_ACTION_TEXT: 
			{
				SendClientMessage(playerid,START_HELP_COLOR,line[idx]);
			}
			case STARTHELP_ACTION_INTERIOR:
			{
				itmp[2] = strval(strtok(line,idx,','));
				SetPlayerInterior(playerid,itmp[2]);
			}
			case STARTHELP_ACTION_CAMERA_POS:
			{
				ftmp[0] = floatstr(strtok(line,idx,','));
				ftmp[1] = floatstr(strtok(line,idx,','));
				ftmp[2] = floatstr(strtok(line,idx,','));
				ftmp[3] = floatstr(strtok(line,idx,','));
				ftmp[4] = floatstr(strtok(line,idx,','));
				ftmp[5] = floatstr(strtok(line,idx,','));
				SetPlayerCameraPos(playerid,ftmp[0],ftmp[1],ftmp[2]);
				SetPlayerCameraLookAt(playerid,ftmp[3],ftmp[4],ftmp[5]);
			}
		}		
		// file end
		fclose(datafile);		
		return itmp[0];
	}
	else 
	{
		printf("[ERROR] start help file not found (%s)",MODE_DIR START_HELP_FILE);
		return -1;
	}
}

// --------------------------------------------------
// Obligatory functions
// --------------------------------------------------
Player_OnGameModeInit()
{
	// WOW
	lag = TextDrawCreate(605.0,10.0,"TPS:000");
	TextDrawUseBox(lag, 0);
	TextDrawFont(lag, 1);
	TextDrawSetShadow(lag,0); // no shadow
    TextDrawSetOutline(lag,2); // thickness 1
    TextDrawBackgroundColor(lag,0x000000FF);
    TextDrawColor(lag,0xFFFFFFFF);
    TextDrawAlignment(lag,3);
	TextDrawLetterSize(lag,0.5,1.5);
	
	
	//WOW
	//SetTimer("Autosave",TIME_FOR_SAVE,1);
	print("Players manager loaded.");
}

Player_OnVehicleRespray(playerid, vehicleid, color1, color2)
{
	#pragma unused playerid
	col1{vehicleid}=color1;
	col2{vehicleid}=color2;
}
Player_OnVehicleSpawn(vehicleid)
{
//for(new i=0;i<=GetPlayersCount();i++)
foreach(new i : Player)
	{
		if(GetPVarInt(i,"vid")==vehicleid) return OnSavedCarResp(vehicleid);
	}
return 1;
}
Player_OnPlayerConnect(playerid)
{
    if(IsPlayerNPC(playerid)) return 1;
	//if(IsValidNPC(playerid)) return 1;
	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid,name,sizeof(name));
	if(!strcmp("Unknown",name)) 
	{
		SendClientMessage(playerid,SYSTEM_COLOR,"Использование ника Unknown недопустимо");
		return Kick(playerid);
	}
	TogglePlayerSpectating(playerid,true);//set to true will make it right, maybe...;)
	
	SetPVarInt(playerid,"Skin",-1);
	SetPVarString(playerid,"Account",name);
	format(stmp,sizeof(stmp),MODE_DIR PLAYERS_DIR "%s" PLAYERS_FILE_TYPE,name);
	if(fexist(stmp))
	{
		format(stmp,sizeof(stmp),"{FF9900}Wars of the Gangs %s{FFFFFF}\nС возвращением, %s!\nВведите свой пароль чтобы приступить к игре",MODE_VERSION,name);
		ShowPlayerDialog(playerid,DIALOG_LOGIN,DIALOG_STYLE_PASSWORD,MODE_NAME,stmp,"Готово","Выход");
	}
	else
	{
		format(stmp,sizeof(stmp),"{FF9900}Wars of the Gangs %s{FFFFFF}\nЗдравствуйте, %s!\nВведите свой пароль чтобы зарегистрироваться",MODE_VERSION,name);
		ShowPlayerDialog(playerid,DIALOG_REGISTER,DIALOG_STYLE_PASSWORD,MODE_NAME,stmp,"Готово","Выход");
        SetPVarFloat(playerid,"MaxHealth",100.0);		
	}
	
	format(stmp,sizeof(stmp),".:: Игрок %s подключился к серверу ::.",oGetPlayerName(playerid));
	#if defined _geolocation_included
		new country[24],city[16], ISP[16];
		GetPlayerCountry(playerid,country);
		GetPlayerCity(playerid,city);
		GetPlayerISP(playerid, ISP);
		format(stmp,sizeof(stmp),"%s \n(%s, %s, %s)",stmp,country,city,ISP);
		print(stmp);
	#endif
	SendClientMessageToAll(COLOR_YELLOW,stmp);
	PlayAudioStreamForPlayer(playerid,"https://dl.dropboxusercontent.com/u/45522772/SAMP/new_snd_login.mp3");
	for(new i=0;i<20;i++)
		{
		SendClientMessage(playerid,COLOR_WHITE," ");
		}
	
	//PlayCameraMover(playerid,LoadCameraMover("login1"));
	return 1;
}

// public	Welcome(playerid,step)
// {
// //IsPlayerWelcome{playerid}=1;
// //TogglePlayerSpectating(playerid, 0);
// //PlayCameraMover(playerid,LoadCameraMover("login1"));
// new welcometext[80];
// // new Float:x,Float:y,Float:z;
// switch(step)
// {
	// case 1:
		// {
		// //TogglePlayerSpectating(playerid, 1);
		// //PlayAudioStreamForPlayer(playerid,"http://dl.dropbox.com/u/45522772/Georgethorogood_-_Bad_To_The_Bone.mp3");
		// for(new i=0;i<20;i++)
		// {
		// SendClientMessage(playerid,COLOR_WHITE," ");
		// }
		// //PlayerSpectateVehicle(playerid, GetPlayerVehicleID(4),3);
		// format(welcometext,sizeof(welcometext),"Welcome to...");
		// GameTextForPlayer(playerid,welcometext,9000,6);
		// SetTimerEx("Welcome",9000,false,"dd",playerid,2);
		// return 1;
		// }
	// case 2:
		// {
		// format(welcometext,sizeof(welcometext),"~y~Wars of the Gangs");
		// //PlayerSpectateVehicle(playerid, GetPlayerVehicleID(4),2);
		// GameTextForPlayer(playerid,welcometext,3000,4);
		// SetTimerEx("Welcome",12000,false,"dd",playerid,3);
		// return 1;
		// }
	// case 3:
		// {
		// format(welcometext,sizeof(welcometext),"Created by...");
		// GameTextForPlayer(playerid,welcometext,3000,6);
		// SetTimerEx("Welcome",8000,false,"dd",playerid,4);
		// return 1;
		// }
	// case 4:
		// {
		// //GetPlayerPos(1,x,y,z);
		// //SetPlayerCameraPos(playerid,x+20,y+20,z);
		// //PlayerSpectateVehicle(playerid, GetPlayerVehicleID(1),2);
		// format(welcometext,sizeof(welcometext),"~r~009 & LukeS");
		// GameTextForPlayer(playerid,welcometext,3000,3);
		// SetTimerEx("Welcome",5000,false,"dd",playerid,5);
		// return 1;
		// }
	// case 5:
		// {
		// //GetPlayerPos(2,x,y,z);
		// //SetPlayerCameraPos(playerid,x,y,z);
		// //PlayerSpectateVehicle(playerid, GetPlayerVehicleID(2),3);
		// //format(welcometext,sizeof(welcometext),"~g~~h~Let the journey begins...");
		// //GameTextForPlayer(playerid,welcometext,3000,6);
		// SetTimerEx("Welcome",10000,false,"dd",playerid,6);
		
		
		// return 1;
		// }
	// case 6:
		// {
		// //TogglePlayerSpectating(playerid, 0);
		// return 1;
		
		// }


// }


// return 1;

// }
camera_handler(playerid,node)
{
switch(node)
	{
		case 1:
			{
			format(stmp,sizeof(stmp),"Welcome to~n~~y~Wars of the Gangs %s",MODE_VERSION);
			GameTextForPlayer(playerid,stmp,35000,6);
			}
		case 3:
			{
			format(stmp,sizeof(stmp),"Created by~n~~y~009");
			GameTextForPlayer(playerid,stmp,5000,1);
			}
	
	}
return 1;
}
Player_OnPlayerRequestClass(playerid,classid)
{
#pragma unused classid
new clid=GetPVarInt(playerid,"Skin");
if(clid!=-1)
{
SetPlayerSkin(playerid,clid);
//TogglePlayerSpectating(playerid,false);
//SpawnPlayer(playerid);
}
return 1;
}
Player_ResetSkin(playerid)
{
SetPVarInt(playerid,"Skin",-1);
return 1;
}
Player_OnPlayerDisconnect(playerid,reason)
{
	#pragma unused reason
	new vid=GetPVarInt(playerid,"vid");
	if(vid!=-1) DestroyVehicle(vid);
	SavePlayer(playerid,SAVE_REASON_DISCONNECT);
	format(stmp,sizeof(stmp),".:: Игрок %s отключился от сервера ::.",oGetPlayerName(playerid));
	SendClientMessageToAll(COLOR_YELLOW,stmp);
}

Player_OnPlayerDeath(playerid,killerid,reason)
{
	#pragma unused reason
	#pragma unused reason
	SetPVarInt(playerid,"Deaths",GetPVarInt(playerid,"Deaths") + 1);
	if(killerid != INVALID_PLAYER_ID)
	{
    new xpgive=(1+GetPVarInt(playerid,"Level"))*10*(GetPVarInt(killerid,"Level")/10+1);
	format(stmp, sizeof(stmp), "Вы убили игрока %s и получили %d опыта" , oGetPlayerName(playerid),xpgive);
	SendClientMessage(killerid,COLOR_RED,stmp);
    GivePlayerXP(killerid,xpgive);	
	SetPVarInt(killerid,"Kills",GetPVarInt(killerid,"Kills") + 1);
	}
	return 1;
}

Player_OnPlayerSpawn(playerid)
{
	// check quest 
	#if defined _progress_included
		SetProgressBarValue(XPBar[playerid], GetPVarInt(playerid,"XP"));
		UpdateProgressBar(XPBar[playerid],playerid);
		ShowProgressBarForPlayer(playerid, XPBar[playerid]);
		format(stmp,sizeof(stmp),"%d",GetPVarInt(playerid,"Level"));
		TextDrawSetString(lvl[playerid],stmp);
		TextDrawShowForPlayer(playerid,lvl[playerid]);
	#endif
	if(GetPVarInt(playerid,"SeenIntro")==0)
		{
		//TogglePlayerSpectating(playerid, 1);
		//SetPlayerVirtualWorld(playerid,5310);
		PlayCameraMover(playerid,LoadCameraMover("login1"));
		
		//Welcome(playerid,0);
		}
	if(GetPlayerQuest(playerid) == INVALID_QUEST_ID)
	{
		//SetPVarInt(playerid,"Skin",GetPlayerSkin(playerid));
		SetPlayerSkin(playerid, GetPVarInt(playerid, "Skin"));
		oSetPlayerHealth(playerid,100.0);
		SetPlayerArmour(playerid,GetPVarFloat(playerid,"Armor"));
		SetPlayerScore(playerid,GetPVarInt(playerid,"Level"));
		for(new i = 0;i <= 11;i++)
		{
			format(stmp,sizeof(stmp),"SlotWeapon_%d",i);
			itmp[0] = GetPVarInt(playerid,stmp);
			format(stmp,sizeof(stmp),"SlotAmmo_%d",i);
			itmp[1] = GetPVarInt(playerid,stmp);
			oGivePlayerWeapon(playerid,itmp[0],itmp[1]);
		}
		SetPlayerColor(playerid,PLAYERS_COLOR);
	}
	for(new i = 0;i <= 10;i++)
	{
		format(stmp,sizeof(stmp),"Skill_%d",i);
		SetPlayerSkillLevel(playerid,i,GetPVarInt(playerid,stmp));
	}
	// preloading animations
	ApplyAnimation(playerid,"AIRPORT","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"Attractors","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"BAR","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"BASEBALL","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"BD_FIRE","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"benchpress","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"BF_injection","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"BIKED","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"BIKEH","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"BIKELEAP","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"BIKES","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"BIKEV","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"BIKE_DBZ","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"BLOWJOBZ","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"BMX","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"BOMBER","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"BOX","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"BSKTBALL","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"BUDDY","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"BUS","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"CAMERA","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"CAR","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"CARRY","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"CAR_CHAT","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"CASINO","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"CHAINSAW","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"CHOPPA","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"CLOTHES","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"COACH","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"COLT45","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"COP_AMBIENT","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"COP_DVBYZ","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"CRACK","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"CRIB","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"DAM_JUMP","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"DANCING","null",0.0,0,0,0,0,0);
	ApplyAnimation(playerid,"DEALER","null",0.0,0,0,0,0,0);
}
Player_UpdateLag()
{
format(stmp,sizeof(stmp),"TPS:%d",GetServerTickRate());
TextDrawSetString(lag,stmp);

}
Player_OnPlayerUpdate(playerid)
{
  #pragma unused playerid
  return 1;
}
Player_Update() /// one per 5 seconds!!!!!!
{
	//format(stmp,sizeof(stmp),"%d",GetPlayerVirtualWorld(2));
	//printf("TICK:%d",regtick);
	save_tick+=5;
	//printf("%d",save_tick);
	if(save_tick==TIME_FOR_SAVE)
	{
		save_tick=0;
		Autosave();
		
	
	}
	
	//for(new i = 0;i <= GetPlayersCount();i++)
	foreach(new i : Player)
	{
	//	printf("START ID IS %d",i);
		//if(!IsPlayerConnected(i)) continue;
		if(IsPlayerNPC(i)) continue;
		if(GetPlayerState(i) == PLAYER_STATE_NONE)  continue;
		if(GetPlayerState(i) == PLAYER_STATE_WASTED) continue;
		if(GetPlayerState(i) == PLAYER_STATE_SPECTATING) continue;
		GetPlayerVelocity(i,ftmp[0],ftmp[1],ftmp[2]);
		if(ftmp[0]+ftmp[1]+ftmp[2]!=0) continue;
		ftmp[0] = oGetPlayerHealth(i);
		if(ftmp[0] >= GetPVarFloat(i,"MaxHealth")) continue;
		ftmp[0] += GetPVarFloat(i,"Regeneration");
		if(ftmp[0] > GetPVarFloat(i,"MaxHealth")) ftmp[0] = GetPVarFloat(i,"MaxHealth");
		//printf("ftmp is %f ",ftmp[0]);
		oSetPlayerHealth(i,ftmp[0]);
		//SetPlayerHealth(i,ftmp[0]);

		//print("SET");
	}

	//return 1;
}


Player_Dialog(playerid,dialogid,response,listitem,inputtext[])
{
	#pragma unused listitem
	
	switch(dialogid)
	{
		case DIALOG_LOGIN:
		{
			if(!response) return ShowPlayerDialog(playerid,DIALOG_LOGIN,DIALOG_STYLE_PASSWORD,MODE_NAME,"Введите свой пароль чтобы приступить к игре","Готово","Выход");
			if(!strlen(inputtext)) return ShowPlayerDialog(playerid,DIALOG_LOGIN,DIALOG_STYLE_PASSWORD,MODE_NAME,"Введите свой пароль чтобы приступить к игре","Готово","Выход");
			// load
			if(!LoadPlayer(playerid,inputtext)) return ShowPlayerDialog(playerid,DIALOG_LOGIN,DIALOG_STYLE_PASSWORD,MODE_NAME,"Введите свой пароль чтобы приступить к игре","Готово","Выход");
			// selecting
			TogglePlayerSpectating(playerid,false);
			return 1;
		}
		case DIALOG_REGISTER:
		{
			if(!response) return ShowPlayerDialog(playerid,DIALOG_REGISTER,DIALOG_STYLE_PASSWORD,MODE_NAME,"Введите свой пароль чтобы зарегистрироваться","Готово","Выход");
			if(!strlen(inputtext)) return ShowPlayerDialog(playerid,DIALOG_REGISTER,DIALOG_STYLE_PASSWORD,MODE_NAME,"Введите свой пароль чтобы зарегистрироваться","Готово","Выход");

			// pvar
			SetPVarInt(playerid,"Logged",1);
			SetPVarString(playerid,"Password",inputtext);
			SetPVarInt(playerid,"Level",0);
			SetPVarInt(playerid,"Kills",0);
			SetPVarInt(playerid,"Deaths",0);
			oSetPlayerMoney(playerid,0);
			SetPVarInt(playerid,"BankMoney",0);
			SetPVarInt(playerid,"XP",0);
			SetPVarInt(playerid,"UpgradesPoints",0);
			SetPVarFloat(playerid,"Regeneration",0.0);
			SetPVarFloat(playerid,"MaxHealth",100.0);
			SetPVarInt(playerid,"SeenIntro",1);
			for(new i = 0;i <= 10;i++)
			{
				format(stmp,sizeof(stmp),"Skill_%d",i);
				SetPVarInt(playerid,stmp,0);
				SetPlayerSkillLevel(playerid,i,0);
			}
			SavePlayer(playerid,SAVE_REASON_AUTOSAVE);
			// start show help
			SetPVarInt(playerid,"StartHelpStep",0);
			itmp[0] = ChangeStartHelpStep(playerid,0);
			//if(itmp[0] != -1) SetTimerEx("StartHelpNextStep",itmp[0],0,"d",playerid);
			
			//IsPlayerWelcome{playerid}=1;
			return 1;
		}
		case DIALOG_UPGRADE:
		{
			if(!response) return 1;
			
			if((itmp[0] = GetPVarInt(playerid,"UpgradesPoints")) < 1)
			return
			ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"Меню апгрейдов","У вас нету upgrade point'ов!","Ок","");
			switch(listitem)
			{
				case 0: // health
				{
					if((ftmp[0] = GetPVarFloat(playerid,"MaxHealth")) < 100.0)
					{
						//SetPVarFloat(playerid,"MaxHealth",ftmp[0] + 5.0);
						//SetPVarInt(playerid,"UpgradesPoints",itmp[0] - 1);
					}
				}
				case 1: // regeneration
				{
					if(GetPVarInt(playerid,"UpgradesPoints")<2) return SendClientMessage(playerid, COLOR_LIGHTRED,"Требуется 2 очка прокачки на этот навык!");
					if((ftmp[0] = GetPVarFloat(playerid,"Regeneration")) < 25.0)
					{
						SetPVarFloat(playerid,"Regeneration",ftmp[0] + 5.0);
						SetPVarInt(playerid,"UpgradesPoints",itmp[0] - 2);
					}
				}
				case 2: // skill
				{
					if((itmp[1] = GetPVarInt(playerid,"Skill_" #WEAPONSKILL_PISTOL)) < 999)
					{
						itmp[1] += 100;
						SetPVarInt(playerid,"Skill_" #WEAPONSKILL_PISTOL,itmp[1]);
						SetPlayerSkillLevel(playerid,WEAPONSKILL_PISTOL,itmp[1]);
						SetPVarInt(playerid,"UpgradesPoints",itmp[0] - 1);
					}
				}
				case 3: // skill
				{
					if(GetPVarInt(playerid,"UpgradesPoints")<2) return SendClientMessage(playerid, COLOR_LIGHTRED,"Требуется 2 очка прокачки на этот навык!");
					if((itmp[1] = GetPVarInt(playerid,"Skill_" #WEAPONSKILL_PISTOL_SILENCED)) < 999)
					{
						itmp[1] += 100;
						SetPVarInt(playerid,"Skill_" #WEAPONSKILL_PISTOL_SILENCED,itmp[1]);
						SetPlayerSkillLevel(playerid,WEAPONSKILL_PISTOL_SILENCED,itmp[1]);
						SetPVarInt(playerid,"UpgradesPoints",itmp[0] - 2);
					}
				}
				case 4: // skill
				{
					if(GetPVarInt(playerid,"UpgradesPoints")<2) return SendClientMessage(playerid, COLOR_LIGHTRED,"Требуется 2 очка прокачки на этот навык!");
					if((itmp[1] = GetPVarInt(playerid,"Skill_" #WEAPONSKILL_DESERT_EAGLE)) < 999)
					{
						itmp[1] += 100;
						SetPVarInt(playerid,"Skill_" #WEAPONSKILL_DESERT_EAGLE,itmp[1]);
						SetPlayerSkillLevel(playerid,WEAPONSKILL_DESERT_EAGLE,itmp[1]);
						SetPVarInt(playerid,"UpgradesPoints",itmp[0] - 2);
					}
				}
				case 5: // skill
				{
					if(GetPVarInt(playerid,"UpgradesPoints")<4) return SendClientMessage(playerid, COLOR_LIGHTRED,"Требуется 4 очка прокачки на этот навык!");
					if((itmp[1] = GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SHOTGUN)) < 999)
					{
						itmp[1] += 100;
						SetPVarInt(playerid,"Skill_" #WEAPONSKILL_SHOTGUN,itmp[1]);
						SetPlayerSkillLevel(playerid,WEAPONSKILL_SHOTGUN,itmp[1]);
						SetPVarInt(playerid,"UpgradesPoints",itmp[0] - 4);
					}
				}
				case 6: // skill
				{
					if(GetPVarInt(playerid,"UpgradesPoints")<5) return SendClientMessage(playerid, COLOR_LIGHTRED,"Требуется 5 очков прокачки на этот навык!");
					if((itmp[1] = GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SAWNOFF_SHOTGUN)) < 999)
					{
						itmp[1] += 100;
						SetPVarInt(playerid,"Skill_" #WEAPONSKILL_SAWNOFF_SHOTGUN,itmp[1]);
						SetPlayerSkillLevel(playerid,WEAPONSKILL_SAWNOFF_SHOTGUN,itmp[1]);
						SetPVarInt(playerid,"UpgradesPoints",itmp[0] - 5);
					}
				}
				case 7: // skill
				{
					if(GetPVarInt(playerid,"UpgradesPoints")<5) return SendClientMessage(playerid, COLOR_LIGHTRED,"Требуется 5 очков прокачки на этот навык!");
					if((itmp[1] = GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SPAS12_SHOTGUN)) < 999)
					{
						itmp[1] += 100;
						SetPVarInt(playerid,"Skill_" #WEAPONSKILL_SPAS12_SHOTGUN,itmp[1]);
						SetPlayerSkillLevel(playerid,WEAPONSKILL_SPAS12_SHOTGUN,itmp[1]);
						SetPVarInt(playerid,"UpgradesPoints",itmp[0] - 5);
					}
				}
				case 8: // skill
				{
					if(GetPVarInt(playerid,"UpgradesPoints")<3) return SendClientMessage(playerid, COLOR_LIGHTRED,"Требуется 3 очка прокачки на этот навык!");
					if((itmp[1] = GetPVarInt(playerid,"Skill_" #WEAPONSKILL_MICRO_UZI)) < 999)
					{
						itmp[1] += 100;
						SetPVarInt(playerid,"Skill_" #WEAPONSKILL_MICRO_UZI,itmp[1]);
						SetPlayerSkillLevel(playerid,WEAPONSKILL_MICRO_UZI,itmp[1]);
						SetPVarInt(playerid,"UpgradesPoints",itmp[0] - 3);
					}
				}
				case 9: // skill
				{
					if(GetPVarInt(playerid,"UpgradesPoints")<4) return SendClientMessage(playerid, COLOR_LIGHTRED,"Требуется 4 очка прокачки на этот навык!");
					if((itmp[1] = GetPVarInt(playerid,"Skill_" #WEAPONSKILL_MP5)) < 999)
					{
						itmp[1] += 100;
						SetPVarInt(playerid,"Skill_" #WEAPONSKILL_MP5,itmp[1]);
						SetPlayerSkillLevel(playerid,WEAPONSKILL_MP5,itmp[1]);
						SetPVarInt(playerid,"UpgradesPoints",itmp[0] - 4);
					}
				}
				case 10: // skill
				{
					if(GetPVarInt(playerid,"UpgradesPoints")<5) return SendClientMessage(playerid, COLOR_LIGHTRED,"Требуется 6 очков прокачки на этот навык!");
					if((itmp[1] = GetPVarInt(playerid,"Skill_" #WEAPONSKILL_AK47)) < 999)
					{
						itmp[1] += 100;
						SetPVarInt(playerid,"Skill_" #WEAPONSKILL_AK47,itmp[1]);
						SetPlayerSkillLevel(playerid,WEAPONSKILL_AK47,itmp[1]);
						SetPVarInt(playerid,"UpgradesPoints",itmp[0] - 5);
					}
				}
				case 11: // skill
				{
					if(GetPVarInt(playerid,"UpgradesPoints")<6) return SendClientMessage(playerid, COLOR_LIGHTRED,"Требуется 6 очков прокачки на этот навык!");
					if((itmp[1] = GetPVarInt(playerid,"Skill_" #WEAPONSKILL_M4)) < 999)
					{
						itmp[1] += 100;
						SetPVarInt(playerid,"Skill_" #WEAPONSKILL_M4,itmp[1]);
						SetPlayerSkillLevel(playerid,WEAPONSKILL_M4,itmp[1]);
						SetPVarInt(playerid,"UpgradesPoints",itmp[0] - 6);
					}
				}
				case 12: // skill
				{
					if(GetPVarInt(playerid,"UpgradesPoints")<6) return SendClientMessage(playerid, COLOR_LIGHTRED,"Требуется 6 очков прокачки на этот навык!");
					if((itmp[1] = GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SNIPERRIFLE)) < 999)
					{
						itmp[1] += 100;
						SetPVarInt(playerid,"Skill_" #WEAPONSKILL_SNIPERRIFLE,itmp[1]);
						SetPlayerSkillLevel(playerid,WEAPONSKILL_SNIPERRIFLE,itmp[1]);
						SetPVarInt(playerid,"UpgradesPoints",itmp[0] - 6);
					}
				}
				case 13: // armor
				{
					if((ftmp[0] = GetPVarFloat(playerid,"Armor")) < 100.0)
					{
						SetPVarFloat(playerid,"Armor",ftmp[0] + 10.0);
						SetPVarInt(playerid,"UpgradesPoints",itmp[0] - 1);
					}
				}
				case 14: // DriverSkill
				{
					if((itmp[1] = GetPVarInt(playerid,"DriverSkill")) < 1000)
					{
						SetPVarInt(playerid,"DriverSkill",itmp[1] + 100);
						SetPVarInt(playerid,"UpgradesPoints",itmp[0] - 1);
					}
				}
				case 15: //FightStyle
				{	
					if((itmp[1] = GetPVarInt(playerid,"FightStyle")) < 10)
						{
						SetPVarInt(playerid,"FIghtStyle",itmp[1] + 1);
						SetPVarInt(playerid,"UpgradesPoints",itmp[0] - 1);
						}
					
				}
				case 16: //SharpShooting 
				{	
					if(GetPVarInt(playerid,"UpgradesPoints")<2) return SendClientMessage(playerid, COLOR_LIGHTRED,"Требуется 2 очка прокачки на этот навык!");
					if((itmp[1] = GetPVarInt(playerid,"SharpShooting")) < 10)
						{
						SetPVarInt(playerid,"SharpShooting",itmp[1] + 1);
						SetPVarInt(playerid,"UpgradesPoints",itmp[0] - 2);
						}
					
				}	
			}
			//format(stmp,sizeof(stmp),"Max health (%.0f)\nRegeneration (%.0f)\nColt skill (%d)\n",GetPVarFloat(playerid,"MaxHealth"),GetPVarFloat(playerid,"Regeneration"),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_PISTOL));
			//format(stmp,sizeof(stmp),"%sSilenced skill (%d)\nDesert eagle skill (%d)\nShotgun skill (%d)\n",stmp,GetPVarInt(playerid,"Skill_" #WEAPONSKILL_PISTOL_SILENCED),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_DESERT_EAGLE),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SHOTGUN));
			//format(stmp,sizeof(stmp),"%sSawnoff skill (%d)\nS.P.A.S. skill (%d)\nMicro UZI skill (%d)\n",stmp,GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SAWNOFF_SHOTGUN),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SPAS12_SHOTGUN),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_MICRO_UZI));
			//format(stmp,sizeof(stmp),"%sMP5 skill (%d)\nAK47 skill (%d)\nM4 skill (%d)\nSniper rifle skill (%d)\nArmor (%.0f)\nНавык Вождения(%d)\n",stmp,GetPVarInt(playerid,"Skill_" #WEAPONSKILL_MP5),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_AK47),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_M4),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SNIPERRIFLE),GetPVarFloat(playerid,"Armor"),GetPVarInt(playerid,"DriverSkill"));
			new tmp[24];
			format(tmp,sizeof(tmp),"Доступно SP: %d", GetPVarInt(playerid,"UpgradesPoints"));
			format(stmp,sizeof(stmp),"Max health (%.0f)\nRegeneration (%.0f) - 2SP\nColt skill (%d) - 1SP\n",GetPVarFloat(playerid,"MaxHealth"),GetPVarFloat(playerid,"Regeneration"),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_PISTOL));
			format(stmp,sizeof(stmp),"%sSilenced skill (%d) - 2SP\nDesert eagle skill (%d) - 2SP\nShotgun skill (%d) - 4SP\n",stmp,GetPVarInt(playerid,"Skill_" #WEAPONSKILL_PISTOL_SILENCED),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_DESERT_EAGLE),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SHOTGUN));
			format(stmp,sizeof(stmp),"%sSawnoff skill (%d) - 5SP\nS.P.A.S. skill (%d) - 5SP\nMicro UZI skill (%d) - 3SP\n",stmp,GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SAWNOFF_SHOTGUN),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SPAS12_SHOTGUN),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_MICRO_UZI));
			format(stmp,sizeof(stmp),"%sMP5 skill (%d) - 4SP\nAK47 skill (%d) - 5SP\nM4 skill (%d) - 6SP\nSniper rifle skill (%d) - 6SP\nБроня(%.0f) - 1SP\n",stmp,GetPVarInt(playerid,"Skill_" #WEAPONSKILL_MP5),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_AK47),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_M4),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SNIPERRIFLE),GetPVarFloat(playerid,"Armor") );
			format(stmp,sizeof(stmp),"%sНавык Вождения(%d) - 1SP\nНавык борьбы(%d) - 1SP\nТочная Стрельба(%d) - 2sp",stmp, GetPVarInt(playerid,"DriverSkill"),GetPVarInt(playerid,"FightStyle"),GetPVarInt(playerid,"SharpShooting"));
			ShowPlayerDialog(playerid,DIALOG_UPGRADE,DIALOG_STYLE_LIST,tmp,stmp,"Улучшить","Отмена");
			return 1;
		}
		case DIALOG_RADIO:
		{
			if(!response) return 1;
			TuneRadio(playerid,listitem);
			return 1;
		}
	}
	return 0;
}

Player_Command(playerid,cmdid,params[])
{
	switch(cmdid)
	{
		case PLAYER_STAT_CMD:
		{
			stmp[0] = 0;
			format(stmp,sizeof(stmp),"\
				Уровень доступа:				%s\n\
				Банда:						%s\n\
				Уровень:					%d\n\
				Опыт:						%d\n\
				До следующего уровня:			%d\n\
				Деньги:					%d\n\
				Доход от бизнесов:				%d\n\
				Убийства:					%d\n\
				Смерти:					%d\n\
				Очки прокачки:				%d\n\
				Навык вождения:				%d\n\
				Навык борьбы:				%d\n\
				Точная стрельба:			%d\n\
				Максимум жизни:				%.0f\n\
				Броня:						%.0f\n\
				Регенерация:					%.0f\n\
				",
				oGetPlayerAdminLevelName(playerid),
				oGetPlayerGangName(playerid),
				GetPVarInt(playerid,"Level"),
				GetPVarInt(playerid,"XP"),
				PlayerXPToLevel(playerid),
				oGetPlayerMoney(playerid),
				GetPVarInt(playerid,"BusinessIncome"),
				GetPVarInt(playerid,"Kills"),
				GetPVarInt(playerid,"Deaths"),
				GetPVarInt(playerid,"UpgradesPoints"),
				GetPVarInt(playerid,"DriverSkill"),
				GetPVarInt(playerid,"FightStyle"),
				GetPVarInt(playerid,"Sharpshooting"),
				GetPVarFloat(playerid,"MaxHealth"),
				GetPVarFloat(playerid,"Armor"),
				GetPVarFloat(playerid,"Regeneration")
			);
			format(stmp,sizeof(stmp),"%s\
				Скилл пистолета:				%d\n\
				Скилл пистолета с глушителем:		%d\n\
				Скилл \"Пустынного орла\"			%d\n\
				Скилл дробовика:				%d\n\
				Скилл обреза:					%d\n\
				Скилл S.P.A.S.:					%d\n\
				Скилл Микро узи:				%d\n\
				Скилл MP5:					%d\n\
				Скилл АК47:					%d\n\
				Скилл М4:					%d\n\
				Скилл снайперской винтовки:			%d\n\
				",
				stmp,
				GetPVarInt(playerid,"Skill_" #WEAPONSKILL_PISTOL),
				GetPVarInt(playerid,"Skill_" #WEAPONSKILL_PISTOL_SILENCED),
				GetPVarInt(playerid,"Skill_" #WEAPONSKILL_DESERT_EAGLE),
				GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SHOTGUN),
				GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SAWNOFF_SHOTGUN),
				GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SPAS12_SHOTGUN),
				GetPVarInt(playerid,"Skill_" #WEAPONSKILL_MICRO_UZI),
				GetPVarInt(playerid,"Skill_" #WEAPONSKILL_MP5),
				GetPVarInt(playerid,"Skill_" #WEAPONSKILL_AK47),
				GetPVarInt(playerid,"Skill_" #WEAPONSKILL_M4),
				GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SNIPERRIFLE)
			);
			ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"Статистика",stmp,"Ок","");
			return 1;
		}
		case PLAYER_HELP_CMD:
		{
			stmp[0] = 0;
			strcat(stmp,MODE_NAME " - TDM-RPG мод, здесь вы можете улучшать своего персонажа, вступать в банды, учавствовать в гонках,\nдезматчах и проходить миссии.\n");
			strcat(stmp,"/savecar - сохранить ваш траспорт\n/skydive-прыжок с парашютом\n/info - изменения и информация о версии\n/savechar - сохранить персонажа(автоматически сохраняется при выходе из игры)\n/changepass - изменить пароль от аккаунта\n/kill - убить себя\n/credits - узнать список разработчиков\n");
			strcat(stmp,"/tp - список телепортов\n/flip - перевернуть транспорт с крыши(только в стант мире)\n/upgrade - меню модификации персонажа\n/pm [id] [сообщение] - послать личное сообщение игроку\n/givecash [id] [кол-во] - передать игроку денег\n/skydive - прыгнуть с парашютом");
			strcat(stmp,"/b help - помощь по бизнесам\n/h help - помощь по домам\n/g help - помощь по бандам\n/dm help - помощь по дезматчам\n/race help - помощь по гонкам\n");
			strcat(stmp,"/bank help - помощь по банкам\n/gz help - помощь по зонам банд\n/tq help - помощь по командным миссиям\n/w help - помощь по вечному оружию\n/copshelp и /thiefhelp - помощь по фракциям");			
			ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"Помощь->Игрок",stmp,"Ок","");
			return 1;
		}
		case PLAYER_SAVECHAR_CMD:
		{
			SavePlayer(playerid,SAVE_REASON_SAVECHAR);
			SendClientMessage(playerid,SYSTEM_COLOR,"Аккаунт успешно сохранён");
			return 1;
		}
		case PLAYER_CHANGEPASS_CMD:
		{
			if(GetPVarInt(playerid,"Logged") != 1) return 0;
			
			SetPVarString(playerid,"Password",params);
			SavePlayer(playerid,SAVE_REASON_AUTOSAVE);
			SendClientMessage(playerid,SYSTEM_COLOR,"Пароль успешно сменён.");
			return 1;
		}
		case PLAYER_KILL_CMD:
		{
			oSetPlayerHealth(playerid,0.0);
			return 1;
		}
		case PLAYER_CREDITS_CMD:
		{
			ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"Создатели",
				"Главный разработчик: 009\n\
				Второй разработчик: ZiGGi\n\
                Продолжил дело: LukeS\n\
				Спасибо: MadCat(за координаты домов, бизнесов, гонок, десматчей)"
				,"Ок","");
			return 1;
		}
		case PLAYER_GIVECASH_CMD:
		{
			new idx;
			new id = strval(strtok(params,idx)),
				amount = strval(strtok(params,idx));
			
			if(!IsPlayerConnected(id))
			{
				format(stmp,sizeof(stmp),"Этот игрок(%d) оффлайн",id);
				SendClientMessage(playerid,COLOR_WHITE,stmp);
				return 1;
			}
			if(amount <= 0)
			{
				return SendClientMessage(playerid,COLOR_WHITE,"Вы ввели неверное количество денег!");
			}
			
			oGivePlayerMoney(playerid,-amount);
			oGivePlayerMoney(id,amount);

			format(stmp,sizeof(stmp),"Игрок %s(%d) передал вам %d$",oGetPlayerName(playerid),playerid,amount);
			SendClientMessage(id,COLOR_GREEN,stmp);
			format(stmp,sizeof(stmp),"Вы передали %d$ Игроку %s(%d)",amount,oGetPlayerName(id),id);
			SendClientMessage(playerid,COLOR_GREEN,stmp);
			return 1;
		}
		case PLAYER_FLIP_CMD:
		{
			if(GetPlayerVirtualWorld(playerid) != STUNT_VIRTUAL_WORLD)
				return SendClientMessage(playerid,SYSTEM_COLOR,"Эта команда доступна только в станотовом мире!");
			itmp[0] = GetPlayerVehicleID(playerid);
			GetVehicleZAngle(itmp[0], ftmp[0]);
			SetVehicleZAngle(itmp[0], ftmp[0]);
			return 1;
		}
		case PLAYER_UPGRADE_CMD: //450 after rebalance 2 1 1 1 6 6 5 4 3 5 5 4 2 2 1 2 - 500 total
		{
			new tmp[24];
			format(tmp,sizeof(tmp),"Доступно SP: %d", GetPVarInt(playerid,"UpgradesPoints"));
			format(stmp,sizeof(stmp),"Max health (%.0f)\nRegeneration (%.0f) - 2SP\nColt skill (%d) - 1SP\n",GetPVarFloat(playerid,"MaxHealth"),GetPVarFloat(playerid,"Regeneration"),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_PISTOL));
			format(stmp,sizeof(stmp),"%sSilenced skill (%d) - 2SP\nDesert eagle skill (%d) - 2SP\nShotgun skill (%d) - 4SP\n",stmp,GetPVarInt(playerid,"Skill_" #WEAPONSKILL_PISTOL_SILENCED),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_DESERT_EAGLE),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SHOTGUN));
			format(stmp,sizeof(stmp),"%sSawnoff skill (%d) - 5SP\nS.P.A.S. skill (%d) - 5SP\nMicro UZI skill (%d) - 3SP\n",stmp,GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SAWNOFF_SHOTGUN),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SPAS12_SHOTGUN),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_MICRO_UZI));
			format(stmp,sizeof(stmp),"%sMP5 skill (%d) - 4SP\nAK47 skill (%d) - 5SP\nM4 skill (%d) - 6SP\nSniper rifle skill (%d) - 6SP\nБроня(%.0f) - 1SP\n",stmp,GetPVarInt(playerid,"Skill_" #WEAPONSKILL_MP5),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_AK47),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_M4),GetPVarInt(playerid,"Skill_" #WEAPONSKILL_SNIPERRIFLE),GetPVarFloat(playerid,"Armor") );
			format(stmp,sizeof(stmp),"%sНавык Вождения(%d) - 1SP\nНавык борьбы(%d) - 1SP\nТочная Стрельба(%d) - 2sp",stmp, GetPVarInt(playerid,"DriverSkill"),GetPVarInt(playerid,"FightStyle"),GetPVarInt(playerid,"SharpShooting"));
			ShowPlayerDialog(playerid,DIALOG_UPGRADE,DIALOG_STYLE_LIST,tmp,stmp,"Улучшить","Отмена");
			return 1;
		}
		case PLAYER_PM_CMD:
		{
			stmp[0] = 0;
			new idx=0;
			stmp = strtok(params,idx);
			if(strlen(stmp) == 0) return SendClientMessage(playerid,0xFF0000FF,"Применение: /pm [id] [сообщение]");
			new id = strval(stmp);
			if(!IsPlayerConnected(id) || id == playerid) return SendClientMessage(playerid,0xFF0000FF,"/pm: Вы ввели неправильный id");
			
			if(strlen(params[idx]) == 0) return SendClientMessage(playerid,0xFF0000FF,"Применение: /pm [id] [сообщение]");
			
			format(stmp,sizeof(stmp),">> %s(%i): %s",oGetPlayerName(id),id,params[idx]);
			SendClientMessage(playerid,0xFFD720FF,stmp);
			
			format(stmp,sizeof(stmp),"** %s(%i): %s",oGetPlayerName(playerid),playerid,params[idx]);
			SendClientMessage(id,0xFFD720FF,stmp);
			return 1;
		}
        case PLAYER_INFO_CMD:
        {
           ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"Информация о сервере",
            "Версия: UPRISING 1.3\n\
             {FFFF00}Изменения:\n\
			{FFFFFF}+Охота за головами(Доступна с 21 вечера до 7 утра)\n\
			*Множество мелких улучшений стабильности(не забывайте сообщать о багах!)"
            ,"Ок","");
           return 1;
        }
        case PLAYER_ADMINS_CMD:
        {
			itmp[1]=0;
		    SendClientMessage(playerid, COLOR_WHITE,"Администрация онлайн");
			foreach(new adm : Player)
			{
			 if(GetPVarInt(adm,"AdminLevel") == 0) continue;
			 format(stmp, sizeof(stmp), "%s" , oGetPlayerName(adm), adm);
			 SendClientMessage(playerid, COLOR_LIGHTBLUE,stmp);
			 itmp[1]++;
			} 
		    if(itmp[1] == 0) SendClientMessage(playerid, COLOR_GREY,"Сейчас ни одного админа/модера нет, но это не значит что они не следят за вами...");
			format(stmp, sizeof(stmp), "игрок %s Посмотрел список админов" , oGetPlayerName(playerid),playerid);
			NotifyAdmins(stmp);
		    return 1;
	    }
		case PLAYER_SKYDIVE_CMD:
		{
		SendClientMessage(playerid,COLOR_LIGHTGREEN,"Вы прыгнули с парашютом. Удачного полета! =)");
		GivePlayerWeapon(playerid,46,1);
		new Float:X,Float:Y,Float:Z;
		GetPlayerPos(playerid,X,Y,Z);
		SetPlayerPos(playerid,X,Y,Z+1600);
		return 1;
		}
		case PLAYER_PLATE_CMD:
		{
		 new idx=0;
		 new res[200];
		 res = strtok(params, idx);
		 if(strlen(res) == 0) return SendClientMessage(playerid,COLOR_BLUE,"Пример использования: /plate номер");
		 SendClientMessage(playerid,COLOR_LIGHTRED,"Вы изменили номер машины");
		 SetVehicleNumberPlate(GetPlayerVehicleID(playerid),params);
		 new Float:x,Float:y,Float:z,Float:ang;
		 GetVehiclePos(GetPlayerVehicleID(playerid),x,y,z);
		 GetVehicleZAngle(GetPlayerVehicleID(playerid),ang);
		 //SetVehicleToRespawn(GetPlayerVehicleID(playerid));
		 SetVehiclePos(GetPlayerVehicleID(playerid),x,y,z);
		 PutPlayerInVehicle(playerid,GetPlayerVehicleID(playerid),0);
		 SetVehicleZAngle(GetPlayerVehicleID(playerid),ang);
		 //PutPlayerInVehicle(playerid,AddStaticVehicle(449,-2264.6704,524.7512,35.5995,0.4107,1,74),0);
		 return 1;
		}
		case PLAYER_RADIO_CMD:
		{
		if(IsPlayerInAnyVehicle(playerid))
			{
			new File:datafile = fopen(MODE_DIR RADIO_FILE,io_read),
			line[MAX_STRING];
			if(strlen(params) > 0) itmp[0] = 1;
			else itmp[0] = 0;
			itmp[1] = 0;
			if(datafile)
			{
				stmp[0] = 0;
				while(fread(datafile,line))
				{
					if(!CheckFileLine(line)) continue;
					itmp[2] = strfind(line,",") + 1;
					if(itmp[0])
					{
						if(!strcmp(params,line[ itmp[2] ],true))
						{
							TuneRadio(playerid,itmp[1]);
							fclose(datafile);
							return 1;
						}
						itmp[1]++;
					}

					format(stmp,sizeof(stmp),"%s%s\n",stmp,line[ itmp[2] ]);
				}
				fclose(datafile);
				}
			ShowPlayerDialog(playerid,DIALOG_RADIO,DIALOG_STYLE_LIST,"Радиостанции",stmp,"Настроиться","Отмена");
			}
		else SendClientMessage(playerid,COLOR_WHITE,"Вы не в машине");
		return 1;
		}
		case PLAYER_SAVECAR_CMD:
		{
		if(IsPlayerInAnyVehicle(playerid))
			{
			new str[100];
			new Float:x,Float:y,Float:z,Float:ang,mod;
			GetVehiclePos(GetPlayerVehicleID(playerid),x,y,z);
		    GetVehicleZAngle(GetPlayerVehicleID(playerid),ang);
			new vehid=GetPlayerVehicleID(playerid);
			mod=GetVehicleModel(vehid);
			format(str,sizeof(str),"%f,%f,%f,%f,%d,%d,%d,%d,%s",x,y,z,ang,mod,col1{vehid},col2{vehid},GetVehiclePaintJob(vehid),params);
			SetPVarString(playerid,"sv_vehdata",str);
			//get mods
			strdel(str,0,100);
			for(new i=0;i<14;i++)
				{
				format(str,sizeof(str),"%s,%d",str,GetVehicleComponentInSlot(GetPlayerVehicleID(playerid),i));
				}
			strdel(str,0,1);
			//printf("mods %s",str);
			//SendClientMessageToAll(COLOR_WHITE,str);
			SetPVarString(playerid,"sv_vehmod",str);
			SetVehicleToRespawn(vehid);
			SavePlayer(playerid, SAVE_REASON_SAVECHAR);
			CreateSavedVehicle(playerid);
			//OnSavedCarResp(GetPVarInt(playerid,"vid"));
			SendClientMessage(playerid,COLOR_BLUE,"Машина сохранена");
			}
		else SendClientMessage(playerid,COLOR_WHITE,"Вы не в машине");
		return 1;
		}
		case PLAYER_BEER_CMD:
		{
		format(stmp,sizeof(stmp),"The current server tick rate is: %i and heap is %d kb", GetServerTickRate(),heapspace()/1024);
		SendClientMessage(playerid,-1,stmp);
		//SetPlayerSpecialAction(playerid,SPECIAL_ACTION_DRINK_BEER);
		//SetPlayerDrunkLevel(playerid,3000);
		//PlayAudioStreamForPlayer(playerid,"https://dl.dropbox.com/u/45522772/02%20Bottoms%20Up.wma");
		//SetPlayerPos(playerid,1641.47,-4732.15,12.29);
		//GivePlayerMoney(playerid,50000); //DIRTY CHEAT MONEY!!!!
		//PlayCameraMover(playerid,LoadCameraMover("login1"));
		//oSetPlayerHealth(playerid,50.0);
		//SetPlayerColor(playerid, GetPlayerColor( playerid) | 0x000000FF );
		TextDrawShowForPlayer(playerid,lag);
		
		return 1;
		}
     }
	return 0;
}
Player_OnPlayerEnterVehicle(playerid,vehicleid)
{
new Float:x,Float:y,Float:z,model=GetVehicleModel(vehicleid);
if((model==432||model==425) && (GetPVarInt(playerid,"DriverSkill")<900))
{
SendClientMessage(playerid,COLOR_LIGHTRED,"Ваш навык вождения недостачно высок!Требуется 900 или выше");
GetPlayerPos(playerid,x,y,z);
SetPlayerPos(playerid,x,y,z);
}
if(model==594 && (GetPVarInt(playerid,"DriverSkill")<1000))
{
SendClientMessage(playerid,COLOR_LIGHTRED,"Интересно, и как же ЭТО водить?");
GetPlayerPos(playerid,x,y,z);
SetPlayerPos(playerid,x,y,z);
}
if(model==520 && (GetPVarInt(playerid,"DriverSkill")<750))
{
SendClientMessage(playerid,COLOR_LIGHTRED,"Ваш навык вождения недостачно высок!Требуется 750 или выше");
GetPlayerPos(playerid,x,y,z);
SetPlayerPos(playerid,x,y,z);
}
if(model==447 && (GetPVarInt(playerid,"DriverSkill")<400))
{
SendClientMessage(playerid,COLOR_LIGHTRED,"Ваш навык вождения недостачно высок!Требуется 400 или выше");
GetPlayerPos(playerid,x,y,z);
SetPlayerPos(playerid,x,y,z);
}
if(model==601 && (GetPVarInt(playerid,"DriverSkill")<250))
{
SendClientMessage(playerid,COLOR_LIGHTRED,"Ваш навык вождения недостачно высок!Требуется 250 или выше");
GetPlayerPos(playerid,x,y,z);
SetPlayerPos(playerid,x,y,z);
}
return 1;
}
stock TuneRadio(playerid,pointid)
{
	if(pointid==0) return StopAudioStreamForPlayer(playerid);
	new File:df = fopen(MODE_DIR RADIO_FILE,io_read),
		c,url[128];
	if(df)
	{
		while(fread(df,stmp))
		{
			if(!CheckFileLine(stmp)) continue;
			//printf("c is %d pointid is %d",c,pointid);
			if(c == pointid)
			{
				c = 0;
				// url
				strmid(url,stmp,0,strfind(stmp,","));
				// tune
				format(url,sizeof(url),"http://%s",url);
			    PlayAudioStreamForPlayer(playerid,url);
				break;
			}
			else c++;
		}
		fclose(df);
	}
	else print("[ERROR] radios file not exist");
	return 1;
}

Player_OnPlayerTakeDamage(playerid, issuerid, Float: amount, weaponid, bodypart)
{
#pragma unused amount
    if(issuerid != INVALID_PLAYER_ID && bodypart == 9)//
    {
		if(random(101)<GetWeapCritMod(weaponid)*GetPVarInt(issuerid,"SharpShooting"))
		{
        SetPlayerHealth(playerid, 0.0);
		GameTextForPlayer(issuerid, "~r~ HEADSHOT!", 2000, 3);
		}
    }
    return 1;

}


GetWeapCritMod(weaponid)
{
switch(weaponid)
	{
		case 34:
			return 5; //Sniper rifle
		case 33:
			return 4; //Country rifle
		case 24:
			return 3; //Desert eagle
		case 23:
			return 2; //Silenced 9mm
	}
return 1;
}
