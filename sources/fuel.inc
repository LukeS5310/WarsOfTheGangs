/*
*	Created:			10.06.12
*	Updated:			30.10.12
*	Author:				LukeS
*	Description:		Система бензина(80% сделано)
*	TODO:				Транспорт без бензина
*/
#if defined _fuel_included
	#endinput
#endif

#define _fuel_included
#pragma library fuel

//========================================
//Defines
//========================================
#define 	MAX_FUEL 			3000
#define		FUEL_DIV			(MAX_FUEL/100)
//========================================
//Statics
//========================================
static 	fuel[MAX_VEHICLES],
		Text:td_fuel[MAX_PLAYERS],
		Text:td_bg;
static NoFuelVehicles[] =
{
	592, 577, 511, 512,
	593, 520, 553, 476,
	519, 460, 513, 548,
	425, 417, 487, 488,
	497, 563, 447, 469,
	509, 481, 510, 472,
	473, 493, 595, 484,
	430, 453, 452, 446,
	454, 538, 537, 449,
	539
};
//========================================
//Obligatory Functions
//========================================
Fuel_OnGameModeInit()
{
for(new i=0;i<MAX_VEHICLES;i++) fuel[i]=MAX_FUEL;
td_bg=TextDrawCreate(43,324,"Fuel:IIIII");
TextDrawBackgroundColor(td_bg,COLOR_BLACK);
TextDrawFont(td_bg,2);
TextDrawLetterSize(td_bg,0.5,1.5);
TextDrawColor(td_bg,COLOR_RED);
TextDrawSetShadow(td_bg,1);
//TextDrawHideForPlayer(playerid,td_bg);
return 1;
}
Fuel_OnVehicleSpawn(vehicleid)
{
fuel[vehicleid]=MAX_FUEL;//-random(MAX_FUEL)+1;

return 1;
}
Fuel_OnPlayerConnect(playerid)
{
//td_fuel[playerid]=CreateProgressBar(43.0,324.0,_,_,COLOR_BLUE,100.0);
td_fuel[playerid]=TextDrawCreate(43,324,"Fuel:100");
TextDrawBackgroundColor(td_fuel[playerid],COLOR_BLACK);
TextDrawFont(td_fuel[playerid],2);
TextDrawLetterSize(td_fuel[playerid],0.5,1.5);
TextDrawColor(td_fuel[playerid],COLOR_SPRINGGREEN);
TextDrawSetShadow(td_fuel[playerid],1);
TextDrawHideForPlayer(playerid,td_fuel[playerid]);
//TextDrawShowForPlayer(playerid,td_bg);
return 1;
}
Fuel_OnPlayerEnterVehicle(playerid,vehicleid,ispass)
{
#pragma unused ispass,playerid
if(IsPlayerNPC(playerid)) return 1;
if(!IsUseFuel(vehicleid)) return 1;
format(stmp,sizeof(stmp),"Fuel:%s",MakeBar(fuel[vehicleid]/FUEL_DIV));
//TextDrawSetString(td_fuel[playerid],stmp);
//TextDrawShowForPlayer(playerid,td_fuel[playerid]);
//SetPlayerProgressBarValue(playerid,td_fuel[playerid], fuel[vehicleid]/30);
//UpdatePlayerProgressBar(playerid,td_fuel[playerid]);
//ShowPlayerProgressBar(playerid, td_fuel[playerid]);
return 1;
}
Fuel_OnPlayerExitVehicle(playerid,vehicleid)
{
#pragma unused vehicleid,playerid
//HidePlayerProgressBar(playerid,td_fuel[playerid]);
//TextDrawHideForPlayer(playerid,td_fuel[playerid]);
return 1;
}
Fuel_OnPlayerDisconnect(playerid,reason)
{
#pragma unused reason
//DestroyProgressBar(td_fuel[playerid]);
TextDrawDestroy(td_fuel[playerid]);
return 1;
}
Fuel_OnPlayerSpawn(playerid)
{
//HideProgressBarForPlayer(playerid,td_fuel[playerid]);
TextDrawHideForPlayer(playerid,td_fuel[playerid]);
return 1;
}
Fuel_OnPlayerStateChange(playerid,oldstate,newstate)
{
#pragma unused playerid,oldstate,newstate
return 1;
}
Fuel_tick()
{

//for(new i=0;i<=GetPlayersCount();i++)
foreach(new i : Player)
	{
	if(IsPlayerNPC(i)) continue;
	if(!IsPlayerInAnyVehicle(i))
	{
	TextDrawHideForPlayer(i,td_fuel[i]);
	TextDrawHideForPlayer(i,td_bg);
	//HideProgressBarForPlayer(i,td_fuel[i]);
	continue;
	}
	itmp[1]=GetPlayerVehicleID(i);
	if(!IsUseFuel(itmp[1])) continue;
	if(GetPlayerVehicleSeat(i)==0)
	{
		if(IsAtFuelStation(i)) Refuel(i);
		else 
		{
			if(fuel[itmp[1]]>1)
			{
			GetVehicleVelocity(itmp[1], ftmp[1], ftmp[2],ftmp[3]);
			
			if(ftmp[1] > 0.1 || ftmp[2] > 0.1 || ftmp[1] < -0.1 || ftmp[2] < -0.1) fuel[itmp[1]]=fuel[itmp[1]]-2;
			else fuel[itmp[1]]--;
			}
			else SetVehicleParamsEx(itmp[1], 0, -1, -1, -1, -1, -1, -1);
		}
	}
	format(stmp,sizeof(stmp),"Fuel:%s",MakeBar(fuel[itmp[1]]/FUEL_DIV));
	TextDrawSetString(td_fuel[i],stmp);
	//SetProgressBarValue(td_fuel[i], fuel[itmp[1]]/30);
	AjustColor(td_fuel[i],fuel[itmp[1]]/FUEL_DIV);
	TextDrawShowForPlayer(i,td_fuel[i]);
	TextDrawShowForPlayer(i,td_bg);
	//UpdateProgressBar(td_fuel[i],i);
	}
return 1;
}
AjustColor(Text:barid,fuellvl)
{
if(fuellvl>=90) return TextDrawColor(barid, COLOR_BLUE);
if(fuellvl<90 && fuellvl>=50) return TextDrawColor(barid, COLOR_SPRINGGREEN);
if(fuellvl<50 && fuellvl>=30) return TextDrawColor(barid, COLOR_YELLOW);
if(fuellvl<30 && fuellvl>=15) return TextDrawColor(barid, COLOR_ORANGE);
if(fuellvl<15 && fuellvl>=0) return TextDrawColor(barid, COLOR_LIGHTRED);
return 1;
}
Refuel(playerid)
{
itmp[1]=GetPlayerVehicleID(playerid);
if(GetPlayerMoney(playerid)>0)
	{
		if(fuel[itmp[1]]>=MAX_FUEL) return 1;
		oGivePlayerMoney(playerid,-10);
		fuel[itmp[1]]=fuel[itmp[1]]+(FUEL_DIV*2);
		SetVehicleParamsEx(itmp[1], 1, -1, -1, -1, -1, -1, -1);
	}

else SendClientMessage(playerid,COLOR_YELLOW,"У вас недостаточно денег!");
//printf("%d is refueling",playerid);
return 1;
}
MakeBar(fuelam)
{
new bar[6];
if(fuelam>=80)
	{
	
	format(bar,sizeof(bar),"IIIII"); 
	return bar;
	}
if(fuelam<80 && fuelam>=60)
	{ 
	format(bar,sizeof(bar),"IIII");
	return bar;
	}
if(fuelam<60 && fuelam>=40) 
	{
	format(bar,sizeof(bar),"III"); 
	return bar;
	}
if(fuelam<40 && fuelam>=20) 
	{
	format(bar,sizeof(bar),"II"); 
	return bar;
	}
if(fuelam<20 && fuelam >=5) 
	{
	format(bar,sizeof(bar),"I"); 
	return bar;
	}
format(bar,sizeof(bar)," "); 
return bar;
	
///return bar;
}
//=======================================
//Stocks
//=======================================

