/*
*	Created:			04.06.10
*	Author:				009
*	Description:		обьекты, транспорт, классы.
*/

#if defined _world_included
	#endinput
#endif

#define _world_included
#pragma library world

new Text:txtTimeDisp;
// --------------------------------------------------
// defines
// --------------------------------------------------
#define TELEPORT_TICK_TIME		60000
#define WORLD_DIR				"world/"
#define OBJECTS_DIR				"objects/"
#define VEHICLES_DIR			"vehicles/"
#define TELEPORTS_FILE			"teleports.txt"
#define OBJECTS_FILE			"objects.txt"
#define VEHICLES_FILE			"vehicles.txt"
#define SELECT_CLASS_INTERIOR	0
#define TIMEZONE_OFFSET			3  //offset is made regarding your local time, not UTC
#define SELECT_CLASS_POS		1476.4343,-1665.8435,14.5532
//#define SELECT_CLASS_POS		-1637.898315,-2239.265136,31.476562
#define SELECT_CLASS_ANGLE		360
#define SELECT_CLASS_CAMERA		1476.370971, -1661.906860, 15.391772
#define SELECT_CLASS_LOOK		1476.414916, -1665.778564, 14.387964
#define USE_SYNC_TIME
// check functions
#if !defined StripNL
		#error StripNL not found in script.
#endif
#if !defined strtok
	#error strtok not found in script.
#endif
#if !defined IsValidObjectModel
	#error IsValidObjectModel not found in script.
#endif
#if !defined IsValidVehicleModel
	#error IsValidVehicleModel not found in script.
#endif
#if !defined IsValidSkin
	#error IsValidSkin not found in script.
#endif

// --------------------------------------------------
// enums
// --------------------------------------------------
enum
{
	WORLD_TELEPORT_CMD
};

// --------------------------------------------------
// statics
// --------------------------------------------------

// --------------------------------------------------
// forwards
// --------------------------------------------------
forward sync_Time();

// --------------------------------------------------
// publics
// --------------------------------------------------
public sync_Time()
{
	new Hour,Minut;
    new timestr[32];
	gettime(Hour,Minut);
	Hour = GetHourSummed(Hour,TIMEZONE_OFFSET);
	SetWorldTime(Hour);
	format(timestr,32,"%02d:%02d",Hour,Minut);
	TextDrawSetString(txtTimeDisp,timestr);
	//for(new playerid = 0;playerid <= GetPlayersCount();playerid++)
	foreach(new playerid : Player)
	{
		SetPlayerTime(playerid,Hour,Minut);
	}
		
}

// --------------------------------------------------
// stocks
// --------------------------------------------------
stock GetHourSummed(currentHour,offset)
{
	new resultHour = currentHour+offset;
	if(resultHour > 23)
		return resultHour-24;
	else if(resultHour < 0)
		return resultHour+24;
	return resultHour;
}
stock LoadObjectsFromFile(filename[],VirtualWorld=0)
{
	printf("NOW LOADING FROM %s",filename);
	new File:df = fopen(filename,io_read),
		k,
		c,
		string[128];
	if(df)
	{
		while(fread(df,string))
		{
			//print("READING");
			if(!CheckFileLine(stmp)) continue;
			// null and strip
			k = 0;
			// model
			itmp[0] = strval(strtok(string,k,','));
			// coords
			ftmp[0] = floatstr(strtok(string,k,','));
			ftmp[1] = floatstr(strtok(string,k,','));
			ftmp[2] = floatstr(strtok(string,k,','));
			// angles
			ftmp[3] = floatstr(strtok(string,k,','));
			ftmp[4] = floatstr(strtok(string,k,','));
			ftmp[5] = floatstr(strtok(string,k,','));
			// check model
			//print("MODEL CHECK");
			if(!IsValidObjectModel(itmp[0])) continue;
			// create object
			//print("OK. ATTEMPTING CREATION");
#if defined _streamer_objects_included
			CreateStreamedObject(itmp[0],ftmp[0],ftmp[1],ftmp[2],ftmp[3],ftmp[4],ftmp[5],VirtualWorld);
#else
			if(c < MAX_OBJECTS) CreateObject(itmp[0],ftmp[0],ftmp[1],ftmp[2],ftmp[3],ftmp[4],ftmp[5]);
#endif
			// counter
			c++;
		}
		fclose(df);
	}
	return c;
}
stock LoadVehiclesFromFile(filename[])
{
	new File:df = fopen(filename,io_read),
		k,
		c,
		string[128];
	if(df)
	{
		while(fread(df,string))
		{
			
			if(!CheckFileLine(stmp)) continue;
			// null
			k = 0;
			// model
			itmp[0] = strval(strtok(string,k,','));
			// coords
			ftmp[0] = floatstr(strtok(string,k,','));
			ftmp[1] = floatstr(strtok(string,k,','));
			ftmp[2] = floatstr(strtok(string,k,','));
			// angle
			ftmp[3] = floatstr(strtok(string,k,','));
			// check model
			if(!IsValidVehicleModel(itmp[0])) continue;
			// create vehicle
			new clr1=random(128);
			new clr2=random(128);
			new vehid = AddStaticVehicle(itmp[0],ftmp[0],ftmp[1],ftmp[2],ftmp[3],clr1,clr2);
			col1{vehid}=clr1;
			col2{vehid}=clr2;
			new plat[32];
			format(plat,sizeof(plat),"WOG %d%d%d%d",random(9),random(9),random(9),random(9));
			SetVehicleNumberPlate(vehid,plat);
			// counter
			c++;
		}
		fclose(df);
	}
	return c;
}

