/* Copyright (c) 2002-2012 Croteam Ltd. 
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

// SkyFloat.h : header file
//
#ifndef SKYFLOAT_H
#define SKYFLOAT_H 1

#include <afxwin.h>

void AFXAPI DDX_SkyFloat(CDataExchange* pDX, int nIDC, float &fNumber, BOOL &bValid);
void AFXAPI DDX_SkyFloat(CDataExchange* pDX, int nIDC, float &fNumber);
BOOL FloatFromString(HWND  pWnd, float &fNumber, BOOL &bValid);
void StringFromFloat(HWND hWnd, float fNumber, BOOL &bValid);
#endif // SKYFLOAT_H
