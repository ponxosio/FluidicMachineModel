/*
 * AutoEnumerate.cpp
 *
 *  Created on: 7 de abr. de 2016
 *      Author: angel
 */

#include "AutoEnumerate.h"

AutoEnumerate::AutoEnumerate() {
	this->actualValue = 0;
}

AutoEnumerate::~AutoEnumerate() {}

int AutoEnumerate::getValue() {
	return actualValue;
}

int AutoEnumerate::getNextValue() {
	return actualValue ++;
}

void AutoEnumerate::reset() {
	this->actualValue = 0;
}

void AutoEnumerate::reset(int initialValue) {
    this->actualValue = initialValue;
}

void AutoEnumerate::nextNumber() {
	this->actualValue ++;
}
