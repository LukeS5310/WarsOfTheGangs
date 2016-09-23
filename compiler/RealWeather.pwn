#include <a_samp>
#include <a_http>

//http://articles.org.ru/cn/showdetail.php?cid=7065 <- коды символов для парсинга названия города!

#define FileName "Config"   //имя файла конфигурации (ковычки обязательны!)
#define TimeUpdate 20       //время проверки обновления погоды (в минутах)
#define TimePause 10        //пауза для апдейта погоды
#define DIALOGID 899
#define COLOR_WHITE 0xFFFFFFAA 		// white

forward WeatherParser(index, response_code, data[]);
forward UpdateWeather();

enum infoWeather
{
	date[3],            //дата, на которую составлен прогноз в данном блоке ([day], [monthe], [year])
	hour,               //местное время, на которое составлен прогноз
	tod,                //время суток, для которого составлен прогноз: 0 - ночь 1 - утро, 2 - день, 3 - вечер
	weekday,            //день недели, 1 - воскресенье, 2 - понедельник, и т.д.
	cloudiness,         //облачность по градациям:  0 - ясно, 1- малооблачно, 2 - облачно, 3 - пасмурно
	precipitation,      //тип осадков: 4 - дождь, 5 - ливень, 6,7 – снег, 8 - гроза, 9 - нет данных, 10 - без осадков
	rpower,             //интенсивность осадков, если они есть. 0 - возможен дождь/снег, 1 - дождь/снег
	spower,             //вероятность грозы, если прогнозируется: 0 - возможна гроза, 1 - гроза
	pressure[2],        //атмосферное давление, в мм.рт.ст. ([max], [min])
	temperature[2],     //температура воздуха, в градусах Цельсия ([max], [min])
	wind[4],            //приземный ветер ([min], [max], [direction])
	relwet[3],          //относительная влажность воздуха, в % ([max], [min])
	heat[2],            //комфорт - температура воздуха по ощущению одетого по сезону человека, выходящего на улицу ([max], [min])
};

enum infoConfig
{
	nameParameter[64],      //имя параметра
	valueParameter[32],    //значение параметра
	bool:readParameter,     //прочтен ли параметр из файла конфигурации
};

/**
*   стандартные параметры конфигурации
*   подгружаются в том случае если файл
*	конфигурации не был загружен или создан
*/
new Config[][infoConfig] = {
	{"CityCode", "28440", false},       	//код вашего города
	{"CityName", "Екатеринбург", false},	//разрешить хранение и показ прогноза погоды
	{"RealWeather", "1", false},        	//включить режим реальной погоды на сервере
	{"ShowForecast", "1", false}			//разрешить хранение и показ прогноза погоды
//	{"AccessForecast", "0", false},     	//разрешить делать прогноз погоды по своему городу
};

/*
*   массив где сохраняется прогноз на 4 дня
*   5 дня - ограничение Gismeteo.ru
*	данное значение менять запрещено!
*/
new Forecast[5][infoWeather];

/*
*   внутреннее хранилище ошибок
*	[0] - фаил конфигурации не загружен
*	[1] - запрос не осуществлен
*   [2] - запрос вернул неверные данные
*   [3] - во время парсинга данных произошла ошибка
*/
new errorCode[4];
new runUpdate = 0 char, Timer = -1;	//ключи

public OnFilterScriptInit()
{
	errorCode[0] = LoadConfig();    //загружаем конфиг
	errorCode[1] = LoadWeather();   //загружаем первый прогноз
	if(strval(Config[2][valueParameter]))UpdateWeather();	//если включен режим реальной погоды меняем погоду
	return 1;
}

public OnFilterScriptExit()
{
	if(Timer != -1)KillTimer(Timer);	//убиваем таймер при выходе если он создан
	return 1;
}

