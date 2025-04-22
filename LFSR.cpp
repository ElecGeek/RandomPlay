#include "LFSR.hxx"



LFSR_indexes::LFSR_indexes():
  indexes_list( {
	  { 3, 0x000003 },
	  { 4, 0x000003 },
	  { 5, 0x00001b },
	  { 6, 0x00002d },
	  { 7, 0x000053 },
	  { 8, 0x00004d },
	  { 9, 0x0000f9 },
	  { 10, 0x00025b },
	  { 11, 0x0001e5 },
	  { 12, 0x000a69 },
	  { 13, 0x000693 },
	  { 14, 0x000a65 },
	  { 15, 0x004d33 },
	  { 16, 0x00a52b },
	  { 17, 0x005267 },
	  { 18, 0x01927d },
	  { 19, 0x052795 },
	  { 20, 0x05ff2b },
	  { 21, 0x056d79 },
	  { 22, 0x1cca5d },
	  { 23, 0x26e7ad },
	  { 24, 0x399b5d }
    })
{}	  

optional< const unsigned long >LFSR_indexes::operator[]( const unsigned short&bits )
{
  auto it = indexes_list.find( bits );
  // perhaps, in a far future, we are going to have a find that do the job directly
  if ( it != indexes_list.end() )
	return it->second;
  else
	return nullopt;
}

LFSR_parameters::LFSR_parameters( const unsigned short&number_bits, const unsigned long &indexes ):
  number_bits( number_bits ), indexes( indexes )
{
  // Temporary
  initial_state[0]=true;

}

LFSR_extract::LFSR_extract():
  extract_data({
	  { "destination 1" , { 0, 1 } },
	  { "destination 2" , { 2 } }
	})
{}
optional<pair<const unsigned short,const unsigned short> > LFSR_extract::test_and_required_bits()const
{
  set<unsigned short> bits_involved;
  bool isOk = true;
  unsigned short nbre_bits = 0;
  unsigned short the_max = 0;

  for_each( extract_data.begin(), extract_data.end(),[&](const pair<string, deque<unsigned short> >&a){
	  for_each( a.second.begin(), a.second.end(), [&](const unsigned short&b){
		  if ( bits_involved.find( b ) != bits_involved.end() )
			{
			  bits_involved.insert(b);
			  nbre_bits += 1;
			  if ( b > the_max )
				the_max = b;
			}
		  else
			{
			  isOk = false;
			}
		});
	});

  if ( isOk )
	return make_pair(nbre_bits,the_max);
  else
	return nullopt;
}


void LFSR_histo::operator()(const unsigned long&a)
{
  auto histo_iter = histo.find( a );
  if ( histo_iter != histo.end() )
	histo_iter->second += 1;
  else
	histo.insert(make_pair( a , 1 ));
}

deque<pair<string, unsigned long> >LFSR_histo::GetStatistics()const
{
  deque<pair<string, unsigned long> >the_return;

  unsigned long the_sum = 0;
  unsigned long the_N = 0;

  for_each( histo.begin(), histo.end(), [&](const pair<unsigned long,unsigned long>&a){
	  the_sum+=a.second;
	  the_N += 1;
	});

  if ( the_N == 0 )
	{
	  the_return.push_back( make_pair( "Bad", 2 ));
	}
  else if ( the_sum % the_N == 0 )
	{
	  the_return.push_back( make_pair( "Good", 0 ));
	  the_return.push_back( make_pair( "Number of occurrences", the_sum / the_N ));
	}else
	// Until now this didn't happened.
	// The code is going to be written on the first problem
	  the_return.push_back( make_pair( "Bad", 1 ));
  the_return.push_back( make_pair( "Number of elements", the_N ));

  return the_return;
}


ostream&operator<<(ostream&ostr,const LFSR_histo&a)
{
  for( auto the_data: a.histo )
	  ostr<<the_data.first<<" "<<the_data.second<<"\t";
  return ostr;
}

LFSR_iterator::LFSR_iterator( const LFSR_parameters&LFSR_init, const LFSR_extract&extract ):
  LFSR_init( LFSR_init ),
  extract( extract ),
  nbreRuns( 0 ),
  current_state( LFSR_init.initial_state ),
  maskXor( LFSR_init.indexes )
{}


const unsigned long LFSR_iterator::operator()()
{
  // Select the bits that should be involved in the XOR
  bitset<24>maskedState = current_state & maskXor;
  bool xor_result = false;
  if ( maskedState.count() % 2 == 1 )
	xor_result = true;

  // Got problems with g++13 then do it by hands
  //current-State = rotr( currentState, 1 );
  for ( unsigned long ind = 1; ind < LFSR_init.number_bits ; ind++ )
	current_state[ ind - 1 ] = current_state[ ind ];
  current_state[ LFSR_init.number_bits - 1 ] = xor_result;

  nbreRuns += 1;
  return nbreRuns;
}
const bitset<24>LFSR_iterator::GetState()const
{
  return current_state;
}


int main()
{
  unsigned short nbreBits=24;
  LFSR_histo theHisto;

  optional< const unsigned long >indexes=LFSR_indexes()[ nbreBits ];

  if ( indexes.has_value() )
	{
	  LFSR_parameters theData( nbreBits , *indexes );

	  LFSR_iterator LFSR_parameters_iter(theData, LFSR_extract() );
	  unsigned long iter_count;

	  cout << LFSR_parameters_iter.GetState() << ", now run"<<endl;
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
	  cout << endl;

	  

	}else
	cerr << "Sorry, we don't have the data for this number of bits" << endl;

  return 0;
}
