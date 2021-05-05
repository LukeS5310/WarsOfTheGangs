/*
*	Created:			04.06.10
*	Author:				009
*   Description:        Wars of the Gangs - TDM мод с RPG элементами.Присутствуют соревнования: миссии, мини-миссии, командные задания, гонки, дезматчи.
*                       Так же присутствует множество телепортов.Множество мест для станта.Система прокачки персонажей.
*                       Игроки могут улучшать свои скиллы оружия, увеличить максимальное количество жизни(начальное 100, максимум 150), увеличить регенерацию.
*                       Присутствует недвижимость.Бизнесы и дома.Дом можно выбрать местом спавна.С бизнесов идут деньги в банк.
*                       В банке можно хранить деньги.
*                       Главная черта мода - создание банд и война между ними.Есть зоны банд,любая банда может захватить зону.Если игроков владеющей банды нет на сервере то зона не может быть захвачена.
*                       При удалении банды зона становится нейтральной.Если владельцы банды очень давно не были на сервере то зона может быть захвачена.Если вы в зоне то над GPS пишется банда - владелец.
*/
/*

TODO:
Bugfixes
Localization
race finalization
фиксы в тюнинге загруженного личного ТС(!)
фиксы со скином игрока при тимквестах(!)- done

*/

//standart include
#include <a_samp>
#include <foreach>
// cmd parser
#define RegisterCmd(%1,%2,%3,%4,%5) if(!strcmp(%1,%2,true,%3) && ((%1[%3] == ' ') || (%1[%3] == 0))) return %4_Command(playerid,%5,%1[%3 + 1])
// text parser
#define RegisterText(%1,%2,%3,%4)	if(%1[0] == %2) if(%3_Text(playerid,%4,%1[1])) return 0
// dialog caller
#define RegisterDialog(%1,%2,%3,%4,%5,%6) case %6: return %5_Dialog(%1,%6,%2,%3,%4)

// global core defines
#define MAX_TMP_STRING_SIZE			1024
#define MAX_TMP_INT_SIZE			4
#define MAX_TMP_FLOAT_SIZE      	6
#define GetPlayersCount()           pcount
#define SetPlayerVirtualWorld(%1,%2) oSetPlayerVirtualWorld(%1,%2)
// global core enumerations
enum
{
	DIALOG_NONE_ACTION,
	DIALOG_LOGIN,
	DIALOG_REGISTER,
	DIALOG_TELEPORTS,
	DIALOG_DEATHMATCHES,
	DIALOG_RACES,
	DIALOG_GANG_INVITE,
	DIALOG_TEAM_QUESTS,
	DIALOG_TEAM_QUEST_TEAM,
	DIALOG_GANG_WAR,
	DIALOG_GANG_WAR_WEAPONS,
	DIALOG_MISSION_BANK,
	DIALOG_WEAPONS,
	DIALOG_UPGRADE,
	DIALOG_CLICK,
	DIALOG_CLICK2,
	DIALOG_CLICKSELF,
	DIALOG_DRINK,
	DIALOG_HMENU,
	DIALOG_BMENU,
	DIALOG_BANKMENU,
	DIALOG_BANKMENU2,
	DIALOG_BANKMENU3,
	DIALOG_RADIO,
	DIALOG_INCAR,
	DIALOG_PLATE,
	DIALOG_NEON,
	DIALOG_WIN,
	DIALOG_GANG,
	DIALOG_GANG_CAPITAL,
	DIALOG_HELP,
	DIALOG_HELP2,
	DIALOG_HELP3,
	DIALOG_HELP4,
	DIALOG_FIGHT,
	DIALOG_INVIZ,
	DIALOG_LOTTERY
};
enum (+=10_000)
{
	DEATHMATCH_VIRTUAL_WORLD = 10_000,
	RACE_VIRTUAL_WORLD,
	TEAM_QUEST_VIRTUAL_WORLD,
	GANG_WAR_VIRTUAL_WORLD,
	STUNT_VIRTUAL_WORLD
};

// global core vars
stock stmp[MAX_TMP_STRING_SIZE];
stock itmp[MAX_TMP_INT_SIZE];
stock Float:ftmp[MAX_TMP_FLOAT_SIZE];
stock PlayerOnPickup[MAX_PLAYERS];
stock pcount;

// forwards
//forward Check();
forward OneSecTimer();
forward FiveSecTimer();
forward OnCameraReachNode(playerid, camera, node);
// mode info
#define MODE_NAME	            	"Wars Of The Gangs"
#define MODE_VERSION            	"NEXT 1.0a"
#define MODE_DIR                	"WoG/"
// settings
#define PRINT_STATS_DATA
#define SHOW_KILL_STAT
//#define ON_PLAYER_UPDATE_STREAMERS
#define TIMESTAMP_IN_LOG
#define CJ_ANIMATIONS
#define DISABLE_STUNT_BONUS
//#define INTERIORS_DISABLE
//#define USE_ANTISLEEP_PLUGIN
//#define VEHICLE_HEALTH_CHANGE_SPY
//#define PLAYER_HEALTH_CHANGE_SPY
//#define PLAYER_ARMOUR_CHANGE_SPY
//#define PLAYER_WEAPON_CHANGE_SPY
#define PLAYER_SPECIALACTION_CHANGE_SPY
#define SYSTEM_COLOR                0xFFFFFFFF
#define MAX_STRING                  128
#define PLAYERS_COLOR				0xAAAAAAAA
#define PRESSED(%0) \
	(((newkeys & (%0)) == (%0)) && ((oldkeys & (%0)) != (%0)))

//gamemode includes
//#tryinclude "cnpc"

#tryinclude "geoip"
#tryinclude "colors"
#tryinclude "utils"
//#tryinclude "progress" //progress bar test
#tryinclude "fuel"
#tryinclude "zones"
#tryinclude "markers"
#tryinclude "streamer_objects"
#tryinclude "streamer_checkpoints"
#tryinclude "streamer_icons"
#tryinclude "quest"
#tryinclude "protect"
#tryinclude "player"
#tryinclude "world"
#tryinclude "admin"
#tryinclude "houses"
#tryinclude "businesses"
//#tryinclude "minimissions" //оффнуто, создает баг в общем таймере
#tryinclude "bank"
#tryinclude "deathmatch"
#tryinclude "race"
#tryinclude "gangs"
#tryinclude "team_quests"
#tryinclude "missions"
#tryinclude "factions"
#tryinclude "weapons"
#tryinclude "click"
#tryinclude "guide"
#tryinclude "groundhold"
//#tryinclude "npcs"
#tryinclude "holidays"
//#tryinclude "tests"
#tryinclude "lottery"
#tryinclude "CAM"
// plugins
#if defined USE_ANTISLEEP_PLUGIN
	#tryinclude "plugins/antisleep"
#endif

main() {}

// -----------------------------------------------------------------------------
//                                  Standart SA:MP's callbacks
// -----------------------------------------------------------------------------

