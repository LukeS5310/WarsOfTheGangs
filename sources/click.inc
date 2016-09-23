//
// Created:     09.11.09
// Aurthor:   ElBI(Система переделана под особенности нашего сервера - LukeS)
//

#define _click_included
#define GetPlayerClickedPlayerid(%0) GetPVarInt(%0,"ClickedPlayerid")
#define SetPlayerClickedPlayerid(%0,%1) SetPVarInt(%0,"ClickedPlayerid",%1)

//NEXTGEN STUFF WOW SUCH ADVANCED

new PlayerText:Pmodel[MAX_PLAYERS];
new PlayerText:PNick[MAX_PLAYERS];
new PlayerText:PXP[MAX_PLAYERS];
new Text:box;
new Text:Training;
//new PlayerText:CSkillName[MAX_PLAYERS];
new PlayerText:CInvizTime[MAX_PLAYERS];


new inviztimer[MAX_PLAYERS];
//WOW
click_OnPlayerConnect(playerid)
{
//#pragma unused playerid
CInvizTime[playerid] = CreatePlayerTextDraw(playerid, 605.0, 53.0, "00:00");
 //playerid,CInvizTime[playerid] = TextDrawCreate(605.0,25.0,"00:00");
PlayerTextDrawUseBox(playerid,CInvizTime[playerid], 0);
PlayerTextDrawFont(playerid,CInvizTime[playerid], 3);
PlayerTextDrawSetShadow(playerid,CInvizTime[playerid],0); // no shadow
PlayerTextDrawSetOutline(playerid,CInvizTime[playerid],2); // thickness 1
PlayerTextDrawBackgroundColor(playerid,CInvizTime[playerid],0x000000FF);
PlayerTextDrawColor(playerid,CInvizTime[playerid],COLOR_BLUE);
PlayerTextDrawAlignment(playerid,CInvizTime[playerid],3);
PlayerTextDrawLetterSize(playerid,CInvizTime[playerid],0.5,1.5);

Pmodel[playerid] = CreatePlayerTextDraw(playerid, 87.000007, 152.236968, "MODEL");
PlayerTextDrawLetterSize(playerid, Pmodel[playerid], 0.467000, 1.790814);
PlayerTextDrawTextSize(playerid, Pmodel[playerid], 104.000236, 107.851722);
PlayerTextDrawAlignment(playerid, Pmodel[playerid], 1);
PlayerTextDrawColor(playerid, Pmodel[playerid], -1);
PlayerTextDrawUseBox(playerid, Pmodel[playerid], true);
PlayerTextDrawBoxColor(playerid, Pmodel[playerid], 255);
PlayerTextDrawSetShadow(playerid, Pmodel[playerid], 72);
PlayerTextDrawSetOutline(playerid, Pmodel[playerid], 0);
PlayerTextDrawBackgroundColor(playerid, Pmodel[playerid], 51);
PlayerTextDrawFont(playerid, Pmodel[playerid], 5);
PlayerTextDrawSetProportional(playerid, Pmodel[playerid], 1);

PNick[playerid] = CreatePlayerTextDraw(playerid, 87.333328, 143.111022, oGetPlayerName(playerid));
PlayerTextDrawLetterSize(playerid, PNick[playerid], 0.449999, 1.600000);
PlayerTextDrawAlignment(playerid, PNick[playerid], 1);
PlayerTextDrawColor(playerid, PNick[playerid], -1);
PlayerTextDrawUseBox(playerid, PNick[playerid], true);
PlayerTextDrawBoxColor(playerid, PNick[playerid], 0);
PlayerTextDrawSetShadow(playerid, PNick[playerid], 0);
PlayerTextDrawSetOutline(playerid, PNick[playerid], 1);
PlayerTextDrawBackgroundColor(playerid, PNick[playerid], 51);
PlayerTextDrawFont(playerid, PNick[playerid], 1);
PlayerTextDrawSetProportional(playerid, PNick[playerid], 1);

//format(stmp,sizeof(stmp),"%d XP",GetPVarInt(playerid,"XP"));
PXP[playerid] = CreatePlayerTextDraw(playerid, 139.000015, 244.740753, "0");
PlayerTextDrawLetterSize(playerid, PXP[playerid], 0.310333, 1.645629);
PlayerTextDrawAlignment(playerid, PXP[playerid], 2);
PlayerTextDrawColor(playerid, PXP[playerid], -1);
PlayerTextDrawSetShadow(playerid, PXP[playerid], 0);
PlayerTextDrawSetOutline(playerid, PXP[playerid], 1);
PlayerTextDrawBackgroundColor(playerid, PXP[playerid], 51);
PlayerTextDrawFont(playerid, PXP[playerid], 2);
PlayerTextDrawSetProportional(playerid, PXP[playerid], 1);

return 1;
}
click_OnGameModeInit()
{
box = TextDrawCreate(426.666778, 159.544509, "usebox");
TextDrawLetterSize(box, 0.000000, 10.560698);
TextDrawTextSize(box, 526.000000, 0.000000);
TextDrawAlignment(box, 1);
TextDrawColor(box, 0);
TextDrawUseBox(box, true);
TextDrawBoxColor(box, 102);
TextDrawSetShadow(box, 0);
TextDrawSetOutline(box, 0);
TextDrawFont(box, 0);

Training = TextDrawCreate(426.666717, 150.577743, "Training");
TextDrawLetterSize(Training, 0.449999, 1.600000);
TextDrawAlignment(Training, 1);
TextDrawColor(Training, -1);
TextDrawSetShadow(Training, 0);
TextDrawSetOutline(Training, 1);
TextDrawBackgroundColor(Training, 51);
TextDrawFont(Training, 1);
TextDrawSetProportional(Training, 1);


return 1;
}

