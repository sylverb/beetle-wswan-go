/* Mednafen - Multi-system Emulator
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <libretro.h>
#include "mempatcher.h"
#include "wswan-settings.h"
#include "gw_malloc.h"

static uint8 **RAMPtrs = NULL;
static uint32 PageSize;
static uint32 NumPages;

bool MDFNMP_Init(uint32 ps, uint32 numpages)
{
 PageSize = ps;
 NumPages = numpages;

 RAMPtrs = (uint8 **)itc_calloc(numpages, sizeof(uint8 *));
 return(1);
}

void MDFNMP_Kill(void)
{
 if(RAMPtrs)
 {
  RAMPtrs = NULL;
 }
}

void MDFNMP_AddRAM(uint32 size, uint32 A, uint8 *RAM)
{
 uint32 AB = A / PageSize;
 
 size /= PageSize;

 for(unsigned int x = 0; x < size; x++)
 {
  RAMPtrs[AB + x] = RAM;
  if(RAM) /* Don't increment the RAM pointer if we're passed a NULL pointer */
   RAM += PageSize;
 }
}