public OnGameModeInit()
{
	AllowAdminTeleport(1);
    format(stmp,sizeof(stmp),"Loading version %s",MODE_VERSION);
    SetGameModeText(stmp);
	// start load
	print("----------------------------------");
	print("Running " MODE_NAME " version " MODE_VERSION);
	print("Created by:	009");
	// load all systems
	// protect init
	//printf("TIIIIIIIIIIIIIIIIIIIME %s",ConvertSecMins(599));
	//printf("%s",formcomma("100"));
	//printf("%s",FormComma("754398"));
	//printf("%s",formcomma("6500"));
	//printf("%s",FormComma("24348541"));
#if defined _tests_included
	Test();
#endif
#if defined _click_included
	click_OnGameModeInit();
#endif
#if defined _npcs_included
	Npcs_OnGameModeInit();
#endif
#if defined _guide_included
	Guide_OGMI();
#endif
#if defined _protect_included
    Protect_OnGameModeInit();
#endif
	// objects streamer init
#if defined _streamer_objects_included
    StreamerObj_OnGameModeInit();
#endif
	// checkpoints streamer init

#if defined _streamer_checkpoints_included
    StreamerCP_OnGameModeInit();
#endif
	// icons streamer init
#if defined _streamer_icons_included
    StreamerIcon_OnGameModeInit();
#endif
	// players handler init
#if defined _player_included
    Player_OnGameModeInit();
#endif
#if defined _GH_included
	GH_OnGameModeInit();
#endif
	// world init
#if defined _world_included
    World_OnGameModeInit();
#else
    AddPlayerClass(0,0.0,0.0,0.0,0.0,0,0,0,0,0,0);
#endif
	// houses init
#if defined _houses_included
    Houses_OnGameModeInit();
#endif
#if defined _fuel_included
	Fuel_OnGameModeInit();
#endif
	// businesses init
#if defined _businesses_included
    Businesses_OnGameModeInit();
#endif
	// admin init
#if defined _admin_included
	Admin_OnGameModeInit();
#endif
	// mini missions
#if defined _minimissions_included
    MiniMissios_OnGameModeInit();
#endif
	// banks init
#if defined _banks_included
    Banks_OnGameModeInit();
#endif
	// deathmatch init
#if defined _deathmatch_included
    Deathmatch_OnGameModeInit();
#endif
#if defined _factions_included
    Factions_OnGameModeInit();
#endif
	// race init
#if defined _race_included
    Race_OnGameModeInit();
#endif
	// gang init
#if defined _gangs_included
    Gangs_OnGameModeInit();
#endif
	// team quest init
#if defined _team_quests_included
    TeamQuest_OnGameModeInit();
#endif
	// mission init
#if defined _missions_included
    Missions_OnGameModeInit();
#endif
	// weapon init
#if defined _weapons_included
	Weapons_OnGameModeInit();
#endif
#if defined _utils_included
	Utils_OnGameModeInit();
#endif
#if defined _holidays_included
	Holidays_OnGameModeInit();
#endif
#if defined _lottery_included
	Lottery_OnGameModeInit();
	
#endif
	// timers
	SetTimer("OneSecTimer",1000,1);
	print("One second timer loaded.");
	SetTimer("FiveSecTimer",5000,1);
	print("Five second timer loaded.");

	//SetTimer("Check",100,1);
	// settings
#if defined CJ_ANIMATIONS
	UsePlayerPedAnims();
#endif
#if defined INTERIORS_DISABLE
	DisableInteriorEnterExits();
#endif
	// set real gamemode text
	SetGameModeText(MODE_NAME " " MODE_VERSION);
	// print completed data
	print(MODE_NAME " version " MODE_VERSION " loaded.");
	print("----------------------------------");
}

public OnGameModeExit()
{
#if defined _gangs_included
    Gangs_OnGameModeExit();
#endif
	print(MODE_NAME " version " MODE_VERSION " unloaded.");
	return 1;
}

public OnPlayerConnect(playerid)
{
    //if(IsPlayerNPC(playerid)) return 1;
	//if(IsValidNPC(playerid)) return 1;
	IsPlayerSpawned{playerid} = 0; // fix for Player_Update()  (death on spawn)
	// counter
    if(pcount < playerid) pcount = playerid;
    // pickups
    PlayerOnPickup[playerid] = 0;
    
	// systems
	//TogglePlayerClock(playerid, 1);
	
//SetPlayerCameraPos(playerid, 1476.370971, -1661.906860, 15.391772);	
//SetPlayerCameraLookAt(playerid, 1476.414916, -1665.778564, 14.387964);
#if defined _streamer_objects_included
    StreamerObj_OnPlayerConnect(playerid);
#endif
#if defined _streamer_checkpoints_included
    StreamerCP_OnPlayerConnect(playerid);
#endif
#if defined _streamer_icons_included
    StreamerIcon_OnPlayerConnect(playerid);
#endif
#if defined _player_included
    Player_OnPlayerConnect(playerid);
#endif
#if defined _houses_included
    Houses_OnPlayerConnect(playerid);
#endif
#if defined _quest_included
    Quest_OnPlayerConnect(playerid);
#endif
#if defined _minimissions_included
    MiniMissions_OnPlayerConnect(playerid);
#endif
#if defined _deathmatch_included
    Deathmatch_OnPlayerConnect(playerid);
#endif
#if defined _race_included
    Race_OnPlayerConnect(playerid);
#endif
#if defined _gangs_included
    Gangs_OnPlayerConnect(playerid);
#endif
#if defined _team_quests_included
    TeamQuest_OnPlayerConnect(playerid);
#endif
#if defined _click_included
	click_OnPlayerConnect(playerid);
#endif
#if defined _fuel_included
	Fuel_OnPlayerConnect(playerid);
#endif
#if defined _lottery_included
	Lottery_OnPlayerConnect(playerid);
	
#endif
	return 1;
}

public OnPlayerDisconnect(playerid,reason)
{
    if(IsPlayerNPC(playerid)) return 1;
	//if(IsValidNPC(playerid)) return 1;
	IsPlayerSpawned{playerid} = 0; // fix for Player_Update()  (death on spawn)
	// counter
    if(pcount == playerid)
	{
		do pcount--;
		while((IsPlayerNPC(playerid) || (IsPlayerConnected(pcount) == 0)) && (pcount > 0));
	}
    // pickups
    PlayerOnPickup[playerid] = 0;
	
	// systems
#if defined _streamer_objects_included
    StreamerObj_OnPlayerDisconnect(playerid);
#endif
#if defined _streamer_checkpoints_included
    StreamerCP_OnPlayerDisconnect(playerid);
#endif
#if defined _streamer_icons_included
    StreamerIcon_OnPlayerDisconnect(playerid);
#endif
#if defined _player_included
    Player_OnPlayerDisconnect(playerid,reason);
#endif
#if defined _minimissions_included
    MiniMissions_OnPlayerDisconnect(playerid,reason);
#endif
#if defined _deathmatch_included
    Deathmatch_OnPlayerDisconnect(playerid,reason);
#endif
#if defined _race_included
    Race_OnPlayerDisconnect(playerid,reason);
#endif
#if defined _team_quests_included
    TeamQuest_OnPlayerDisconnect(playerid,reason);
#endif
#if defined _missions_included
    Missions_OnPlayerDisconnect(playerid,reason);
#endif
#if defined _quest_included
    Quest_OnPlayerDisconnect(playerid,reason);
#endif
#if defined _factions_included
	Factions_OnPlayerDisconnect(playerid);
#endif
#if defined _fuel_included
	Fuel_OnPlayerDisconnect(playerid,reason);
#endif
#if defined _click_included
	click_OnPlayerDisconnect(playerid);
#endif
#if defined _gangs_included
	Gangs_OnPlayerDisconnect(playerid,reason);
#endif
#if defined _lottery_included
	Lottery_OnPlayerDisconnect(playerid);
	
#endif
	return 1;
}