click_OnPlayerSpawn(playerid)
{

PlayerTextDrawSetPreviewModel(playerid,Pmodel[playerid], GetPlayerSkin(playerid)); //default CJ









if(inviztimer[playerid]>0) 
{
PlayerTextDrawShow(playerid,CInvizTime[playerid]);
SetPlayerColor(playerid,GetPlayerColor( playerid ) & 0xFFFFFF00);
}

return 1;
}
click_OnPlayerDisconnect(playerid)
{
PlayerTextDrawDestroy(playerid,PNick[playerid]);
PlayerTextDrawDestroy(playerid,PXP[playerid]);
PlayerTextDrawDestroy(playerid,Pmodel[playerid]);
PlayerTextDrawDestroy(playerid,CInvizTime[playerid]);
inviztimer[playerid]=0;
return 1;
}

stock click_OnPlayerClickPlayer(playerid, clickedplayerid)
{
	if(playerid == clickedplayerid) return click_OnPlayerClickSelf(playerid);
	SetPlayerClickedPlayerid(playerid,clickedplayerid);
	new listitems[256]; // не менять!
	listitems = "Отправить денег\nЛичное сообщение\nЖалоба";
	if((GetPVarInt(playerid,"AdminLevel") !=0)) strcat(listitems, "\nКикнуть\nЗаткнуть\nРазоткнуть\nВ тюрьму\nИз тюрьмы\nТелепортироваться к игроку\nТелепортировать его к себе", sizeof(listitems));	
	ShowPlayerDialog(playerid,DIALOG_CLICK,DIALOG_STYLE_LIST,"Выберите действие",listitems," Выбрать"," Отмена");
	return 1;
}

