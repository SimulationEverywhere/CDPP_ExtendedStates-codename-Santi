/*******************************************************************
*
*  DESCRIPTION: class Impresion
*
*  AUTHOR: Daniel A. Rodriguez 
*
*  EMAIL: mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/2/1999 (v2)
*
*******************************************************************/

#ifndef __IMPRESION_H
#define __IMPRESION_H

#include <string>

/** declarations **/
class Impresion
{
public:
	Impresion(int precision = 3, int width = 10, bool notCient = false,
	          bool printZero = true, bool flatlog = false,
		  int flatlogplane = 1)
			:iPrecision(precision)
			,iWidth(width)
			,iFlatLogPlane(flatlogplane)
			,iNotCientifica(notCient)
			,iprintZero(printZero)
			,iFlatLog(flatlog)
			{}

	int Precision()			// Obtiene la precision.
			{return iPrecision;}

	int Width()			// Obtiene el ancho.
			{return iWidth;}
			
	bool NotacionCientifica()
			{return iNotCientifica;}

	bool PrintZero()
			{return iprintZero;}

	int Precision(int p)		// Asigna la precision.
			{return (iPrecision = p);}

	int Width(int p)		// Asigna el ancho.
			{return (iWidth = p);}

	bool NotacionCientifica(bool notCient)
			{return (iNotCientifica = notCient);}
	
	bool PrintZero(bool pz)
			{return (iprintZero = pz);}

	bool FlatLog(bool pz)
			{return (iFlatLog = pz);}

	bool FlatLog()
			{return (iFlatLog);}

	int FlatLogPlane(int pz)
			{return (iFlatLogPlane = pz);}

        int FlatLogPlane()
			{return (iFlatLogPlane);}

	static Impresion Default;	// Objeto que contendra
					// los datos usados para
					// la impresion de numeros
					// reales.

private:
	int iPrecision, iWidth, iFlatLogPlane;
	bool iNotCientifica, iprintZero, iFlatLog;
}; // Impresion


#endif	// Impresion_H
