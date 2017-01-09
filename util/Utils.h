/*
 * Utils.h
 *
 *  Created on: 3 de mar. de 2016
 *      Author: angel
 */

#ifndef SRC_UTIL_UTILS_H_
#define SRC_UTIL_UTILS_H_

#include <cstdio>
#include <cctype>
#include <chrono>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

#include <vector>
#include <tuple>

using namespace std;

/**
 * Class only eith static methods that do miscelaneus staff
 */
class Utils {

public:
	/**
	 * Divides a string into chunks separating by a delimeter.
	 * @param str string to be divided
	 * @param tokens vector where the chunks will be returned
	 * @param delimeter string that represents where to cut.
	 */
	static void tokenize(const string& str, vector<string> &tokens, const string& delimeter = " ");
	/**
	 * Return the path of the current directory where the program is running
	 * @return a string with the path of the current directory where the program is running if no error has occurred,
	 * an empty one otherwise.
	 */
    static string getCurrentDir() throw (std::runtime_error);
	/**
	 * return the current timestamp in Milliseconds since epoch time
	 */
	static long getCurrentTimeMilis();

	/**
	 * Checks if an object can be cast to a given subtype,
	 *
	 * USE ONLY WITH POLYMORFYC TYPES
	 * @param src pointer to the object trying to be casted
	 * @return true if the object can be casted, false otherwise
	 */
	template<class DstType, class SrcType>
	static bool IsType(const SrcType* src) {
		return dynamic_cast<const DstType*>(src) != 0;
	}

	/**
	* Check if a ends with b,
	*
	* @param a string to compare
	* @param b end of the string 
	* @return true if a ends with b, false otherwise
	*/
	static bool ends_with(std::string const &a, std::string const &b);

	static bool starts_with(std::string const &a, std::string const &b);

    inline static std::string trim(const std::string &s)
    {
       auto  wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
       return std::string(wsfront,std::find_if_not(s.rbegin(),std::string::const_reverse_iterator(wsfront),[](int c){return std::isspace(c);}).base());
    }

    static size_t strToSizeT(const std::string & str) throw(std::invalid_argument);

    static size_t cantorParingFunction(int k1, int k2);
    static size_t cantorParingFunction(long k1, long k2);
private:
	Utils();
	virtual ~Utils();
};

typedef struct {
    //cantor paring function
    size_t operator()(std::tuple<int, int> x) const throw () {
        int k1 = std::get<0>(x);
        int k2 = std::get<1>(x);
        return Utils::cantorParingFunction(k1, k2);
    }
} PairIntIntHashFunction;

typedef struct {
    //cantor paring function
    size_t operator()(std::tuple<long, long> x) const throw () {
        long k1 = std::get<0>(x);
        long k2 = std::get<1>(x);
        return Utils::cantorParingFunction(k1, k2);
    }
} PairLongLongHashFunction;

typedef struct {
    //cantor paring function
    bool operator()(const std::tuple<long, long> & x, const std::tuple<long, long> & y) const throw () {
        long kx1 = std::get<0>(x);
        long kx2 = std::get<1>(x);
        long ky1 = std::get<0>(y);
        long ky2 = std::get<1>(y);
        //return Utils::cantorParingFunction(kx1, kx2) < Utils::cantorParingFunction(ky1, ky2);
        return kx1 < ky1;
    }
} PairLongLongCompareFunction;

#endif /* SRC_UTIL_UTILS_H_ */