stock click_OnPlayerClickSelf(playerid)
{
ShowHUD(playerid);	 
new listitems[320];
listitems = "Инфо о персонаже\nУбить себя\nВыбросить текущее оружие\nАдминистрация онлайн\nИнфо о версии\nНачать/присоединиться к гонке\nНачать/присоединиться к ДМ\nНачать/присоединиться к Командному заданию\nНавыки...\nТелепортироваться\nНастроить приёмник в машине\nСбросить скин\nВыпить\nСменить стиль боя\nНевидимость...";
ShowPlayerDialog(playerid,DIALOG_CLICKSELF,DIALOG_STYLE_LIST,"Выберите действие",listitems," Выбрать"," Отмена");
return 1;
}

Click_Dialog(playerid,dialogid,response,listitem,inputtext[])
{
   HideHUD(playerid);
   new ctmp[255];
   if(!response) return 1;
   new recid=GetPlayerClickedPlayerid(playerid);
   switch(dialogid)
   {
    case DIALOG_CLICK:   
	{
		new GuiHeader[32],GuiText[32];
		if(listitem == 0)
		{
		    GuiHeader = " Отправить деньги";
		    GuiText =  " Введите сумму $\n\n";
			//format(stmp,sizeof(stmp),"%d %d",recid,inputtext);
			//Player_Command(playerid,PLAYER_GIVECASH_CMD,stmp);
			SetPVarInt(playerid,"action",0);
		}
		if(listitem == 1)
		{
			GuiHeader = " Личное сообщение";
		    GuiText =  " Введите сообщение\n\n";
			SetPVarInt(playerid,"action",1);
		}
		if(listitem == 2)
		{
		    GuiHeader = " Жалоба на игрока";
		    GuiText =  " Введите жалобу\n\n";
			SetPVarInt(playerid,"action",2);
		}
		if(listitem == 3)
		{
		    GuiHeader = " Кикнуть игрока";
		    GuiText =  " Введите причину\n\n";
			SetPVarInt(playerid,"action",3);
		}
		if(listitem == 4)
		{
		    GuiHeader = " Поставить мут игроку";
		    GuiText =  " Введите причину\n\n";
			SetPVarInt(playerid,"action",4);
		}
		if(listitem == 5)
		{
		    GuiHeader = " Cнять мут игроку";
		    GuiText =  " Успешно\n\n";
			SetPVarInt(playerid,"action",5);
		}
		if(listitem == 6)
		{
		    GuiHeader = " Посадить игрока в тюрьму";
		    GuiText =  " Введите причину\n\n";
			SetPVarInt(playerid,"action",6);
		}
		if(listitem == 7)
		{
		    GuiHeader = " Выпустить игрока из тюрьмы";
		    GuiText =  " Успешно\n\n";
			SetPVarInt(playerid,"action",7);
		}
		if(listitem == 8)
		{
		    GuiHeader = " Телепортироваться к игроку";
		    GuiText =  " Успешно\n\n";
			SetPVarInt(playerid,"action",8);
		}
		if(listitem == 9)
		{
		    GuiHeader = " Телепортировать к себе";
		    GuiText =  " Успешно\n\n";
			SetPVarInt(playerid,"action",9);
		}
		if(GetPVarInt(playerid,"action") ==5 || GetPVarInt(playerid,"action") ==7 || GetPVarInt(playerid,"action") ==8 || GetPVarInt(playerid,"action") ==9 )
		{
        ShowPlayerDialog(playerid,DIALOG_CLICK2,DIALOG_STYLE_MSGBOX,GuiHeader,GuiText,"Ок","");
		}
		else
		{
		ShowPlayerDialog(playerid,DIALOG_CLICK2,DIALOG_STYLE_INPUT,GuiHeader,GuiText,"Отправить","Отмена ");
		}
       	return 1;	
    }
	case DIALOG_CLICK2:
	{   
	   format(ctmp,sizeof(ctmp),"%d %s",recid,inputtext);
	   new act=GetPVarInt(playerid,"action");
	   switch(act)
	   {
		case 0:
		{
		Player_Command(playerid,PLAYER_GIVECASH_CMD,ctmp);
		return 1;
		}
		case 1:
		{
		//SendClientMessageToAll(COLOR_WHITE,ctmp);
		Player_Command(playerid,PLAYER_PM_CMD,ctmp);
		return 1;
		}
		case 2:
		{
		format(ctmp,sizeof(ctmp),"Жалоба на игрока %s Причина - %s",oGetPlayerName(recid),inputtext);
		NotifyAdmins(ctmp);
		return 1;
		}
		case 3:
		{
        Admin_Command(playerid,ADMIN_KICK_CMD,ctmp);
		return 1;
		}
		case 4:
		{
        Admin_Command(playerid,ADMIN_MUTE_CMD,ctmp);
		return 1;
		}
		case 5:
		{
        Admin_Command(playerid,ADMIN_UNMUTE_CMD,ctmp);
		return 1;
		}
		case 6:
		{
        Admin_Command(playerid,ADMIN_JAIL_CMD,ctmp);
		return 1;
		}
		case 7:
		{
        Admin_Command(playerid,ADMIN_UNJAIL_CMD,ctmp);
		return 1;
		}
		case 8:
		{
        Admin_Command(playerid,ADMIN_TELETO_CMD,ctmp);
		return 1;
		}
		case 9:
		{
        Admin_Command(playerid,ADMIN_TELEHERE_CMD,ctmp);
		return 1;
		}
	   }
	}
    case DIALOG_CLICKSELF:
	{
	 
     switch(listitem)
	 {
	  case 0:
	  {
	 
	  Player_Command(playerid,PLAYER_STAT_CMD," ");
	  return 1;
	  }
	  case 1:
	  {
	  Player_Command(playerid,PLAYER_KILL_CMD," ");
	  return 1;
	  }
	  case 2:
	  {
	  Weapons_Command(playerid,WEAPONS_DROP_CMD," ");
	  return 1;
	  }
	  case 3:
	  {
	  Player_Command(playerid,PLAYER_ADMINS_CMD," ");
	  return 1;
	  }
	  case 4:
	  {
	  Player_Command(playerid,PLAYER_INFO_CMD," ");
	  return 1;
	  }
	  case 5:
	  {
	  Race_Command(playerid,RACE_MENU_CMD," ");
	  return 1;
	  }
	  case 6:
	  {
	  Deathmatch_Command(playerid,DEATHMATCH_MENU_CMD," ");
	  return 1;
	  }
	  case 7:
	  {
	  TeamQuest_Command(playerid,TEAM_QUEST_MENU_CMD," ");
	  return 1;
	  }
	  case 8:
	  {
	  Player_Command(playerid,PLAYER_UPGRADE_CMD," ");
	  return 1;
	  }
	  case 9:
	  {
	  World_Command(playerid,WORLD_TELEPORT_CMD," ");
	  return 1;
	  }
	  case 10:
	  {
	  Player_Command(playerid,PLAYER_RADIO_CMD," ");
	  return 1;
	  }
	  case 11:
	  {
	  Player_ResetSkin(playerid);
	  SendClientMessage(playerid,COLOR_GREEN,"Сохр. Данные о вашем скине сброшены, теперь вы можете выставить себе новый скин");
	  return 1;
	  }
	  case 12:
	  {
		//new listitems[320];
		stmp = "Пиво\nВино\nВиски\nSprunk\nЗа любовь!\nЗакурить";
		ShowPlayerDialog(playerid,DIALOG_DRINK,DIALOG_STYLE_LIST,"Бар",stmp," Выбрать"," Отмена");
		return 1;
	  }
	  case 13:
	  {
		//new listitems[320];
		stmp = "Обычный";//\nБокс\nКунг-фу\nКикбоксинг\nУдар с захвата\nУдар локтем";
		if(GetPVarInt(playerid,"FightStyle")>1) strcat(stmp,"\nБокс",sizeof(stmp));
		if(GetPVarInt(playerid,"FightStyle")>3) strcat(stmp,"\nКунг-Фу",sizeof(stmp));
		if(GetPVarInt(playerid,"FightStyle")>5) strcat(stmp,"\nКикбоксинг",sizeof(stmp));
		if(GetPVarInt(playerid,"FightStyle")>9) strcat(stmp,"\nУдар с захвата\nУдар локтем",sizeof(stmp));
		ShowPlayerDialog(playerid,DIALOG_FIGHT,DIALOG_STYLE_LIST,"Стиль боя",stmp," Выбрать"," Отмена");
		return 1;
	  }
	  case 14: //INVIZ
	  {
		stmp = "10 Минут - 100,000$\n15 Минут - 250,000$\n30 Минут -500,000$";
		ShowPlayerDialog(playerid,DIALOG_INVIZ,DIALOG_STYLE_LIST,"Невидимость",stmp," Выбрать"," Отмена");
		return 1;
	  
	  }
	 }
	}
	case DIALOG_INVIZ:
	{
	//new tmp[16];
	if(inviztimer[playerid]>0) return SendClientMessage(playerid,COLOR_LIGHTRED,"Вы уже используете невидимость!");
	switch(listitem)
		{
		case 0: //10m
			{
			if(oGetPlayerMoney(playerid)<100000) return SendClientMessage(playerid,COLOR_LIGHTRED,"У вас недостаточно денег!");
			
			inviztimer[playerid]=600;
			PlayerTextDrawShow(playerid,CInvizTime[playerid]);
			//format(stmp,sizeof(stmp),"%s",ConvertSecMins(inviztimer[playerid]));
			//PlayerTextDrawSetString(playerid,CInvizTime[playerid],stmp);
			SendClientMessage(playerid,COLOR_BLUE,"Вы невидимы на радаре на 10 минут!");
			oGivePlayerMoney(playerid,-100000);
			}
		case 1: //15m
			{
			if(oGetPlayerMoney(playerid)<250000) return SendClientMessage(playerid,COLOR_LIGHTRED,"У вас недостаточно денег!");
			inviztimer[playerid]=900;
			PlayerTextDrawShow(playerid,CInvizTime[playerid]);
			//format(stmp,sizeof(stmp),"%s",ConvertSecMins(inviztimer[playerid]));
			//PlayerTextDrawSetString(playerid,CInvizTime[playerid],stmp);
			SendClientMessage(playerid,COLOR_BLUE,"Вы невидимы на радаре на 15 минут!");
			oGivePlayerMoney(playerid,-250000);
			}
		case 2: //30m
			{
			if(oGetPlayerMoney(playerid)<500000) return SendClientMessage(playerid,COLOR_LIGHTRED,"У вас недостаточно денег!");
			inviztimer[playerid]=1800;
			PlayerTextDrawShow(playerid,CInvizTime[playerid]);
			//format(stmp,sizeof(stmp),"%s",ConvertSecMins(inviztimer[playerid]));
			//PlayerTextDrawSetString(playerid,CInvizTime[playerid],stmp);
			SendClientMessage(playerid,COLOR_BLUE,"Вы невидимы на радаре на 30 минут!");
			oGivePlayerMoney(playerid,-500000);
			}
	
		}

	
			//SetPlayerMarkerForPlayer( i, playerid, ( GetPlayerColor( playerid ) & 0xFFFFFF00 ) );
	SetPlayerColor(playerid,GetPlayerColor( playerid ) & 0xFFFFFF00);
			//print("SET");
			
	return 1;
	}
	
	case DIALOG_FIGHT:
	{
		switch(listitem)
		{
		case 0: //default
			{
			SetPlayerFightingStyle(playerid,4);
			return 1;
			}
		case 1: //BOX
			{
			SetPlayerFightingStyle(playerid,5);
			//print("SET");
			return 1;
			}
		case 2: //ДЖЕКИ ЧАН
			{
			SetPlayerFightingStyle(playerid,6);
			return 1;
			}	
		case 3: //KICKBOX
			{
			SetPlayerFightingStyle(playerid,7);
			return 1;
			}
		case 4: //GRABKICK
			{
			SetPlayerFightingStyle(playerid,15);
			return 1;
			}		
		case 5: //EBLOW
			{
			SetPlayerFightingStyle(playerid,16);
			return 1;
			}
		}
	}
	case DIALOG_DRINK:
	{
		switch(listitem)
		{
			case 0:
			{
			SetPlayerSpecialAction(playerid,SPECIAL_ACTION_DRINK_BEER);
			return 1;
			
			}
			case 1:
			{
			SetPlayerSpecialAction(playerid,SPECIAL_ACTION_DRINK_WINE);
			return 1;
			}
			case 2:
			{
			SetPlayerSpecialAction(playerid,SPECIAL_ACTION_DRINK_WINE);
			return 1;
			
			}
			case 3:
			{
			SetPlayerSpecialAction(playerid,SPECIAL_ACTION_DRINK_SPRUNK);
			return 1;
			}
			case 4:
			{
			SetPlayerSpecialAction(playerid,SPECIAL_ACTION_DRINK_WINE);
			PlayAudioStreamForPlayer(playerid,"https://dl.dropboxusercontent.com/u/45522772/SAMP/DFL.mp3");
			format(stmp,sizeof(stmp),"%s пьет за любовь!",oGetPlayerName(playerid));
			SendClientMessageToAll(COLOR_YELLOW,stmp);
			return 1;			
			}
			case 5:
			{
			SetPlayerSpecialAction(playerid,SPECIAL_ACTION_SMOKE_CIGGY);
			return 1;
			}
		}
		
	}
   }
   return 1;
}

