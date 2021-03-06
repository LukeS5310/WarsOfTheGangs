/*
*	Created:			21.08.12
*	Author:				009
*	Last Modifed:		-
*	Description:		dijkstra by GTA SA Nodes (Controllable NPC)
*/


#if defined _dijkstra_included
	#endinput
#endif


#define _dijkstra_included
#pragma library dijkstra

#define MAX_POINTS_TO_LOAD 10000
//#define DEBUG(%1); printf(%1);
#define DEBUG(%1); 

enum npoints_data
{
	Float:pos[3],
	links[5],
	linkcount,
	Float:dist,
	parentid,
	step
};
new npoints[MAX_POINTS_TO_LOAD][npoints_data],
	npoints_check[MAX_POINTS_TO_LOAD char],
	npoints_count,
	points_link[64][1000],
	Node:nodes[64];

stock dijkstra_init()
{
	new string[32];
	// ��������� ����� �����
	for(new i = 0;i < 64;i++)
	{
	    format(string,sizeof(string),"Nodes/NODES%d.DAT",i);
		nodes[i] = OpenNode(string);
	}
}

stock dijkstra_exit()
{
	// ��������� ����� �����
	for(new i = 0;i < 64;i++)
	{
	    CloseNode(nodes[i]);
	}
}
	