public OnPlayerSpawn(playerid)
{
SetPlayerVirtualWorld(playerid,0);
if(IsPlayerNPC(playerid)) return 1;
#if defined DISABLE_STUNT_BONUS
	EnableStuntBonusForPlayer(playerid,false);
#endif
	// systems
#if defined _fuel_included
	Fuel_OnPlayerSpawn(playerid);
#endif
#if defined _player_included
    Player_OnPlayerSpawn(playerid);
#endif
#if defined _houses_included
    Houses_OnPlayerSpawn(playerid);
#endif
#if defined _deathmatch_included
    Deathmatch_OnPlayerSpawn(playerid);
#endif
#if defined _gangs_included
    Gangs_OnPlayerSpawn(playerid);
#endif
#if defined _team_quests_included
    TeamQuest_OnPlayerSpawn(playerid);
#endif
#if defined _factions_included
    Factions_OnPlayerSpawn(playerid);
#endif
#if defined _guide_included
	Guide_OnPlayerSpawn(playerid);
#endif
#if defined _click_included
	click_OnPlayerSpawn(playerid);
#endif
#if defined _world_included
    World_OnPlayerSpawn(playerid);
#endif
	IsPlayerSpawned{playerid} = 1; // fix for Player_Update()  (death on spawn)
	return 1;
}

public OnPlayerDeath(playerid, killerid, reason)
{
	IsPlayerSpawned{playerid} = 0; // fix for Player_Update()  (death on spawn)
	// kill chat
#if defined SHOW_KILL_STAT
    SendDeathMessage(killerid,playerid,reason);
#endif
	// systems
#if defined _protect_included
    Protect_OnPlayerDeath(playerid,killerid,reason);
#endif
#if defined _player_included
    Player_OnPlayerDeath(playerid,killerid,reason);
#endif
#if defined _deathmatch_included
    Deathmatch_OnPlayerDeath(playerid,killerid,reason);
#endif
#if defined _race_included
    Race_OnPlayerDeath(playerid,killerid,reason);
#endif
#if defined _gangs_included
    Gangs_OnPlayerDeath(playerid,killerid,reason);
#endif
#if defined _team_quests_included
    TeamQuest_OnPlayerDeath(playerid,killerid,reason);
#endif
#if defined _missions_included
    Missions_OnPlayerDeath(playerid,killerid,reason);
#endif
#if defined _world_included
    World_OnPlayerDeath(playerid);
#endif
#if defined _click_included
	click_OnPlayerDeath(playerid);
#endif
#if defined _factions_included
	Factions_OnPlayerDeath(playerid,killerid,reason);
#endif
	return 1;
}

public OnVehicleSpawn(vehicleid)
{
//systems
#if defined _player_included
	Player_OnVehicleSpawn(vehicleid);
#endif
#if defined _fuel_included
	Fuel_OnVehicleSpawn(vehicleid);
#endif
#if defined _factions_included
	Factions_OnVehicleSpawn(vehicleid);
#endif
	return 1;
}

public OnVehicleDeath(vehicleid, killerid)
{
//systmes
	return 1;
}

