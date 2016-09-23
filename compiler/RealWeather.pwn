#include <a_samp>
#include <a_http>

//http://articles.org.ru/cn/showdetail.php?cid=7065 <- ���� �������� ��� �������� �������� ������!

#define FileName "Config"   //��� ����� ������������ (������� �����������!)
#define TimeUpdate 20       //����� �������� ���������� ������ (� �������)
#define TimePause 10        //����� ��� ������� ������
#define DIALOGID 899
#define COLOR_WHITE 0xFFFFFFAA 		// white

forward WeatherParser(index, response_code, data[]);
forward UpdateWeather();

enum infoWeather
{
	date[3],            //����, �� ������� ��������� ������� � ������ ����� ([day], [monthe], [year])
	hour,               //������� �����, �� ������� ��������� �������
	tod,                //����� �����, ��� �������� ��������� �������: 0 - ���� 1 - ����, 2 - ����, 3 - �����
	weekday,            //���� ������, 1 - �����������, 2 - �����������, � �.�.
	cloudiness,         //���������� �� ���������:  0 - ����, 1- �����������, 2 - �������, 3 - ��������
	precipitation,      //��� �������: 4 - �����, 5 - ������, 6,7 � ����, 8 - �����, 9 - ��� ������, 10 - ��� �������
	rpower,             //������������� �������, ���� ��� ����. 0 - �������� �����/����, 1 - �����/����
	spower,             //����������� �����, ���� ��������������: 0 - �������� �����, 1 - �����
	pressure[2],        //����������� ��������, � ��.��.��. ([max], [min])
	temperature[2],     //����������� �������, � �������� ������� ([max], [min])
	wind[4],            //��������� ����� ([min], [max], [direction])
	relwet[3],          //������������� ��������� �������, � % ([max], [min])
	heat[2],            //������� - ����������� ������� �� �������� ������� �� ������ ��������, ���������� �� ����� ([max], [min])
};

enum infoConfig
{
	nameParameter[64],      //��� ���������
	valueParameter[32],    //�������� ���������
	bool:readParameter,     //������� �� �������� �� ����� ������������
};

/**
*   ����������� ��������� ������������
*   ������������ � ��� ������ ���� ����
*	������������ �� ��� �������� ��� ������
*/
new Config[][infoConfig] = {
	{"CityCode", "28440", false},       	//��� ������ ������
	{"CityName", "������������", false},	//��������� �������� � ����� �������� ������
	{"RealWeather", "1", false},        	//�������� ����� �������� ������ �� �������
	{"ShowForecast", "1", false}			//��������� �������� � ����� �������� ������
//	{"AccessForecast", "0", false},     	//��������� ������ ������� ������ �� ������ ������
};

/*
*   ������ ��� ����������� ������� �� 4 ���
*   5 ��� - ����������� Gismeteo.ru
*	������ �������� ������ ���������!
*/
new Forecast[5][infoWeather];

/*
*   ���������� ��������� ������
*	[0] - ���� ������������ �� ��������
*	[1] - ������ �� �����������
*   [2] - ������ ������ �������� ������
*   [3] - �� ����� �������� ������ ��������� ������
*/
new errorCode[4];
new runUpdate = 0 char, Timer = -1;	//�����

public OnFilterScriptInit()
{
	errorCode[0] = LoadConfig();    //��������� ������
	errorCode[1] = LoadWeather();   //��������� ������ �������
	if(strval(Config[2][valueParameter]))UpdateWeather();	//���� ������� ����� �������� ������ ������ ������
	return 1;
}

public OnFilterScriptExit()
{
	if(Timer != -1)KillTimer(Timer);	//������� ������ ��� ������ ���� �� ������
	return 1;
}