click_update()
{
//print("TICK");
//for(new i=0;i<=GetPlayersCount();i++)
foreach(new i : Player)
			{
			if(inviztimer[i]==1)
			{
			SetPlayerColor(i, GetPlayerColor( i ) | 0x000000FF );
			PlayerTextDrawHide(i,CInvizTime[i]);
			print("HIDDEN");
			//continue;
			}
			if(inviztimer[i]==0) continue;
			format(stmp,sizeof(stmp),"%s",ConvertSecMins(inviztimer[i]));
		//	print(stmp);
			PlayerTextDrawSetString(i,CInvizTime[i],stmp);
			//PlayerTextDrawSetString(i,CInvizTime[i],ConvertSecMins(inviztimer[i]));
			inviztimer[i]--;
			}

}
// click_OnPlayerStreamIn(playerid, forplayerid)
// {

// //if(inviztimer[playerid]>0) SetPlayerMarkerForPlayer( forplayerid, playerid, ( GetPlayerColor( playerid ) & 0xFFFFFF00 ) );

// return 1;
// }
click_OnPlayerDeath(playerid)
{

PlayerTextDrawHide(playerid,CInvizTime[playerid]);
return 1;
}
ShowHUD(playerid)
{
	  PlayerTextDrawShow(playerid,Pmodel[playerid]);
	  PlayerTextDrawShow(playerid,PNick[playerid]);
	  PlayerTextDrawShow(playerid,PXP[playerid]);
	  format(stmp,sizeof(stmp),"%d",GetPVarInt(playerid,"XP"));
	 // new str[3200];
	  
	  format(stmp,sizeof(stmp),"%s XP", FormComma(stmp));
	  //print(stmp);
	  //strins(str,FormComma(stmp),0);
	  PlayerTextDrawSetString(playerid,PXP[playerid],stmp);


	  return 1;
}
HideHUD(playerid)
{

	 PlayerTextDrawHide(playerid,Pmodel[playerid]);
	 PlayerTextDrawHide(playerid,PNick[playerid]);
	 PlayerTextDrawHide(playerid,PXP[playerid]);
	 return 1;
}