public OnPlayerText(playerid, text[])
{
	if(GetPVarInt(playerid,"Logged") != 1)
	{
		SendClientMessage(playerid,COLOR_RED,"Вы должны войти под своим аккаунтом что-бы писать в чат!");
		return 0;
	}
	// admin
#if defined _admin_included
    RegisterText(text,'@',Admin,ADMIN_REPORT_TEXT);
	if(GetPVarInt(playerid,"IsMuted")) return 0;
#endif
    // systems
#if defined _deathmatch_included
    RegisterText(text,'*',Deathmatch,DEATHMATCH_QUEST_TEXT);
#endif
#if defined _race_included
    RegisterText(text,'*',Race,RACE_QUEST_TEXT);
#endif
#if defined _gangs_included
    RegisterText(text,'!',Gangs,GANGS_TEAM_TEXT);
#endif
#if defined _team_quests_included
    RegisterText(text,'*',TeamQuest,TEAM_QUEST_TEAM_TEXT);
    RegisterText(text,'&',TeamQuest,TEAM_QUEST_ANSWER_TEXT);
#endif
	// gettime(itmp[1],itmp[2],itmp[3]);

	// format(stmp,sizeof(stmp),"{%06x}[%02d:%02d:%02d] {%06x}%s:{%06x} %s",COLOR_GREY>>>8,itmp[1],itmp[2],itmp[3],GetPlayerColor(playerid)>>>8,oGetPlayerName(playerid),COLOR_WHITE>>>8,text);
	// SendClientMessageToAll(-1,stmp);
	// if(strlen(stmp)>120) return 1;
	// //printf("SIZE IS %d",sizeof(stmp));
	// else return 0;
	return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	if(GetPVarInt(playerid,"Logged") != 1)
		return SendClientMessage(playerid,COLOR_RED,"Вы должны войти под своим аккаунтом что-бы использовать эту команду!");
    // systems
#if defined _player_included
	RegisterCmd(cmdtext,"/stats",6,Player,PLAYER_STAT_CMD);
	RegisterCmd(cmdtext,"/help",5,Player,PLAYER_HELP_CMD);
	RegisterCmd(cmdtext,"/savechar",9,Player,PLAYER_SAVECHAR_CMD);
	RegisterCmd(cmdtext,"/changepass",11,Player,PLAYER_CHANGEPASS_CMD);
	RegisterCmd(cmdtext,"/kill",5,Player,PLAYER_KILL_CMD);
	RegisterCmd(cmdtext,"/credits",8,Player,PLAYER_CREDITS_CMD);
	RegisterCmd(cmdtext,"/givecash",9,Player,PLAYER_GIVECASH_CMD);
	RegisterCmd(cmdtext,"/flip",5,Player,PLAYER_FLIP_CMD);
	RegisterCmd(cmdtext,"/upgrade",8,Player,PLAYER_UPGRADE_CMD);
	RegisterCmd(cmdtext,"/pm",3,Player,PLAYER_PM_CMD);
	RegisterCmd(cmdtext,"/info",5,Player,PLAYER_INFO_CMD);
	RegisterCmd(cmdtext,"/admins",7,Player,PLAYER_ADMINS_CMD);
	RegisterCmd(cmdtext,"/carresp",8,Admin,ADMIN_CARRESP_CMD);
	RegisterCmd(cmdtext,"/skydive",8,Player,PLAYER_SKYDIVE_CMD);
	RegisterCmd(cmdtext,"/plate",6,Player,PLAYER_PLATE_CMD);
	RegisterCmd(cmdtext,"/radio",6,Player,PLAYER_RADIO_CMD);
	RegisterCmd(cmdtext,"/savecar",8,Player,PLAYER_SAVECAR_CMD);
	RegisterCmd(cmdtext,"/beer",5,Player,PLAYER_BEER_CMD);
#endif
#if defined _world_included
    RegisterCmd(cmdtext,"/tp",3,World,WORLD_TELEPORT_CMD);
#endif
#if defined _houses_included
    RegisterCmd(cmdtext,"/h help",7,Houses,HOUSES_HELP_CMD);
    RegisterCmd(cmdtext,"/h info",7,Houses,HOUSES_INFO_CMD);
    RegisterCmd(cmdtext,"/h buy",6,Houses,HOUSES_BUY_CMD);
    RegisterCmd(cmdtext,"/h sell",7,Houses,HOUSES_SELL_CMD);
    RegisterCmd(cmdtext,"/h spawn",8,Houses,HOUSES_SPAWNSEL_CMD);
	RegisterCmd(cmdtext,"/h free",7,Houses,ADMIN_FREEHOUSE_CMD);
	RegisterCmd(cmdtext,"/h rebuild",10,Houses,ADMIN_REBUILD_CMD);
#endif
#if defined _admin_included
	RegisterCmd(cmdtext,"/cc",3,Admin,ADMIN_CC_CMD);
	RegisterCmd(cmdtext,"/cmdlist",8,Admin,ADMIN_CMDLIST_CMD);
	RegisterCmd(cmdtext,"/ssay",5,Admin,ADMIN_SSAY_CMD);
	RegisterCmd(cmdtext,"/boom",5,Admin,ADMIN_BOOM_CMD);
	RegisterCmd(cmdtext,"/setlvl",7,Admin,ADMIN_SETLVL_CMD);
	RegisterCmd(cmdtext,"/disarm",7,Admin,ADMIN_DISARM_CMD);
	RegisterCmd(cmdtext,"/remmoney",9,Admin,ADMIN_REMMONEY_CMD);
	RegisterCmd(cmdtext,"/agivemoney",11,Admin,ADMIN_GIVEMONEY_CMD);
	RegisterCmd(cmdtext,"/agivexp",8,Admin,ADMIN_GIVEXP_CMD);
	RegisterCmd(cmdtext,"/gweap",6,Admin,ADMIN_GWEAP_CMD);
	RegisterCmd(cmdtext,"/setstatus",10,Admin,ADMIN_SETSTATUS_CMD);
	RegisterCmd(cmdtext,"/ban",4,Admin,ADMIN_BAN_CMD);
	RegisterCmd(cmdtext,"/statuses",9,Admin,ADMIN_STATUSES_CMD);
	RegisterCmd(cmdtext,"/say",4,Admin,ADMIN_SAY_CMD);
	RegisterCmd(cmdtext,"/setskin",8,Admin,ADMIN_SETSKIN_CMD);
	RegisterCmd(cmdtext,"/kick",5,Admin,ADMIN_KICK_CMD);
	RegisterCmd(cmdtext,"/akill",6,Admin,ADMIN_AKILL_CMD);
	RegisterCmd(cmdtext,"/jail",5,Admin,ADMIN_JAIL_CMD);
	RegisterCmd(cmdtext,"/unjail",7,Admin,ADMIN_UNJAIL_CMD);
	RegisterCmd(cmdtext,"/paralyze",9,Admin,ADMIN_PARALYZE_CMD);
	RegisterCmd(cmdtext,"/deparalyze",11,Admin,ADMIN_DEPARALYZE_CMD);
	RegisterCmd(cmdtext,"/mute",5,Admin,ADMIN_MUTE_CMD);
	RegisterCmd(cmdtext,"/unmute",7,Admin,ADMIN_UNMUTE_CMD);
	RegisterCmd(cmdtext,"/tele-to",8,Admin,ADMIN_TELETO_CMD);
	RegisterCmd(cmdtext,"/tele-here",10,Admin,ADMIN_TELEHERE_CMD);
	RegisterCmd(cmdtext,"/teleport",9,Admin,ADMIN_TELEPORT_CMD);
	RegisterCmd(cmdtext,"/upoint",7,Admin,ADMIN_UPGRADEPOINT_CMD);
	RegisterCmd(cmdtext,"/jetpack",8,Admin,ADMIN_JETPACK_CMD);
#endif
#if defined _businesses_included
    RegisterCmd(cmdtext,"/b help",7,Businesses,BUSINESS_HELP_CMD);
    RegisterCmd(cmdtext,"/b info",7,Businesses,BUSINESS_INFO_CMD);
    RegisterCmd(cmdtext,"/b buy",6,Businesses,BUSINESS_BUY_CMD);
    RegisterCmd(cmdtext,"/b sell",7,Businesses,BUSINESS_SELL_CMD);
#endif
#if defined _banks_included
    RegisterCmd(cmdtext,"/bank help",10,Banks,BANKS_HELP_CMD);
    RegisterCmd(cmdtext,"/deposite",9,Banks,BANKS_DEPOSITE_CMD);
    RegisterCmd(cmdtext,"/withdraw",9,Banks,BANKS_WITHDRAW_CMD);
#endif
#if defined _deathmatch_included
	RegisterCmd(cmdtext,"/dm help",8,Deathmatch,DEATHMATCH_HELP_CMD);
	RegisterCmd(cmdtext,"/dm menu",8,Deathmatch,DEATHMATCH_MENU_CMD);
	RegisterCmd(cmdtext,"/dm quit",8,Deathmatch,DEATHMATCH_QUIT_CMD);
#endif
#if defined _race_included
	RegisterCmd(cmdtext,"/race help",10,Race,RACE_HELP_CMD);
	RegisterCmd(cmdtext,"/race menu",10,Race,RACE_MENU_CMD);
	RegisterCmd(cmdtext,"/race quit",10,Race,RACE_QUIT_CMD);
#endif
#if defined _gangs_included
    RegisterCmd(cmdtext,"/g help",7,Gangs,GANGS_HELP_CMD);
    RegisterCmd(cmdtext,"/g create",9,Gangs,GANGS_CREATE_CMD);
    RegisterCmd(cmdtext,"/g delete",9,Gangs,GANGS_DELETE_CMD);
    RegisterCmd(cmdtext,"/g colors",9,Gangs,GANGS_COLORS_CMD);
    RegisterCmd(cmdtext,"/g color",8,Gangs,GANGS_COLOR_CMD);
    RegisterCmd(cmdtext,"/g welcome",10,Gangs,GANGS_WELCOME_CMD);
    RegisterCmd(cmdtext,"/g invite",9,Gangs,GANGS_INVITE_CMD);
    RegisterCmd(cmdtext,"/g quit",7,Gangs,GANGS_QUIT_CMD);
    RegisterCmd(cmdtext,"/g kick",7,Gangs,GANGS_KICK_CMD);
    RegisterCmd(cmdtext,"/g members",10,Gangs,GANGS_MEMBERS_CMD);
    RegisterCmd(cmdtext,"/g stats",8,Gangs,GANGS_STATS_CMD);
    RegisterCmd(cmdtext,"/gz help",8,Gangs,GANGS_ZONE_HELP_CMD);
    RegisterCmd(cmdtext,"/gz info",8,Gangs,GANGS_ZONE_INFO_CMD);
	RegisterCmd(cmdtext,"/gz claim",9,Gangs,GANGS_ZONE_CLAIM_CMD);
    RegisterCmd(cmdtext,"/gw help",8,Gangs,GANGS_WAR_HELP_CMD);
    RegisterCmd(cmdtext,"/gw attack",10,Gangs,GANGS_WAR_ATTACK_CMD);
    RegisterCmd(cmdtext,"/gw join",8,Gangs,GANGS_WAR_JOIN_CMD);
    RegisterCmd(cmdtext,"/gw leave",9,Gangs,GANGS_WAR_LEAVE_CMD);
    RegisterCmd(cmdtext,"/gw start",9,Gangs,GANGS_WAR_START_CMD);
#endif
#if defined _team_quests_included
	RegisterCmd(cmdtext,"/tq help",8,TeamQuest,TEAM_QUEST_HELP_CMD);
	RegisterCmd(cmdtext,"/tq menu",8,TeamQuest,TEAM_QUEST_MENU_CMD);
	RegisterCmd(cmdtext,"/tq quit",8,TeamQuest,TEAM_QUEST_QUIT_CMD);
#endif
#if defined _missions_included
	RegisterCmd(cmdtext,"/dget",5,Missions,MISSIONS_DGET_CMD);
	RegisterCmd(cmdtext,"/dset",5,Missions,MISSIONS_DSET_CMD);
	RegisterCmd(cmdtext,"/dboom",6,Missions,MISSIONS_DBOOM_CMD);
#endif
#if defined _weapons_included
	RegisterCmd(cmdtext,"/w help",7,Weapons,WEAPONS_HELP_CMD);
	RegisterCmd(cmdtext,"/w menu",7,Weapons,WEAPONS_MENU_CMD);
	RegisterCmd(cmdtext,"/w drop",7,Weapons,WEAPONS_DROP_CMD);
#endif
#if defined _factions_included
	RegisterCmd(cmdtext,"/wanted",7,Factions,WANTED_CMD);
	RegisterCmd(cmdtext,"/fbihelp",8,Factions,FBIHELP_CMD);
	RegisterCmd(cmdtext,"/setmaincop",11,Factions,SETMAINCOP);
	RegisterCmd(cmdtext,"/setcop",7,Factions,SETCOP);
	RegisterCmd(cmdtext,"/cops",5,Factions,COPS_CMD);
	RegisterCmd(cmdtext,"/setmainfbi",11,Factions,SETMAINFBI);
	RegisterCmd(cmdtext,"/setfbi",7,Factions,SETFBI);
	RegisterCmd(cmdtext,"/fbis",5,Factions,FBIS_CMD);
	RegisterCmd(cmdtext,"/heli",5,Factions,HELI_CMD);
	RegisterCmd(cmdtext,"/copshelp",7,Factions,COPSHELP_CMD);
	RegisterCmd(cmdtext,"/arrest",7,Factions,ARREST_CMD);
	RegisterCmd(cmdtext,"/handsup",8,Factions,HANDSUP_CMD);
	RegisterCmd(cmdtext,"/suspect",8,Factions,SUSPECT_CMD);
	RegisterCmd(cmdtext,"/setmainthief",13,Factions,SETMAINTHIEF);
	RegisterCmd(cmdtext,"/setthief",9,Factions,SETTHIEF);
	RegisterCmd(cmdtext,"/thieves",8,Factions,THIEVES_CMD);
	RegisterCmd(cmdtext,"/thiefhelp",10,Factions,THIEFHELP_CMD);
	RegisterCmd(cmdtext,"/carwanted",10,Factions,CARWANTED_CMD);
	RegisterCmd(cmdtext,"/setracer",9,Factions,SETRACER);
	RegisterCmd(cmdtext,"/setmainracer",13,Factions,SETMAINRACER);
	RegisterCmd(cmdtext,"/racerhelp",10,Factions,RACERHELP_CMD);
	RegisterCmd(cmdtext,"/racers",7,Factions,RACERS_CMD);
#endif
#if defined _guide_included
	RegisterCmd(cmdtext,"/qadd",5,Guide,GUIDE_QADD_CMD);
	RegisterCmd(cmdtext,"/bug",4,Guide,GUIDE_BUG_CMD);
	
#endif
#if defined _lottery_included
	RegisterCmd(cmdtext,"/lottery",8,Lottery,LOTTERY_CMD);
	
#endif
	return SendClientMessage(playerid,COLOR_WHITE,"SERVER: Неизвестная команда");
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
if(IsPlayerNPC(playerid)) return 1;
	// systems
#if defined _npcs_included
	Npcs_OnPlayerEnterVehicle(playerid,vehicleid,ispassenger);
#endif
#if defined _missions_included
    Missions_OnPlayerEnterVehicle(playerid,vehicleid,ispassenger);
#endif
#if defined _utils_included
    Utils_OnPlayerEnterVehicle(playerid,vehicleid);
#endif
#if defined _player_included
	Player_OnPlayerEnterVehicle(playerid,vehicleid);
#endif
#if defined _fuel_included
	Fuel_OnPlayerEnterVehicle(playerid,vehicleid,ispassenger);
#endif

	return 1;
}

