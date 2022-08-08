/* Copyright (c) 2022 Dreamy Cecil
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

#ifndef CECIL_INCL_ENGINE_COMPATIBILITY_H
#define CECIL_INCL_ENGINE_COMPATIBILITY_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

// CTFileName method from 1.10
void SetAbsolutePath(CTFileName &fnm);

// CDynamicContainer method from 1.10
template<class Type> inline
void ContainerInsert(CDynamicContainer<Type> &cen, Type *ptNewObject, const INDEX iPos)
{
#if SE1_VER > 107
  // Call original method
  cen.Insert(ptNewObject, iPos);

#else
  // Get number of member that need moving and add new one
  const INDEX ctMovees = cen.CStaticStackArray<Type *>::Count() - iPos;
  cen.CStaticStackArray<Type *>::Push();

  // Move all members after insert position one place up
  Type **pptInsertAt = cen.sa_Array + iPos;
  Type **pptMoveTo = pptInsertAt + 1;

  memmove(pptMoveTo, pptInsertAt, sizeof(Type *) * ctMovees);

  // Store pointer to newly inserted member at specified position
  *pptInsertAt = ptNewObject;
#endif
};

#endif
