/*
 * println.h
 *
 * Created: 07.07.2021 09:46:46
 *  Author: bursztyn
 */ 
#include <avr/io.h>

#ifndef PRINTLN_H_
#define PRINTLN_H_

#define PRINTLN_SIZE		24

void println_msg(char* msg);
void println_num(long num);
void println_flo(float flo);
void print_msg(char* msg);
void print_num(long num);
void println_flo(float flo);
void print_flo(float flo);

#endif /* PRINTLN_H_ */