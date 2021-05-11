/*
*	Created:			18.05.2012
*	Author:				LukeS
*	Description:		Фракции
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
			if(GetPVarInt(playerid,"AdminLevel") == 0) return SendClientMessage(playerid,COLOR_WHITE,"Только Админы и модераторы могут назначать глав фракций!");
			new id;
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"Пример использования: /setmaincop id");
			id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"Игрок оффлайн");
			SetPVarInt(id,"Work",COPBOSS);
			format(stmp,sizeof(stmp),"%s назначил игрока %s главой копов",oGetPlayerName(playerid),oGetPlayerName(id));
			SendClientMessageToAll(COLOR_WHITE,stmp);
			return 1;

		}
		case SETCOP:
		{
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"Пример использования: /setcop id");
			new id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"Игрок оффлайн");
			if(GetPVarInt(playerid,"Work")==COPBOSS)
			{
			SetPVarInt(id,"work",COP);
			format(stmp,sizeof(stmp),"Глава копов %s взял на работу игрока %s",oGetPlayerName(playerid),oGetPlayerName(id));
			SendClientMessageToAll(COLOR_WHITE,stmp);
			}
			else SendClientMessage(playerid,COLOR_ORANGE,"Вы не глава копов!");
			return 1;
		}
		case COPS_CMD:
		{
		SendClientMessage(playerid,COLOR_WHITE,"Полиция Онлайн");
		//for(new i = 0;i <= GetPlayersCount();i++)
		foreach(new i : Player)
		{
		//new str[100];
		//if(!IsPlayerConnected(i)) continue;
		if(IsPlayerNPC(i)) continue;
		if(GetPVarInt(i,"Work") == COPBOSS)
		{
		format(stmp,sizeof(stmp),"%s - Главный коп\n",oGetPlayerName(i));
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
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"Пример использования: /arrest id");
			new id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"Игрок оффлайн");
			if(GetPlayerWantedLevel(id)==0) return SendClientMessage(playerid,COLOR_BLUE,"Игрок не в розыске");
			if(GetDistanceBetweenPlayers(playerid,id)>10) return SendClientMessage(playerid,COLOR_BLUE,"Вы далеко от подозреваемого");
			if(GetPlayerSpecialAction(id) != SPECIAL_ACTION_HANDSUP) return SendClientMessage(playerid,COLOR_BLUE,"Игрок не намерен сдаваться!");
			//SetPlayerSpecialAction(id,SPECIAL_ACTION_CUFFED);
			SendClientMessage(playerid,COLOR_BLUE,"Подозреваемый арестован");
			award(playerid,GetPlayerWantedLevel(id),REASON_ARREST);
			format(stmp,sizeof(stmp),"Полиция взяла штраф за нарушения в размере %d $",-1000*GetPlayerWantedLevel(id));
			oGivePlayerMoney(id,-1000*GetPlayerWantedLevel(id));
			//WLevel{id}=0;
			SetPlayerWantedLevel(id,0);
			SendClientMessage(id,COLOR_BLUE,stmp);
			}
			else return SendClientMessage(playerid,COLOR_BLUE,"Вы не полицейский/ФБР!");
			return 1;
		}
		case COPSHELP_CMD:
		{
		ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"Помощь по фракциям",
            "Полиция\n\
			Если игрок кого то убил, у него прибавляется одна звезда розыска\n\
			Чем выше уровень розыска, тем выше награда за устранение преступника\n\
			Команды:\n\
			/setmaincop id (для админов и модеров) - назначить гл. копа\n\
			/setcop id(для главы полиции) - нанять игрока на работу полицейским\n\
			/cops - посмотреть полицейских онлайн\n\
			/wanted - cписок преступников\n\
			/arrest id - арестовать преступника(больше награда)\n\
			/suspect id(только для главы) - Подать в розыск на игрока"
            ,"Ок","");
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
		if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"Пример использования: /arrest id");
		new id=strval(params);
		if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"Игрок оффлайн");
		new k=0;
		id=strval(strtok(params,k));
		new WL=strval(strtok(params,k));
		SetPlayerWantedLevel(id,WL);
		format(stmp,sizeof(stmp),"%s объявил в розыск игрока %s по причине: %s (Уровень розыска:%d)",oGetPlayerName(playerid),oGetPlayerName(id),params[k],WL);
		PlayCrimeReportForPlayer(playerid, id, 3);
		PlayCrimeReportForPlayer(id, id, 3);
		SendClientMessageToAll(COLOR_BLUE,stmp);
		}
		else return SendClientMessage(playerid,COLOR_BLUE,"Только главы полиции и ФБР могут подавать в розыск!");
		return 1;
		}
		case SETMAINTHIEF:
		{
			if(GetPVarInt(playerid,"AdminLevel") == 0) return SendClientMessage(playerid,COLOR_WHITE,"Только Админы и модераторы могут назначать глав фракций!");
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"Пример использования: /setmainthief id");
			new id;
			id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"Игрок оффлайн");
			SetPVarInt(id,"Work",THIEFBOSS);
			format(stmp,sizeof(stmp),"%s назначил игрока %s главой Угонщиков",oGetPlayerName(playerid),oGetPlayerName(id));
			SendClientMessageToAll(COLOR_WHITE,stmp);
			SetPlayerMapIcon(playerid,99,1572.0670,-1477.8331,13.3571,55,0,1);
			return 1;

		}
		case SETTHIEF:
		{
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"Пример использования: /setthief id");
			new id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"Игрок оффлайн");
			if(GetPVarInt(playerid,"Work")==THIEFBOSS)
			{
			SetPVarInt(id,"Work",THIEF);
			format(stmp,sizeof(stmp),"Глава Угонщиков %s взял на работу игрока %s",oGetPlayerName(playerid),oGetPlayerName(id));
			SendClientMessageToAll(COLOR_WHITE,stmp);
			SetPlayerMapIcon(playerid,99,1572.0670,-1477.8331,13.3571,55,0,1);
			}
			else SendClientMessage(playerid,COLOR_ORANGE,"Вы не глава воров!");
			return 1;
		}
		case THIEVES_CMD:
		{
		SendClientMessage(playerid,COLOR_WHITE,"Угонщики Онлайн");
		//for(new i = 0;i <= GetPlayersCount();i++)
		foreach(new i : Player)
		{
		//new str[100];
		//if(!IsPlayerConnected(i)) continue;
		if(IsPlayerNPC(i)) continue;
		if(GetPVarInt(i,"Work") == THIEFBOSS)
		{
		format(stmp,sizeof(stmp),"%s - Главный угонщик\n",oGetPlayerName(i));
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
		ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"Помощь по фракциям",
            "Угонщики\n\
			Угонщикам приходят сообщения, в которых содержится наименование нужного транспорта\n\
			Задача угонщиков - доставить этот транспорт в нужное место за награду\n\
			Команды:\n\
			/setmainthief id (для админов и модеров) - назначить главу угонщиков\n\
			/setthief id(для главы полиции) - нанять игрока на работу угонщиком\n\
			/thieves - посмотреть угонщиков онлайн"
            ,"Ок","");
		return 1;
		}
		case CARWANTED_CMD:
		{
		format(stmp,sizeof(stmp),"На данный момент требуется : %s",oGetVehicleName(carwanted));
		ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"Требуемый транспорт",stmp,"Ок","");
		return 1;
		}
		case WANTED_CMD:
		{
		format(stmp,sizeof(stmp),"Игрок			Уровень Розыска\n");
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
		ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"В розыске",stmp,"Ок","");
		return 1;
		}
		case SETMAINRACER:
		{
			if(GetPVarInt(playerid,"AdminLevel") == 0) return SendClientMessage(playerid,COLOR_WHITE,"Только Админы и модераторы могут назначать глав фракций!");
			new id;
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"Пример использования: /setmainracer id");
			id=strval(params);
			
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"Игрок оффлайн");
			SetPVarInt(id,"Work",RACERBOSS);
			format(stmp,sizeof(stmp),"%s назначил игрока %s главой Уличных гонщиков",oGetPlayerName(playerid),oGetPlayerName(id));
			SendClientMessageToAll(COLOR_WHITE,stmp);
			return 1;

		}
		case SETRACER:
		{
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"Пример использования: /setracer id");
			new id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"Игрок оффлайн");
			if(GetPVarInt(playerid,"Work")==RACERBOSS)
			{
			SetPVarInt(id,"Work",RACER);
			format(stmp,sizeof(stmp),"Глава Уличных гонщиков %s взял на работу игрока %s",oGetPlayerName(playerid),oGetPlayerName(id));
			SendClientMessageToAll(COLOR_WHITE,stmp);
			}
			else SendClientMessage(playerid,COLOR_ORANGE,"Вы не глава Гонщиков!");
			return 1;
		}
		case RACERHELP_CMD:
		{
		ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"Помощь по фракциям",
            "Уличные гонщики:\n\
			Уличные гонщики прирожденые водители и непревзойденные механики.\n\
			Они могут чинить свой транспорт фактически на ходу, а так же дополнительно улучшать его.\n\
			Так же они получают дополнительные бонусы за гонки.\n\
			Команды:\n\
			/setmainracer id (для админов и модеров) - назначить главу гонщиков\n\
			/setracer id(для главы уличных гонщиков) - нанять игрока на работу гонщиком\n\
			/racers - посмотреть гонщиков онлайн"
            ,"Ок","");
		return 1;
		}
		case RACERS_CMD:
		{
		SendClientMessage(playerid,COLOR_WHITE,"Гонщики Онлайн");
		//for(new i = 0;i <= GetPlayersCount();i++)
		foreach(new i : Player)
		{
		//new str[100];
		//if(!IsPlayerConnected(i)) continue;
		if(IsPlayerNPC(i)) continue;
		if(GetPVarInt(i,"Work") == RACERBOSS)
		{
		format(stmp,sizeof(stmp),"%s - Глава уличных гонщиков\n",oGetPlayerName(i));
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
			if(GetPVarInt(playerid,"AdminLevel") == 0) return SendClientMessage(playerid,COLOR_WHITE,"Только Админы и модераторы могут назначать глав фракций!");
			new id;
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"Пример использования: /setmainfbi id");
			id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"Игрок оффлайн");
			SetPVarInt(id,"Work",FBIBOSS);
			format(stmp,sizeof(stmp),"%s назначил игрока %s главой FBI",oGetPlayerName(playerid),oGetPlayerName(id));
			SendClientMessageToAll(COLOR_WHITE,stmp);
			return 1;

		}
		case SETFBI:
		{
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"Пример использования: /setFBI id");
			new id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"Игрок оффлайн");
			if(GetPVarInt(playerid,"Work")==FBIBOSS)
			{
			SetPVarInt(id,"work",FBI);
			format(stmp,sizeof(stmp),"Глава ФБР %s взял на работу игрока %s",oGetPlayerName(playerid),oGetPlayerName(id));
			SendClientMessageToAll(COLOR_WHITE,stmp);
			}
			else SendClientMessage(playerid,COLOR_ORANGE,"Вы не глава копов!");
			return 1;
		}
		case FBIS_CMD:
		{
		SendClientMessage(playerid,COLOR_WHITE,"ФБР Онлайн");
		//for(new i = 0;i <= GetPlayersCount();i++)
		foreach(new i : Player)
		{
		//new stmp[100];
		if(!IsPlayerConnected(i)) continue;
		if(IsPlayerNPC(i)) continue;
		if(GetPVarInt(i,"Work") == FBIBOSS)
		{
		format(stmp,sizeof(stmp),"%s - Глава ФБР\n",oGetPlayerName(i));
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
			if(strlen(params) == 0) return SendClientMessage(playerid,COLOR_BLUE,"Пример использования: /heli id");
			new id=strval(params);
			if(!IsPlayerConnected(id)) return SendClientMessage(playerid,COLOR_BLUE,"Игрок оффлайн");
			if(GetPlayerWantedLevel(id)==0) return SendClientMessage(playerid,COLOR_BLUE,"Игрок не в розыске");
			//if(GetDistanceBetweenPlayers(playerid,id)>10) return SendClientMessage(playerid,COLOR_BLUE,"Вы далеко от подозреваемого");
			//if(GetPlayerSpecialAction(id) != SPECIAL_ACTION_HANDSUP) return SendClientMessage(playerid,COLOR_BLUE,"Игрок не намерен сдаваться!");
			//SetPlayerSpecialAction(id,SPECIAL_ACTION_CUFFED);
			chasing=id;
			FBI_HeliUpdate(id);
			SendClientMessage(playerid,COLOR_BLUE,"Вертолет начал преследование");
			//award(playerid,GetPlayerWantedLevel(id),REASON_ARREST);
			//format(stmp,sizeof(stmp),"Полиция взяла штраф за нарушения в размере %d $",-1000*GetPlayerWantedLevel(id));
			//GivePlayerMoney(id,-1000*GetPlayerWantedLevel(id));
			//WLevel{id}=0;
			//SetPlayerWantedLevel(id,0);
			//SendClientMessage(id,COLOR_BLUE,stmp);
			}
			else return SendClientMessage(playerid,COLOR_BLUE,"Вы не ФБР!");
			return 1;
		}
		case FBIHELP_CMD:
		{
		ShowPlayerDialog(playerid,DIALOG_NONE_ACTION,DIALOG_STYLE_MSGBOX,"Помощь по фракциям",
            "ФБР:\n\
			ФБР берутся за крупные преступления\n\
			Они могут вызывать вертолет слежения, который будет сообщать полициии ФБР местонахождения преступника\n\
			Команды:\n\
			/setmainfbi id (для админов и модеров) - назначить главу ФБР\n\
			/setfbi id(для главы ФБР) - нанять игрока на работу\n\
			/wanted - список преступников\n\
			/fbis - посмотреть агентов ФБР онлайн\n\
			/arrest id - арестовать преступника(больше награда)\n\
			/suspect id(только для главы) - Подать в розыск на игрока"
            ,"Ок","");
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
	else SendClientMessage(killerid,COLOR_BLUE,"Вы убили невиновного!");
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
		else SendClientMessage(killerid,COLOR_BLUE,"Пусть копы занимаются простыми бандитами, наша задача серьезные преступники");
		SetPlayerWantedLevel(playerid,0);
		}
	else SendClientMessage(killerid,COLOR_BLUE,"Вы убили невиновного!");
	}

	else
	{
		//new i;
		//WLevel{killerid}++;
		SetPlayerWantedLevel(killerid,GetPlayerWantedLevel(killerid)+1);
		SendClientMessage(killerid,COLOR_BLUE,"Вас разыскивает полиция,чтобы сдаться введите /handsup");
		
		format(stmp,sizeof(stmp),"%s разыскивается за убийство %s",oGetPlayerName(killerid),oGetPlayerName(playerid));
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
	format(stmp,sizeof(stmp),"Спасибо за тачку! Вот твой гонорар - %d $ и %d опыта",moneygive,xpgive);
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
		format(stmp,sizeof(stmp),"Вы арестовали преступника с уровнем розыска %d Ваша награда - %d $ и %d опыта",WL,moneygive,xpgive);
		}
		else
		{
		moneygive=100*(10*WL);
		xpgive=1000*(10*WL);
		oGivePlayerMoney(killerid,moneygive);
		GivePlayerXP(killerid,xpgive);
		format(stmp,sizeof(stmp),"Вы устранили преступника с уровнем розыска %d Ваша награда - %d $ и %d опыта",WL,moneygive,xpgive);
		
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
SendClientMessageToAll(COLOR_LIGHTRED,"В лесах Angel Pine прячется преступник, найдите его и получите награду");
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
	format(stmp,sizeof(stmp),"%s убил цель, Награда - 10 000$",oGetPlayerName(playerid));
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
		format(stmp,sizeof(stmp),"Нам нужна машина марки %s вези ее к нам и получишь награду",oGetVehicleName(carwanted));
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
listitems="Включить радио\nСохранить машину";
if(skill>=200) strcat(listitems, "\nЗакрыть/открыть двери");
if(wrk==RACER || wrk == RACERBOSS)
	{
		if(skill>=400) strcat(listitems, "\nПодкрутить движок\nПоставить неон(1000$)");
		if(skill>=500) strcat(listitems, "\nУстановить нитро(5000$)");
	}
ShowPlayerDialog(playerid,DIALOG_INCAR,DIALOG_STYLE_LIST,"Машина",listitems," Выбрать"," Отмена");
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
					ShowPlayerDialog(playerid,DIALOG_PLATE,DIALOG_STYLE_INPUT,"Сохранить Авто","Введите желаемый номер","Сохранить","Отмена ");
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
					format(stmp,sizeof(stmp),"Двигатель частично отремонтирован, текущее состояние ~ %0.f",ftmp[0]/10);
					SendClientMessage(playerid,COLOR_LIGHTGREEN,stmp);
					return 1;
					}
				case 4:
					{
					//VehicleAddNeon(itmp[0],color);
					if(oGetPlayerMoney(playerid)<1000) return SendClientMessage(playerid,COLOR_SPRINGGREEN,"У вас недостаточно денег!");
					oGivePlayerMoney(playerid,-1000);
					ShowPlayerDialog(playerid,DIALOG_NEON,DIALOG_STYLE_LIST,"Выберите цвет","Синий\nКрасный\nЗеленый\nЖелтый\nРозовый\nБелый","Выбрать"," Отмена");
					}
				case 5:
					{
					new skill = GetPVarInt(playerid,"DriverSkill");
					if(oGetPlayerMoney(playerid)<5000) return SendClientMessage(playerid,COLOR_SPRINGGREEN,"У вас недостаточно денег!");
					oGivePlayerMoney(playerid,-5000);
					if(skill<=500)
						{
						AddVehicleComponent(itmp[0],1009);
						SendClientMessage(playerid,COLOR_SPRINGGREEN,"Ваш навык позволил вам установить 2х нитро!");
						}
					if(skill>=750&& skill<900)
						{
						AddVehicleComponent(itmp[0],1008);
						SendClientMessage(playerid,COLOR_SPRINGGREEN,"Ваш навык позволил вам установить 5х нитро!");
						}
					if(skill>900)
						{
						AddVehicleComponent(itmp[0],1010);
						SendClientMessage(playerid,COLOR_SPRINGGREEN,"Ваш навык позволил вам установить 10х нитро!");
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
Factions_OnVehicleSpawn(vehicleid) // Баг - когда спавнится тачка она удаляет любой неон игрока
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
	ShowPlayerDialog(playerid,DIALOG_WIN,DIALOG_STYLE_LIST,"Победа в гонке",stmp,"Выбрать"," Отмена");
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
format(stmp,sizeof(stmp),"|Вертолет|:%s замечен в районе %s, направляется на ",oGetPlayerName(playerid),GetXYZZoneName(ftmp[1],ftmp[2],ftmp[3]));
if(IsPlayerInAnyVehicle(playerid)) GetVehicleZAngle(GetPlayerVehicleID(playerid),ftmp[0]);
//printf("Rotation is %f",ftmp[0]);

else GetPlayerFacingAngle(playerid,ftmp[0]);
if((ftmp[0]>315.0 && ftmp[0]<361.0)||(ftmp[0]>0.0 && ftmp[0]<=45.0)) strcat(stmp,"север.");
if(ftmp[0]>45.0 && ftmp[0]<=135.0) strcat(stmp,"восток.");
if(ftmp[0]>135.0 && ftmp[0]<=225.0) strcat(stmp,"юг.");
if(ftmp[0]>225.0 && ftmp[0]<=315.0) strcat(stmp,"запад.");
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
if(HasSupplied[playerid]==1) return SendClientMessage(playerid,COLOR_BLUE,"Вы уже получили свой набор");
itmp[0]=GetPVarInt(playerid,"Work");
if(itmp[0]==COP)
	{
		oGivePlayerWeapon(playerid,3,1); //дубинка
		oGivePlayerWeapon(playerid,22,150); //Пистолет
		SendClientMessage(playerid,COLOR_BLUE,"Вот ваш набор вооружения. Удачного патруля!");
	
	}
if(itmp[0]==COPBOSS)
	{
		oGivePlayerWeapon(playerid,3,1); //дубинка
		oGivePlayerWeapon(playerid,22,150); //Пистолет
		oGivePlayerWeapon(playerid,25,25); //Дробовик
		SendClientMessage(playerid,COLOR_BLUE,"Вот ваш набор вооружения. Берегите себя, сэр!");
	
	}
if(itmp[0]==FBI)
	{
		oGivePlayerWeapon(playerid,23,100); //Пистолет с глушителем
		oGivePlayerWeapon(playerid,29,150); //СМГ
		SetPlayerArmour(playerid,100); //Бронежилет
		SendClientMessage(playerid,COLOR_BLUE,"Вот ваш набор вооружения. Задайте им жару, сэр!");
	
	}
if(itmp[0]==FBIBOSS)
	{
		oGivePlayerWeapon(playerid,24,120); //Дигл
		oGivePlayerWeapon(playerid,29,150); //СМГ
		SetPlayerArmour(playerid,100); //Бронежилет
		oGivePlayerWeapon(playerid,27,50); //боевой дробовик
		SendClientMessage(playerid,COLOR_BLUE,"Вот ваш набор вооружения. Удачной охоты, сэр!");
	
	}
else return SendClientMessage(playerid,COLOR_BLUE,"Чем могу быть полезен,сэр?");
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
			SendClientMessage(playerid,COLOR_LIGHTGREEN,"Ваш Hotring доставлен на парковку возле стадиона");
			return 1;
			}
		case 1: //bloodring banger
			{
			AddStaticVehicle(504,0.0,0.0,0.0,0.0,random(128),random(128));
			SendClientMessage(playerid,COLOR_LIGHTGREEN,"Ваш Bloodring доставлен на парковку возле стадиона");
			return 1;			
			}
		case 2: //XP bonus
			{
			itmp[0]=(1+GetPVarInt(playerid,"Level"))*100;
			GivePlayerXP(playerid,itmp[0]);
			format(stmp,sizeof(stmp),"Вы получили %d бонусного опыта",itmp[0]);
			SendClientMessage(playerid,COLOR_LIGHTGREEN,stmp);
			return 1;
			}
		case 3: //large XP bonus
			{
			itmp[0]=(1+GetPVarInt(playerid,"Level"))*1000;
			GivePlayerXP(playerid,itmp[0]);
			format(stmp,sizeof(stmp),"Вы получили %d бонусного опыта",itmp[0]);
			SendClientMessage(playerid,COLOR_LIGHTGREEN,stmp);
			return 1;			
			}
		case 4://$$$
			{
			itmp[0]=(1+GetPVarInt(playerid,"Level"))*150;
			oGivePlayerMoney(playerid,itmp[0]);
			format(stmp,sizeof(stmp),"Вы получили %d бонусных денег",itmp[0]);
			SendClientMessage(playerid,COLOR_LIGHTGREEN,stmp);
			return 1;
			}
		case 5://large$$$
			{
			itmp[0]=(1+GetPVarInt(playerid,"Level"))*1500;
			oGivePlayerMoney(playerid,itmp[0]);
			format(stmp,sizeof(stmp),"Вы получили %d бонусных денег",itmp[0]);
			SendClientMessage(playerid,COLOR_LIGHTGREEN,stmp);
			return 1;
			}
		case 6: //no cops triforce
			{
			SetPlayerWantedLevel(playerid,0);
			SendClientMessage(playerid,COLOR_LIGHTGREEN,"Копы отстали от вас!");
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