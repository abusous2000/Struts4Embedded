/*
 * pSelf->c
 *
 *  Created on: Jul 12, 2019
 *      Author: abusous2000
 */
#include "CPal.h"

#if HAL_USE_PAL
static CPalTypedef *initCPal(CPalTypedef *pSelf){
  palSetLineMode(pSelf->line,pSelf->mode);
#if (PAL_USE_CALLBACKS == TRUE) || defined(__DOXYGEN__)
  if (pSelf->risingfallingEdge != 0 )
     palEnableLineEvent(pSelf->line, pSelf->risingfallingEdge);
  if ( pSelf->cb != NULL)
     palSetLineCallback(pSelf->line, pSelf->cb, (void*)pSelf->arg);
#endif

  return pSelf;
}

static CPalTypedef *setCPal(CPalTypedef *pSelf){
  palSetLine(pSelf->line);

  return pSelf;
}
static CPalTypedef *clearCPal(CPalTypedef *pSelf){
  palClearLine(pSelf->line);

  return pSelf;
}
static CPalTypedef *toggleCPal(CPalTypedef *pSelf){
  palToggleLine(pSelf->line);

  return pSelf;
}
static bool readCPal(CPalTypedef *pSelf){
  return palReadLine(pSelf->line) != 0?true:false;
}
CPalTypedef *initCPalInstance(CPalTypedef *pSelf){
  pSelf->self  = pSelf;
  pSelf->init  = initCPal;
  pSelf->set   = setCPal;
  pSelf->clear = clearCPal;
  pSelf->read = readCPal;
  pSelf->toggle= toggleCPal;
  return pSelf;
}
CPalTypedef newCPal(void){
  return (CPalTypedef){
  .self  = NULL,
  .init  = initCPal,
  .set   = setCPal,
  .clear = clearCPal,
  .read  = readCPal,
  .toggle= toggleCPal,
  };
}

#endif