public OnPlayerGiveDamageActor(playerid, damaged_actorid, Float: amount, weaponid, bodypart)
{

#if defined _factions_included
	Factions_OPGDA(playerid, damaged_actorid, Float: amount, weaponid, bodypart);
#endif
return 1;
}
public OnPlayerTakeDamage(playerid, issuerid, Float: amount, weaponid, bodypart)
{

#if defined _player_included
	Player_OnPlayerTakeDamage(playerid, issuerid, Float: amount, weaponid, bodypart);
#endif
return 1;
}
public OnPlayerExitVehicle(playerid, vehicleid)
{
#if defined _fuel_included
Fuel_OnPlayerExitVehicle(playerid,vehicleid);
#endif
#if defined _npcs_included
Npcs_OnPlayerExitVehicle(playerid, vehicleid);
#endif
	return 1;
}

public OnPlayerStateChange(playerid, newstate, oldstate)
{
#if defined _player_included
if((oldstate == PLAYER_STATE_DRIVER || PLAYER_STATE_PASSENGER)&& IsPlayerSpawned{playerid}==1 && GetPVarInt(playerid,"SeenIntro")==1) StopAudioStreamForPlayer(playerid);
#endif
#if defined _fuel_included
	Fuel_OnPlayerStateChange(playerid,newstate,oldstate);
#endif

	return 1;
}

public OnPlayerEnterCheckpoint(playerid)
{
	// systems
#if defined _streamer_checkpoints_included
    StreamerCP_OnPlayerEnterCP(playerid);
#endif
#if defined _missions_included
    Missions_OPEC(playerid);
#endif
#if defined _guide_included
	Guide_OPEC(playerid);
#endif
	return 1;
}

public OnPlayerLeaveCheckpoint(playerid)
{
	return 1;
}

