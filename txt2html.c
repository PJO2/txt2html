// ------------------------
// - Translate non Ascii characters to Html
// ------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

// some usefull shortcuts
#define SizeOfTab(x) ( sizeof(x) / sizeof(x[0]) )
#define bool int
enum { FALSE=0, TRUE };
enum { EMERGENCY, ALERT, CRITICAL, ERROR, WARNING, NOTICE, INFORMATIONAL, DEBUG};

int verbose=WARNING;


// ---------------------
// Data Structure and Management
// ---------------------

// utf8 character table with their html equivalent
// utf8 type is wchar_t (defined in stddef)
struct S_map {
   wchar_t   utf8;    // the utf8 text character
   char     *html;    // the html plain ascii string to substitute by
   int       len;     // its length (in bytes)
};

// the data table, will be sorted in the main function
struct S_map tmap[] = {
   { L'"', "&quot;", 6 },
  // { L'<', "&lt;",   4 },
  // { L'>', "&gt;",   4 },
   { L'’', "'",        1 },
   { L'à', "&agrave;", 8 },
   { L'á', "&aacute;", 8 },
   { L'â', "&acirc;",  7 },
   { L'ã', "&atilde;", 8 },
   { L'ä', "&auml;",   6 },
   { L'å', "&aring;",  7 },
   { L'æ', "&aelig;",  7 },
   { L'ç', "&ccedil;", 8 },
   { L'è', "&egrave;", 8 },
   { L'é', "&eacute;", 8 },
   { L'ê', "&ecirc;",  7 },
   { L'ë', "&euml;",   6 },
   { L'ì', "&igrave;", 8 },
   { L'í', "&iacute;", 8 },
   { L'î', "&icirc;",  7 },
   { L'ï', "&iuml;",   6 },
   { L'ð', "&eth;",    5 },
   { L'ñ', "&ntilde;", 8 },
   { L'ò', "&ograve;", 8 },
   { L'ó', "&oacute;", 8 },
   { L'ô', "&ocirc;",  7 },
   { L'õ', "&otilde;", 8 },
   { L'ö', "&ouml;",   6 },
   { L'ø', "&oslash;", 8 },
   { L'œ', "&oelig;",  7 },
   { L'€', "&euro;",   6 },
   { L'ù', "&ugrave;", 8 },
   { L'ú', "&uacute;", 8 },
   { L'û', "&ucirc;",  7 },
   { L'ü', "&uuml;",   6 },
   { L'ý', "&yacute;", 8 },
   { L'þ', "&thorn;",  7 },
   { L'ÿ', "&yuml;",   6 },
   { L'À', "&Agrave;", 8 },
   { L'Á', "&Aacute;", 8 },
   { L'Â', "&Acirc;",  7 },
   { L'Ã', "&Atilde;", 8 },
   { L'Ä', "&Auml;",   6 },
   { L'Å', "&Aring;",  7 },
   { L'Æ', "&AElig;",  7 },
   { L'Ç', "&Ccedil;", 8 },
   { L'È', "&Egrave;", 8 },
   { L'É', "&Eacute;", 8 },
   { L'Ê', "&Ecirc;",  7 },
   { L'Ë', "&Euml;",   6 },
   { L'Ì', "&Igrave;", 8 },
   { L'Í', "&Iacute;", 8 },
   { L'Î', "&Icirc;",  7 },
   { L'Ï', "&Iuml;",   6 },
   { L'Ð', "&ETH;",    5 },
   { L'Ñ', "&Ntilde;", 8 },
   { L'Ò', "&Ograve;", 8 },
   { L'Ó', "&Oacute;", 8 },
   { L'Ô', "&Ocirc;",  7 },
   { L'Õ', "&Otilde;", 8 },
   { L'Ö', "&Ouml;",   6 },
   { L'Ø', "&Oslash;", 8 },
   { L'Ù', "&Ugrave;", 8 },
   { L'Ú', "&Uacute;", 8 },
   { L'Û', "&Ucirc;",  7 },
   { L'Ü', "&Uuml;",   6 },
   { L'Ý', "&Yacute;", 8 },
   { L'Þ', "&THORN;",  7 },
   { L'ß', "&szlig;",  7 }
};

// compare two S_map entries based on the utf8 value
static int sort_by_utf8 (const void *a, const void *b)
{
wchar_t a8 = ((struct S_map *) a)->utf8,
        b8 = ((struct S_map *) b)->utf8;
   return (a8>b8) - (a8<b8); 
} // sort_by_utf8


