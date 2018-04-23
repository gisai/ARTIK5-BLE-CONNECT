/***********************************************************************************************//**
 * \file   readfile.h
 * \brief  Read-File header file
 ***************************************************************************************************
 * 
 **************************************************************************************************/

#ifndef RFILE_H
#define RFILE_H

/***************************************************************************************************
 * Type Definitions
 **************************************************************************************************/
#define Image_Size	4736

/***************************************************************************************************
 * Function Declarations
 **************************************************************************************************/

int readFile(unsigned char *p);
int ascii_to_hex(char c);


#endif /* RFILE_H */