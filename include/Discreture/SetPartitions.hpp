#pragma once

#include <numeric>

#include "VectorHelpers.hpp"
#include "Misc.hpp"
#include "Sequences.hpp"
#include "NumberRange.hpp"
#include "Partitions.hpp"
#include <boost/iterator/iterator_facade.hpp>

namespace dscr
{

////////////////////////////////////////////////////////////
/// \brief class of set_partitions of the number n.
/// \param IntType should be an integral type with enough space to store n and k. It can be signed or unsigned.
/// # Example:
///
///	 set_partitions X(3);
///		for (auto& x : X)
///			cout << x << endl;
///
/// Prints out all set partitions of {0,1,2}:
///
/// 	[ [ 0 ] [ 1 ] [ 2 ] ]
///		[ [ 0 1 ] [ 2 ] ]
///		[ [ 0 2 ] [ 1 ] ]
///		[ [ 1 2 ] [ 0 ] ]
///		[ [ 0 1 2 ] ]
///
///
///	# Example 2:
///	One can specify the number of parts:
///
///		set_partitions X(4,2);
///		for (auto& x : X)
///			cout << x << endl;
///
/// Prints out all set partitions of {0,1,2,3,4} with exactly 2 parts:
///
///		[ [ 0 1 2 ] [ 3 ] ]
///		[ [ 0 1 3 ] [ 2 ] ]
///		[ [ 0 2 3 ] [ 1 ] ]
///		[ [ 1 2 3 ] [ 0 ] ]
///		[ [ 0 1 ] [ 2 3 ] ]
///		[ [ 0 2 ] [ 1 3 ] ]
///		[ [ 0 3 ] [ 1 2 ] ]
///
///
////////////////////////////////////////////////////////////
template <class IntType>
class basic_set_partitions
{
public:

	using difference_type = long long;
	using size_type = long long;
	using number_partition = std::vector<IntType> ;
	using value_type = std::vector<number_partition>;
	using set_partition = value_type;
	//Declarations.
	class iterator;

	// **************** Begin static functions

	static bool next_set_partition(set_partition& data, const number_partition& part)
	{
		int n = std::accumulate(part.begin(), part.end(), 0);
		int anteriorpos = pop(data, n - 1);
		int curr = n - 2;
		int currpos = 0;

		while (true)
		{
			currpos = pop(data, curr);

			if (shouldBreak(data, part, currpos, anteriorpos))
				break;

			anteriorpos = currpos;
			--curr;

			if (curr == -1)
				break;
		}

		if (curr == -1)
			return false;

		auto newpos = NextAcceptablePlaceToAdd(data, part, currpos);

		data[newpos].push_back(curr);

		for (int i = curr + 1; i < n; ++i)
		{
			data[NextAcceptablePlaceToAdd(data, part)].push_back(i);
		}

		return true;
	}

	static void fill_first_set_partition(set_partition& data, const number_partition& part)
	{
		IntType numpart = 0;
		IntType etiqueta = 0;
		data.resize(part.size());

		for (auto x : part)
		{
			data[numpart].resize(x);

			for (IntType i = 0; i < x; ++i, ++etiqueta)
			{
				data[numpart][i] = etiqueta;
// 					cout << etiqueta << endl;
			}

			++numpart;
		}
	}

	// **************** End static functions

public:

	////////////////////////////////////////////////////////////
	/// \brief Constructor
	///
	/// \param n is an integer >= 0
	///
	////////////////////////////////////////////////////////////
	basic_set_partitions(IntType n) : m_n(n), m_minnumparts(1), m_maxnumparts(n), m_begin(n, n), m_end()
	{
		m_end.m_ID = size();
	}

	////////////////////////////////////////////////////////////
	/// \brief Constructor
	///
	/// \param n is an integer >= 0
	/// \param numparts is an integer >= 1 and <= n
	///
	////////////////////////////////////////////////////////////
	basic_set_partitions(IntType n, IntType numparts) : m_n(n), m_minnumparts(numparts), m_maxnumparts(numparts), m_begin(n, numparts), m_end()
	{
		m_end.m_ID = size();
	}

