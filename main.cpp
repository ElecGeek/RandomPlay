#include "LFSR.hxx"
#include <getopt.h>
#include <fstream>
#include <sstream>


enum { dry_and_run, run_direct, dry_only } debugLevel = dry_only;


string Help()
{
  ostringstream theHelpText;

  theHelpText << "The help has to be written" << endl;

  return theHelpText.str();
}

/** Run the test for a LFSR of N bits
 *
 * A test is run and an histogram is built,
 *   based on the full LFSR.\n
 * It is intended to run in a loop of all the LFSR sizes.
 * There is another function intended to build an histogram
 *   for each extraction.
 */
void TestHistoSizeN(const unsigned short&nbreBits)
{
  LFSR_histo theHisto;

  optional< const unsigned long >indexes=LFSR_indexes()[ nbreBits ];

  cout<<nbreBits<<" bits:\t";
  if ( indexes.has_value() )
	{
	  LFSR_parameters theData( nbreBits , *indexes );

	  LFSR_iterator LFSR_parameters_iter(theData, LFSR_extract() );
	  unsigned long iter_count;

	  for ( unsigned long ind = 0 ; ind < ( pow( 2, nbreBits ) -1 ); ind++)
		{
		  iter_count = LFSR_parameters_iter();
		  //		  cout << LFSR_parameters_iter.GetState() << "   ";
		  // cout << iter_count << endl;
		  theHisto( ( LFSR_parameters_iter.GetState()).to_ulong());
		}

	  //	  cout<<theHisto<<endl;
	  deque<pair<string, unsigned long> >histo_result;
	  histo_result = theHisto.GetStatistics();
	  for( auto histo_info : histo_result )
		cout << histo_info.first << ": " << histo_info.second << "\t";
	  cout << LFSR_parameters_iter.GetState() << ", after ran";
	}else
	cout << "Sorry, we don't have the data for this number of bits";
  cout<<endl;
}
/** Run the test for a LFSR of N bits
 *
 * A test is run and an histogram is built,
 *   for each extraction.\n
 * It is intended to run in a loop of all the LFSR sizes.
 * There is another function intended to build an histogram
 *   based on the full LFSR..
 */
void TestHistoPerExtract(const unsigned short&ind)
{}

int main(int argc,char *argv[] )
{
  int opt;

  bool testRequired = false;
  bool helpRequired = false;
  unsigned short testBitN;
  while (( opt= getopt( argc, argv, "d:t:h" )) != EOF ) 
	{
	  switch ( opt )
		{
		case 'd':
		  switch( atoi( optarg ))
			{
			case 0:  debugLevel = run_direct;  break;
			case 1:  debugLevel = dry_and_run;  break;
			case 2:  debugLevel = dry_only;  break;
			}
		  break;
		case 'h':
		  helpRequired = true;
		  break;
		case 't':
		  testRequired = true;
		  testBitN = atoi( optarg );
		  break;
		}
	}

  if ( helpRequired )
	{
	  cout << Help();
	  exit( 0 );
	}
  if ( testRequired )
	{
	  if ( testBitN != 0 )
		{
		  TestHistoPerExtract( testBitN );
		}else{
		unsigned short ind;
		for( ind = 2; ind< 27; ind++ )
		  TestHistoSizeN(ind);
	  }
	  exit( 0 );
	}


  return 0;
}
