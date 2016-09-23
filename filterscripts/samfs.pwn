// This is a comment
// uncomment the line below if you want to write a filterscript
//#define FILTERSCRIPT

#include <a_samp>
#include <a_npc>

new mars,samol;
public OnFilterScriptInit()
{
print("NPC LOADED");
ConnectNPC("Ashot","racer1");
ConnectNPC("Ahmed","racer2");
mars = CreateVehicle(429, 0.0, 0.0, 5.0, 0.0, random(255),random(255), 5000);
samol = CreateVehicle(506, 0.0, 0.0, 5.0, 0.0, random(255), random(255), 5000);
//SetVehicleVirtualWorld(samol,5310);
//SetVehicleVirtualWorld(mars,5310);
return 1;
}
public OnPlayerSpawn(playerid)
{
             if(IsPlayerNPC(playerid)) //Checks if the player that just spawned is an NPC.
             {
                      //SetPlayerVirtualWorld(playerid,5310);
                      new npcname[MAX_PLAYER_NAME];
                      GetPlayerName(playerid, npcname, sizeof(npcname));
                      if(!strcmp(npcname, "Ashot", true)) //проверяем имя MyFirstNPC
                      {
                       print("NPC SEL OWPAIOHFWA");
                       PutPlayerInVehicle(playerid, mars, 0); // Зажаем NPC В созданую для него машину
                       return 1;
                      }
                      if(!strcmp(npcname, "Ahmed", true))
                      {
                       PutPlayerInVehicle(playerid, samol, 0);
                       return 1;
                      }
                      return 1;
             }
             return 1;
}

