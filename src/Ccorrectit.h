/*
 * Ccorrectit.h
 *
 *  Created on: 2013-4-8
 *      Author: zr
 */

#ifndef CCORRECTIT_H_
#define CCORRECTIT_H_

class Ccorrectit {
public:
	Ccorrectit();
	virtual ~Ccorrectit();
	static void correctgps_worker(CClient* pclient, INFO_MAP * pinfomap);
};

#endif /* CCORRECTIT_H_ */