static int DisplayTMap (void)
{
int ark;
   printf ("----------\n");
   for (ark=0 ;  ark<SizeOfTab(tmap) ; ark++)
      printf ("utf8 char %04X (%c) will be replaced by %s\n", 
            tmap[ark].utf8, tmap[ark].utf8, tmap[ark].html);
   printf ("----------\n");
   return ark;
} // DisplayTMap


// ---------------------
// Process a single file
// ---------------------

// input file should be a .txt file
// output file is .html
const char TEXT_EXT[] = ".txt";
const char HTML_EXT[] = ".html";

// Error checking macro
#define FAIL(x)  { fprintf (stderr, "error processing file %s: %s\n", infile, x); goto free; }

// Process a single file
int Process (const char *infile)
{
char *locale = setlocale(LC_ALL, "");  // accepts all utf8 char
const char *ext;                       // points on extension in infile string
char *outfile=NULL;                    // outfile name (ends with .html)
FILE *in=NULL, *out=NULL;
int   utf8char;  // the utf8 character we have read
bool Ok=FALSE;   // set to TRUE if current file correctly processed
int linenb;      // a line counter for reporting

   // check that file terminates with ".txt"
   // search for extension
   ext = strrchr (infile, '.');
   if ( ext==NULL 
     || strcmp (ext, TEXT_EXT) != 0 )
            FAIL("filename does not terminate with .txt: skipping");

   // outfile is infile with the ext .txt changed to .html
   if ( (outfile = malloc ( ext - infile + strlen(HTML_EXT) + 1)) == NULL ) 
            FAIL("can not allocate memory");
   memcpy (outfile, infile, ext - infile);
   strcpy (&outfile[ext - infile], HTML_EXT);

   // open both files
   if ( (in = fopen (infile, "rt")) == NULL )
            FAIL ("can not open for reading");
   if ( (out = fopen (outfile, "wt")) == NULL )
            FAIL ("can not open for writing");

   linenb=1; // start at line #1 !
   // read a single utf8 character (either 8 or 16 bits) 
   while ( (utf8char = fgetwc(in)) != WEOF )
   {
   int ark;     
      if (utf8char=='\n') linenb++;   // just for display
      // scan the table tmap using a binary search (not as fast as a hash but simpler)
      const struct S_map pattern = { utf8char, "", 0 }; // the structure to be found;
      const struct S_map *pmap = bsearch (&pattern, tmap, SizeOfTab(tmap), sizeof pattern, sort_by_utf8);
      if (pmap==NULL) // not found =# simply write the utf8 char back
      {
         if ((int) utf8char > 255) 
            if (verbose>=NOTICE) 
               printf ("warning: file %s, special char %c(%04X) not translated\n", 
                        infile, utf8char, utf8char);
         Ok = ( fputc (utf8char, out) != EOF );
      }
      else   // found, write the html equivalent
      {
         if (verbose==DEBUG)
             printf ("file %s, line #%3d: replacing utf8 char %04X (%c) by %s)\n", 
                      infile, linenb, utf8char, utf8char, pmap->html); 
         Ok = ( fwrite (pmap->html, pmap->len, 1, out) == 1 );
      }
      if (! Ok) 
                  FAIL("write error");
   } // read next character
   
   // free resources
   free:
   if (out!=NULL)  fclose (out);
   if (in!=NULL)   fclose (in);
   if (outfile!=NULL)  free (outfile);
   if (Ok && verbose>=WARNING)
        printf ("%s: %d line%c\n", 
                 infile, linenb, linenb>1 ? 's' : ' ');
      
   return Ok;
} // Process


// main : loop on files given as arguments
int main (int argc, char *argv[])
{
int firstarg=1;

   // parse some arguments (verbose or quiet mode)
   for (firstarg=1 ;
        firstarg<argc && argv[firstarg][0]=='-' ;
        firstarg++)
   {
      switch ( argv[firstarg][1] )    // can be \0 but exists anyway
      {
         case 'd' :  verbose=DEBUG;  break;
         case 'v' :  verbose=NOTICE; break;
         case 'q' :  verbose=ERROR;  break;
      }
   }
   if (argc-firstarg<1) { printf ("Usage: %s [-dqv] file1.txt [file2.txt ...]\n", argv[0]);  exit(1); }

   // ensure tmap is sorted
   qsort (tmap, SizeOfTab(tmap), sizeof(struct S_map), sort_by_utf8);
   if (verbose==DEBUG)
      DisplayTMap ();

   for (int ark=firstarg; ark<argc ; ark++)
      Process (argv[ark]);
return 0;
} // main