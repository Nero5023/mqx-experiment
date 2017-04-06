/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2013 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
***************************************************************************//*!
*
* @file io.c
*
* @author Freescale
*
* @version 0.0.1
*
* @date Jun. 25, 2013
*
* @brief provide serial Input/Output routines. 
*
*******************************************************************************/
#include "uart.h"
#include "stdio.h"

//********************************************************************/
char
in_char (void)
{
//	return uart0_getchar(UART0_BASE_PTR);
	uint_8 temp;
	return uart_re1(UART_0,&temp);
}
/********************************************************************/
void
out_char (char ch)
{
//	uart0_putchar(UART0_BASE_PTR, ch);
	uart_send1(UART_0,ch);
}
/********************************************************************/
int
char_present (void)
{
//	return UART_CharPresent(UART4);
	return 0;
}

#ifdef KEIL
/********************************************************************/
int fputc(int ch, FILE *f)
{
	out_char((char)ch);
	return 1;
}
#endif


/********************************************************************/
