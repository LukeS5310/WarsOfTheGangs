/*

	TJs Anti Air Filterscript
	Author: TJ (TTJJ SA:MP forums)
	Credits: TJ, LucifeR (SetObjectToFaceCords functionality)

*/
#include <a_samp>

#define MAX_MISSLES 400//The maximum number of missles that can be created at any one time
#define MISSLE_INTERVAL 200//The timers interval, increasing this will reduce the number of missles fired
#define MISSLE_SPEED 215.0//The speed of the missles, increase this to make the missles go faster and the avoidance of them harder
#define BLAST_RADIUS 30.0//The blast radius for the explosion caused by the missles

forward antiair();

new anti_air_timer;
new missles[MAX_MISSLES];

new Float:anti_air_origins[4][4] = {//The anti-air positions. More can be defined by adding coords to this list, Syntax: X, Y, Z, Range

	{-1365.7441,1489.6222,11.0391,450.0},
	{-1379.0414,1493.6753,16.3203,450.0},
	{-1380.1416,1484.3296,16.3203,450.0},
	{187.3830,2084.9536,22.6468,450.0}

};

public OnFilterScriptInit()
{
	print("\n--------------------------------------");
	print(" TJs Anti-Air Filterscript v1.0");
	print("--------------------------------------\n");
	anti_air_timer = SetTimer("antiair",MISSLE_INTERVAL,1);
	ResetMissles();
	return 1;
}

public OnFilterScriptExit()
{
	KillTimer(anti_air_timer);
	return 1;
}

public antiair()
{

	for(new i = 0; i < MAX_PLAYERS; i ++)
	{
	
	    if(IsPlayerConnected(i))
	    {
	    
	        for(new a = 0; a < sizeof(anti_air_origins); a ++)
			{
			
				if(IsPlayerInRangeOfPoint(i,anti_air_origins[a][3],anti_air_origins[a][0],anti_air_origins[a][1],anti_air_origins[a][2]))
				{
				
				    if(IsInAircraft(i))
				    {
				    
						new slot = FetchNextMissleSlot();
						if(slot > -1)
						{
						
						    new Float:X, Float:Y, Float:Z;
						    GetPlayerPos(i,X,Y,Z);
						    missles[slot] = CreateObject(345,anti_air_origins[a][0],anti_air_origins[a][1],anti_air_origins[a][2],0.0,0.0,0.0);
						    SetObjectToFaceCords(missles[slot], X, Y, Z);
						    MoveObject(missles[slot],X,Y,Z,MISSLE_SPEED);
						
						}
				    
				    }
				
				}
			
			}
	    
	    }
	
	}

}

public OnObjectMoved(objectid)
{

	new slot = FetchMissleSlot(objectid);
	if(slot > -1)
	{
	
	    new Float:X, Float:Y, Float:Z;
	    GetObjectPos(objectid,X,Y,Z);
	    CreateExplosion(X,Y,Z,2,BLAST_RADIUS);
	    DestroyObject(objectid);
	    missles[slot] = -1;
	
	}

}

stock FetchMissleSlot(objectid)
{

	for(new i = 0; i < MAX_MISSLES; i ++)
	{
	
	    if(missles[i] == objectid) return i;
	
	}
	return -1;

}

stock IsInAircraft(playerid)
{

	if(!IsPlayerInAnyVehicle(playerid)) return false;
	new vehicleid = GetPlayerVehicleID(playerid);
	switch(GetVehicleModel(vehicleid))
	{
	
		case
	    	460,464,476,511,512,513,519,520,553,577,592,593,
	    	417,425,447,465,469,487,488,497,501,548,563:
		return true;
		
	}
	return false;
}

stock ResetMissles()
{

	for(new i = 0; i < MAX_MISSLES; i ++) missles[i] = -1;

}

stock FetchNextMissleSlot()
{

	for(new i = 0; i < MAX_MISSLES; i ++)
	{
	
	    if(missles[i] == -1) return i;
	
	}
	return -1;

}

stock SetObjectToFaceCords(objectid, Float:x1, Float:y1, Float:z1)
{

    //   SetObjectToFaceCords() By LucifeR   //

    new Float:x2,Float:y2,Float:z2;
    GetObjectPos(objectid, x2,y2,z2);

    new Float:DX = floatabs(x2-x1);
    new Float:DY = floatabs(y2-y1);
    new Float:DZ = floatabs(z2-z1);

    new Float:yaw = 0;
    new Float:pitch = 0;

    if(DY == 0 || DX == 0)
    {
    
    	if(DY == 0 && DX > 0)
		{
        	yaw = 00;
            pitch = 0;
		}
        else if(DY == 0 && DX < 0)
		{

			yaw = 180;
            pitch = 180;

		}
        else if(DY > 0 && DX == 0)
		{

			yaw = 90;
            pitch = 90;

		}
        else if(DY < 0 && DX == 0)
		{
        	yaw = 270;
            pitch = 270;

		}
        else if(DY == 0 && DX == 0)
		{
        	yaw = 0;
        	pitch = 0;
		}
		
    }
    else
    {
		yaw = atan(DX/DY);
		pitch = atan(floatsqroot(DX*DX + DZ*DZ) / DY);
        if(x1 > x2 && y1 <= y2)
		{
		
        	yaw = yaw + 90;
            pitch = pitch - 45;
            
		}
        else if(x1 <= x2 && y1 < y2)
		{
		
        	yaw = 90 - yaw;
            pitch = pitch - 45;

		}
        else if(x1 < x2 && y1 >= y2)
		{
		
            yaw = yaw - 90;
            pitch = pitch - 45;
            
		}
        else if(x1 >= x2 && y1 > y2)
		{
		
        	yaw = 270 - yaw;
            pitch = pitch + 315;
            
		}
        if(z1 < z2)
        {
        
    		pitch = 360-pitch;
    		
        }
        SetObjectRot(objectid, 0, 0, yaw);
        SetObjectRot(objectid, 0, pitch, yaw+90);

	}
        
}