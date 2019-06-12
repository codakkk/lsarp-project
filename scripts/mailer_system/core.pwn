#define MAILER_URL "185.25.204.170/server_mailer/emailconfig.php" // This has to be defined BEFORE you include mailer.

stock SendMail( const szReceiver[ ], const szSubject[ ], const szMessage[ ] )
{
	new rec[255], subject[255], mess[255];
	set(rec, szReceiver);
	set(subject, szSubject);
	set(mess, szMessage);

	StringURLEncode(rec);
	StringURLEncode(subject);
	StringURLEncode(mess);

	new str[256];
	format(str, sizeof(str), "%s?sendto=%s&subject=%s&body=%s",
								MAILER_URL, rec, subject, mess);
	printf("%s", str);
	HTTP( 0xD00D, HTTP_GET , str, "", "OnMailScriptResponse" );
}

forward OnMailScriptResponse( iIndex, iResponseCode, const szData[ ] );
public  OnMailScriptResponse( iIndex, iResponseCode, const szData[ ] )
{
	if ( szData[ 0 ] )
		printf( "Mailer script says: %s", szData );
}

stock StringURLEncode( szString[ ], iSize = sizeof( szString ) )
{
	for ( new i = 0, l = strlen( szString ); i < l; i++ )
	{
		switch ( szString[ i ] )
		{
			case '!', '(', ')', '\'', '*',
			     '0' .. '9',
			     'A' .. 'Z',
			     'a' .. 'z':
			{
				continue;
			}
			
			case ' ':
			{
				szString[ i ] = '+';
				
				continue;
			}
		}
		
		new
			s_szHex[ 8 ]
		;
		
		if ( i + 3 >= iSize )
		{
			szString[ i ] = EOS;
			
			break;
		}
		
		if ( l + 3 >= iSize )
			szString[ iSize - 3 ] = EOS;
		
		format( s_szHex, sizeof( s_szHex ), "%02h", szString[ i ] );
		
		szString[ i ] = '%';
		
		strins( szString, s_szHex, i + 1, iSize );
		
		l += 2;
		i += 2;
		
		if ( l > iSize - 1 )
			l = iSize - 1;
	}
}