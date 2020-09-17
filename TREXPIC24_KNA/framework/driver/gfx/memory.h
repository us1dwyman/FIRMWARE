/* 
 * File:   memory.h
 * Author: us1jkaze
 *
 * Created on February 27, 2019, 4:02 PM
 */

#ifndef MEMORY_H
#define	MEMORY_H

#ifdef	__cplusplus
extern "C" {
#endif
unsigned int offset;
unsigned long progAddr;
unsigned int progDataL;             
unsigned char progDataH;   
    
void Memory_Write(unsigned long *prog_address, unsigned int prog_data_low, unsigned char prog_data_high);



#ifdef	__cplusplus
}
#endif

#endif	/* MEMORY_H */

