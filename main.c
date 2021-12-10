#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <qrencode.h>
#include <gd.h>

gdImage *QrEncode ( char *str )
{
 if( !str || !strlen ( str ) )
  return NULL;

 QRcode *code;
 gdImage *Image;
 unsigned char *row = 0, *p = 0;

 code = QRcode_encodeString8bit ( str, 3, QR_ECLEVEL_H );
 if( !code )
  return NULL;

 p = code->data;
 int rell = ( code->width * 10 + 100 ), last = 0, ty = 0;

 Image = gdImageCreateTrueColor ( rell, rell );
 if( !Image )
  return NULL;

 for( int i = 0; i < ( rell ); i++ ) {
  for( int j = 0; j < ( rell ); j++ ) {
   gdImageSetPixel ( Image, i, j, 0xFFFFFF );
  }
 }

 for( int i = 50; i < ( code->width * 10 ) + 100; i++ ) {
  if( i < ( ( code->width * 10 ) + 50 ) ) {
   row = ( p + ( ( ( i / 10 ) - 5 ) * code->width ) );
   last = 0;
   for( int j = 50; j < ( code->width * 10 ) + 50; j++ ) {
	int ty = 0;
	if( row[( j / 10 ) - 5] & 1 ) {
	 ty = 1;
	 last = 1;
	} else if( last ) {
	 last = 0;
	 ty = 0;
	}
	if( ty ) {
	 gdImageSetPixel ( Image, i, j, 0x000000 );
	}
   }
  }
 }
 return Image;
}

#include <time.h>
#include <sys/stat.h>

char gchar ( a, b )
char a, b;
{
 return a + ( rand (  ) % ( b - a + 1 ) );
}

char *gen ( int len )
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

#include <archive.h>
#include <archive_entry.h>

int main ( signed Argsc, char *( Args[] ) )
{
 char *base = "https://vaccinations.covid19sadc.com/general/qr";

 int j = 0;
 int limit = 100;

 srand ( time ( NULL ) );
 struct archive *ar = archive_write_new (  );
 struct archive_entry *ent = archive_entry_new (  );

 int ret = 0;
 ret = archive_write_set_format ( ar, ARCHIVE_FORMAT_ISO9660 );
 if( ARCHIVE_OK != ret )
  return 1;
 ret = archive_write_open_filename ( ar, "/sdcard/SADCvac.iso" );
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
