/*
--BBBB----L----------AAAA---CCCCCCCC-K----K------BBBB------AAAA---L--------L---------
--B---B---L---------A----A--C--------K---K-------B---B----A----A--L--------L---------
--B----B--L--------A------A-C--------K--K--------B----B--A------A-L--------L---------
--BBBBB---L--------A------A-C--------KKK---------BBBBB---A------A-L--------L---------
--B----B--L--------A------A-C--------KKKK--------B----B--A------A-L--------L---------
--B-----B-L--------AAAAAAAA-C--------K---K-------B-----B-AAAAAAAA-L--------L---------
--B-----B-L--------A------A-C--------K----K------B-----B-A------A-L--------L---------
--BBBBBB--LLLLLLLL-A------A-CCCCCCCC-K-----K-----BBBBBB--A------A-LLLLLLLL-LLLLLLLL--
*/
#include <a_samp>

#define FILTERSCRIPT
#define pub%0(%1) forward%0(%1);public%0(%1)
#define pid playerid
#define cGreen 0x00cc00ff
#define cGrey 0x333333ff

//[Спидометр]===================================================================
new PlayerText:TD_SPEED[5][MAX_PLAYERS];
new Timer_Speed[MAX_PLAYERS];
new VehicleNames[][] = {
     "Landstalker","Bravura","Buffalo","Linerunner","Pereniel","Sentinel","Dumper","Firetruck","Trashmaster","Stretch","Manana","Infernus","Voodo o","Pony","Mule","Cheetah","Ambulance","Leviathan","Moonbeam","Esperanto",
     "Taxi","Washington","Bobcat","Mr Whoopee","BF Injection","Hunter","Premier","Enforcer","Securicar","Banshee","Predator","Bus","Rhino","Barracks","Hotknife","Trailer","Previon","Coach","C abbie","Stallion",
     "Rumpo","RC Bandit","Romero","Packer","Monster","Admiral","Squalo","Seasparrow","Pizzaboy","Tram","Trailer","Turismo","Speeder","Reefer","Tropic","Flatb ed","Yankee","Caddy","Solair","Berkley's RC Van",
     "Skimmer","PCJ-600","Faggio","Freeway","RC Baron","RC Raider","Glendale","Oceanic","Sanchez","Sparrow","Patriot","Quad","Coastguard","Dinghy","Hermes","Sabre","Rustler","ZR3 50","Walton","Regina",
     "Comet","BMX","Burrito","Camper","Marquis","Baggage","Dozer","Maverick","News Chopper","Rancher","FBI Rancher","Virgo","Greenwood","Jetmax","Hotring","Sandking","Blista Compact","Police Maverick","Boxville","Benson",
     "Mesa","RC Goblin","Hotring Racer","Hotring Racer","Bloodring Banger","Rancher","Super GT","Elegant","Journey","Bike","Mountain Bike","Beagle","Cropdust","Stunt","Tanker","RoadTrain","Nebula","Majestic","Buccaneer","Shamal",
     "Hydra","FCR-900","NRG-500","HPV1000","Cement Truck","Tow Truck","Fortune","Cadrona","FBI Truck","Willard","Forklift","Tractor","Combine","Feltzer","Remington","Slamvan","Blade","Freight","Streak","Vortex",
     "Vincent","Bullet","Clover","Sadler","Firetruck","Hustler","Intruder","Primo","Cargobob","Tampa","Sunrise","Merit","Utility","Nevada","Yosem ite","Windsor","Monster","Monster","Uranus","Jester",
     "Sultan","Stratum","Elegy","Raindance","RC Tiger","Flash","Tahoma","Savanna","Bandito","Freight","Trailer","Kart","Mower","Duneride","Sweeper","Broadway","Tornado","AT-400","DFT-30"," Huntley",
     "Stafford","BF-400","Newsvan","Tug","Trailer","Emperor","Wayfarer","Euros","Hotdog","Club","Trailer","Trailer","Andromada","Dodo","RC Cam","Launch","Police Car (LSPD)","Police Car (SFPD)","Police Car (LVPD)","Police Ranger",
     "Picador","S.W.A.T. Van","Alpha","Phoenix","Glendale","Sadler","Luggage Trailer","Luggage Trailer","Stair Trailer","Boxville","Farm Plow","Utility Trailer"
};
//==============================================================================
//[Двигатель]===================================================================
new lights,alarm,doors,bonnet,boot,objective;
enum SysCar
{
	bool:vEngine,
	vFuel,
	vPower,
	vProbeg
}
new CarSystem[MAX_VEHICLES][SysCar];
new Timer_CarSystem[MAX_PLAYERS];
//==============================================================================
//[бензин]======================================================================
new PlayerText:TD_FUEL[6][MAX_PLAYERS];
new Timer_Fuel[MAX_PLAYERS];
//==============================================================================
//[Аккумулятор]=================================================================
new PlayerText:TD_POWER[6][MAX_PLAYERS];
new Timer_Power[MAX_PLAYERS];
//==============================================================================
//[Пробег]======================================================================
new PlayerText:TD_PROBEG[1][MAX_PLAYERS];
new Float:Probeg[MAX_VEHICLES] = 0.0;
new Timer_Probeg[MAX_PLAYERS];
//==============================================================================
//[Поворотники]=================================================================
new PlayerText:TD_POVOROT[2][MAX_PLAYERS];
new Indicators_xqz[MAX_VEHICLES][6];
new bool:pRight[MAX_PLAYERS];
new bool:pLeft[MAX_PLAYERS];
new pColor = 0x333333ff;
new Timer_Povorot[MAX_PLAYERS];
//==============================================================================

