#ifndef CYGONCE_HAL_PLF_INTR_H
#define CYGONCE_HAL_PLF_INTR_H

//==========================================================================
//
//      plf_intr.h
//
//      Atlas Interrupt and clock support
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Contributors: nickg, jskov,
//               gthomas, jlarmour, dmoseley
// Date:         2000-06-06
// Purpose:      Define Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock for the Atlas board.
//              
// Usage:
//              #include <cyg/hal/plf_intr.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/plf_io.h>

//--------------------------------------------------------------------------
// Interrupt vectors.

#ifndef CYGHWR_HAL_INTERRUPT_VECTORS_DEFINED

// These are decoded via the IP bits of the cause
// register when an external interrupt is delivered.

#define CYGNUM_HAL_INTERRUPT_IRQ               0
#define CYGNUM_HAL_INTERRUPT_FIQ               1

// Min/Max ISR numbers and how many there are
#define CYGNUM_HAL_ISR_MIN                     0
#define CYGNUM_HAL_ISR_MAX                     1
#define CYGNUM_HAL_ISR_COUNT                   2

#define CYGHWR_HAL_INTERRUPT_VECTORS_DEFINED

#endif

//--------------------------------------------------------------------------
// Interrupt controller access.



//--------------------------------------------------------------------------
// Control-C support.

#if defined(CYGDBG_HAL_MIPS_DEBUG_GDB_CTRLC_SUPPORT)

# define CYGHWR_HAL_GDB_PORT_VECTOR CYGNUM_HAL_INTERRUPT_SER

externC cyg_uint32 hal_ctrlc_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data);

# define HAL_CTRLC_ISR hal_ctrlc_isr

#endif


//--------------------------------------------------------------------------
// Microsecond delay function provided in hal_misc.c
externC void hal_delay_us(cyg_uint32 us);

#define HAL_DELAY_US(n)          hal_delay_us(n)

//----------------------------------------------------------------------------
// Reset.
#ifndef CYGHWR_HAL_RESET_DEFINED
#define CYGHWR_HAL_RESET_DEFINED
#define HAL_PLATFORM_RESET()

#define HAL_PLATFORM_RESET_ENTRY 0x00000000

#endif // CYGHWR_HAL_RESET_DEFINED

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_PLF_INTR_H
// End of plf_intr.h
