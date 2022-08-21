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

#include <StdH.h>

#include <CoreLib/Networking/CommInterface.h>

// Define CNameTable_CTFileName
#define TYPE CTFileName
#define CNameTable_TYPE CNameTable_CTFileName
#define CNameTableSlot_TYPE CNameTableSlot_CTFileName

#define NAMETABLE_CASESENSITIVE 0
#include <Engine/Templates/NameTable.h>
#include <Engine/Templates/NameTable.cpp>

#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

// Enough memory for writing (128 MB)
static const ULONG _ulMaxWriteMemory = (1 << 20) * 128;

// CTStream patches
class CStreamPatch : public CTStream {
  public:
    // Allocate memory normally
    void P_AllocVirtualMemory(ULONG ulBytesToAllocate)
    {
      strm_pubBufferBegin = (UBYTE *)AllocMemory(ulBytesToAllocate);
      strm_pubBufferEnd = strm_pubBufferBegin + ulBytesToAllocate;

      strm_pubCurrentPos = strm_pubBufferBegin;
      strm_pubMaxPos = strm_pubBufferBegin;

      strm_pubEOF = strm_pubBufferBegin + ulBytesToAllocate;
    };

    // Free memory normally
    void P_FreeBuffer(void)
    {
      if (strm_pubBufferBegin != NULL) {
        FreeMemory(strm_pubBufferBegin);

        strm_pubBufferBegin = NULL;
        strm_pubBufferEnd   = NULL;
        strm_pubCurrentPos  = NULL;
        strm_pubEOF         = NULL;
        strm_pubMaxPos      = NULL;
      }
    };
};

// CTFileStream patches
class CFileStreamPatch : public CStreamPatch {
  // Copies of private fields
  public:
    FILE *fstrm_pFile;
    INDEX fstrm_iZipHandle;
    INDEX fstrm_iLastAccessedPage;
    BOOL fstrm_bReadOnly;

  public:
    void P_Create(const CTFileName &fnFileName, CTStream::CreateMode cm)
    {
      CTFileName fnmFullFileName;
      INDEX iFile = ExpandFilePath(EFP_WRITE, fnFileName, fnmFullFileName);

      ASSERT(fnFileName.Length() > 0);
      ASSERT(fstrm_pFile == NULL);

      fstrm_pFile = fopen(fnmFullFileName, "wb+");

      if (fstrm_pFile == NULL) {
        Throw_t(TRANS("Cannot create file `%s' (%s)"), fnmFullFileName.str_String, strerror(errno));
      }

      // Allocate enough memory for writing
      P_AllocVirtualMemory(_ulMaxWriteMemory);

      strm_strStreamDescription = fnFileName;
      fstrm_bReadOnly = FALSE;
    };

    void P_Open(const CTFileName &fnFileName, CTStream::OpenMode om)
    {
      ASSERT(fnFileName.Length() > 0);
      ASSERT(fstrm_pFile == NULL && fstrm_iZipHandle == -1);

      CTFileName fnmFullFileName;
      INDEX iFile = ExpandFilePath((om == OM_READ) ? EFP_READ : EFP_WRITE, fnFileName, fnmFullFileName);

      if (om == OM_READ) {
        fstrm_pFile = NULL;

        // [Cecil] NOTE: UNZIP* functions aren't exported
        if (iFile == EFP_MODZIP || iFile == EFP_BASEZIP) {
          // Retrieve ZIP handle to the file
          //fstrm_iZipHandle = UNZIPOpen_t(fnmFullFileName);
          FuncPtr<INDEX (*)(const CTFileName &)> pOpenFunc = ADDR_UNZIP_OPEN;
          fstrm_iZipHandle = (pOpenFunc.pFunction)(fnmFullFileName);

          // Allocate as much memory as the decompressed file size
          //const SLONG slFileSize = UNZIPGetSize(fstrm_iZipHandle);
          FuncPtr<SLONG (*)(INDEX)> pGetSizeFunc = ADDR_UNZIP_GETSIZE;
          const SLONG slFileSize = (pGetSizeFunc.pFunction)(fstrm_iZipHandle);

          P_AllocVirtualMemory(slFileSize);
          
          // Read file contents into the stream
          //UNZIPReadBlock_t(fstrm_iZipHandle, strm_pubBufferBegin, 0, slFileSize);
          FuncPtr<void (*)(INDEX, UBYTE *, SLONG, SLONG)> pReadBlockFunc = ADDR_UNZIP_READBLOCK;
          (pReadBlockFunc.pFunction)(fstrm_iZipHandle, strm_pubBufferBegin, 0, slFileSize);

        } else if (iFile == EFP_FILE) {
          // Open file for reading
          fstrm_pFile = fopen(fnmFullFileName, "rb");

          // Allocate as much memory as the file size
          fseek(fstrm_pFile, 0, SEEK_END);
          const SLONG slFileSize = ftell(fstrm_pFile);
          fseek(fstrm_pFile, 0, SEEK_SET);

          P_AllocVirtualMemory(slFileSize);

          // Read file contents into the stream
          fread(strm_pubBufferBegin, slFileSize, 1, fstrm_pFile);

        } else {
          Throw_t(TRANS("Cannot open file `%s' (%s)"), fnmFullFileName.str_String, TRANS("File not found"));
        }

        fstrm_bReadOnly = TRUE;

      } else if (om == OM_WRITE) {
        // Open file for updating
        fstrm_pFile = fopen(fnmFullFileName, "rb+");
        fstrm_bReadOnly = FALSE;

        // Allocate enough memory for writing
        P_AllocVirtualMemory(_ulMaxWriteMemory);

      } else {
        FatalError(TRANS("File stream opening requested with unknown open mode: %d\n"), om);
      }

      if (fstrm_pFile == NULL && fstrm_iZipHandle == -1) {
        Throw_t(TRANS("Cannot open file `%s' (%s)"), fnmFullFileName.str_String, strerror(errno));
      }

      strm_strStreamDescription = fnmFullFileName;
    };

