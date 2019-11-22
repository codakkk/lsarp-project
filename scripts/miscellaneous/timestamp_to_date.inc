/*
-               Timestamp To Date converter                 -
-                    Made by Jochemd                    -
-          http://forum.sa-mp.com/member.php?u=580      -
 
native TimestampToDate(Timestamp, &year, &month, &day, &hour, &minute, &second, HourGMT, MinuteGMT = 0);
native DateToTimestamp(str[11]);
*/
 
 
#include <a_samp>
#include <sscanf2>
 
#define SPLITTER .
 
static MonthTimes[12][4] =
{
        { 31, 31, 2678400, 2678400 },
        { 28, 29, 2419200, 2505600 },
        { 31, 31, 2678400, 2678400 },
        { 30, 30, 2592000, 2592000 },
        { 31, 31, 2678400, 2678400 },
        { 30, 30, 2592000, 2592000 },
        { 31, 31, 2678400, 2678400 },
        { 31, 31, 2678400, 2678400 },
        { 30, 30, 2592000, 2592000 },
        { 31, 31, 2678400, 2678400 },
        { 30, 30, 2592000, 2592000 },
        { 31, 31, 2678400, 2678400 }
};
 
static IsLeapYear(year)
{
        if(year % 4 == 0) return 1;
        else return 0;
}
 
//AbyssMorgan
//%d:%02d:%02d:%02d (day hour minute second) 
#define SecToTimeDay(%0)            ((%0) / 86400),(((%0) % 86400) / 3600),((((%0) % 86400) % 3600) / 60),((((%0) % 86400) % 3600) % 60) 
#define MSToTimeDay(%0)                SecToTimeDay((%0)/1000) 

//%02d:%02d:%02d (hour minute second) 
#define SecToTime(%0)                ((%0) / 3600),(((%0) % 3600) / 60),(((%0) % 3600) % 60) 
#define MSToTime(%0)                SecToTime((%0)/1000) 

//%02d:%02d (minute second) 
#define SecToTimeMini(%0)            ((%0) / 60),((%0) % 60) 
#define MSToTimeMini(%0)            SecToTimeMini((%0)/1000)  



