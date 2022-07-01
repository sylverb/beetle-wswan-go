#include "wswan.h"
#include "wswan-interrupt.h"
#include "wswan-v30mz.h"
#include "../wswan-state.h"

static uint8 IStatus;
static uint8 IEnable;
static uint8 IVectorBase;

static bool IOn_Cache;
static uint32 IOn_Which;
static uint32 IVector_Cache;

static void RecalcInterrupt(void)
{
   unsigned i;

   IOn_Cache = false;
   IOn_Which = 0;
   IVector_Cache = 0;

   for(i = 0; i < 8; i++)
   {
      if(IStatus & IEnable & (1 << i))
      {
         IOn_Cache = true;
         IOn_Which = i;
         IVector_Cache = (IVectorBase + i) * 4;
         break;
      }
   }
}

void WSwan_Interrupt(int which)
{
   if(IEnable & (1 << which))
      IStatus |= 1 << which;

   RecalcInterrupt();
}

void WSwan_InterruptWrite(uint32 A, uint8 V)
{
   switch(A)
   {
      case 0xB0:
         IVectorBase = V; RecalcInterrupt();
         break;
      case 0xB2:
         IEnable = V; IStatus &= IEnable; RecalcInterrupt();
         break;
      case 0xB6:
         IStatus &= ~V;
         RecalcInterrupt();
         break;
   }
}

uint8 WSwan_InterruptRead(uint32 A)
{
   switch(A)
   {
      case 0xB0:
         return(IVectorBase);
      case 0xB2:
         return(IEnable);
      case 0xB6:
         return(1 << IOn_Which); //return(IStatus);
   }

   return 0;
}

void WSwan_InterruptCheck(void)
{
   if(IOn_Cache)
      v30mz_int(IVector_Cache, false);
}

void WSwan_InterruptReset(void)
{
   IEnable = 0x00;
   IStatus = 0x00;
   IVectorBase = 0x00;
   RecalcInterrupt();
}

int WSwan_InterruptStateAction(StateMem *sm, int load, int data_only)
{
   SFORMAT StateRegs[] =
   {
      { &(IStatus), (uint32_t)sizeof(IStatus), MDFNSTATE_RLSB, "IStatus" },
      { &(IEnable), (uint32_t)sizeof(IEnable), MDFNSTATE_RLSB, "IEnable"},
      { &(IVectorBase), (uint32_t)sizeof(IVectorBase), MDFNSTATE_RLSB, "IVectorBase" },
      { 0, 0, 0, 0 }
   };

   if(!MDFNSS_StateAction(sm, load, data_only, StateRegs, "INTR", false))
      return(0);

   if(load)
      RecalcInterrupt();

   return(1);
}