public OnPlayerStateChange(playerid, newstate, oldstate)
{
	/*
	*   ����������� ��������� ����� ��� ��������� ������
	*/
	new time[3];
	gettime(time[0],time[1],time[0]);
	if(runUpdate - time[1] <= 0)
	{
	    runUpdate = (time[1] + TimeUpdate) > 59 ? (time[1] + TimeUpdate) - 60 : (time[1] + TimeUpdate) ;
	    errorCode[1] = LoadWeather();   //��������� �������
	    if(strval(Config[2][valueParameter]))	//���� ������� ����� �������� ������ ������ ������
		{
		    if(Timer != -1)KillTimer(Timer);
		    /*
		    *   ������ �������� � ����� Gismeteo.ru �������� �����,
		    *   ������� ��� ����� �� ���������� ������, ����������� 10 ��������� �����
		    */
			Timer = SetTimer("UpdateWeather",TimePause*1000,false);
		}
	}
	return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	if(!strcmp(cmdtext, "/weather", true,8))	//�������� ��������
	{
	    if(!strval(Config[3][valueParameter]))return SendClientMessage(playerid,COLOR_WHITE,"DEBUG:������� �� ��� ����� �����");
		new timeDay[][6] = {"����","����","����","�����"};
		new Cloudiness[][12] = {"����", "�����������", "�������", "��������"};
		new Precipitation[][12] = {"�����", "������", "����", "�����", "��� ������", "��� �������"};
		new Wind[][17] = {"��������", "������-���������", "���������", "���-���������", "�����", "���-��������", "��������", "������-��������"};
		new Weekday[][12] = { "�����������", "�����������", "�������", "�����", "�������", "�������", "�������"};
		new string[1524],y,u;
		format(string, sizeof(string), " ");
	    for(new i; i < sizeof(Forecast);i++)
	    {
	        u = Forecast[i][weekday] - 1;
			switch(Forecast[i][precipitation])
			{
			    case 4:y = 0;
			    case 5:y = 1;
			    case 6,7:y = 2;
			    case 8:y = 3;
			    case 9:y = 4;
			    case 10:y = 5;
			}
	        format(string, sizeof(string), "%s�� %s (%s) %d.%d.%d ���������:\n\t����������: %s\n\t������: %s\n\t����������� ��������: %d � %d ��.��.��.\n", string,
				Weekday[u], timeDay[Forecast[i][tod]], Forecast[i][date][0], Forecast[i][date][1], Forecast[i][date][2],
				Cloudiness[Forecast[i][cloudiness]],Precipitation[y],
				Forecast[i][pressure][0], Forecast[i][pressure][1]);
			format(string, sizeof(string), "%s\t����������� �������: %d � %d �C\n\t����� %s: %d � %d �/�\n\t��������� �������: %d � %d %%\n\t�������: %d � %d �C\n\n", string,
				Forecast[i][temperature][0], Forecast[i][temperature][1],
				Wind[Forecast[i][wind][2]], Forecast[i][wind][0], Forecast[i][wind][1],
				Forecast[i][relwet][0], Forecast[i][relwet][1],
				Forecast[i][heat][0], Forecast[i][heat][1]);
	    }
	    strcat(string,"\n\t\t\t������� ������������ ������ Gismeteo.ru � ���������� $tepashka");
	    new tmp[128];
	    format(tmp, sizeof(tmp), "�. %s. ������� ������ �� ��������� ���", Config[1][valueParameter]);
	    ShowPlayerDialog(playerid, DIALOGID, DIALOG_STYLE_MSGBOX, tmp, string, "Ok", "Close");
		return 1;
	}
	if(!strcmp(cmdtext, "/wdebug", true,7)) //��������� ����������
	{
		if(!IsPlayerAdmin(playerid))return 1;
		new string[256],errorKey = 1;
		format(string, sizeof(string), "���������:\n");
		new Load[][3] = {" *", "�"};
		for(new i; i < sizeof(Config);i++)
		{
			format(string, sizeof(string), "%s\t%s%s\t\t\"%s\"\n", string, Config[i][nameParameter], Load[Config[i][readParameter]], Config[i][valueParameter]);
		}
		format(string, sizeof(string), "%s\n\n������:\n", string);
		if(errorCode[0])
		{
			format(string, sizeof(string), "%s!\t���� ������������ ����� ��������.\n", string);
			errorKey = 0;
		}
		if(errorCode[1])
		{
			format(string, sizeof(string), "%s!\t������ �� Gismeteo.ru �� ��� ����������.\n", string);
			errorKey = 0;
		}
		if(errorCode[2])
		{
			format(string, sizeof(string), "%s!\t������ � Gismeteo.ru �� ������.\n", string);
			errorKey = 0;
		}
		if(errorCode[3])
		{
			format(string, sizeof(string), "%s!\t�� ����� �������� ������ ��������� ������.\n", string);
			errorKey = 0;
		}
		if(errorKey)
		{
			format(string, sizeof(string), "%s\t������ ���.", string);
		}
		strcat(string,"\n\n* - ������������ �������� �� ���������");
		ShowPlayerDialog(playerid, DIALOGID, DIALOG_STYLE_MSGBOX, "������ �������� �������:", string, "Ok", "Close");
		return 1;
	}
	if(!strcmp(cmdtext, "/wupdate", true,8))    //������ ������ �������� (���������� ����������)
	{
		if(!IsPlayerAdmin(playerid))return 1;
	    errorCode[1] = LoadWeather();
	    if(Timer != -1)KillTimer(Timer);
	    Timer = SetTimer("UpdateWeather",TimePause*1000,false);
	    strins(Config[2][valueParameter],"1",0);
	    Config[2][readParameter] = true;
		SendClientMessage(playerid, 0x666666ff, "WeatherSystem: Update.");
		return 1;
	}
	if(!strcmp(cmdtext, "/wset", true, 6))		//���������� ������ ������� (���������� �����������)
	{
	    if(!IsPlayerAdmin(playerid))return 1;
	    strins(Config[2][valueParameter],"0",0);
	    Config[2][readParameter] = true;
	    new tmp[4];
	    strmid(tmp,cmdtext,12,strlen(cmdtext));
		if(!strlen(tmp))return SendClientMessage(playerid, 0x666666ff, "USAGE: /wset [weatherid]");
		SetWeather(strval(tmp));
		SendClientMessage(playerid, 0x666666ff, "WeatherSystem: Weather Set.");
		return 1;
	}
	return 0;
}

