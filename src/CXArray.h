/*
 * CXArray.h
 *
 *  Created on: 2013-4-9
 *      Author: zsg
 */

#ifndef CXARRAY_H_
#define CXARRAY_H_

#include "api.h"
#ifdef USE_ARRAY

class Coffsetdata
{
public:
        Coffsetdata();
        virtual ~Coffsetdata();
        Coffsetdata(unsigned int x,double dx,double dy){
                 _x  = x;
                 _dx =dx;
                 _dy =dy;
        }
        unsigned int _x; // ŒŽŸ­¶È¡Á100
        double _dx;
    double _dy;
};

class CXArray
{
public:
        CXArray();
        virtual ~CXArray();

        Coffsetdata arraylng[ARRAY_COUNT];

        void append(unsigned int x,double dx,double dy);
        int getcount();
        CPOI getGpsxy(CPOI baiduxy,unsigned int key);
private:
        unsigned int index;

};

#else

class CLNGMAP
{
public:
        CLNGMAP();
        ~CLNGMAP();
public:


        LNGMAP  lngmap;

};
#endif

#endif /* CXARRAY_H_ */
