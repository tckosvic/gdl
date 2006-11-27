/***************************************************************************
                          basic_fun_jmg.cpp  -  basic GDL library function
                             -------------------
    begin                : 2004
    copyright            : (C) 2004 by Joel Gales
    email                : jomoga@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "includefirst.hpp"

#include <string>
#include <fstream>

#include "envt.hpp"
#include "dinterpreter.hpp"
#include "basic_pro_jmg.hpp"

#define MAXNDLL 40

//#define GDL_DEBUG
#undef GDL_DEBUG

namespace lib {

  using namespace std;

  void (*dynPro[MAXNDLL/2])( EnvT* e);
  BaseGDL*(*dynFun[MAXNDLL/2])( EnvT* e);


  void point_lun( EnvT* e) 
  { 
    e->NParam( 1);//, "POINT_LUN");

    DLong lun;
    e->AssureLongScalarPar( 0, lun);

    if( lun == 0 || abs(lun) > maxLun)
      throw GDLException( e->CallingNode(), 
  			  "POINT_LUN:  File unit is not within allowed range.");

    GDLStream& actUnit = fileUnits[ abs(lun)-1];

    if( !actUnit.IsOpen()) 
      throw GDLException( e->CallingNode(), 
  			  "POINT_LUN:  File unit is not open: " +i2s(abs(lun)));

    if (lun < 0) {
      BaseGDL** retPos = &e->GetPar( 1);

      delete *retPos;
      *retPos = new DLongGDL( actUnit.Tell());
      return;

    } else {
      DLong pos;
      e->AssureLongScalarPar( 1, pos);
      actUnit.Seek( pos);
    }
  }

   
  void linkimage( EnvT* e) 
  {

    const char *error;
    void *module[MAXNDLL];
    static int count_pro=0;
    static int count_fun=0;
    int count;

    if (count_pro == MAXNDLL/2) {
      printf("Maximum number of dynamic procedures reached: %d\n", 
	     MAXNDLL/2);
      return;
    }

    if (count_fun == MAXNDLL/2) {
      printf("Maximum number of dynamic functions reached: %d\n", 
	     MAXNDLL/2);
      return;
    }

    count = count_pro + count_fun;

    SizeT nParam=e->NParam();

    DString funcName;
    e->AssureScalarPar<DStringGDL>( 0, funcName);

    DString shrdimgName;
    e->AssureScalarPar<DStringGDL>( 1, shrdimgName);

    DLong funcType;
    e->AssureLongScalarPar( 2, funcType);

    DString entryName;
    e->AssureScalarPar<DStringGDL>( 3, entryName);

    /* Load dynamically loaded library */
    module[count] = dlopen(shrdimgName.c_str(), RTLD_LAZY);
    if (!module[count]) {
      fprintf(stderr, "Couldn't open %s: %s\n", shrdimgName.c_str(), dlerror());
      return;
    }

    /* Get symbol */
    dlerror();

    if (funcType == 0) {
      (void* &) dynPro[count_pro] = 
	(void *) dlsym(module[count], entryName.c_str());
    } else if (funcType == 1) {
      (BaseGDL* &) dynFun[count_fun] = 
	(BaseGDL*) dlsym(module[count], entryName.c_str());
    } else {
      printf("Improper function type: %d\n", funcType);
      dlclose(module[count]);
      return;
    }

    if ((error = dlerror())) {
      fprintf(stderr, "Couldn't find %s: %s\n", entryName.c_str(), error);
      return;
    }

    if (funcType == 0){
      new DLibPro(lib::dynPro[count_pro],funcName.c_str(),8);
      count_pro++;
    } else if (funcType == 1) {
      new DLibFun(lib::dynFun[count_fun],funcName.c_str(),8);
      count_fun++;
    }
  }

  void wait( EnvT* e) 
  { 
    time_t now;
    time_t later;

    e->NParam( 1);//, "WAIT");

    DDouble waittime;
    e->AssureDoubleScalarPar( 0, waittime);

    if( waittime < 0)
      throw GDLException( e->CallingNode(), 
  			  "WAIT:  Argument must be non-negative"
			  +e->GetParString( 0));

    now = time((time_t *)NULL);
    later = time((time_t *)NULL);

    while ( difftime(later, now) < waittime ) {
      later = time((time_t *)NULL);
    }
  }

} // namespace