public OnPlayerEnterRaceCheckpoint(playerid)
{
	// systems
#if defined _race_included
	Race_OnPlayerEnterRaceCP(playerid);
#endif
#if defined _missions_included
    Missions_OPERC(playerid);
#endif
	return 1;
}

public OnPlayerLeaveRaceCheckpoint(playerid)
{
	return 1;
}

public OnRconCommand(cmd[])
{
	return 1;
}

public OnPlayerRequestClass(playerid,classid)
{
	// systems
#if defined _player_included
	Player_OnPlayerRequestClass(playerid,classid);
#endif
#if defined _world_included
    World_OnPlayerRequestClass(playerid,classid);
#endif
	return 1;
}

public OnObjectMoved(objectid)
{
	return 1;
}

public OnPlayerObjectMoved(playerid, objectid)
{
	return 1;
}

public OnPlayerPickUpPickup(playerid, pickupid)
{
    // pickups
    if((GetTickCount() - PlayerOnPickup[playerid]) < 1000)
	{
	    PlayerOnPickup[playerid] = GetTickCount();
		return 1;
	}
    PlayerOnPickup[playerid] = GetTickCount();
    
    // systems
#if defined _houses_included
    Houses_OnPlayerPickUpPickup(playerid,pickupid);
#endif
#if defined _businesses_included
    Businesses_OnPlayerPickUpPickup(playerid,pickupid);
#endif
#if defined _banks_included
    Banks_OnPlayerPickUpPickup(playerid,pickupid);
#endif
#if defined _missions_included
    Missions_OnPlayerPickUpPickup(playerid,pickupid);
#endif
#if defined _missions_included
	Weapons_OnPlayerPickUpPickup(playerid,pickupid);
#endif
	return 1;
}

public OnVehicleMod(playerid, vehicleid, componentid)
{
	return 1;
}

public OnVehiclePaintjob(playerid, vehicleid, paintjobid)
{
	format(stmp,sizeof(stmp),"PJOBCHANGE TO %d", paintjobid);
	SendClientMessage(playerid, COLOR_RED, stmp);
	#if defined _utils_included 
	SetVehiclePaintJob(vehicleid,paintjobid);
	#endif
	return 1;
}

public OnVehicleRespray(playerid, vehicleid, color1, color2)
{
#if defined _player_included
	Player_OnVehicleRespray(playerid, vehicleid, color1, color2);
#endif
	return 1;
}

public OnPlayerSelectedMenuRow(playerid, row)
{
	return 1;
}

public OnPlayerExitedMenu(playerid)
{
	return 1;
}

public OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid)
{
	return 1;
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
#if defined _click_included
if( PRESSED (KEY_YES)) return click_OnPlayerClickSelf(playerid);
#endif
if( PRESSED ( KEY_NO ) )
	{
		#if defined _houses_included
		if( IsPlayerAtHouse(playerid) ) return ShowPlayerDialog(playerid,DIALOG_HMENU,DIALOG_STYLE_LIST,"Дом","Купить дом\nПродать дом\nНазначить местом спавна\nИнфо о доме"," Выбрать"," Отмена");
		#endif
		#if defined _businesses_included
		if(IsPlayerAtBusiness(playerid)) return ShowPlayerDialog(playerid,DIALOG_BMENU,DIALOG_STYLE_LIST,"Бизнес","Купить\nПродать\nИнфо"," Выбрать"," Отмена");
		#endif
		#if defined _banks_included
		if( IsPlayerAtBank(playerid) ) return ShowPlayerDialog(playerid,DIALOG_BANKMENU,DIALOG_STYLE_LIST,"Банк","Положить деньги\nВзять деньги\nБаланс"," Выбрать"," Отмена");
		#endif
		#if defined _weapons_included
		if( IsPlayerAtAmmunation(playerid)) return Weapons_Command(playerid,WEAPONS_MENU_CMD," ");
		#endif
		#if defined _factions_included
		if(IsPlayerAtSupply(playerid)) return Factions_SupplyPlayer(playerid);
		if(IsPlayerInAnyVehicle(playerid)) return Factions_VehInteract(playerid);
		#endif
		#if defined _guide_included
		return ShowPlayerDialog(playerid,DIALOG_HELP,DIALOG_STYLE_LIST,"Помощь","Инфо о моде\nИгрок\nОружие\nБанды\nБизнес\nФракции\nГонки\nТимквесты\nДезматчи"," Выбрать"," Отмена");
		#endif
		//return 1;
	}
#if defined _gangs_included	
if( PRESSED (KEY_CTRL_BACK)&& !IsPlayerInAnyVehicle(playerid)) return Gang_Menu(playerid);
#endif

if(PRESSED(KEY_SECONDARY_ATTACK))
	{
		if(GetVehicleModel(GetPlayerVehicleID(playerid))==594) //fix for RC cam
		{
		SetVehicleToRespawn(GetPlayerVehicleID(playerid));
		
		}
	
	}
return 1;
}

public OnRconLoginAttempt(ip[], password[], success)
{
	return 1;
}

public OnPlayerUpdate(playerid)
{
	if(GetPlayerVirtualWorld(playerid) == STUNT_VIRTUAL_WORLD)
	{
		SetPlayerArmedWeapon(playerid,0);
		oSetPlayerHealth(playerid,100.0);
		if(IsPlayerInAnyVehicle(playerid)) RepairVehicle(GetPlayerVehicleID(playerid));
	}
	// protect
#if defined _protect_included
	if(IsPlayerSpawned{playerid} == 1) if(!Protect_OnPlayerUpdate(playerid)) return 0;
#endif
	// streamers
#if defined ON_PLAYER_UPDATE_STREAMERS

#if defined _streamer_objects_included
    StreamerObj_OnPlayerUpdate(playerid);
#endif
#if defined _streamer_checkpoints_included
    StreamerCP_OnPlayerUpdate(playerid);
#endif
#if defined _streamer_icons_included
    StreamerIcon_OnPlayerUpdate(playerid);
#endif

#endif
	// systems
#if defined _admin_included
    Admin_OnPlayerUpdate(playerid);
#endif
#if defined _player_included
    Player_OnPlayerUpdate(playerid);
#endif
#if defined _deathmatch_included
    Deathmatch_OnPlayerUpdate(playerid);
#endif
#if defined _world_included
    World_OnPlayerUpdate(playerid);
#endif
	// fix glases
	if((GetPlayerWeapon(playerid) == WEAPON_CAMERA) || (GetPlayerWeapon(playerid) == 44) || (GetPlayerWeapon(playerid) == 45))
	{
	    new k[3];
	    GetPlayerKeys(playerid,k[0],k[1],k[2]);
	    if(k[0] & KEY_FIRE) return 0;
	}
	// spys
#if defined VEHICLE_HEALTH_CHANGE_SPY
	static
		Float:PlayerVehicleHealth[MAX_PLAYERS],
		Float:v_health,
		vid;
    if(IsPlayerInAnyVehicle(playerid))
	{
		vid = GetPlayerVehicleID(playerid);
		GetVehicleHealth(vid,v_health);
		if(v_health != PlayerVehicleHealth[playerid])
		{
			// OnPlayerVehicleHealthChange
			OnPlayerVehicleHealthChange(playerid,vid,PlayerVehicleHealth[playerid],v_health);
			// data update
			PlayerVehicleHealth[playerid] = v_health;
		}
	}
#endif
#if defined PLAYER_HEALTH_CHANGE_SPY
	static
		Float:PlayerHealth[MAX_PLAYERS],
		Float:p_health;
	GetPlayerHealth(playerid,p_health);
	if(p_health != PlayerHealth[playerid])
	{
		// OnPlayerHealthChange
		OnPlayerHealthChange(playerid,PlayerHealth[playerid],p_health);
		// data update
		PlayerHealth[playerid] = p_health;
	}
#endif
#if defined PLAYER_ARMOUR_CHANGE_SPY
	static
		Float:PlayerArmour[MAX_PLAYERS],
		Float:p_armour;
	GetPlayerHealth(playerid,p_armour);
	if(p_armour != PlayerArmour[playerid])
	{
		// OnPlayerArmourChange
		OnPlayerArmourChange(playerid,PlayerArmour[playerid],p_armour);
		// data update
		PlayerArmour[playerid] = p_armour;
	}
#endif
#if defined PLAYER_WEAPON_CHANGE_SPY
	static
		PlayerWeapon[MAX_PLAYERS char],
		p_weapon;
	p_weapon = GetPlayerWeapon(playerid);
	printf("p_weapon %d",p_weapon); // check invalid data
	if(p_weapon != PlayerWeapon{playerid})
	{
		// OnPlayerWeaponChange
		OnPlayerWeaponChange(playerid,PlayerWeapon{playerid},p_weapon);
		// data update
		PlayerWeapon{playerid} = p_weapon;
	}
#endif
#if defined PLAYR_SPECIALACTION_CHANGE_SPY //disabled
	static
		PlayerSpecialAction[MAX_PLAYERS char],
		p_specialaction;
	p_specialaction = GetPlayerSpecialAction(playerid);
	if(p_specialaction != PlayerSpecialAction{playerid})
	{
		// OnPlayerSpecialActionChange
		OnPlayerSpecialActionChange(playerid,PlayerSpecialAction{playerid},p_specialaction);
		// data update
		PlayerSpecialAction{playerid} = p_specialaction;
	}
#endif
	return 1;
}

