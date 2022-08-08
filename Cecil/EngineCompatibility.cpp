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

#include "StdH.h"

static INDEX GetSlashPosition(const CHAR *pszString) {
  for (INDEX iPos = 0; *pszString != '\0'; ++iPos, ++pszString)
  {
    if (*pszString == '\\' || *pszString == '/') {
      return iPos;
    }
  }

  return -1;
};

// CTFileName method from 1.10
void SetAbsolutePath(CTFileName &fnm) {
  // Collect path parts
  CTString strRemaining(fnm);
  CStaticStackArray<CTString> astrParts;

  INDEX iSlashPos = GetSlashPosition(strRemaining);

  if (iSlashPos <= 0) {
    return; // Invalid path
  }

  for (;;) {
    CTString &strBeforeSlash = astrParts.Push();
    CTString strAfterSlash;

    strRemaining.Split(iSlashPos, strBeforeSlash, strAfterSlash);
    strAfterSlash.TrimLeft(strAfterSlash.Length() - 1);
    strRemaining = strAfterSlash;

    iSlashPos = GetSlashPosition(strRemaining);
    if (iSlashPos <= 0) {
      astrParts.Push() = strRemaining;
      break;
    }
  }

  INDEX iPart;

  // Remove certain path parts
  for (iPart = 0; iPart < astrParts.Count(); ++iPart) {
    if (CTString("..") != astrParts[iPart]) {
      continue;
    }

    if (iPart == 0) {
      return; // Invalid path
    }

    // Remove ordered
    CStaticStackArray<CTString> astrShrinked;
    astrShrinked.Push(astrParts.Count() - 2);
    astrShrinked.PopAll();

    for (INDEX iCopiedPart = 0; iCopiedPart < astrParts.Count(); ++iCopiedPart)
    {
      if ((iCopiedPart != iPart - 1) && (iCopiedPart != iPart)) {
        astrShrinked.Push() = astrParts[iCopiedPart];
      }
    }

    astrParts.MoveArray(astrShrinked);
    iPart -= 2;
  }

  // Set new content
  strRemaining.Clear();

  for (iPart = 0; iPart < astrParts.Count(); ++iPart) {
    strRemaining += astrParts[iPart];

    if (iPart < astrParts.Count() - 1) {
      #ifdef PLATFORM_WIN32
        strRemaining += CTString("\\");
      #else
        strRemaining += CTString("/");
      #endif
    }
  }

  fnm = strRemaining;
};