public OnPlayerStateChange(playerid, newstate, oldstate)
{
	/*
	*   Отслеживаем изменение время для изменения погоды
	*/
	new time[3];
	gettime(time[0],time[1],time[0]);
	if(runUpdate - time[1] <= 0)
	{
	    runUpdate = (time[1] + TimeUpdate) > 59 ? (time[1] + TimeUpdate) - 60 : (time[1] + TimeUpdate) ;
	    errorCode[1] = LoadWeather();   //загружаем прогноз
	    if(strval(Config[2][valueParameter]))	//если включен режим реальной погоды меняем погоду
		{
		    if(Timer != -1)KillTimer(Timer);
		    /*
		    *   запрос прогноза с сайта Gismeteo.ru занимает время,
		    *   поэтому для смены на актуальную погоду, выдерживаем 10 секундную паузу
		    */
			Timer = SetTimer("UpdateWeather",TimePause*1000,false);
		}
	}
	return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	if(!strcmp(cmdtext, "/weather", true,8))	//просмотр прогноза
	{
	    if(!strval(Config[3][valueParameter]))return SendClientMessage(playerid,COLOR_WHITE,"DEBUG:Валится на той самой опции");
		new timeDay[][6] = {"ночь","утро","день","вечер"};
		new Cloudiness[][12] = {"ясно", "малооблачно", "облачно", "пасмурно"};
		new Precipitation[][12] = {"дождь", "ливень", "снег", "гроза", "нет данных", "без осадков"};
		new Wind[][17] = {"северный", "северо-восточный", "восточный", "юго-восточный", "южный", "юго-западный", "западный", "северо-западный"};
		new Weekday[][12] = { "Воскресенье", "Понедельник", "Вторник", "Среду", "Четверг", "Пятницу", "Субботу"};
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
	        format(string, sizeof(string), "%sНа %s (%s) %d.%d.%d ожидается:\n\tОблачность: %s\n\tОсадки: %s\n\tАтмосферное давление: %d — %d мм.рт.ст.\n", string,
				Weekday[u], timeDay[Forecast[i][tod]], Forecast[i][date][0], Forecast[i][date][1], Forecast[i][date][2],
				Cloudiness[Forecast[i][cloudiness]],Precipitation[y],
				Forecast[i][pressure][0], Forecast[i][pressure][1]);
			format(string, sizeof(string), "%s\tТемпература воздуха: %d — %d °C\n\tВетер %s: %d — %d м/с\n\tВлажность воздуха: %d — %d %%\n\tКомфорт: %d — %d °C\n\n", string,
				Forecast[i][temperature][0], Forecast[i][temperature][1],
				Wind[Forecast[i][wind][2]], Forecast[i][wind][0], Forecast[i][wind][1],
				Forecast[i][relwet][0], Forecast[i][relwet][1],
				Forecast[i][heat][0], Forecast[i][heat][1]);
	    }
	    strcat(string,"\n\t\t\tПрогноз предоставлен сайтом Gismeteo.ru и реализован $tepashka");
	    new tmp[128];
	    format(tmp, sizeof(tmp), "г. %s. Прогноз погоды на ближайшие дни", Config[1][valueParameter]);
	    ShowPlayerDialog(playerid, DIALOGID, DIALOG_STYLE_MSGBOX, tmp, string, "Ok", "Close");
		return 1;
	}
	if(!strcmp(cmdtext, "/wdebug", true,7)) //системная информация
	{
		if(!IsPlayerAdmin(playerid))return 1;
		new string[256],errorKey = 1;
		format(string, sizeof(string), "Параметры:\n");
		new Load[][3] = {" *", " "};
		for(new i; i < sizeof(Config);i++)
		{
			format(string, sizeof(string), "%s\t%s%s\t\t\"%s\"\n", string, Config[i][nameParameter], Load[Config[i][readParameter]], Config[i][valueParameter]);
		}
		format(string, sizeof(string), "%s\n\nОшибки:\n", string);
		if(errorCode[0])
		{
			format(string, sizeof(string), "%s!\tФайл конфигурации небыл загружен.\n", string);
			errorKey = 0;
		}
		if(errorCode[1])
		{
			format(string, sizeof(string), "%s!\tЗапрос на Gismeteo.ru на был произведен.\n", string);
			errorKey = 0;
		}
		if(errorCode[2])
		{
			format(string, sizeof(string), "%s!\tДанные с Gismeteo.ru не пришли.\n", string);
			errorKey = 0;
		}
		if(errorCode[3])
		{
			format(string, sizeof(string), "%s!\tВо время парсинга данных произошла ошибка.\n", string);
			errorKey = 0;
		}
		if(errorKey)
		{
			format(string, sizeof(string), "%s\tОшибок нет.", string);
		}
		strcat(string,"\n\n* - используется значение по умолчанию");
		ShowPlayerDialog(playerid, DIALOGID, DIALOG_STYLE_MSGBOX, "Данные погодной системы:", string, "Ok", "Close");
		return 1;
	}
	if(!strcmp(cmdtext, "/wupdate", true,8))    //ручной апдейт прогноза (автопогода включиться)
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
	if(!strcmp(cmdtext, "/wset", true, 6))		//установить погоду ручками (автопогода отключиться)
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
*   Установка погоды в соответствии с прогнозом на сейчас
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
*   Парсер прогноза приходящего с Gismeteo.ru
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
*   загрузка фала конфигурации
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