public OnPlayerStreamIn(playerid, forplayerid)
{
// #if defined _click_included
	// click_OnPlayerStreamIn(playerid, forplayerid);
// #endif
	return 1;
}

public OnPlayerStreamOut(playerid, forplayerid)
{
	return 1;
}

public OnVehicleStreamIn(vehicleid, forplayerid)
{
	// systems
#if defined _minimissions_included
    MiniMissions_OnVehicleStreamIn(vehicleid,forplayerid);
#endif

	return 1;
}

public OnVehicleStreamOut(vehicleid, forplayerid)
{
	return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	switch(dialogid)
	{
		// systems
#if defined _player_included
        RegisterDialog(playerid,response,listitem,inputtext,Player,DIALOG_RADIO);
	    RegisterDialog(playerid,response,listitem,inputtext,Player,DIALOG_LOGIN);
	    RegisterDialog(playerid,response,listitem,inputtext,Player,DIALOG_REGISTER);
		RegisterDialog(playerid,response,listitem,inputtext,Player,DIALOG_UPGRADE);
#endif
#if defined _world_included
	    RegisterDialog(playerid,response,listitem,inputtext,World,DIALOG_TELEPORTS);
#endif
#if defined _deathmatch_included
	    RegisterDialog(playerid,response,listitem,inputtext,Deathmatch,DIALOG_DEATHMATCHES);
#endif
#if defined _race_included
	    RegisterDialog(playerid,response,listitem,inputtext,Race,DIALOG_RACES);
#endif
#if defined _gangs_included
	    RegisterDialog(playerid,response,listitem,inputtext,Gangs,DIALOG_GANG_INVITE);
	    RegisterDialog(playerid,response,listitem,inputtext,Gangs,DIALOG_GANG_WAR);
	    RegisterDialog(playerid,response,listitem,inputtext,Gangs,DIALOG_GANG_WAR_WEAPONS);
		RegisterDialog(playerid,response,listitem,inputtext,Gangs,DIALOG_GANG);
		RegisterDialog(playerid,response,listitem,inputtext,Gangs,DIALOG_GANG_CAPITAL);
#endif
#if defined _guide_included
		RegisterDialog(playerid,response,listitem,inputtext,Guide,DIALOG_HELP);
		RegisterDialog(playerid,response,listitem,inputtext,Guide,DIALOG_HELP2);
		RegisterDialog(playerid,response,listitem,inputtext,Guide,DIALOG_HELP3);
		RegisterDialog(playerid,response,listitem,inputtext,Guide,DIALOG_HELP4);
#endif
#if defined _team_quests_included
	    RegisterDialog(playerid,response,listitem,inputtext,TeamQuest,DIALOG_TEAM_QUESTS);
	    RegisterDialog(playerid,response,listitem,inputtext,TeamQuest,DIALOG_TEAM_QUEST_TEAM);
#endif
#if defined _missions_included
	    RegisterDialog(playerid,response,listitem,inputtext,Missions,DIALOG_MISSION_BANK);
#endif
#if defined _weapons_included
		RegisterDialog(playerid,response,listitem,inputtext,Weapons,DIALOG_WEAPONS);
#endif
#if defined _click_included
		RegisterDialog(playerid,response,listitem,inputtext,Click,DIALOG_CLICK);
		RegisterDialog(playerid,response,listitem,inputtext,Click,DIALOG_CLICK2);
		RegisterDialog(playerid,response,listitem,inputtext,Click,DIALOG_CLICKSELF);
		RegisterDialog(playerid,response,listitem,inputtext,Click,DIALOG_DRINK);
		RegisterDialog(playerid,response,listitem,inputtext,Click,DIALOG_FIGHT);
		RegisterDialog(playerid,response,listitem,inputtext,Click,DIALOG_INVIZ);
		
#endif
#if defined _factions_included
		RegisterDialog(playerid,response,listitem,inputtext,Factions,DIALOG_INCAR);
		RegisterDialog(playerid,response,listitem,inputtext,Factions,DIALOG_PLATE);
		RegisterDialog(playerid,response,listitem,inputtext,Factions,DIALOG_NEON);	
		RegisterDialog(playerid,response,listitem,inputtext,Factions,DIALOG_WIN);
#endif
#if defined _houses_included
		RegisterDialog(playerid,response,listitem,inputtext,Houses,DIALOG_HMENU);
#endif
#if defined _businesses_included
		RegisterDialog(playerid,response,listitem,inputtext,Businesses,DIALOG_BMENU);
#endif
#if defined _banks_included
	RegisterDialog(playerid,response,listitem,inputtext,Banks,DIALOG_BANKMENU);
	RegisterDialog(playerid,response,listitem,inputtext,Banks,DIALOG_BANKMENU2);
	RegisterDialog(playerid,response,listitem,inputtext,Banks,DIALOG_BANKMENU3);	
#endif
#if defined _lottery_included
	RegisterDialog(playerid,response,listitem,inputtext,Banks,DIALOG_LOTTERY);
#endif
	}
	return 1;
}

