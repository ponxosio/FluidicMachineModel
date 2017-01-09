/*
 * AutoEnumerate.h
 *
 *  Created on: 7 de abr. de 2016
 *      Author: angel
 */

#ifndef SRC_UTIL_AUTOENUMERATE_H_
#define SRC_UTIL_AUTOENUMERATE_H_

/**
 * Class that generates a natural sequence of integer numbers, as an autoenumerate type of a bbdd.
 */
class AutoEnumerate {
public:
	AutoEnumerate();
	virtual ~AutoEnumerate();

	/**
	 * Return the actual value
	 * @return the actual value
	 */
	int getValue();
	/**
	 * Return the actual value and advance the sequence
	 * @return the actual value
	 */
	int getNextValue();
	/**
	 * advance the sequence to next number
	 */
	void nextNumber();
	/**
	 * Reset the sequence to the start.
	 */
	void reset();
    void reset(int initialValue);
protected:
	int actualValue;
};

#endif /* SRC_UTIL_AUTOENUMERATE_H_ */
