#include "LFSR.hxx"



LFSR_indexes::LFSR_indexes():
  indexes_list( {
	  { 2, 0x000003 },
	  { 3, 0x000006 },
	  { 4, 0x00000c },
	  { 5, 0x000014 },
	  { 6, 0x000030 },
	  { 7, 0x000060 },
	  { 8, 0x0000b8 },
	  { 9, 0x000110 },
	  { 10, 0x000240 },
	  { 11, 0x000500 },
	  { 12, 0x000e08 },
	  { 13, 0x001c80 },
	  { 14, 0x003802 },
	  { 15, 0x000600 },
	  { 16, 0x00d008 },
	  { 17, 0x012000 },
	  { 18, 0x020400 },
	  { 19, 0x072000 },
	  { 20, 0x090000 },
	  { 21, 0x140000 },
	  { 22, 0x300000 },
	  { 23, 0x420000 },
	  { 24, 0xe10000 } })
{}	  

optional< const unsigned short >LFSR_indexes::operator[]( const unsigned short&bits )
{
  auto it = indexes_list.find( bits );
  // perhaps, in a far future, we are going to have a find that do the job directly
  if ( it != indexes_list.end() )
	return it->second;
  else
	return nullopt;
}

LFSR_parameters::LFSR_parameters( const unsigned short&number_bits, const unsigned short &indexes ):
  number_bits( number_bits ), indexes( indexes )
{
  // Temporary
  initial_state[0]=true;

}

LFSR_extract::LFSR_extract()
{}

void LFSR_histo::operator()(const unsigned long&a)
{
  auto histo_iter = histo.find( a );
  if ( histo_iter != histo.end() )
	histo_iter->second += 1;
  else
	histo.insert(make_pair( a , 1 ));
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


const unsigned short LFSR_iterator::operator()()
{
  // Select the bits that should be involved in the XOR
  bitset<24>maskedState = current_state & maskXor;
  bool xor_result = false;
  if ( maskedState.count() % 2 == 1 )
	xor_result = true;

  // Got problems with g++13 then do it by hands
  //current-State = rotl( currentState, 1 );
  for ( unsigned short ind = LFSR_init.number_bits - 1; ind > 0; --ind )
	current_state[ ind ] = current_state[ ind - 1 ];
  current_state[ 0 ] = xor_result;

  nbreRuns += 1;
  return nbreRuns;
}
const bitset<24>LFSR_iterator::GetState()const
{
  return current_state;
}


int main()
{
  unsigned short nbreBits=5;
  LFSR_histo theHisto;

  optional< const unsigned short >indexes=LFSR_indexes()[ nbreBits ];

  if ( indexes.has_value() )
	{
	  LFSR_parameters theData( nbreBits , *indexes );

	  LFSR_iterator LFSR_parameters_iter(theData, LFSR_extract() );
	  unsigned short iter_count;

	  for ( unsigned short ind = 0 ; ind < ( pow( 2, nbreBits ) -1 +1 ); ind++)
		{
		  cout << LFSR_parameters_iter.GetState() << "   ";
		  iter_count = LFSR_parameters_iter();
		  cout << iter_count << endl;
		  theHisto( ( LFSR_parameters_iter.GetState()).to_ulong());
		}
	  cout<<theHisto<<endl;

	}else
	cerr << "Sorry, we don't have the data for this number of bits" << endl;

  return 0;
}