public OnPlayerClickPlayer(playerid, clickedplayerid, source)
{
    click_OnPlayerClickPlayer(playerid, clickedplayerid);
	return 1;
}

// -----------------------------------------------------------------------------
//                                  Other callbacks
// -----------------------------------------------------------------------------

//public Check()
//{
	// antisleep plugin
//#if defined _antisleep_included
    //AntiSleepUpdate();
//#endif
//}

#if defined _streamer_checkpoints_included
public OnPlayerEnterStreamedCheckpoint(playerid,checkpointid)
{
#if defined _GH_included
	GH_OPESC(playerid,checkpointid);
#endif
#if defined _factions_included
	Factions_OPESC(playerid,checkpointid);
#endif
#if defined _missions_included
    Missions_OPESC(playerid,checkpointid);
#endif

}
#endif

#if defined _quest_included
public OnPlayerChangeQuest(playerid,oldquest,newquest)
{

}
#endif

#if defined VEHICLE_HEALTH_CHANGE_SPY
forward OnPlayerVehicleHealthChange(playerid,vehicleid,Float:old_health,Float:new_health);
public OnPlayerVehicleHealthChange(playerid,vehicleid,Float:old_health,Float:new_health)
{

#if defined _missions_included
    Missions_OPVHC(playerid,vehicleid,old_health,new_health);
#endif

}
#endif

#if defined PLAYER_HEALTH_CHANGE_SPY
forward OnPlayerHealthChange(playerid,Float:old_health,Float:new_health);
public OnPlayerHealthChange(playerid,Float:old_health,Float:new_health)
{

}
#endif

#if defined PLAYER_ARMOUR_CHANGE_SPY
forward OnPlayerArmourChange(playerid,Float:old_armour,Float:new_armour);
public OnPlayerArmourChange(playerid,Float:old_armour,Float:new_armour)
{

}
#endif

#if defined PLAYER_WEAPON_CHANGE_SPY
forward OnPlayerWeaponChange(playerid,old_weapon,new_weapon);
public OnPlayerWeaponChange(playerid,old_weapon,new_weapon)
{

}
#endif

#if defined PLAYER_SPECIALACTION_CHANGE_SPY
forward OnPlayerSpecialActionChange(playerid,old_special_action,new_special_action);
public OnPlayerSpecialActionChange(playerid,old_special_action,new_special_action)
{

#if defined _protect_included
    Protect_OPSAC(playerid,old_special_action,new_special_action);
#endif

}
#endif

#if defined _player_included
public OnSavePlayerData(playerid,File:datafile,reason)
{
#if defined _businesses_included
    Business_OnSavePlayerData(playerid,datafile,reason);
#endif
#if defined _houses_included
    Houses_OnSavePlayerData(playerid,datafile,reason);
#endif
#if defined _banks_included
    Banks_OnSavePlayerData(playerid,datafile,reason);
#endif
#if defined _admin_included
    Admin_OnSavePlayerData(playerid,datafile,reason);
#endif
#if defined _gangs_included
    Gangs_OnSavePlayerData(playerid,datafile,reason);
#endif
#if defined _factions_included
	Factions_OnSavePlayerData(playerid,File:datafile,reason);
#endif
#if defined _weapons_included
	Weapons_OnSavePlayerData(playerid,datafile,reason);
#endif
}

public OnLoadPlayerData(playerid,datastr[],separatorpos)
{
#if defined _businesses_included
    Business_OnLoadPlayerData(playerid,datastr,separatorpos);
#endif
#if defined _houses_included
    Houses_OnLoadPlayerData(playerid,datastr,separatorpos);
#endif
#if defined _banks_included
    Banks_OnLoadPlayerData(playerid,datastr,separatorpos);
#endif
#if defined _admin_included
    Admin_OnLoadPlayerData(playerid,datastr,separatorpos);
#endif
#if defined _gangs_included
    Gangs_OnLoadPlayerData(playerid,datastr,separatorpos);
#endif
#if defined _factions_included
	Factions_OnLoadPlayerData(playerid,datastr,separatorpos);
#endif
#if defined _weapons_included
	Weapons_OnLoadPlayerData(playerid,datastr,separatorpos);
#endif
}
#endif
public OnPlayerWinsRace(playerid,place)
{
//systems
#if defined _factions_included
	Factions_OnPlayerWinsRace(playerid,place);
#endif
return 1;
}
public OnPlayerClickTextDraw(playerid,Text:clickedid)
{
//systems
#if defined _factions_included
	Factions_OPCTD(playerid,clickedid);
#endif
#if defined _lottery_included
	Lt_OnPlayerClickTextDraw(playerid,Text:clickedid);
#endif
return 1;
}



public OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid)
{
#if defined _lottery_included
	Lt_OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid);
#endif

return 1;
}
// OnCameraLeaveNode(playerid, camera, node)
// {
// #if defined _player_included

// #endif
// return 1;
// }

public OnCameraReachNode(playerid, camera, node)
{
#pragma unused camera
#if defined _player_included
camera_handler(playerid,node);
#endif
//return 1;
}
public FiveSecTimer()
{

#if defined _factions_included
	Factions_tick();
#endif

#if defined _player_included
	Player_Update();
#endif
#if defined _holidays_included
	//Holidays_tick();
	Nightmare_tick();
#endif
#if defined _GH_included
	GH_Tick();
#endif
#if defined _businesses_included
    Business_Update();
#endif

return 1;
}
public OneSecTimer()
{
//print("fired overal");
#if defined _protect_included
	Protect_tick();
	//print("Protect is not ok");
#endif

#if defined _utils_included
//MakeTPS();
#endif
#if defined _world_included
sync_Time();
#endif
#if defined _npcs_included
	NPC_Tick();
#endif
#if defined _fuel_included
	Fuel_tick();
#endif
#if !defined ON_PLAYER_UPDATE_STREAMERS
#if defined _streamer_objects_included
	//Streamer_Objects();
	//print("objects streaming");
#endif
#if defined _streamer_icons_included
	Streamer_Icons();
#endif
#if defined _streamer_checkpoints_included
	Streamer_Checkpoints();
#endif
#endif
#if defined _click_included
	click_update();
#endif
#if defined _player_included
	Player_UpdateLag();
#endif

#if defined _race_included
	RaceTick();
#endif
#if defined _team_quests_included
	
	TeamQuestsTick();
#endif


#if defined _gangs_included

	Gangs_Update();
#endif

#if defined _minimissions_included
	
	MiniMissionsTick();
#endif



//Sync_Zone();

return 1;
}
#undef SetPlayerVirtualWorld
oSetPlayerVirtualWorld(playerid,VWorld)
{
SetPlayerVirtualWorld(playerid,VWorld);
OnPlayerVirtualWorldChange(playerid,VWorld);
return 1;
}
stock OnPlayerVirtualWorldChange(playerid,VWorld)
{
#if defined _streamer_icons_included
	StreamerIcon_OPVWC(playerid,VWorld);
#endif
#if defined _streamer_checkpoints_included
	StreamerCP_OPVWC(playerid,VWorld);
#endif
#if defined _streamer_objects_included
	StreamerObj_OPVWC(playerid,VWorld);
#endif
return 1;
}