stock CalculatePath(Float:startx,Float:starty,Float:startz,Float:endx,Float:endy,Float:endz,Float:path[][3],&pathlen)
{
	new header[4];
	new Float:tpos[3];
	new startnode;
	new endnode;
	new startpoint;
	new endpoint;
	new Float:startlen = 65000.0;
	new Float:endlen = 65000.0;
	new Float:tmp[5];
	new pointid;
	new nodes_to_load[64 char];
	new startdata[2];
	new enddata[2];
	new maxpath = pathlen;
	new Float:min_dist;
	npoints_count = 0;
	
	DEBUG("CalculatePath(%f,%f,%f,%f,%f,%f)",startx,starty,startz,endx,endy,endz);
	
	// �������� ����� �������� � ������� ��������� ������ � �����
	startdata[0] = floatround(((startx + 3000.0) / 750.0),floatround_floor);
	startdata[1] = floatround(((starty + 3000.0) / 750.0),floatround_floor);
	enddata[0] = floatround(((endx + 3000.0) / 750.0),floatround_floor);
	enddata[1] = floatround(((endy + 3000.0) / 750.0),floatround_floor);
	startnode = (startdata[1] * 8) + startdata[0];
	endnode = (enddata[1] * 8) + enddata[0];
	
	DEBUG("%d %d",startnode,endnode);
	
	// �������� ��������� ��� ��������
	for(new i = 0;i < 2;i++)
	{
		if(startdata[i] > enddata[i])
		{
		    new temp = startdata[i];
		    startdata[i] = enddata[i];
		    enddata[i] = temp;
		}
	}
	
	DEBUG("%d %d %d %d",startdata[0],startdata[1],enddata[0],enddata[1]);
	
	// �������� �������� ��� �������� �����
	for(new i = startdata[0];i <= enddata[0];i++)
	{
	    for(new j = startdata[1];j <= enddata[1];j++)
	    {
	        new nodeid = (j * 8) + i;
	        nodes_to_load{nodeid} = 1;
	    }
	}
	// ��������� ��������� ����
	for(new nodeid = 0;nodeid < 64;nodeid++)
	{
	    if(!nodes_to_load{nodeid}) continue;

	    startdata[0] = npoints_count;
	    // �������� ����������� ����� �����
		GetNodeHeader(nodes[nodeid],header[0],header[1],header[2],header[3]);
		// ������ ��� ������������ ����
		for(new i = 0;i < header[1];i++)
		{
		    // �������� ������� �����
			SetNodePoint(nodes[nodeid],i);
			GetNodePointPos(nodes[nodeid],tpos[0],tpos[1],tpos[2]);
			// ��������� � � ������ �����
			npoints[npoints_count][pos][0] = tpos[0];
			npoints[npoints_count][pos][1] = tpos[1];
			npoints[npoints_count][pos][2] = tpos[2];
			// �������� ����� �����
			npoints[npoints_count][links][0] = GetNodePointLinkId(nodes[nodeid]);
			npoints[npoints_count][linkcount] = GetNodePointLinkCount(nodes[nodeid]);
			if(npoints[npoints_count][linkcount] > 5) printf("ERROR! linkcount %d",npoints[npoints_count][linkcount]);
			// ������� ������� �����
			points_link[nodeid][i] = npoints_count + 1;
			// �������� ������
			npoints_check{npoints_count} = 0;
			npoints[npoints_count][dist] = 65000.0;
			npoints[npoints_count][parentid] = 0;
			npoints[npoints_count][step] = 0;
			// ��� �������� ������ ������������ �������������...
			if(nodeid == startnode)
			{
				// ��������� ���������� �� �� �� ����� ������
				tmp[0] = tpos[0] - startx;
				tmp[1] = tpos[1] - starty;
				tmp[2] = tpos[2] - startz;
				tmp[3] = floatsqroot((tmp[0]*tmp[0]) + (tmp[1]*tmp[1]) + (tmp[2]*tmp[2]));
				// ��������� ����� �� ��� ����� �� ����� ������
				if(startlen > tmp[3])
				{
				    DEBUG("set start %d %f [%f,%f,%f]",npoints_count,tmp[3],tpos[0],tpos[1],tpos[2]);
					
				    // ����� �����, ��������� �
				    startpoint = npoints_count;
				    startlen = tmp[3];
				}
			}
			// ��� �������� ����� ������������ �������������...
			if(nodeid == endnode)
			{
				// ��������� ���������� �� �� �� ����� ������
				tmp[0] = tpos[0] - endx;
				tmp[1] = tpos[1] - endy;
				tmp[2] = tpos[2] - endz;
				tmp[3] = floatsqroot((tmp[0]*tmp[0]) + (tmp[1]*tmp[1]) + (tmp[2]*tmp[2]));
				// ��������� ����� �� ��� ����� �� ����� �����
				if(endlen > tmp[3])
				{
				    DEBUG("set end %d %f [%f,%f,%f]",npoints_count,tmp[3],tpos[0],tpos[1],tpos[2]);
					
				    // ����� �����, ��������� �
				    endpoint = npoints_count;
				    endlen = tmp[3];
				}
			}
			// ����������� ����������� ����������� �����
			npoints_count++;
			if(npoints_count == MAX_POINTS_TO_LOAD)
			{
			    printf("ERROR! npoints count limit!");
			    break;
			}
		}
		
		DEBUG("read links [%d - %d]",startdata[0],npoints_count);
		
		// ������ ����� ����������� �����
		for(new i = startdata[0];i < npoints_count;i++)
		{
		    // �������� �����
		    new lid = npoints[i][links][0];
		    new lc = 0;
		    for(new j = 0;j < npoints[i][linkcount];j++)
		    {
		        SetNodeLink(nodes[nodeid],(lid + j));
		        pointid = points_link[GetNodeLinkAreaId(nodes[nodeid])][GetNodeLinkNodeId(nodes[nodeid])];
		        if(pointid != 0)
		        {
		       		DEBUG("link %d[%d] - %d",i,j,pointid);
					
		            npoints[i][links][lc] = pointid - 1;
		            lc++;
		        }
		    }
			npoints[i][linkcount] = lc;
		}
	}
	// ��������� ����� ����

	DEBUG("start path calc %d - %d",startpoint,endpoint);
	
	// ������ ���������� ����� ������
	npoints[startpoint][dist] = 0.0;
	// �������� �������� � ���� �������
	new cur = startpoint,
	    last;
	// ������������ ���� �� ��������� ������������ �������
	while(!npoints_check{cur})
	{
	    npoints_check{cur} = 1;
	    last = cur;

		// ����� �� ����
	    if(cur == endpoint) break;

	    for(new i = 0;i < npoints[cur][linkcount];i++)
	    {
	        new pid = npoints[cur][links][i];
	        // ��������� ������ �� ����������� �����
	        if(npoints_check{pid}) continue;
	        // ��������� ���������� �� ������� ������� �� ���������
			tmp[0] = npoints[pid][pos][0] - npoints[cur][pos][0];
			tmp[1] = npoints[pid][pos][1] - npoints[cur][pos][1];
			tmp[2] = npoints[pid][pos][2] - npoints[cur][pos][2];
			tmp[3] = floatsqroot((tmp[0]*tmp[0]) + (tmp[1]*tmp[1]) + (tmp[2]*tmp[2]));
			// �������� ���������� �� ������
	        new Float:d = npoints[cur][dist] + tmp[3];
			
	        DEBUG("[%d - %d] %f - %f",cur,pid,tmp[3],d);
			
	        // �������� �������
	        if(d < npoints[pid][dist])
	        {
	            npoints[pid][dist] = d;
	            npoints[pid][parentid] = cur;
	            npoints[pid][step] = npoints[cur][step] + 1;
	        }
	    }

	    // ���� ������������� ������� � ����������� �����������
		min_dist = 65000.0;
	    for(new i = 0;i < npoints_count;i++)
	    {
	        if(npoints_check{i}) continue;
	        
	        if(npoints[i][dist] < min_dist) 
			{
				cur = i;
				min_dist = npoints[i][dist];
			}
	    }
		
	    DEBUG("new cur %d %f",cur,npoints[cur][dist]);
	}
	// ������������� ����
	if(last != endpoint)
	{
		pathlen = 0;
	}
	else
	{
		cur = last;
		pathlen = npoints[cur][step];

		DEBUG("data %d",pathlen);
		
		for(new i = 0;i <= pathlen;i++)
		{
			if(i == maxpath) break;
			
			for(new j = 0;j < 3;j++) path[pathlen - i][j] = npoints[cur][pos][j];
			cur = npoints[cur][parentid];
		}
	}
}