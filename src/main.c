/*XXX This Document was modified on 1635489520 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dnqr.h>
#include <time.h>
#include <sys/stat.h>
#include <archive.h>
#include <archive_entry.h>

char gchar ( a, b )
char a, b;
{
 return a + ( rand (  ) % ( b - a + 1 ) );  // A Random Char generator
}

char *gen ( int len )           // A string length(len) of random chars
{
 char ans[len];
 ans[len] = 0;

 while( --len > -1 ) {
  if( gchar ( 'a', 'b' ) == 'a' )
   ans[len] = gchar ( 'a', 'z' );
  else
   ans[len] = gchar ( '0', '9' );
 }
 return strdup ( ans );
}

int main ( signed Argsc, char *( Args[] ) )
{
 char *base = "https://vaccinations.covid19sadc.com/general/qr";    // The Base of the Site To Create QR codes upon

 int j = 0;
 int limit = 100;               // The Limit of OutPut QrCodes To Be Generated

 srand ( time ( NULL ) );       // Seeding current time for rand ()

 struct archive *ar = archive_write_new (  );
 struct archive_entry *ent = archive_entry_new (  );

 int ret = 0;
 ret = archive_write_set_format ( ar, ARCHIVE_FORMAT_ISO9660 ); //The Desired output format

 if( ARCHIVE_OK != ret )
  return 1;

 ret = archive_write_open_filename ( ar, "./SADCvac.iso" ); // The file to insert the archived Images
 if( ARCHIVE_OK != ret )
  return 1;

 while( 1 ) {
  char *( xBases[6] ) = {[5] = 0,[0] = gen ( 8 ),[1] = gen ( 4 ),[2] =
       gen ( 4 ),[3] = gen ( 4 ),[4] = gen ( 12 )
  };
  char *nn = NULL;
  char *path = NULL;

  asprintf ( &nn, "%s/%s-%s-%s-%s-%s", base, xBases[0], xBases[1], xBases[2],
             xBases[3], xBases[4] );
  asprintf ( &path, "./SADCbruteQRs/%s-%s.png", xBases[4], xBases[2] );

  {
   archive_entry_set_gname ( ent, nn );
   archive_entry_set_uname ( ent, "dittonedo" );
   archive_entry_set_perm ( ent, 0700 );
   archive_entry_set_ctime ( ent, time ( NULL ), 0 );
   archive_entry_set_filetype ( ent, S_IFREG );
   archive_entry_set_pathname ( ent, path );
  }
  {
   gdImagePtr tmp = QrEncode ( nn );
   int siz = 0;
   void *dp = gdImagePngPtr ( tmp, &siz );
   archive_entry_set_size ( ent, siz );

   ret = archive_write_header ( ar, ent );
   if( ARCHIVE_OK != ret ) {
	//fprintf ( stderr, "%s\n", archive_error_string ( ar ) );
	break;
   }

   archive_entry_clear ( ent );
   if( siz ) {
	ret = archive_write_data ( ar, dp, siz );
	if( ret == ARCHIVE_OK )
	 break;
   }
   printf ( "[\033[33m%5d|\033[34m%5d\033[0m] %s\r", j, limit, nn );
   gdFree ( dp );
   gdImageDestroy ( tmp );
  }

  char **p = xBases;

  while( p ) {
   if( !*p ) {
	break;
   }
   if( ( p - xBases ) < 4 )
	free ( *p );
   p++;
  }
  free ( path );
  free ( nn );
  if( j++ >= limit )
   break;

 }
 archive_write_close ( ar );
 archive_entry_free ( ent );
 return 0;
}
