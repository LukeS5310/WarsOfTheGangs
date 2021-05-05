/*
*	Created:			24.11.12
*	Author:				LukeS
*	Description:		Праздники, включение автоматических эвентов в зависимости от даты
*/

#if defined _holidays_included
	#endinput
#endif

#define _holidays_included
#pragma library holidays

//====================================
// Defines
//====================================
#define GIFT_UPDATE_TICK 		60*30 //раз в полчаса обновляется подарок
#define FB_FILE				"memories.txt"
#define NIGHTMARE_END			480
new gift,hol_tick=0,n_tick=0, bool:Happened;
Holidays_OnGameModeInit()
{
CheckNY();

return 1;
}
/* Holidays_OnPlayerRequestClass(playerid,classid)
{
#pragma unused classid
SetPlayerInterior(playerid,0);
SetPlayerPos(playerid,1492.4565,-1639.5093,15.0469);
SetPlayerFacingAngle(playerid,276.1474);
SetPlayerCameraPos(playerid,1498.2986,-1639.0354,16.0469);
SetPlayerCameraLookAt(playerid,1492.4565,-1639.5093,15.0469);
PlayAudioStreamForPlayer(playerid,"https://dl.dropbox.com/u/45522772/SAMP/Kan_R._Gao_-_Lullaby_Music_Box.mp3");
return 1;
} */
CheckNY()
{
getdate(itmp[0],itmp[1],itmp[2]);//0-год 1-месяц 2-день
if(itmp[2]>1&& itmp[1]==11) 
{
	printf("С наступающим %d годом!",itmp[0]+1);
	return 1;
}
return 0;
}
Holidays_tick()
{
if(hol_tick==0)
{
//Gift_Update();
//print("gift updated");
hol_tick=GIFT_UPDATE_TICK;
}
hol_tick--;
return 1;
}
Nightmare_tick() //5 SEC
{
if(n_tick==NIGHTMARE_END) n_tick=0;
if((IsNight()==1) && n_tick==0)
	{
	if(Happened) return 1;
	itmp[0]=random(10000); 
	
	
	//print("night time attempt");
	if(itmp[0]>9995)
		{
		Happened = true; // раз за ночь а то как то часто слишком снятся нашему серву кошмары
		n_tick+=5;
		for(itmp[0]=0;itmp[0]<=GetPlayersCount();itmp[0]++) PlayAudioStreamForPlayer(itmp[0],"https://dl.dropbox.com/u/45522772/SAMP/Nightmare.mp3");
		for(new i=0;i<20;i++)
		{
		SendClientMessageToAll(COLOR_WHITE," ");
		}
		//printf("nightmare with itmp=%d",itmp[0]);
		return 1;
		}
	//print("failed");
	}
if(!IsNight())
	{
	Happened=false; 
	
	
	}
if(n_tick>0)
	{
	n_tick++;
	itmp[0]=random(100);
	if(itmp[0]<2) flashback();
	//if(n_tick=>NIGHTMARE_END) n_tick=0;
	}
//printf("n_tick is %d", n_tick);
return 1;
}

stock flashback()
{
	new pointid=random(50);
	new File:df = fopen(MODE_DIR FB_FILE,io_read),
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
				//strmid(url,stmp,0,strfind(stmp,","));
				// tune
				format(url,sizeof(url),"%s",stmp);
			    SendClientMessageToAll(COLOR_GREY,url);
				break;
			}
			else c++;
		}
		fclose(df);
	}
	else print("[ERROR] memories file not exist");
	return 1;
}

stock Gift_Update()
{
	new File:df = fopen(MODE_DIR HOUSES_DIR "randompos.txt",io_read);
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
		//gift=AddStaticPickup(19054+random(4),1,ftmp[0],ftmp[1],ftmp[2],-1);//
		format(stmp,sizeof(stmp),"Подарок появился в районе %s",GetXYZZoneName(ftmp[0],ftmp[1],ftmp[2]));
		printf("DEBUG:%s",stmp);
		SendClientMessageToAll(COLOR_WHITE,stmp);
		fclose(df);
	}
	else print("[ERROR] not found random spawns file");
	return 1;
}