static stock const
	SECONDS_PER_MINUTE		= 60,
	SECONDS_PER_HOUR		= 3600,
	SECONDS_PER_DAY			= 86400,
	SECONDS_PER_YEAR		= 31556952,	// based on 365.2425 days per year
	monthdays[12]			= {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	//monthnames[12]			= {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"},
	weekdays[7][10] = {"Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};

stock TimestampToDate(sec1970, &year, &month, &day, &hour, &minute, &second, gmh = 1, gmm = 0) {
	new days, seconds;

	for(year = 1970; ; year += 1) {
		days = (365 + (!(year & 0x03) ? 1 : 0)); // Will fail for 2100
		seconds = days * SECONDS_PER_DAY;
		if(seconds > sec1970)
			break;
		sec1970 -= seconds;
	}

	for(month = 1; ; month += 1) {
		days = monthdays[month - 1];
		seconds = days * SECONDS_PER_DAY;
		if(seconds > sec1970)
			break;
		sec1970 -= seconds;
	}

	for(day = 1; sec1970 >= SECONDS_PER_DAY; day += 1)
		sec1970 -= SECONDS_PER_DAY;

	for(hour = gmh; sec1970 >= SECONDS_PER_HOUR; hour += 1)
		sec1970 -= SECONDS_PER_HOUR;

	for(minute = gmm; sec1970 >= SECONDS_PER_MINUTE; minute += 1)
		sec1970 -= SECONDS_PER_MINUTE;

	second = sec1970;
}

stock datetime2stamp(&sec1970, year, month, day, hour, minute, second, gmh = 0, gmm = 0) {
	for(new y = 1970; y < year; y++)
        day += (365 + (!(y & 0x03) ? 1 : 0));

	for(new m = 1; m < month; m++)
		day += monthdays[m - 1];

    if(!(year & 0x03) && month > 2)
        day += 1;
    day -= 1;
	
    sec1970 += (day * SECONDS_PER_DAY);
    sec1970 += ((hour + gmh) * SECONDS_PER_HOUR);
    sec1970 += ((minute + gmm) * SECONDS_PER_MINUTE);
    sec1970 += second;

    return sec1970;
}

stock weekday(day, month, year) {
    if (month <= 2) {
        month += 12;
		--year;
	}
    new j = year % 100;
    new e = year / 100;
    return ((day + (month+1)*26/10 + j + j/4 + e/4 - 2*e) % 7);
}

stock SecondsInTime(&sec1970, year, month, day, hour, minute, second) {
    sec1970 += (year * SECONDS_PER_YEAR);
    sec1970 += (month * SECONDS_PER_DAY * 30);
    sec1970 += (day * SECONDS_PER_DAY);
    sec1970 += (hour * SECONDS_PER_HOUR);
    sec1970 += (minute * SECONDS_PER_MINUTE);
    sec1970 += second;
	
    //sec1970 += (y * 31556952) + (m * 2592000) + (d * 86400) + (h * 3600) + (i * 60) + s;
}

stock TimeInSeconds(sec1970, &year, &month, &day, &hour, &minute, &second) {
	#define MINUTES_IN_HOUR 60
	#define HOURS_IN_DAY 24
	#define DAYS_IN_WEEK 7
	#define DAYS_IN_MONTH 30
	#define DAYS_IN_YEAR 365.2425

	second = sec1970 % SECONDS_PER_MINUTE;
	sec1970 /= SECONDS_PER_MINUTE;
	minute = sec1970 % MINUTES_IN_HOUR;
	sec1970 /= MINUTES_IN_HOUR;
	hour = sec1970 % HOURS_IN_DAY;
	sec1970 /= HOURS_IN_DAY;
	day = sec1970 % DAYS_IN_WEEK;
	sec1970 /= DAYS_IN_WEEK;
	//week = sec1970 & WEEKS_IN_MONTH;
	month = sec1970 / WEEKS_IN_MONTH;
	year = floatround(sec1970 / DAYS_IN_YEAR, floatround_floor);
	
	/*seconds = sec1970 % SECONDS_PER_MINUTE;
	minutes = sec1970 / SECONDS_PER_MINUTE % MINUTES_IN_HOUR;
	hours = sec1970 / SECONDS_PER_MINUTE / MINUTES_IN_HOUR % HOURS_IN_DAY;
	days = sec1970 / SECONDS_PER_MINUTE / MINUTES_IN_HOUR / HOURS_IN_DAY;*/
}


/*stock TimestampToDate(Timestamp, &year, &month, &day, &hour, &minute, &second, HourGMT, MinuteGMT = 0)
{
        new tmp = 2;
        year = 1970;
        month = 1;
        Timestamp -= 172800; // Delete two days from the current timestamp. This is necessary, because the timestamp retrieved using gettime() includes two too many days.
        for(;;)
        {
                if(Timestamp >= 31536000)
                {
                        year ++;
                        Timestamp -= 31536000;
                        tmp ++;
                        if(tmp == 4)
                        {
                                if(Timestamp >= 31622400)
                                {
                                        tmp = 0;
                                        year ++;
                                        Timestamp -= 31622400;
                                }
                                else break;
                        }
                }
                else break;
        }              
        for(new i = 0; i < 12; i ++)
        {
                if(Timestamp >= MonthTimes[i][2 + IsLeapYear(year)])
                {
                        month ++;
                        Timestamp -= MonthTimes[i][2 + IsLeapYear(year)];
                }
                else break;
        }
        day = 1 + (Timestamp / 86400);
        Timestamp %= 86400;
        hour = HourGMT + (Timestamp / 3600);
        Timestamp %= 3600;
        minute = MinuteGMT + (Timestamp / 60);
        second = (Timestamp % 60);
        if(minute > 59)
        {
                minute = 0;
                hour ++;
        }
        if(hour > 23)
        {
                hour -= 24;
                day ++;
        }      
        if(day > MonthTimes[month][IsLeapYear(year)])
        {
                day = 1;
                month ++;
        }
        if(month > 12)
        {
                month = 1;
                year ++;
        }
        return 1;
}
 
stock DateToTimestamp(str[11])
{
        new date[3]; // date[0] = day           date[1] = month                 date[2] = year
        if(!sscanf(str,"p<"#SPLITTER">ddd",date[0],date[1],date[2]))
        {
                new total = 0, tmp = 0;
                total += date[0] * 86400;
                if(date[1] == 2 && date[0] < 29) tmp = ((date[2] - 1968) / 4 - 2);
                else tmp = ((date[2] - 1968) / 4 - 1);
                total += tmp * 31622400;
                total += (date[2] - 1970 - tmp) * 31536000;
                for(new i = 1; i < date[1]; i ++) total += MonthTimes[i][0 + IsLeapYear(date[2])] * 86400;
                return total;
        }
        else return -1;
}*/