/*
*   ��������� ������ � ������������ � ��������� �� ������
*/
public UpdateWeather()
{
	if(Timer != -1)KillTimer(Timer);
	Timer = -1;
	switch(Forecast[0][precipitation])
	{
	    case 6,7:
	    {
			SetWeather(19);
			return 1;
	    }
	    case 4,5:
	    {
			SetWeather(16);
			return 1;
	    }
		case 8:
		{
		SetWeather(8);
		return 1;
		}
	}
	switch(Forecast[0][cloudiness])
	{
	    case 0:SetWeather(10);
	    case 1:SetWeather(12);
	    case 2:SetWeather(13);
	    case 3:SetWeather(14);
	}
	//new str[20];
	//format(str,sizeof(str),"%d",Forecast[0][cloudiness]);
	//SendClientMessageToAll(COLOR_WHITE,str);
	return 1;
}

/*
*   ������ �������� ����������� � Gismeteo.ru
*/
public WeatherParser(index, response_code, data[])
{
	if(response_code == 200)
	{
	    errorCode[3] = true;
	    new endPosition = 0, startPosition = endPosition, stringNum = 0, dayNum = 0, subStringNum = 0;
	    do
		{
		    endPosition = strfind(data, "\n", false, startPosition);
			if(-1 < endPosition < strlen(data))
			{
			    stringNum++;
			    if(4 < stringNum < 44)
			    {
			        new tempString[256];
			        strmid(tempString,data,startPosition,endPosition);
			        if(strfind(tempString, "</TOWN>", false) > -1)break;
			        if(strfind(tempString, "</FORECAST>", false) > -1)
					{
					    if(!strval(Config[3][valueParameter]))break;
					    subStringNum = 0;
						dayNum++;
					}
			        else
			        {
			            if(subStringNum < 7)
						{
						    new temp[64];
						    switch(subStringNum)
						    {
						        case 0:
						        {
						            strmid(temp,tempString,18,20);
						            Forecast[dayNum][date][0] = strval(temp);
						            strmid(temp,tempString,29,31);
						            Forecast[dayNum][date][1] = strval(temp);
						            strmid(temp,tempString,39,43);
						            Forecast[dayNum][date][2] = strval(temp);
						            strmid(temp,tempString,51,53);
						            Forecast[dayNum][hour] = strval(temp);
						            strmid(temp,tempString,60,61);
						            Forecast[dayNum][tod] = strval(temp);
						            strmid(temp,tempString,84,85);
						            Forecast[dayNum][weekday] = strval(temp);
								}
						        case 1:
						        {
						            strmid(temp,tempString,27,28);
						            Forecast[dayNum][cloudiness] = strval(temp);
						            new a = strfind(tempString, "\"", false, 46);
						            strmid(temp,tempString,45,a);
						            Forecast[dayNum][precipitation] = strval(temp);
						            strmid(temp,tempString,a+10,a+11);
						            Forecast[dayNum][rpower] = strval(temp);
						            strmid(temp,tempString,a+21,a+22);
						            Forecast[dayNum][spower] = strval(temp);
								}
						        case 2:
						        {
						            strmid(temp,tempString,19,22);
						            Forecast[dayNum][pressure][0] = strval(temp);
						            strmid(temp,tempString,29,32);
						            Forecast[dayNum][pressure][1] = strval(temp);
								}
						        case 3:
						        {
						            new a = strfind(tempString, "\"", false, 22);
						            strmid(temp,tempString,22,a);
						            Forecast[dayNum][temperature][0] = strval(temp);
						            a = strfind(tempString, "\"", false, a+1)+1;
						            strmid(temp,tempString,a,strfind(tempString, "\"", false, a+1));
						            Forecast[dayNum][temperature][1] = strval(temp);
								}
						        case 4:
						        {
						            new a = strfind(tempString, "\"", false, 15);
						            strmid(temp,tempString,15,a);
						            Forecast[dayNum][wind][0] = strval(temp);
						            a = strfind(tempString, "\"", false, a+1)+1;
						            strmid(temp,tempString,a,strfind(tempString, "\"", false, a+1));
						            Forecast[dayNum][wind][1] = strval(temp);
						            a = strfind(tempString, "\"", false, a+5)+1;
						            strmid(temp,tempString,a,strfind(tempString, "\"", false, a+1));
						            Forecast[dayNum][wind][2] = strval(temp);
								}
						        case 5:
						        {
						            strmid(temp,tempString,17,19);
						            Forecast[dayNum][relwet][0] = strval(temp);
						            strmid(temp,tempString,26,28);
						            Forecast[dayNum][relwet][1] = strval(temp);
								}
						        case 6:
						        {
						            new a = strfind(tempString, "\"", false, 15);
						            strmid(temp,tempString,15,a);
						            Forecast[dayNum][heat][0] = strval(temp);
						            a = strfind(tempString, "\"", false, a+1)+1;
						            strmid(temp,tempString,a,strfind(tempString, "\"", false, a+1));
						            Forecast[dayNum][heat][1] = strval(temp);
								}
						    }
							subStringNum++;
			            }
					}
				}
			}
		    startPosition = endPosition+1;
	    }
	    while(-1 < endPosition < strlen(data));
	    errorCode[3] = false;
	}
	else errorCode[2] = true;
	return;
}

/*
*   �������� ���� ������������
*/
LoadConfig()
{
	if(!fexist(FileName".cfg"))return 1;
	new File: file = fopen(FileName".cfg", io_read);
	if(!file)return 1;
	for(new string[128], key, start = 0, end = 0; fread(file, string);)
	{
		key = -1;
	    for(new i; i < sizeof(Config);i++)
		{
		    if(-1 < strfind(string, Config[i][nameParameter], false))
			{
			    key = i;
				break;
			}
		}
		if(key == -1)continue;
		start = strfind(string, "\"", false);
		end = strfind(string, "\"", false, start+1);
		if(end == start)
		{
			Config[key][readParameter] = false;
			continue;
		}
		strmid(Config[key][valueParameter], string, ++start, end, 32);
		Config[key][readParameter] = true;
	}
	fclose(file);
	return 0;
}

LoadWeather()
{
	new tmp[40];
	format(tmp, sizeof(tmp), "informer.gismeteo.ru/xml/%s_1.xml", Config[0][valueParameter]);
	return !HTTP(-1, HTTP_GET, tmp, "", "WeatherParser");
}