stock TeleportTo(playerid,pointid)
{
	new File:df = fopen(MODE_DIR WORLD_DIR TELEPORTS_FILE,io_read),
		c;
	if(df)
	{
		while(fread(df,stmp))
		{
			if(!CheckFileLine(stmp)) continue;
			
			if(c == pointid)
			{
				c = 0;
				// coords
				itmp[1] = strval(strtok(stmp,c,','));
				ftmp[0] = floatstr(strtok(stmp,c,','));
				ftmp[1] = floatstr(strtok(stmp,c,','));
				ftmp[2] = floatstr(strtok(stmp,c,','));
				ftmp[3] = floatstr(strtok(stmp,c,'|'));
				// teleport	
				if(IsPlayerInAnyVehicle(playerid))
				{
					itmp[0] = GetPlayerVehicleID(playerid);
					SetVehicleVirtualWorld(itmp[0],((itmp[1] == 1)?STUNT_VIRTUAL_WORLD:0));
					SetPlayerVirtualWorld(playerid,((itmp[1] == 1)?STUNT_VIRTUAL_WORLD:0));
					SetVehiclePos(itmp[0],ftmp[0],ftmp[1],ftmp[2]);
					SetVehicleZAngle(itmp[0],ftmp[3]);
					PutPlayerInVehicle(playerid,itmp[0],0);
				}
				else
				{
					SetPlayerVirtualWorld(playerid,((itmp[1] == 1)?STUNT_VIRTUAL_WORLD:0));
					SetPlayerPos(playerid,ftmp[0],ftmp[1],ftmp[2]);
					SetPlayerFacingAngle(playerid,ftmp[3]);
				}
				break;
			}
			else c++;
		}
		fclose(df);
	}
	else print("[ERROR] teleports file not exist");
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
World_OnGameModeInit()
{
	static 
		i,
		counter[3];
	new 
		k,
		string[128];
	// add all worked classes
	for(i = 0;i < 300;i++)
	{
		if(IsValidSkin(i)) 
		{
			AddPlayerClass(i,0.0,0.0,0.0,0.0,0,0,0,0,0,0);
			// counter
			counter[0]++;
		}
	}
	// load objects from file
	new File:df = fopen(MODE_DIR WORLD_DIR OBJECTS_FILE,io_read);
	if(df)
	{
		while(fread(df,stmp))
		{
			if(!CheckFileLine(stmp)) continue;
			// load file
			k=0;
			string = strtok(stmp,k,',');
			format(string,sizeof(string),MODE_DIR WORLD_DIR OBJECTS_DIR "%s",string);
			if(fexist(string)) counter[1] += LoadObjectsFromFile(string,((strval(strtok(stmp,k,',')) == 1)?STUNT_VIRTUAL_WORLD:0));
		}
		fclose(df);
	}
	// load vehicles from file
	df = fopen(MODE_DIR WORLD_DIR VEHICLES_FILE,io_read);
	if(df)
	{
		while(fread(df,stmp))
		{
			if(!CheckFileLine(stmp)) continue;
			// load file
			format(stmp,sizeof(stmp),MODE_DIR WORLD_DIR VEHICLES_DIR "%s",stmp);
			if(fexist(stmp)) counter[2] += LoadVehiclesFromFile(stmp);
		}
		fclose(df);
	}
	// data
#if defined PRINT_STATS_DATA
	printf("Classes: %d",counter[0]);
	printf("Objects: %d",counter[1]);
	printf("Vehicles: %d",counter[2]);
	#if defined _streamer_objects_included
		print("Objects streamer used");
	#else
		print("Objects streamer not used");
	#endif
	#if defined USE_SYNC_TIME
		sync_Time();
		print("Sync time used");
	#else
		print("Sync time not used");
	#endif
#endif
    txtTimeDisp = TextDrawCreate(605.0,25.0,"00:00");
	TextDrawUseBox(txtTimeDisp, 0);
	TextDrawFont(txtTimeDisp, 3);
	TextDrawSetShadow(txtTimeDisp,0); // no shadow
    TextDrawSetOutline(txtTimeDisp,2); // thickness 1
    TextDrawBackgroundColor(txtTimeDisp,0x000000FF);
    TextDrawColor(txtTimeDisp,0xFFFFFFFF);
    TextDrawAlignment(txtTimeDisp,3);
	TextDrawLetterSize(txtTimeDisp,0.5,1.5);
	
	
	
	print("World loaded.");
}

World_OnPlayerRequestClass(playerid,classid)
{
	#pragma unused classid
	#if defined _holidays_included
		if(CheckNY()) 
		{
		Holidays_OnPlayerRequestClass(playerid,classid);
		return 1;
		}
	#endif
	SetPlayerInterior(playerid,SELECT_CLASS_INTERIOR);
	SetPlayerPos(playerid,SELECT_CLASS_POS);
	SetPlayerFacingAngle(playerid,SELECT_CLASS_ANGLE);
	SetPlayerCameraPos(playerid,SELECT_CLASS_CAMERA);
	SetPlayerCameraLookAt(playerid,SELECT_CLASS_LOOK);
	return 1;
}

World_Command(playerid,cmdid,params[])
{
	#pragma unused params
	switch(cmdid)
	{
		case WORLD_TELEPORT_CMD:
		{
			// check quest
			if(GetPlayerQuest(playerid) != INVALID_QUEST_ID) return SendClientMessage(playerid,SYSTEM_COLOR,"Вы выполняете задание");
			// file read
			new File:datafile = fopen(MODE_DIR WORLD_DIR TELEPORTS_FILE,io_read),
				line[MAX_STRING];
			// checker
			if(strlen(params) > 0) itmp[0] = 1;
			else itmp[0] = 0;
			itmp[1] = 0;
			if(datafile)
			{
				stmp[0] = 0;
				while(fread(datafile,line))
				{
					if(!CheckFileLine(line)) continue;
					itmp[2] = strfind(line,"|") + 1;
					if(itmp[0])
					{
						if(!strcmp(params,line[ itmp[2] ],true))
						{
							TeleportTo(playerid,itmp[1]);
							fclose(datafile);
							return 1;
						}
						itmp[1]++;
					}

					format(stmp,sizeof(stmp),"%s%s\n",stmp,line[ itmp[2] ]);
				}
				fclose(datafile);
				ShowPlayerDialog(playerid,DIALOG_TELEPORTS,DIALOG_STYLE_LIST,"Телепортация",stmp,"Выбрать","Отмена");
			}
			else print("[ERROR] teleports file not exist");
			return 1;
		}
	}
	return 0;
}

World_Dialog(playerid, dialogid, response, listitem, inputtext[])
{
	#pragma unused inputtext
	switch(dialogid)
	{
		case DIALOG_TELEPORTS:
		{
			if(!response) return 1;
			TeleportTo(playerid,listitem);
			return 1;
		}
	}
	return 1;
}

World_OnPlayerUpdate(playerid)
{
	if(GetPlayerVirtualWorld(playerid) == STUNT_VIRTUAL_WORLD)
	{
		SetPlayerArmedWeapon(playerid,0);
		oSetPlayerHealth(playerid,100.0);
		if(IsPlayerInAnyVehicle(playerid))
		{
			static 
				Float:health,
				vid;
			vid = GetPlayerVehicleID(playerid);
			GetVehicleHealth(vid,health);
			if(health <= 999.0) RepairVehicle(vid);
		}
	}
}
World_OnPlayerSpawn(playerid)
{
  TextDrawShowForPlayer(playerid,txtTimeDisp);
  return 1;
}
World_OnPlayerDeath(playerid)
{
  TextDrawHideForPlayer(playerid,txtTimeDisp);
  return 1;
}