stock IsAtFuelStation(playerid)
{
	if(IsPlayerInRangeOfPoint(playerid,6.0,1004.0070,-939.3102,42.1797) || IsPlayerInRangeOfPoint(playerid,6.0,1944.3260,-1772.9254,13.3906))
		return 1;
	else if(IsPlayerInRangeOfPoint(playerid,6.0,-90.5515,-1169.4578,2.4079) || IsPlayerInRangeOfPoint(playerid,6.0,-1609.7958,-2718.2048,48.5391))
 		return 1;
	else if(IsPlayerInRangeOfPoint(playerid,6.0,-2029.4968,156.4366,28.9498) || IsPlayerInRangeOfPoint(playerid,8.0,-2408.7590,976.0934,45.4175))
 		return 1;
	else if(IsPlayerInRangeOfPoint(playerid,8.0,-2243.9629,-2560.6477,31.8841) || IsPlayerInRangeOfPoint(playerid,8.0,-1676.6323,414.0262,6.9484))
 		return 1;
	else if(IsPlayerInRangeOfPoint(playerid,6.0,2202.2349,2474.3494,10.5258) || IsPlayerInRangeOfPoint(playerid,10.0,614.9333,1689.7418,6.6968))
 		return 1;
	else if(IsPlayerInRangeOfPoint(playerid,8.0,-1328.8250,2677.2173,49.7665) || IsPlayerInRangeOfPoint(playerid,6.0,70.3882,1218.6783,18.5165))
 		return 1;
	else if(IsPlayerInRangeOfPoint(playerid,8.0,2113.7390,920.1079,10.5255) || IsPlayerInRangeOfPoint(playerid,6.0,-1327.7218,2678.8723,50.0625))
 		return 1;
      else if(IsPlayerInRangeOfPoint(playerid,8.0,2146.5759,2750.8442,10.6447) || IsPlayerInRangeOfPoint(playerid,6.0,2146.8779,2739.6157,10.6435))
 		return 1;
      else if(IsPlayerInRangeOfPoint(playerid,8.0,1595.3046,2191.1331,10.6454) || IsPlayerInRangeOfPoint(playerid,6.0,1596.3274,2206.7070,10.6449))
 		return 1;
      else if(IsPlayerInRangeOfPoint(playerid,8.0,-736.9305,2742.6138,47.0158) || IsPlayerInRangeOfPoint(playerid,6.0,1382.2878,460.1438,20.3452))
	  	return 1;
	return 0;
}
stock IsUseFuel(vehicleid)
{
	itmp[0] = GetVehicleModel(vehicleid);

	for(new i=0; i<sizeof(NoFuelVehicles); i++)
	{
		if(NoFuelVehicles[i] == itmp[0]) return 0;
	}
	return 1;
}