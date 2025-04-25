#include <bitset>
// #include <bit>
#include <map>
#include <vector>
#include <algorithm>
#include <optional>
#include <iostream>
#include <deque>
#include <cmath>
#include <set>
#include <limits>
using namespace std;

/** @brief indexes holder
 *
 * The LFRS algorithm pick up (for the XOR) bits at indexes,
 *   according with the total number of bits in the counter.\n
 * This structure returns the indexes list, or not found.
 */
struct LFSR_indexes
{
  /** Defines the maximum size of the LFSR counter
   */
  using LFSR_type = unsigned long;
  const map< unsigned short, LFSR_type >indexes_list;
  LFSR_indexes();
  optional< const LFSR_type >operator[]( const unsigned short& );
};

/** @brief Parameters of the system
 *
 * The number of bits of the LFSR counter is the sum of:
 * * the total number of extracted bits
 * * the power of 2 of the number of repeats again of each outputs\n
 * This class holds this number, the related indexes and the initial state (TODO)
 * An iterator (see below) runs the algorithm.\n
 * With this strategy, a dry run can be done first,
 *   the real run restarts with the same data. 
 */
struct LFSR_parameters
{
  const unsigned short& number_bits;
  const LFSR_indexes::LFSR_type& indexes;
  bitset<numeric_limits<LFSR_indexes::LFSR_type>::digits >initial_state;
  // pickup list
  LFSR_parameters()=delete;
  LFSR_parameters( const unsigned short&number_bits, const LFSR_indexes::LFSR_type&indexes );
};
/** @brief extracts the outputs from the LFSR
 *
 * This class holds the data of the extracted bits.
 * From a LFSR state, it takes one of more set of bits.\n
 */
struct LFSR_extract
{
  const map< string, deque< unsigned short > >extract_data;
  explicit LFSR_extract();
  optional<pair<const unsigned short, const unsigned short> > test_and_required_bits()const;
};
/** @brief Computation of histograms
 *
 * This class is intended to test the software and its data (indexes).
 */
class LFSR_histo
{
  map< LFSR_indexes::LFSR_type, unsigned long> histo;
public:
  void operator()(const LFSR_indexes::LFSR_type&);
  /** @brief Get statistics of the histogram
   *
   * Despite classics statistics, with average, std deviation, dissymmetry, flatness,
   *   this function checks if all the keys have the same number of occurrences.\n
   */
  deque<pair<string, unsigned long> >GetStatistics()const;
  friend ostream&operator<<(ostream&,const LFSR_histo&);
};

/** @ brief Iterates the random states
 *
 * TODO
 */
class LFSR_iterator
{
  const LFSR_parameters&LFSR_init;
  const LFSR_extract&extract;
  unsigned long nbreRuns;
  bitset<numeric_limits<LFSR_indexes::LFSR_type>::digits >current_state;
  bitset<numeric_limits<LFSR_indexes::LFSR_type>::digits >maskXor;
  LFSR_iterator()=delete;
public:
  LFSR_iterator( const LFSR_parameters&LFSR_init, const LFSR_extract&extract );
  const LFSR_indexes::LFSR_type operator()();
  const bitset<numeric_limits<LFSR_indexes::LFSR_type>::digits >GetState()const;
};
