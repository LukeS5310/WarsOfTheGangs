/*
*	Created:			11.06.12
*	Author:				LukeS
*	Description:		Гид
*	Info:				Умная система помощи(Переделка под стартовую миссию(20%готово))
*/

#if defined _guide_included
	#endinput
#endif

#define _guide_included
#pragma library guide
// --------------------------------------------------
// defines
// --------------------------------------------------

#define GUIDE_NAME		"|SMS|:"
#define VISUAL_BASIC	"Говно"
//static tut_step[MAX_PLAYERS], obj_veh;

enum
{
GUIDE_REQUEST_TEXT,
GUIDE_QADD_CMD,
GUIDE_BUG_CMD
};
Guide_OGMI()
{
//obj_veh=AddStaticVehicle(439,860.8735,-849.4833,77.1646,25.2571,120,99);
return 1;
}
Guide_OnPlayerSpawn(playerid)
{
//#pragma unused playerid
if(GetPVarInt(playerid,"Level")==0)
{
SendClientMessage(playerid,COLOR_YELLOW,"Приветствуем на Нашем Сервере! Чтобы ознакомиться с возможностями в данном моде нажмите 'N'. Удачной игры!");
}

if(GetPVarInt(playerid,"Level")<50)
{
oGivePlayerWeapon(playerid,22,200);
}
//tut_step[playerid]=0;
//format(stmp,sizeof(stmp),"%s Приветствую тебя, %s! Вижу ты только что прибыл.В этом городе трудно выжить в одиночку",GUIDE_NAME,oGetPlayerName(playerid));
//SendClientMessage(playerid,COLOR_YELLOW,stmp);
//format(stmp,sizeof(stmp),"но я смогу помочь тебе, за определенную услугу... Посмотрим для начала чего ты стоишь, доберись, до моего особняка, в вайнвуде.");
//SendClientMessage(playerid,COLOR_YELLOW,stmp);
//TogglePlayerCheckpointStreamer(playerid,0);
//SetPlayerCheckpoint(playerid,869.7365,-850.6883,77.4375,3);
 // 

//} */
return 1;
}
Guide_OPEC(playerid)
{
#pragma unused playerid
/* tut_step[playerid]++;
new engine, lights, alarm, doors, bonnet, boot, objective;
switch(tut_step[playerid])
{ 
	case 1:
	{	
		format(stmp,sizeof(stmp),"%s отлично, %s! Возьми-ка эту машину, и отвези к моему старому знакомому.",GUIDE_NAME,oGetPlayerName(playerid));
		SendClientMessage(playerid,COLOR_YELLOW,stmp);
		format(stmp,sizeof(stmp),"Ему очень нужна эта машина, в долгу он не останется");
		SendClientMessage(playerid,COLOR_YELLOW,stmp);
		SetPlayerCheckpoint(playerid,154.498458,-1943.330932,3.773437,3);
		GetVehicleParamsEx(obj_veh, engine, lights, alarm, doors, bonnet, boot, objective);
		SetVehicleParamsEx(obj_veh, engine, lights, alarm, doors, bonnet, boot, 1);
		return 1;
	}
	case 2:
	{
		SetVehicleToRespawn(obj_veh);
		return 1;
	}
} */
return 1;
}
Guide_Command(playerid,cmdid,params[])
{
new str[50];
switch(cmdid)
	{	
		case GUIDE_QADD_CMD:
		{
		if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"Пример использования: /qadd вопрос");
		format(str,sizeof(str),"[REQ1717]: %s",params);
		print(str);
		SendClientMessage(playerid,COLOR_YELLOW,"Спасибо за помощь в улучшении сервера! Ваш вопрос будет рассмотрен на внесение в базу знаний гида");
		return 1;
		}
		case GUIDE_BUG_CMD:
		{
		if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"Пример использования: /bug описание ошибки");
		format(str,sizeof(str),"[%s] : [BUG DESCRIPTION]: %s",oGetPlayerName(playerid),params);
		print(str);
		SendClientMessage(playerid,COLOR_YELLOW,"Спасибо за помощь в улучшении сервера! Ошибка будет исправлена в ближайших обновлениях!");
		return 1;
		}

	}
return 0;
}
Guide_Dialog(playerid,dialogid,response,listitem,inputtext[])
{
	//#pragma unused listitem
	#pragma unused inputtext
	switch(dialogid)
	{
		case DIALOG_HELP:
		{
		if(!response) return 1;
		switch(listitem)
			{
				case 0://мод
				{
				Player_Command(playerid,PLAYER_INFO_CMD," ");
				return 1;
				}
				case 1://игрок
				{
				Player_Command(playerid,PLAYER_HELP_CMD," ");
				return 1;
				}
				case 2://оружие
				{
				Weapons_Command(playerid,WEAPONS_HELP_CMD," ");
				return 1;
				}
				case 3://банды
				{
				ShowPlayerDialog(playerid,DIALOG_HELP2,DIALOG_STYLE_LIST,"Помощь->Банды","Банды\nЗоны"," Выбрать"," Отмена");
				return 1;
				}
				case 4://businass :D
				{
				Businesses_Command(playerid,BUSINESS_HELP_CMD," ");
				return 1;
				}
				case 5://Фракции
				{
				ShowPlayerDialog(playerid,DIALOG_HELP3,DIALOG_STYLE_LIST,"Помощь->Фракции","Полиция\nАвтоугонщики\nФБР\nУличные гонщики"," Выбрать"," Отмена");
				return 1;
				}
				case 6://гонки
				{
				Race_Command(playerid,RACE_HELP_CMD," ");
				return 1;
				}
				case 7://tq
				{
				TeamQuest_Command(playerid,TEAM_QUEST_HELP_CMD," ");
				return 1;
				}
				case 8://dm
				{
				Deathmatch_Command(playerid,DEATHMATCH_HELP_CMD," ");
				return 1;
				}
			
			}
		
		return 1;
		}
		case DIALOG_HELP2://банды и зоны
		{
		if(!response) return 1;
		switch(listitem)
			{
			case 0:
				{
				Gangs_Command(playerid,GANGS_HELP_CMD," ");
				
				return 1;
				}
			case 1:
				{
				Gangs_Command(playerid,GANGS_ZONE_HELP_CMD," ");
				
				return 1;
				}
			
			}
		return 1;
		}
		case DIALOG_HELP3://Фракции
		{
		if(!response) return 1;
		switch(listitem)
			{
			case 0:
				{
				Factions_Command(playerid,COPSHELP_CMD," ");
				
				return 1;
				}
			case 1:
				{
				Factions_Command(playerid,THIEFHELP_CMD," ");
				
				return 1;
				}
			case 2:
				{
				Factions_Command(playerid,FBIHELP_CMD," ");
				
				return 1;
				}
			case 3:
				{
				Factions_Command(playerid,RACERHELP_CMD," ");
				
				return 1;
				}
			}
		return 1;
		}
	}
	return 0;	
}