    void P_Close(void)
    {
      if (fstrm_pFile == NULL && fstrm_iZipHandle == -1) {
        ASSERT(FALSE);
        return;
      }

      strm_strStreamDescription = "";

      if (fstrm_pFile != NULL) {
        // Flush written data back into the file
        if (!fstrm_bReadOnly) {
          fseek(fstrm_pFile, 0, SEEK_SET);
          fwrite(strm_pubBufferBegin, GetStreamSize(), 1, fstrm_pFile);
          fflush(fstrm_pFile);
        }

        fclose(fstrm_pFile);
        fstrm_pFile = NULL;

      // [Cecil] NOTE: UNZIP* functions aren't exported
      } else if (fstrm_iZipHandle >= 0) {
        //UNZIPClose(fstrm_iZipHandle);
        FuncPtr<void (*)(INDEX)> pCloseFunc = ADDR_UNZIP_CLOSE;
        (pCloseFunc.pFunction)(fstrm_iZipHandle);

        fstrm_iZipHandle = -1;
      }

      // Clear allocated memory
      P_FreeBuffer();

      // Clear file dictionary
      strm_dmDictionaryMode = DM_NONE;
      strm_ntDictionary.Clear();
      strm_afnmDictionary.Clear();
      strm_slDictionaryPos = 0;
    };
};

// Dummy stream page method
static void DummyPageFunc(INDEX iPage) {
  (void)iPage;
};

// Dummy method
static void DummyFunc(void) {
  NOTHING;
};

extern void CECIL_ApplyCompatibilityPatch(void) {
  // CTStream
  void (CTStream::*pAllocMemory)(ULONG) = &CTStream::AllocateVirtualMemory;
  NewRawPatch(pAllocMemory, &CStreamPatch::P_AllocVirtualMemory, "CTStream::AllocateVirtualMemory(...)");

  void (CTStream::*pFreeBufferFunc)(void) = &CTStream::FreeBuffer;
  NewRawPatch(pFreeBufferFunc, &CStreamPatch::P_FreeBuffer, "CTStream::FreeBuffer()");

  // CTFileStream
  void (CTFileStream::*pCreateFunc)(const CTFileName &, CTStream::CreateMode) = &CTFileStream::Create_t;
  NewRawPatch(pCreateFunc, &CFileStreamPatch::P_Create, "CTFileStream::Create_t(...)");
  
  void (CTFileStream::*pOpenFunc)(const CTFileName &, CTStream::OpenMode) = &CTFileStream::Open_t;
  NewRawPatch(pOpenFunc, &CFileStreamPatch::P_Open, "CTFileStream::Open_t(...)");
  
  void (CTFileStream::*pCloseFunc)(void) = &CTFileStream::Close;
  NewRawPatch(pCloseFunc, &CFileStreamPatch::P_Close, "CTFileStream::Close()");

  // Dummy methods
  void (CTStream::*pPageFunc)(INDEX) = &CTStream::CommitPage;
  NewRawPatch(pPageFunc, &DummyPageFunc, "CTStream::CommitPage(...)");

  pPageFunc = &CTStream::DecommitPage;
  NewRawPatch(pPageFunc, &DummyPageFunc, "CTStream::DecommitPage(...)");

  pPageFunc = &CTStream::ProtectPageFromWritting;
  NewRawPatch(pPageFunc, &DummyPageFunc, "CTStream::ProtectPageFromWritting(...)");

  void (CNetworkLibrary::*pFinishCRCFunc)(void) = &CNetworkLibrary::FinishCRCGather;
  NewRawPatch(pFinishCRCFunc, &DummyFunc, "CNetworkLibrary::FinishCRCGather()");
};
