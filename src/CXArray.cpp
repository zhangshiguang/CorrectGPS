/*
 * CXArray.cpp
 *
 *  Created on: 2013-4-9
 *      Author: zsg
 */
#include "log.h"
#include "CXArray.h"

// XArray.cpp: implementation of the CXArray class.
// 1.  本数组类是纬度为key的map映射value；
// 2.  本数组类保存对应的经度和纠偏值；
// 3.  由百度坐标，转gps坐标；
//   author 张世光 2013.3

#ifdef USE_ARRAY
#include <string.h>
Coffsetdata::Coffsetdata()
{
}

Coffsetdata::~Coffsetdata()
{
}

CXArray::CXArray():index(0)
{
        memset(arraylng,0,sizeof(arraylng));
}

CXArray::~CXArray()
{
}
void CXArray::append(unsigned int x,double dx,double dy)
{
        unsigned int tindex =0;
        if (0==index)
        {
                arraylng[tindex]._x = x;
                arraylng[tindex]._dx = dx;
                arraylng[tindex]._dy = dy;
                ++index;

        }
        else if (index>0)
        {
                tindex = index -1;
                while (arraylng[tindex]._x >x)
                {
                        if (0 == tindex)
                        {
                                break;
                        }
                        else
                        {
                                --tindex;
                        }

                }
                //ÕÒµœÍ·
                if (0==tindex && arraylng[tindex]._x >x)
                {
                        memcpy(arraylng+tindex+1,arraylng+tindex, (index-tindex-1)*sizeof(arraylng[0]));
                        arraylng[tindex]._x = x;
                        arraylng[tindex]._dx = dx;
                        arraylng[tindex]._dy = dy;
                        ++index;
                        return;
                }
                //Ã»ÕÒµœÍ·
                if (tindex != index -1)//ÐèÒªÌÚ³öÎ»ÖÃ
                {

        #if 0//def _DEBUG
                	FLOG_MSG("[log][CXArray][append](x(%d)ÔÚÊýŸÝ¿âÖÐ·ÇÕýÐò)\n", x);
        #endif
                        if (arraylng[tindex]._x ==x)
                        {
#ifdef _DEBUG
                                FLOG_MSG("[log][CXArray][append](%d)ÖØžŽ.",x);
#endif
                                return;
                        }
                        else
                        {
                                memcpy(arraylng+tindex+2,arraylng+tindex+1, (index-tindex-1)*sizeof(arraylng[0]));
                        }
                        //DEBUGOUT("...end.\n");
                }


                if (tindex<ARRAY_COUNT-1)
                {
					#if 0//def _DEBUG
                        FLOG_MSG("[log][CXArray][append](%d)(%d)\n",index, tindex);
					#endif
                        arraylng[tindex+1]._x = x;
                        arraylng[tindex+1]._dx = dx;
                        arraylng[tindex+1]._dy = dy;
                        ++index;
                }
                else
                {
				#ifdef _DEBUG
                	FLOG_MSG("[error][CXArray][append](%d-%f-%f).",x, dx,dy);
				#endif
                }

        }



}
int CXArray::getcount()
{
        return index;
}
/*****************************************
°Ù¶ÈµØÍŒŸ­Î³¶Èxy£»
È¡ÆäŸ­¶È0.01£¬µÃµœx1,y1;
ŽÓmapÖÐ²éÑ¯y1£»
ÕÒµœºóŽÓŸ­¶ÈÊý×éÖÐ²éÑ¯x1£¬²¢ÕÒµœÆ«ÒÆdx,dy;
·µ»Ø gpsx =x-dx£» gpsy = y-dy;
Ç°Ìá£ºŸ­¶ÈÊý×éÊÇÏßÐÔµÄ¡£
*/
CPOI CXArray::getGpsxy(CPOI baiduxy,unsigned int key)
{
        CPOI gpsxy;

        unsigned int i= (key - this->arraylng[0]._x);
        bool f=false;

        if( key == this->arraylng[i]._x)
        {
                f = true;
        }
        else
        {   //ÊýŸÝ²»ÊÇÁ¬ÐøµÄ
                i=0;
                while (i<ARRAY_COUNT)
                {
                        if (key == this->arraylng[i]._x )
                        {
                                f=true;
                                break;
                        }
                        ++i;
                }
        }
        if (f)
        {
                gpsxy.dx = baiduxy.dx -this->arraylng[i]._dx;
                gpsxy.dy = baiduxy.dy -this->arraylng[i]._dy;
        }

        return gpsxy;
}
#else
/****
ÖÐ¹úµØÍŒŸ­Î³¶ÈÆ«ÒÆÐÞÕýÊýŸÝ¿â£¬Ÿ«¶È£º0.01Ÿ«¶È£¬ÊýŸÝÁ¿£º9813676 Ìõ¡£
Ê¹ÓÃ·œ·š£º
1¡¢ŒÙÉèÔ­ÊŒgpsÊýŸÝÎª£š73.528888£¬39.368888£©
2¡¢²éÑ¯ÊýŸÝ¿â£ºSELECT * FROM offset where lng=73.52 and lat=39.36£¬µÃ³öoffsetlat¡¢offsetlng
3¡¢ÓÃÔ­ÊŒµÄlatŒÓÉÏoffsetlat£¬lngŒÓÉÏoffsetlng£¬µÃ³öŸÀÆ«ºóµÄŸ­Î³¶È£¬¿ÉÒÔÖ±œÓÔÚgoogleµØÍŒÉÏÏÔÊŸÕýÈ·Î»ÖÃÁË¡£

  ÒÔÉÏ·œ·š£¬°Ù¶ÈµØÍŒºÍ¹ÈžèµØÍŒÍšÓÃ£¬µ«ÊÇŸÀÆ«ÊýŸÝ¿âÊÇÍêÈ«²»Í¬µÄ£¬Ô­Òò¿ÉÒÔ²é¿ŽÆäËû²©¿Í£¬ÓÐÏêÏžµÄËµÃ÷£¡


          ÎÒÃÇÓµÓÐÒÔÏÂÊýŸÝ¿â£º
          ŸÀÆ«ÊýŸÝ¿â(googleµØÍŒ£©
          Ÿ«¶ÈÎª0.01£ºŸ­Î³¶ÈÃ¿±ä»¯0.01£¬È¡Ò»ŽÎÆ«ÒÆÖµ£¬¹²980ÍòÌõ£¬accessÎÄŒþÔŒ600mb£¬Îó²î5~10Ã×¡£
          Ÿ«¶ÈÎª0.001£ºŸ­Î³¶ÈÃ¿±ä»¯0.001£¬È¡Ò»ŽÎÆ«ÒÆÖµ£¬¹²9ÒÚÌõ£¬csvÎÄŒþ¹²20gb£¬Îó²îÔŒ2Ã×¡£

                °Ù¶ÈµØÍŒŸÀÆ«ÊýŸÝ¿â
                Ÿ«¶È0.01£¬¹²2800ÍòÌõ

                  È«¹ú¹Èžè¡¢mapabcÊýŸÝpoi
                  mapabcµØÍŒ(¹Èžè£©È«¹úÐËÈ€µãpoiÉÌ»§ÊýŸÝ¹²1500¶àÍòÌõ£¬°üº¬µØÖ·¡¢µç»°¡¢Ÿ­Î³¶È¡¢Àà±ð£¬CSVžñÊœ¡£

                        È«¹ú°Ù¶ÈÊýŸÝpoi
                        ³¬¹ý2000ÍòÌõ£¬Ãâ·ÑžüÐÂ

Èç¹ûÐèÒªŸÀÆ«ÊýŸÝ¿â£¬ÇëÓëÎÒÁªÏµ£ºQQ 2640056519
***/
CLNGMAP::CLNGMAP()
{

}
CLNGMAP::~CLNGMAP()
{
        LNGMAP::iterator it= lngmap.begin();

        while(it !=lngmap.end())
        {
                 delete it->second ;
                 ++it;
        }
        lngmap.clear();
}
#endif