public OnFilterScriptInit()
{
    ManualVehicleEngineAndLights();
    for(new v; v<MAX_VEHICLES; v++)
    {
        CarSystem[v][vFuel]=200;
        CarSystem[v][vPower]=1000;
    }
    return 1;
}
public OnPlayerConnect(pid)
{
    //=================[Скорость]==================
	TD_SPEED[2][pid] = CreatePlayerTextDraw(pid, 640.0, 400.0, "LD_SPAC:white");
	PlayerTextDrawFont(pid, TD_SPEED[2][pid], 4);
	PlayerTextDrawAlignment(pid, TD_SPEED[2][pid], 3);
	PlayerTextDrawColor(pid, TD_SPEED[2][pid], 0x000000ff);
	PlayerTextDrawTextSize(pid, TD_SPEED[2][pid], -160.0, 47.5);

    TD_SPEED[4][pid] = CreatePlayerTextDraw(pid, 530.0, 400.0, "411");
	PlayerTextDrawFont(pid, TD_SPEED[4][pid], TEXT_DRAW_FONT_MODEL_PREVIEW);
	PlayerTextDrawTextSize(pid, TD_SPEED[4][pid], 60.0, 40.0);
	PlayerTextDrawSetPreviewRot(playerid, TD_SPEED[4][pid], -20.0, 0.0, -40.0, 1.0);
	PlayerTextDrawSetPreviewVehCol(pid, TD_SPEED[4][pid], 0x000000FF, 0x000000FF);
	
    TD_SPEED[0][pid] = CreatePlayerTextDraw(pid, 560.000000, 429.000000, "KM/H");
	PlayerTextDrawLetterSize(pid, TD_SPEED[0][pid], 0.150000, 0.600000);
	PlayerTextDrawAlignment(pid, TD_SPEED[0][pid], 2);
	PlayerTextDrawColor(pid, TD_SPEED[0][pid], 0x66ccffaa);
	PlayerTextDrawUseBox(pid, TD_SPEED[0][pid], false);
	PlayerTextDrawBoxColor(pid, TD_SPEED[0][pid], 0x333333ff);
	PlayerTextDrawSetShadow(pid, TD_SPEED[0][pid], 0);
	PlayerTextDrawSetOutline(pid, TD_SPEED[0][pid], 1);
	PlayerTextDrawBackgroundColor(pid, TD_SPEED[0][pid], 161476864);
	PlayerTextDrawFont(pid, TD_SPEED[0][pid], 1);
	PlayerTextDrawSetProportional(pid, TD_SPEED[0][pid], 1);

	TD_SPEED[1][pid] = CreatePlayerTextDraw(pid, 560.000000, 410.500000, "100");
	PlayerTextDrawLetterSize(pid, TD_SPEED[1][pid], 0.5, 2.0);
	PlayerTextDrawAlignment(pid, TD_SPEED[1][pid], 2);
	PlayerTextDrawUseBox(pid, TD_SPEED[1][pid], false);
	PlayerTextDrawColor(pid, TD_SPEED[1][pid], 0x66ccffaa);
	PlayerTextDrawSetShadow(pid, TD_SPEED[1][pid], 0);
	PlayerTextDrawSetOutline(pid, TD_SPEED[1][pid], 1);
	PlayerTextDrawBackgroundColor(pid, TD_SPEED[1][pid], 51);
	PlayerTextDrawFont(pid, TD_SPEED[1][pid], 3);
	PlayerTextDrawSetProportional(pid, TD_SPEED[1][pid], 1);
	
	TD_SPEED[3][pid] = CreatePlayerTextDraw(pid, 560.000000, 401.000000, "Elegy");
	PlayerTextDrawLetterSize(pid, TD_SPEED[3][pid], 0.2, 0.8);
	PlayerTextDrawAlignment(pid, TD_SPEED[3][pid], 2);
	PlayerTextDrawColor(pid, TD_SPEED[3][pid], 0xccffffaa);
	PlayerTextDrawSetShadow(pid, TD_SPEED[3][pid], 0);
	PlayerTextDrawSetOutline(pid, TD_SPEED[3][pid], 0);
	PlayerTextDrawFont(pid, TD_SPEED[3][pid], 1);
	//==========================================================================
	//[Бензин]==================================================================
	TD_FUEL[0][pid] = CreatePlayerTextDraw(pid, 595.0, 413.0, "F~n~U~n~E~n~L~n~");
	PlayerTextDrawLetterSize(pid, TD_FUEL[0][pid], 0.15, 0.6);
	PlayerTextDrawAlignment(pid, TD_FUEL[0][pid], 3);
	PlayerTextDrawColor(pid, TD_FUEL[0][pid], 0xccffffaa);
	PlayerTextDrawSetShadow(pid, TD_FUEL[0][pid], 0);
	PlayerTextDrawSetOutline(pid, TD_FUEL[0][pid], 0);
	PlayerTextDrawFont(pid, TD_FUEL[0][pid], 2);
	PlayerTextDrawSetProportional(pid, TD_FUEL[0][pid], 0);
	
	TD_FUEL[1][pid] = CreatePlayerTextDraw(pid, 588.5, 431.0, "LD_SPAC:white");
	PlayerTextDrawFont(pid, TD_FUEL[1][pid], 4);
	PlayerTextDrawAlignment(pid, TD_FUEL[1][pid], 1);
	PlayerTextDrawColor(pid, TD_FUEL[1][pid], 0x66ccffaa);
	PlayerTextDrawTextSize(pid, TD_FUEL[1][pid], 5.0, 3.5);

	TD_FUEL[2][pid] = CreatePlayerTextDraw(pid, 588.5, 426.8, "LD_SPAC:white");
	PlayerTextDrawFont(pid, TD_FUEL[2][pid], 4);
	PlayerTextDrawAlignment(pid, TD_FUEL[2][pid], 1);
	PlayerTextDrawColor(pid, TD_FUEL[2][pid], 0x66ccffaa);
	PlayerTextDrawTextSize(pid, TD_FUEL[2][pid], 5.0, 3.5);

	TD_FUEL[3][pid] = CreatePlayerTextDraw(pid, 588.5, 422.8, "LD_SPAC:white");
	PlayerTextDrawFont(pid, TD_FUEL[3][pid], 4);
	PlayerTextDrawAlignment(pid, TD_FUEL[3][pid], 1);
	PlayerTextDrawColor(pid, TD_FUEL[3][pid], 0x66ccffaa);
	PlayerTextDrawTextSize(pid, TD_FUEL[3][pid], 5.0, 3.5);

	TD_FUEL[4][pid] = CreatePlayerTextDraw(pid, 588.5, 418.7, "LD_SPAC:white");
	PlayerTextDrawFont(pid, TD_FUEL[4][pid], 4);
	PlayerTextDrawAlignment(pid, TD_FUEL[4][pid], 1);
	PlayerTextDrawColor(pid, TD_FUEL[4][pid], 0x66ccffaa);
	PlayerTextDrawTextSize(pid, TD_FUEL[4][pid], 5.0, 3.5);

	TD_FUEL[5][pid] = CreatePlayerTextDraw(pid, 588.5, 414.7, "LD_SPAC:white");
	PlayerTextDrawFont(pid, TD_FUEL[5][pid], 4);
	PlayerTextDrawAlignment(pid, TD_FUEL[5][pid], 1);
	PlayerTextDrawColor(pid, TD_FUEL[5][pid], 0x66ccffaa);
	PlayerTextDrawTextSize(pid, TD_FUEL[5][pid], 5.0, 3.5);
	//==========================================================================
	
	//[Аккумулятор]==================================================================
	TD_POWER[0][pid] = CreatePlayerTextDraw(pid, 521.0, 410.0, "P~n~O~n~W~n~E~n~R~n~");
	PlayerTextDrawLetterSize(pid, TD_POWER[0][pid], 0.15, 0.6);
	PlayerTextDrawAlignment(pid, TD_POWER[0][pid], 1);
	PlayerTextDrawColor(pid, TD_POWER[0][pid], 0xccffffaa);
	PlayerTextDrawSetShadow(pid, TD_POWER[0][pid], 0);
	PlayerTextDrawSetOutline(pid, TD_POWER[0][pid], 0);
	PlayerTextDrawFont(pid, TD_POWER[0][pid], 2);
	PlayerTextDrawSetProportional(pid, TD_POWER[0][pid], 0);

	TD_POWER[1][pid] = CreatePlayerTextDraw(pid, 527.5, 431.0, "LD_SPAC:white");
	PlayerTextDrawFont(pid, TD_POWER[1][pid], 4);
	PlayerTextDrawAlignment(pid, TD_POWER[1][pid], 1);
	PlayerTextDrawColor(pid, TD_POWER[1][pid], 0x66ccffaa);
	PlayerTextDrawTextSize(pid, TD_POWER[1][pid], 5.0, 3.5);

	TD_POWER[2][pid] = CreatePlayerTextDraw(pid, 527.5, 426.8, "LD_SPAC:white");
	PlayerTextDrawFont(pid, TD_POWER[2][pid], 4);
	PlayerTextDrawAlignment(pid, TD_POWER[2][pid], 1);
	PlayerTextDrawColor(pid, TD_POWER[2][pid], 0x66ccffaa);
	PlayerTextDrawTextSize(pid, TD_POWER[2][pid], 5.0, 3.5);

	TD_POWER[3][pid] = CreatePlayerTextDraw(pid, 527.5, 422.8, "LD_SPAC:white");
	PlayerTextDrawFont(pid, TD_POWER[3][pid], 4);
	PlayerTextDrawAlignment(pid, TD_POWER[3][pid], 1);
	PlayerTextDrawColor(pid, TD_POWER[3][pid], 0x66ccffaa);
	PlayerTextDrawTextSize(pid, TD_POWER[3][pid], 5.0, 3.5);

	TD_POWER[4][pid] = CreatePlayerTextDraw(pid, 527.5, 418.7, "LD_SPAC:white");
	PlayerTextDrawFont(pid, TD_POWER[4][pid], 4);
	PlayerTextDrawAlignment(pid, TD_POWER[4][pid], 1);
	PlayerTextDrawColor(pid, TD_POWER[4][pid], 0x66ccffaa);
	PlayerTextDrawTextSize(pid, TD_POWER[4][pid], 5.0, 3.5);

	TD_POWER[5][pid] = CreatePlayerTextDraw(pid, 527.5, 414.7, "LD_SPAC:white");
	PlayerTextDrawFont(pid, TD_POWER[5][pid], 4);
	PlayerTextDrawAlignment(pid, TD_POWER[5][pid], 1);
	PlayerTextDrawColor(pid, TD_POWER[5][pid], 0x66ccffaa);
	PlayerTextDrawTextSize(pid, TD_POWER[5][pid], 5.0, 3.5);
	//==========================================================================
	//[Пробег]==================================================================
	TD_PROBEG[0][pid] = CreatePlayerTextDraw(pid, 560.0, 434.0, "000000 ~w~KM");
	PlayerTextDrawLetterSize(pid, TD_PROBEG[0][pid], 0.2, 0.8);
	PlayerTextDrawFont(pid, TD_PROBEG[0][pid], 3);
	PlayerTextDrawAlignment(pid, TD_PROBEG[0][pid], 2);
	PlayerTextDrawColor(pid, TD_PROBEG[0][pid], 0x66ccffaa);
	//==========================================================================
	return 1;
}
pub UpdateSpeed(pid)
{
    new string[256];
    new v = GetPlayerVehicleID(pid);
    if(!v) return true;
    
	//==================[Скорость]=======================
    format(string,sizeof(string),"%d",SpeedVehicle(pid));
    PlayerTextDrawSetString(pid,TD_SPEED[1][pid], string);
    
    format(string, sizeof(string), "%s", VehicleNames[GetVehicleModel(GetPlayerVehicleID(pid))-400]);
    PlayerTextDrawSetString(pid,TD_SPEED[3][pid], string);
    //==========================================================================
    return true;
}
public OnPlayerCommandText(playerid, cmdtext[])
{
	if (strcmp("/color", cmdtext, true, 10) == 0)
	{
		new string[56];
		format(string, sizeof(string), "Левый: %d Правый: %d", pLeft[pid], pRight[pid]);
		SendClientMessage(pid, pColor, string);
		return 1;
	}
	return 0;
}
public OnPlayerStateChange(pid, newstate, oldstate)
{
	if(newstate == PLAYER_STATE_DRIVER)
    {
		TD_POVOROT[0][pid] = CreatePlayerTextDraw(pid, 615.0, 415.0, "LD_BEAT:right");
		PlayerTextDrawFont(pid, TD_POVOROT[0][pid], 4);
		PlayerTextDrawAlignment(pid, TD_POVOROT[0][pid], 1);
		PlayerTextDrawTextSize(pid, TD_POVOROT[0][pid], 16.0, 16.0);
		PlayerTextDrawColor(pid, TD_POVOROT[0][pid], cGrey);

		TD_POVOROT[1][pid] = CreatePlayerTextDraw(pid, 495.0, 415.0, "LD_BEAT:left");
		PlayerTextDrawFont(pid, TD_POVOROT[1][pid], 4);
		PlayerTextDrawAlignment(pid, TD_POVOROT[1][pid], 1);
		PlayerTextDrawTextSize(pid, TD_POVOROT[1][pid], 16.0, 16.0);
		PlayerTextDrawColor(pid, TD_POVOROT[1][pid], cGrey);
		
        new v = GetVehicleModel(GetPlayerVehicleID(pid));
		PlayerTextDrawSetPreviewModel(pid, TD_SPEED[4][pid], v);
        //[Скорость]============================================================
        SpeedView(pid);
        PlayerTextDrawShow(pid,TD_SPEED[2][pid]);// показываем тд
        PlayerTextDrawShow(pid,TD_SPEED[3][pid]);// показываем тд
        Timer_Speed[pid] = SetTimerEx("UpdateSpeed",100,true,"d",pid);//запускаем
        //======================================================================
        //[Бензин]==============================================================
        PlayerTextDrawShow(pid, TD_FUEL[0][pid]);
        Timer_Fuel[pid] = SetTimerEx("UpdateFuel", 100, true, "d", pid);
        //======================================================================
		//[Аккумулятор]=========================================================
		for(new p; p<6; p++) PlayerTextDrawShow(pid, TD_POWER[p][pid]);
		Timer_Power[pid] = SetTimerEx("UpdatePower", 100, true, "d", pid);
		//======================================================================
		//[Пробег]==============================================================
		PlayerTextDrawShow(pid, TD_PROBEG[0][pid]);
		Timer_Probeg[pid] = SetTimerEx("ToProbeg",1000, true, "d", pid);
		//======================================================================
		//[Поворотники]=========================================================
		for(new po; po<2; po++) PlayerTextDrawShow(pid, TD_POVOROT[po][pid]);
		Timer_Povorot[pid] = SetTimerEx("PovorotColor", 250, true, "d", pid);
		//======================================================================
		return 1;
	}
    if(newstate == PLAYER_STATE_ONFOOT)
    {
        //[Скорость]============================================================
        KillTimer(Timer_Speed[pid]);//Удаляем таймер
        for(new s; s<5; s++) PlayerTextDrawHide(pid,TD_SPEED[s][pid]);//скрываем тд
        //======================================================================
        //[Бензин]==============================================================
        for(new f; f<6; f++) PlayerTextDrawHide(pid, TD_FUEL[f][pid]);
        //======================================================================
        //[Аккумулятор]=========================================================
        for(new p; p<6; p++) PlayerTextDrawHide(pid, TD_POWER[p][pid]);
        //======================================================================
        //[Пробег]==============================================================
        PlayerTextDrawHide(pid, TD_PROBEG[0][pid]);
        KillTimer(Timer_Probeg[pid]);
		//======================================================================
		//[Поворотники]=========================================================
		for(new po; po<2; po++) PlayerTextDrawHide(pid, TD_POVOROT[po][pid]);
		KillTimer(Timer_Povorot[pid]);
		//======================================================================
    }
	return 1;
}
pub PovorotColor(pid)
{
	if(pRight[pid] == true)
	{
	    if(pColor == cGrey) {pColor = cGreen;} else {pColor = cGrey;}
	    PlayerTextDrawHide(pid, TD_POVOROT[0][pid]);
		PlayerTextDrawColor(pid, TD_POVOROT[0][pid], pColor);
		PlayerTextDrawShow(pid, TD_POVOROT[0][pid]);
	}
	else
	{
        PlayerTextDrawHide(pid, TD_POVOROT[0][pid]);
		PlayerTextDrawColor(pid, TD_POVOROT[0][pid], cGrey);
		PlayerTextDrawShow(pid, TD_POVOROT[0][pid]);
	}
	
	if(pLeft[pid] == true)
	{
	    if(pColor == cGrey) {pColor = cGreen;} else {pColor = cGrey;}
	    PlayerTextDrawHide(pid, TD_POVOROT[1][pid]);
		PlayerTextDrawColor(pid, TD_POVOROT[1][pid], pColor);
		PlayerTextDrawShow(pid, TD_POVOROT[1][pid]);
	}
	else
	{
        PlayerTextDrawHide(pid, TD_POVOROT[1][pid]);
		PlayerTextDrawColor(pid, TD_POVOROT[1][pid], cGrey);
		PlayerTextDrawShow(pid, TD_POVOROT[1][pid]);
	}
	if(pLeft[pid] == true && pRight[pid] == true)
	{
	    if(pColor == cGrey) {pColor = cGreen;} else {pColor = cGrey;}
	    PlayerTextDrawHide(pid, TD_POVOROT[0][pid]);
		PlayerTextDrawColor(pid, TD_POVOROT[0][pid], pColor);
		PlayerTextDrawShow(pid, TD_POVOROT[0][pid]);
	    PlayerTextDrawHide(pid, TD_POVOROT[1][pid]);
		PlayerTextDrawColor(pid, TD_POVOROT[1][pid], pColor);
		PlayerTextDrawShow(pid, TD_POVOROT[1][pid]);
	}
	else if(pLeft[pid] == false && pRight[pid] == false)
	{
	    PlayerTextDrawHide(pid, TD_POVOROT[0][pid]);
		PlayerTextDrawColor(pid, TD_POVOROT[0][pid], cGrey);
		PlayerTextDrawShow(pid, TD_POVOROT[0][pid]);
        PlayerTextDrawHide(pid, TD_POVOROT[1][pid]);
		PlayerTextDrawColor(pid, TD_POVOROT[1][pid], cGrey);
		PlayerTextDrawShow(pid, TD_POVOROT[1][pid]);
	}
	return 0;
}
public OnPlayerKeyStateChange(pid, newkeys, oldkeys)
{
	if(newkeys & KEY_LOOK_BEHIND)
	{
	    new v = GetPlayerVehicleID(pid);
	    if(IsPlayerInAnyVehicle(pid))
	    {
	        if(CarSystem[v][vFuel] > 0 && CarSystem[v][vPower] > 0)
	        {
				CarSystem[v][vEngine] = !CarSystem[v][vEngine];
				SetVehicleParamsEx(v,CarSystem[v][vEngine],lights,alarm,doors,bonnet,boot,objective);
				SpeedView(pid);
				if(CarSystem[v][vEngine] == true) {Timer_CarSystem[pid]=SetTimerEx("CarSystemCheck", 10000, true, "d", pid);}
				else KillTimer(Timer_CarSystem[pid]);
			}
			else SendClientMessage(pid, 0xcc0000ff, "Проверьте наличие бензина или заряд аккумулятора!");
		}
	}
	if(IsPlayerInAnyVehicle(playerid) && GetPlayerState(playerid) == 2)
    {
    	if(!IsAPlane(GetPlayerVehicleID(playerid)) && !IsABoat(GetPlayerVehicleID(playerid)))
        {
        	new vid = GetPlayerVehicleID(playerid);
            if(newkeys & ( KEY_LOOK_LEFT ) && newkeys & ( KEY_LOOK_RIGHT ))
            {
                if(Indicators_xqz[vid][2] /*|| Indicators_xqz[vid][5]*/) DestroyObject(Indicators_xqz[vid][5]),DestroyObject(Indicators_xqz[vid][2]), DestroyObject(Indicators_xqz[vid][3]),Indicators_xqz[vid][2]=0, pLeft[pid] = false;
                else if(Indicators_xqz[vid][0] /*|| Indicators_xqz[vid][4]*/) DestroyObject(Indicators_xqz[vid][4]),DestroyObject(Indicators_xqz[vid][0]), DestroyObject(Indicators_xqz[vid][1]),Indicators_xqz[vid][0]=0, pRight[pid] = false;
                else
				{
                	SetVehicleIndicator(vid,1,1);
                	pRight[pid]=true;
                	pLeft[pid]=true;
                }
                return 1;
            }
            if(newkeys & KEY_LOOK_RIGHT)
            {
                if(Indicators_xqz[vid][0] /*|| Indicators_xqz[vid][4]*/) DestroyObject(Indicators_xqz[vid][4]), DestroyObject(Indicators_xqz[vid][0]), DestroyObject(Indicators_xqz[vid][1]),Indicators_xqz[vid][0]=0, pRight[pid] = false;
                else if(Indicators_xqz[vid][2]/*|| Indicators_xqz[vid][5]*/) DestroyObject(Indicators_xqz[vid][5]), DestroyObject(Indicators_xqz[vid][2]), DestroyObject(Indicators_xqz[vid][3]),Indicators_xqz[vid][2]=0, pLeft[pid] = false;
                else
                {
                	SetVehicleIndicator(vid,0,1);
                	pRight[pid]=true;
                }
            }
            if(newkeys & KEY_LOOK_LEFT)
            {
                
                if(Indicators_xqz[vid][2]/*|| Indicators_xqz[vid][5]*/) DestroyObject(Indicators_xqz[vid][5]),DestroyObject(Indicators_xqz[vid][2]), DestroyObject(Indicators_xqz[vid][3]),Indicators_xqz[vid][2]=0, pLeft[pid] = false;
                else if(Indicators_xqz[vid][0] /*|| Indicators_xqz[vid][4]*/) DestroyObject(Indicators_xqz[vid][4]),DestroyObject(Indicators_xqz[vid][0]), DestroyObject(Indicators_xqz[vid][1]),Indicators_xqz[vid][0]=0, pRight[pid] = false;
                else
                {
                	SetVehicleIndicator(vid,1,0);
                	pLeft[pid]=true;
                }
            }
        }
    }
	return 1;
}
pub UpdatePower(pid)
{
	new v = GetPlayerVehicleID(pid);
	if(CarSystem[v][vEngine] == true)
	{
		switch(CarSystem[v][vPower])
		{
	    	case 0:
	    	{
	    	    PlayerTextDrawHide(pid, TD_POWER[1][pid]);
	        	PlayerTextDrawHide(pid, TD_POWER[2][pid]);
	        	PlayerTextDrawHide(pid, TD_POWER[3][pid]);
	        	PlayerTextDrawHide(pid, TD_POWER[4][pid]);
	        	PlayerTextDrawHide(pid, TD_POWER[5][pid]);
	    	}
	    	case 1..200:
	    	{
	        	PlayerTextDrawShow(pid, TD_POWER[1][pid]);
	        	PlayerTextDrawHide(pid, TD_POWER[2][pid]);
	        	PlayerTextDrawHide(pid, TD_POWER[3][pid]);
	        	PlayerTextDrawHide(pid, TD_POWER[4][pid]);
	        	PlayerTextDrawHide(pid, TD_POWER[5][pid]);
	    	}
	    	case 201..400:
	    	{
	        	PlayerTextDrawShow(pid, TD_POWER[1][pid]);
	        	PlayerTextDrawShow(pid, TD_POWER[2][pid]);
	        	PlayerTextDrawHide(pid, TD_POWER[3][pid]);
	        	PlayerTextDrawHide(pid, TD_POWER[4][pid]);
	        	PlayerTextDrawHide(pid, TD_POWER[5][pid]);
	    	}
	    	case 401..600:
	    	{
	        	PlayerTextDrawShow(pid, TD_POWER[1][pid]);
	        	PlayerTextDrawShow(pid, TD_POWER[2][pid]);
	        	PlayerTextDrawShow(pid, TD_POWER[3][pid]);
	        	PlayerTextDrawHide(pid, TD_POWER[4][pid]);
	        	PlayerTextDrawHide(pid, TD_POWER[5][pid]);
	    	}
	    	case 601..800:
	    	{
	        	PlayerTextDrawShow(pid, TD_POWER[1][pid]);
	        	PlayerTextDrawShow(pid, TD_POWER[2][pid]);
	        	PlayerTextDrawShow(pid, TD_POWER[3][pid]);
	        	PlayerTextDrawShow(pid, TD_POWER[4][pid]);
	        	PlayerTextDrawHide(pid, TD_POWER[5][pid]);
	    	}
	    	case 801..1000:
	    	{
	        	PlayerTextDrawShow(pid, TD_POWER[1][pid]);
	        	PlayerTextDrawShow(pid, TD_POWER[2][pid]);
	        	PlayerTextDrawShow(pid, TD_POWER[3][pid]);
	        	PlayerTextDrawShow(pid, TD_POWER[4][pid]);
	        	PlayerTextDrawShow(pid, TD_POWER[5][pid]);
	    	}
		}
	}
	else
	{
		PlayerTextDrawHide(pid, TD_POWER[1][pid]);
	   	PlayerTextDrawHide(pid, TD_POWER[2][pid]);
	   	PlayerTextDrawHide(pid, TD_POWER[3][pid]);
	   	PlayerTextDrawHide(pid, TD_POWER[4][pid]);
	   	PlayerTextDrawHide(pid, TD_POWER[5][pid]);
	}
	return 1;
}
pub UpdateFuel(pid)
{
	new v = GetPlayerVehicleID(pid);
	if(CarSystem[v][vEngine] == true)
	{
		switch(CarSystem[v][vFuel])
		{
	    	case 0:
	    	{
	    	    PlayerTextDrawHide(pid, TD_FUEL[1][pid]);
	        	PlayerTextDrawHide(pid, TD_FUEL[2][pid]);
	        	PlayerTextDrawHide(pid, TD_FUEL[3][pid]);
	        	PlayerTextDrawHide(pid, TD_FUEL[4][pid]);
	        	PlayerTextDrawHide(pid, TD_FUEL[5][pid]);
	    	}
	    	case 1..40:
	    	{
	        	PlayerTextDrawShow(pid, TD_FUEL[1][pid]);
	        	PlayerTextDrawHide(pid, TD_FUEL[2][pid]);
	        	PlayerTextDrawHide(pid, TD_FUEL[3][pid]);
	        	PlayerTextDrawHide(pid, TD_FUEL[4][pid]);
	        	PlayerTextDrawHide(pid, TD_FUEL[5][pid]);
	    	}
	    	case 41..80:
	    	{
	        	PlayerTextDrawShow(pid, TD_FUEL[1][pid]);
	        	PlayerTextDrawShow(pid, TD_FUEL[2][pid]);
	        	PlayerTextDrawHide(pid, TD_FUEL[3][pid]);
	        	PlayerTextDrawHide(pid, TD_FUEL[4][pid]);
	        	PlayerTextDrawHide(pid, TD_FUEL[5][pid]);
	    	}
	    	case 81..120:
	    	{
	        	PlayerTextDrawShow(pid, TD_FUEL[1][pid]);
	        	PlayerTextDrawShow(pid, TD_FUEL[2][pid]);
	        	PlayerTextDrawShow(pid, TD_FUEL[3][pid]);
	        	PlayerTextDrawHide(pid, TD_FUEL[4][pid]);
	        	PlayerTextDrawHide(pid, TD_FUEL[5][pid]);
	    	}
	    	case 121..160:
	    	{
	        	PlayerTextDrawShow(pid, TD_FUEL[1][pid]);
	        	PlayerTextDrawShow(pid, TD_FUEL[2][pid]);
	        	PlayerTextDrawShow(pid, TD_FUEL[3][pid]);
	        	PlayerTextDrawShow(pid, TD_FUEL[4][pid]);
	        	PlayerTextDrawHide(pid, TD_FUEL[5][pid]);
	    	}
	    	case 161..200:
	    	{
	        	PlayerTextDrawShow(pid, TD_FUEL[1][pid]);
	        	PlayerTextDrawShow(pid, TD_FUEL[2][pid]);
	        	PlayerTextDrawShow(pid, TD_FUEL[3][pid]);
	        	PlayerTextDrawShow(pid, TD_FUEL[4][pid]);
	        	PlayerTextDrawShow(pid, TD_FUEL[5][pid]);
	    	}
		}
	}
	else
	{
		PlayerTextDrawHide(pid, TD_FUEL[1][pid]);
	   	PlayerTextDrawHide(pid, TD_FUEL[2][pid]);
	   	PlayerTextDrawHide(pid, TD_FUEL[3][pid]);
	   	PlayerTextDrawHide(pid, TD_FUEL[4][pid]);
	   	PlayerTextDrawHide(pid, TD_FUEL[5][pid]);
	}
	return 1;
}
public OnVehicleDeath(vehicleid)
{
        if(Indicators_xqz[vehicleid][2]) DestroyObject(Indicators_xqz[vehicleid][2]), DestroyObject(Indicators_xqz[vehicleid][3]),DestroyObject(Indicators_xqz[vehicleid][5]),Indicators_xqz[vehicleid][2]=0;
        if(Indicators_xqz[vehicleid][0]) DestroyObject(Indicators_xqz[vehicleid][0]), DestroyObject(Indicators_xqz[vehicleid][1]),DestroyObject(Indicators_xqz[vehicleid][4]),Indicators_xqz[vehicleid][0]=0;
        return 1;
}
pub CarSystemCheck(pid)
{
	for(new v = 1; v < GetVehiclePoolSize(); v++)
	{
		if(CarSystem[v][vFuel] > 0) CarSystem[v][vFuel]--;
		if(CarSystem[v][vPower] > 0) CarSystem[v][vPower]--;
		if(CarSystem[v][vFuel] == 0 || CarSystem[v][vPower] == 0) SetVehicleParamsEx(v,false,lights,alarm,doors,bonnet,boot,objective);
	}
	return 0;
}
pub ToProbeg(playerid)
{
	new string[256];
	if(!IsPlayerInAnyVehicle(playerid)) return 1;
	new Float:sp = SpeedVehicle(playerid);
	new Float:l = (sp/2)/1000;
	Probeg[GetPlayerVehicleID(playerid)] += l;
	CarSystem[GetPlayerVehicleID(playerid)][vProbeg] = floatround(Probeg[GetPlayerVehicleID(playerid)]);
	format(string,sizeof(string),"%06d~w~ km",CarSystem[GetPlayerVehicleID(playerid)][vProbeg]);
	PlayerTextDrawSetString(pid, TD_PROBEG[0][pid], string);
	return 1;
}
stock SpeedView(pid)
{
    new v = GetPlayerVehicleID(pid);
    if(CarSystem[v][vEngine] == true)
	{
		for(new i; i<2; i++)
		{
			PlayerTextDrawShow(pid,TD_SPEED[i][pid]);
		}
		PlayerTextDrawHide(pid,TD_SPEED[4][pid]);
	}
	else
	{
		for(new i; i<2; i++)
		{
			PlayerTextDrawHide(pid,TD_SPEED[i][pid]);
		}
		PlayerTextDrawShow(pid,TD_SPEED[4][pid]);
	}
	return 1;
}
stock SpeedVehicle(pid)
{
    new Float: ST[4];
    new carid = GetPlayerVehicleID(pid);
    if(IsPlayerInAnyVehicle(pid)) GetVehicleVelocity(carid,ST[0],ST[1],ST[2]);
    else GetPlayerVelocity(pid,ST[0],ST[1],ST[2]);
    ST[3] = floatsqroot(floatpower(floatabs(ST[0]), 2.0) + floatpower(floatabs(ST[1]), 2.0) + floatpower(floatabs(ST[2]), 2.0)) * 150;
    return floatround(ST[3]);
}
stock SetVehicleIndicator(vehicleid, leftindicator=0, rightindicator=0)
{
	if(!leftindicator & !rightindicator) return false;
    new Float:_vX[2], Float:_vY[2], Float:_vZ[2];
    if(rightindicator)
    {
    	if(IsTrailerAttachedToVehicle(vehicleid))
        {
            new omg = GetVehicleModel(GetVehicleTrailer(vehicleid));
            GetVehicleModelInfo(omg, VEHICLE_MODEL_INFO_SIZE, _vX[0], _vY[0], _vZ[0]);
            Indicators_xqz[vehicleid][4] = CreateObject(19294, 0, 0, 0,0,0,0);
            AttachObjectToVehicle(Indicators_xqz[vehicleid][4], GetVehicleTrailer(vehicleid),  _vX[0]/2.4, -_vY[0]/3.35, -1.0 ,0,0,0);
        }
        GetVehicleModelInfo(GetVehicleModel(vehicleid), VEHICLE_MODEL_INFO_SIZE, _vX[0], _vY[0], _vZ[0]);
        Indicators_xqz[vehicleid][0] = CreateObject(19294, 0, 0, 0,0,0,0);
        AttachObjectToVehicle(Indicators_xqz[vehicleid][0], vehicleid,  _vX[0]/2.23, _vY[0]/2.23, 0.1 ,0,0,0);
        Indicators_xqz[vehicleid][1] = CreateObject(19294, 0, 0, 0,0,0,0);
        AttachObjectToVehicle(Indicators_xqz[vehicleid][1], vehicleid,  _vX[0]/2.23, -_vY[0]/2.23, 0.1 ,0,0,0);
    }
    if(leftindicator)
    {
        if(IsTrailerAttachedToVehicle(vehicleid))
        {
            new omg = GetVehicleModel(GetVehicleTrailer(vehicleid));
            GetVehicleModelInfo(omg, VEHICLE_MODEL_INFO_SIZE, _vX[0], _vY[0], _vZ[0]);
            Indicators_xqz[vehicleid][5] = CreateObject(19294, 0, 0, 0,0,0,0);
            AttachObjectToVehicle(Indicators_xqz[vehicleid][5], GetVehicleTrailer(vehicleid),  -_vX[0]/2.4, -_vY[0]/3.35, -1.0 ,0,0,0);
        }
        GetVehicleModelInfo(GetVehicleModel(vehicleid), VEHICLE_MODEL_INFO_SIZE, _vX[0], _vY[0], _vZ[0]);
        Indicators_xqz[vehicleid][2] = CreateObject(19294, 0, 0, 0,0,0,0);
        AttachObjectToVehicle(Indicators_xqz[vehicleid][2], vehicleid,  -_vX[0]/2.23, _vY[0]/2.23, 0.1 ,0,0,0);
        Indicators_xqz[vehicleid][3] = CreateObject(19294, 0, 0, 0,0,0,0);
        AttachObjectToVehicle(Indicators_xqz[vehicleid][3], vehicleid,  -_vX[0]/2.23, -_vY[0]/2.23, 0.1 ,0,0,0);
    }
    return 1;
}
stock IsAPlane(carid2)
{
        new carid = GetVehicleModel(carid2);
        if(carid == 592 || carid == 577 || carid == 511 || carid == 512 || carid == 593 || carid == 520 || carid == 553 || carid == 476 || carid == 519 || carid == 460 || carid == 513) return 1;
        return 0;
}

stock IsABoat(carid)
{
        new modelid = GetVehicleModel(carid);
        if(modelid == 430 || modelid == 446 || modelid == 452 || modelid == 453 || modelid == 454 || modelid == 472 || modelid == 473 || modelid == 484 || modelid == 493 || modelid == 595)
        {
                return 1;
        }
        return 0;
}