	////////////////////////////////////////////////////////////
	/// \brief Constructor
	///
	/// \param n is an integer >= 0
	/// \param minnumparts is an integer >= 1 and <= n
	/// \param maxnumparts is an integer >= minnumparts and <= n
	///
	////////////////////////////////////////////////////////////
	basic_set_partitions(IntType n, IntType minnumparts, IntType maxnumparts) : m_n(n), m_minnumparts(minnumparts), m_maxnumparts(maxnumparts), m_begin(n, maxnumparts), m_end()
	{
		m_end.m_ID = size();
	}


	////////////////////////////////////////////////////////////
	/// \brief The total number of set_partitions
	///
	/// \return If the number of parts was not specified, then the bell number B_n. If it was, then the sum of the appropiate stirling numbers of the second kind.
	///
	////////////////////////////////////////////////////////////
	size_type size() const
	{
		size_type toReturn = 0;

		for (size_t k = m_minnumparts; k <= m_maxnumparts; ++k)
			toReturn += stirling_partition_number(m_n, k);

		return toReturn;
	}

	IntType get_n() const
	{
		return m_n;
	}

	////////////////////////////////////////////////////////////
	/// \brief Forward iterator class.
	////////////////////////////////////////////////////////////
	class iterator : public boost::iterator_facade<
													iterator,
													const set_partition&,
													boost::forward_traversal_tag
													>
	{
	public:
		iterator() : m_ID(0), m_data(), m_n(0) {}

		explicit iterator(IntType n, IntType numparts) : m_ID(0), m_data(n), m_n(n), m_npartition()
		{
			basic_partitions<IntType>::first_with_given_number_of_parts(m_npartition, n, numparts);
			fill_first_set_partition(m_data, m_npartition);
		}
		
		inline size_type ID() const
		{
			return m_ID;
		}
	private:
		//prefix
		void increment()
		{
			++m_ID;

			if (!next_set_partition(m_data, m_npartition))
			{
				basic_partitions<IntType>::next_partition(m_npartition, m_n);
				fill_first_set_partition(m_data, m_npartition);
			}

		}
		
		const set_partition& dereference() const
		{
			return m_data;
		}

		bool equal(const iterator& it) const
		{
			return it.ID() == ID();
		}

	private:
		size_type m_ID;
		set_partition m_data;
		IntType m_n;
		number_partition m_npartition;

		friend class basic_set_partitions;
		friend class boost::iterator_core_access;
	}; // end class iterator


	const iterator& begin() const
	{
		return m_begin;
	}

	const iterator& end() const
	{
		return m_end;
	}

private:
	IntType m_n;
	IntType m_minnumparts;
	IntType m_maxnumparts;
	iterator m_begin;
	iterator m_end;


private:
	// Private static functions
	static int pop(set_partition& data, IntType num)
	{
		for (int i = 0; i < data.size(); ++i)
		{
			if (!data[i].empty() && data[i].back() == num)
			{
				data[i].pop_back();
				return i;
			}
		}

// 			cout << "not found, returning -1" << endl;
		return -1;
	}

	static int NextAcceptablePlaceToAdd(const set_partition& data, const number_partition& part, int oldpos = -1)
	{
// 			cout << "Finding if I can put the next number where " << endl;
		for (int i = oldpos + 1; i < data.size(); ++i)
		{
			if (data[i].size() == part[i])
				continue;

			if ((i > 0) && (part[i - 1] == part[i]) && data[i - 1].empty())
				continue;

			return i;
		}

		return -1;
	}
	static bool shouldBreak(const set_partition& data, const number_partition& part, int currpos, int anteriorpos)
	{
		if (currpos == -1)
			return true;

		if (currpos < anteriorpos)
		{
			if (part[currpos] != part[anteriorpos])
				return true;

			if (!data[currpos].empty())
				return true;
		}

		return false;
	}

}; // end class basic_set_partitions

using set_partitions = basic_set_partitions<int>;

}// end namespace dscr;
