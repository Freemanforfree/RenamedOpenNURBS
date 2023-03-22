//
// Copyright (c) 1993-2022 Robert McNeel & Associates. All rights reserved.
// OpenNURBS, Rhinoceros, and Rhino3D are registered trademarks of Robert
// McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//				
// For complete openNURBS copyright information see <http://www.opennurbs.org>.
//
////////////////////////////////////////////////////////////////

#include "opennurbs.h"

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#if defined(MYON_COMPILER_MSC) && defined(MYON_RUNTIME_WIN)
// November 2015: Visual Studo 2013 (and probably others)
// Shlwapi.h and Shlobj.h are not included in opennurbs_system.h 
// because the have gems like "#define small ..." (Thank You Microsoft!).
// Turns out there is plenty of code that uses opennurbs where crazy 
// developers thought "small" would be a reasonable name for a local
// variable.  Reminds me of dealing with AutoDesk's old #define X 0
// in their headers from 20 years ago. 
#pragma MYON_PRAGMA_WARNING_BEFORE_DIRTY_INCLUDE
#include <Shlwapi.h>
#pragma MYON_PRAGMA_WARNING_AFTER_DIRTY_INCLUDE
#pragma MYON_PRAGMA_WARNING_BEFORE_DIRTY_INCLUDE
#include <Shlobj.h>
#pragma MYON_PRAGMA_WARNING_AFTER_DIRTY_INCLUDE
#if defined(_M_X64) && defined(WIN32) && defined(WIN64)
// Shlwapi.h, Shlobj.h and perhaps others, unconditionally define WIN32
#undef WIN32
#endif
#endif

#if defined(MYON_RUNTIME_APPLE)
#include "unistd.h" //for unlink
#endif

///////////////////////////////////////////////////////////////////////////////

void MYON_String::SplitPath(
  const char* path,
  MYON_String* drive,
  MYON_String* dir,
  MYON_String* fname,
  MYON_String* ext
  )
{
  MYON_FileSystemPath::SplitPath(
    path,
    drive,
    dir,
    fname,
    ext
    );
}

bool MYON_FileSystemPath::IsDirectorySeparator(
  char c,
  bool bAllowAlternate
  )
{
  return (c == MYON_FileSystemPath::DirectorySeparatorAsChar || (bAllowAlternate && c == MYON_FileSystemPath::DirectorySeparatorAsChar));
}

bool MYON_FileSystemPath::IsDirectorySeparator(
  wchar_t c,
  bool bAllowAlternate
  )
{
  return (c == MYON_FileSystemPath::DirectorySeparator || (bAllowAlternate && c == MYON_FileSystemPath::AlternateDirectorySeparator));
}

void MYON_FileSystemPath::SplitPath(
    const char* path,
    MYON_String* drive,
    MYON_String* dir,
    MYON_String* file_name_stem,
    MYON_String* ext
    )
{
  const char* dr = 0;
  const char* d = 0;
  const char* f = 0;
  const char* e = 0;

  // Use local path in case drive, dir, file_name_stem or ext are being reused.
  const MYON_String local_path(path);
  path = static_cast<const char*>(local_path);
  on_splitpath(path,&dr,&d,&f,&e);

  if ( 0 != drive )
  {
    if ( 0 != dr )
    {
      int length;
      if ( 0 != d )
        length = (int)(d-dr);
      else if ( 0 != f )
        length = (int)(f-dr);
      else if ( 0 != e )
        length = (int)(e-dr);
      else
        length = MYON_String::Length(dr);
      *drive = MYON_String(dr,length);
    }
    else
      drive->Empty();
  }

  if ( 0 != dir )
  {
    if ( 0 != d )
    {
      int length;
      if ( 0 != f )
        length = (int)(f-d);
      else if ( 0 != e )
        length = (int)(e-d);
      else
        length = MYON_String::Length(d);

      *dir = MYON_String(d,length);
    }
    else
      dir->Empty();
  }

  if ( 0 != file_name_stem )
  {
    if ( 0 != f )
    {
      int length;
      if ( 0 != e )
        length = (int)(e-f);
      else
        length = MYON_String::Length(f);

      *file_name_stem = MYON_String(f,length);
    }
    else
      file_name_stem->Empty();
  }

  if ( 0 != ext )
  {
    *ext = e;
  }
}

void MYON_wString::SplitPath(
  const char* path,
  MYON_wString* drive,
  MYON_wString* dir,
  MYON_wString* fname,
  MYON_wString* ext
  )
{
  MYON_FileSystemPath::SplitPath(
    path,
    drive,
    dir,
    fname,
    ext
    );
}

void MYON_FileSystemPath::SplitPath(
    const char* path,
    MYON_wString* drive,
    MYON_wString* dir,
    MYON_wString* file_name_stem,
    MYON_wString* ext
    )
{
  const char* dr = 0;
  const char* d = 0;
  const char* f = 0;
  const char* e = 0;

  // Use local path in case drive, dir, file_name_stem or ext are being reused.
  const MYON_String local_path(path);
  path = static_cast<const char*>(local_path);
  on_splitpath(path,&dr,&d,&f,&e);

  if ( 0 != drive )
  {
    if ( 0 != dr )
    {
      int length;
      if ( 0 != d )
        length = (int)(d-dr);
      else if ( 0 != f )
        length = (int)(f-dr);
      else if ( 0 != e )
        length = (int)(e-dr);
      else
        length = MYON_String::Length(dr);

      *drive = MYON_wString(dr,length);
    }
    else
      drive->Empty();
  }

  if ( 0 != dir )
  {
    if ( 0 != d )
    {
      int length;
      if ( 0 != f )
        length = (int)(f-d);
      else if ( 0 != e )
        length = (int)(e-d);
      else
        length = MYON_String::Length(d);

      *dir = MYON_wString(d,length);
    }
    else
      dir->Empty();
  }

  if ( 0 != file_name_stem )
  {
    if ( 0 != f )
    {
      int length;
      if ( 0 != e )
        length = (int)(e-f);
      else
        length = MYON_String::Length(f);

      *file_name_stem = MYON_wString(f,length);
    }
    else
      file_name_stem->Empty();
  }

  if ( 0 != ext )
  {
    *ext = e;
  }
}


void MYON_wString::SplitPath(
  const wchar_t* path,
  MYON_wString* drive,
  MYON_wString* dir,
  MYON_wString* fname,
  MYON_wString* ext
  )
{
  MYON_FileSystemPath::SplitPath( 
    path,
    drive,
    dir,
    fname,
    ext
    );
}

void MYON_FileSystemPath::SplitPath(
  const wchar_t* path,
  MYON_wString* drive,
  MYON_wString* dir,
  MYON_wString* file_name_stem_and_extension
)
{
  const wchar_t* dr = 0;
  const wchar_t* d = 0;
  const wchar_t* f = 0;
  const wchar_t* e = 0;

  // Use local path in case drive, dir, file_name_stem or ext are being reused.
  const MYON_wString local_path(path);
  path = static_cast<const wchar_t*>(local_path);
  on_wsplitpath(path,&dr,&d,&f,&e);

  if ( 0 != drive )
  {
    if ( 0 != dr )
    {
      int length;
      if ( 0 != d )
        length = (int)(d-dr);
      else if ( 0 != f )
        length = (int)(f-dr);
      else if ( 0 != e )
        length = (int)(e-dr);
      else
        length = MYON_wString::Length(dr);

      *drive = MYON_wString(dr,length);
    }
    else
      drive->Empty();
  }

  if ( 0 != dir )
  {
    if ( 0 != d )
    {
      int length;
      if ( 0 != f )
        length = (int)(f-d);
      else if ( 0 != e )
        length = (int)(e-d);
      else
        length = MYON_wString::Length(d);
      *dir = MYON_wString(d,length);
    }
    else
      dir->Empty();
  }

  if ( 0 != file_name_stem_and_extension )
  {
    if ( 0 != f )
    {
      *file_name_stem_and_extension = f;
    }
    else if ( 0 != e )
    {
      // "C:/dir/.abc" returns ".abc"
      *file_name_stem_and_extension = e;
    }
    else
      file_name_stem_and_extension->Empty();
  }
}


void MYON_FileSystemPath::SplitPath( 
  const wchar_t* path,
  MYON_wString* drive,
  MYON_wString* dir,
  MYON_wString* file_name_stem,
  MYON_wString* ext
  )
{
  const wchar_t* dr = 0;
  const wchar_t* d = 0;
  const wchar_t* f = 0;
  const wchar_t* e = 0;

  // Use local path in case drive, dir, file_name_stem or ext are being reused.
  const MYON_wString local_path(path);
  path = static_cast<const wchar_t*>(local_path);
  on_wsplitpath(path,&dr,&d,&f,&e);

  if ( 0 != drive )
  {
    if ( 0 != dr )
    {
      int length;
      if ( 0 != d )
        length = (int)(d-dr);
      else if ( 0 != f )
        length = (int)(f-dr);
      else if ( 0 != e )
        length = (int)(e-dr);
      else
        length = MYON_wString::Length(dr);

      *drive = MYON_wString(dr,length);
    }
    else
      drive->Empty();
  }

  if ( 0 != dir )
  {
    if ( 0 != d )
    {
      int length;
      if ( 0 != f )
        length = (int)(f-d);
      else if ( 0 != e )
        length = (int)(e-d);
      else
        length = MYON_wString::Length(d);
      *dir = MYON_wString(d,length);
    }
    else
      dir->Empty();
  }

  if ( 0 != file_name_stem )
  {
    if ( 0 != f )
    {
      int length;
      if ( 0 != e )
        length = (int)(e-f);
      else
        length = MYON_wString::Length(f);
      *file_name_stem = MYON_wString(f,length);
    }
    else
      file_name_stem->Empty();
  }

  if ( 0 != ext )
  {
    *ext = e;
  }
}

const MYON_wString MYON_FileSystemPath::VolumeFromPath(
  const wchar_t* path
)
{
  MYON_wString volume;
  MYON_FileSystemPath::SplitPath(path, &volume, nullptr, nullptr, nullptr);
  return volume;
}

const MYON_wString MYON_FileSystemPath::DirectoryFromPath(
  const wchar_t* path
)
{
  MYON_wString directory;
  MYON_FileSystemPath::SplitPath(path, nullptr, &directory, nullptr, nullptr);
  return directory;
}

const MYON_wString MYON_FileSystemPath::VolumeAndDirectoryFromPath(
  const wchar_t* path
)
{
  MYON_wString volume;
  MYON_wString directory;
  MYON_FileSystemPath::SplitPath(path, &volume, &directory, nullptr, nullptr);
  const MYON_wString volume_and_directory( volume + directory);
  return volume_and_directory;
}

const MYON_wString MYON_FileSystemPath::FileNameFromPath(
  const wchar_t* path,
  bool bIncludeExtension
)
{
  MYON_wString fname;
  if ( bIncludeExtension )
    MYON_FileSystemPath::SplitPath(path, nullptr, nullptr, &fname);
  else
    MYON_FileSystemPath::SplitPath(path, nullptr, nullptr, &fname, nullptr);
  return fname;
}

const MYON_wString MYON_FileSystemPath::FileNameExtensionFromPath(
  const wchar_t* path
)
{
  MYON_wString ext;
  MYON_FileSystemPath::SplitPath(path, nullptr, nullptr, nullptr, &ext);
  return ext;
}

static bool IsAtoZ(int c)
{
  return ((c >= 'A' && c <= 'Z') ||  (c >= 'a' && c <= 'z'));
}

static bool Is0to9(int c)
{
  return (c >= '0' && c <= '9');
}

static bool IsPermittedInPathName(int c)
{
  if ( c >= 0 && c < MYON_wString::Space )
    return false;
  switch (c)
  {
  case '/':
  case '\\':
  case ':':
  case '<':
  case '>':
  case '"':
  case '?':
  case '*':
  case '|':
  //case 127:
    return false;
  }
  return true;
}

static bool IsPermittedInHostName(int c)
{
  return IsPermittedInPathName(c);
}


static bool IsDirSep(int c)
{
  switch (c)
  {
  case '/':
  case '\\':
    return true;
  }
  return false;
}

static bool IsDotDir(const wchar_t* path)
{
  return (nullptr != path && '.' == path[0] && IsDirSep(path[1]));
}

static bool IsDotDotDir(const wchar_t* path)
{
  return (nullptr != path && '.' == path[0] && '.' == path[1] && IsDirSep(path[2]));
}


bool MYON_FileSystemPath::IsRelativePath(
  const wchar_t* path
  )
{
  return MYON_FileSystemPath::IsRelativePath(path, 0);
}

const MYON_wString MYON_FileSystemPath::CurrentDirectory(
  bool bWithTrailingDirectorySeparator
  )
{
#if defined(MYON_RUNTIME_WIN)

  wchar_t* directory = nullptr;
  const unsigned int directory_capacity = 2018;
  const size_t sizeof_directory = directory_capacity * sizeof(directory[0]);
  directory = (wchar_t*)onmalloc(sizeof_directory);
  memset(directory, 0, sizeof_directory);

  DWORD rc = ::GetCurrentDirectory(directory_capacity - 1, directory);
  MYON_wString fullpath = directory;
  onfree(directory);

  if (rc <= 0 || fullpath.IsEmpty() )
  {
    MYON_ERROR("Windows API ::GetCurrentDirectory() failed.");
    return MYON_wString::EmptyString;
  }

  if (bWithTrailingDirectorySeparator)
    fullpath += MYON_FileSystemPath::DirectorySeparator;
  return fullpath;

#elif defined(MYON_RUNTIME_APPLE)

  char sz[PATH_MAX];
  getcwd(sz, PATH_MAX);
  return sz;

#else

  // unsupported OS
  MYON_ERROR("MYON_FileSystemPath::CurrentDirectory() not implemented.");
  return MYON_wString::EmptyString;   
#endif
}

const MYON_wString MYON_FileSystemPath::RemoveVolumeName(
  const wchar_t* path,
  MYON_wString* volume_name
)
{
  const MYON_wString local_path = MYON_FileSystemPath::CleanPath(path);
  path = static_cast<const wchar_t*>(local_path);
  const wchar_t* vol = nullptr;
  const wchar_t* dir = nullptr;
  const wchar_t* fname = nullptr;
  const wchar_t* fext = nullptr;
  on_wsplitpath(path, &vol, &dir, &fname, &fext);
  if (nullptr == dir)
  {
    if (nullptr != fname)
      dir = fname;
    else if (nullptr != fext)
      dir = fext;
  }

  if (nullptr != volume_name)
  {
    size_t length
      = (nullptr != vol && nullptr != dir && vol < dir)
      ? (int)(dir - vol)
      : 0;
    if (length > 0)
      *volume_name = MYON_wString(vol, (int)length);
    else
      *volume_name = MYON_wString::EmptyString;
  }
  return MYON_wString(dir);
}

const MYON_wString MYON_FileSystemPath::RemoveFileName(
  const wchar_t* path,
  MYON_wString* file_name
)
{
  const MYON_wString local_path = MYON_FileSystemPath::CleanPath(path);
  path = static_cast<const wchar_t*>(local_path);
  const wchar_t* vol = nullptr;
  const wchar_t* dir = nullptr;
  const wchar_t* fname = nullptr;
  on_wsplitpath(path, &vol, &dir, &fname, nullptr);
  
  const size_t length
    = (nullptr != fname && nullptr != path && path <= fname)
    ? (int)(fname - path)
    : local_path.Length();

  if (nullptr != file_name)
    *file_name = fname;

  return MYON_wString(path,(int)length);
}


const MYON_wString MYON_FileSystemPath::CombinePaths(
  const wchar_t* left_side,
  bool bLeftSideContainsFileName,
  const wchar_t* right_side,
  bool bRightSideContainsFileName,
  bool bAppendTrailingDirectorySeparator
)
{
  MYON_wString lhs_fname;
  MYON_wString lhs
    = bLeftSideContainsFileName
    ? MYON_FileSystemPath::RemoveFileName(left_side,&lhs_fname)
    : MYON_FileSystemPath::CleanPath(left_side);

  MYON_wString rhs_fname;
  MYON_wString rhs 
    = bRightSideContainsFileName
    ? MYON_FileSystemPath::RemoveFileName(right_side,&rhs_fname)
    : MYON_FileSystemPath::CleanPath(right_side);

  MYON_wString rhs_volume;
  if ( lhs.IsNotEmpty() )
    rhs = MYON_FileSystemPath::RemoveVolumeName(rhs, &rhs_volume);

  if (rhs.IsNotEmpty() && MYON_FileSystemPath::IsDirectorySeparator(rhs[0], true))
  {
    const MYON_wString tmp(static_cast<const wchar_t*>(rhs) + 1);
    rhs = tmp;
  }

  MYON_wString path = lhs;
  bool bPathEndsDirectorySeparator 
    = path.IsNotEmpty()
    && MYON_FileSystemPath::IsDirectorySeparator(path[path.Length() - 1], true);
  
  if (rhs.IsNotEmpty())
  {
    if (path.IsNotEmpty() && false == bPathEndsDirectorySeparator)
      path += MYON_FileSystemPath::DirectorySeparator;
    path += rhs;

    if (lhs.IsNotEmpty() && MYON_FileSystemPath::IsRelativePath(rhs))
      path = MYON_FileSystemPath::CleanPath(path);

    bPathEndsDirectorySeparator 
      = path.IsNotEmpty()
      && MYON_FileSystemPath::IsDirectorySeparator(path[path.Length() - 1], true);
  }

  if (path.IsNotEmpty() && rhs_fname.IsNotEmpty())
    bAppendTrailingDirectorySeparator = true;

  if ((bPathEndsDirectorySeparator ? 0 : 1) != (bAppendTrailingDirectorySeparator ? 0 : 1))
  {
    if (bAppendTrailingDirectorySeparator)
    {
      const wchar_t* vol = nullptr;
      const wchar_t* dir = nullptr;
      // on_wsplitpath is called to avoid appending a directory separator to a 
      on_wsplitpath(static_cast<const wchar_t*>(path), &vol, &dir, nullptr, nullptr);
      if (nullptr != dir && false == MYON_FileSystemPath::IsDirectorySeparator(path[path.Length() - 1], true))
        path += MYON_FileSystemPath::DirectorySeparator;
    }
    else if ( bPathEndsDirectorySeparator )
    {
      path.SetLength(path.Length() - 1);
    }
  }

  path += rhs_fname;

  return path;
}

bool MYON_FileSystemPath::IsRelativePath(
  const wchar_t* path,
  const wchar_t directory_separator
  )
{
  for (;;)
  {
    if (nullptr == path)
      break;
    if ('.' != *path)
      break;
    path++;
    if ('.' == *path)
      path++;
    if (0 != directory_separator)
    {
      if (directory_separator != *path)
        break;
    }
    else
    {
      if (MYON_FileSystemPath::DirectorySeparator != *path && MYON_FileSystemPath::AlternateDirectorySeparator != *path)
        break;
    }

    return true;
  }
  return false;
}

const MYON_wString MYON_FileSystemPath::CleanPath(
  const wchar_t* dirty_path
  )
{
  bool bTrimLeft = true;
  bool bTrimRight = true;
  bool bAllowWindowsUNCHostNameOrDiskLetter = true;
  bool bDeleteWindowsUNCHostNameOrDiskLetter = (MYON_wString::Backslash != MYON_FileSystemPath::DirectorySeparator);
  const wchar_t directory_separator = MYON_FileSystemPath::DirectorySeparator;
  return MYON_FileSystemPath::CleanPath(
    bTrimLeft,
    bTrimRight,
    bAllowWindowsUNCHostNameOrDiskLetter,
    bDeleteWindowsUNCHostNameOrDiskLetter,
    directory_separator,
    dirty_path
    );
}

static const MYON_wString MYON_wString_CleanPathFailed()
{
  return MYON_wString::EmptyString;
}

const MYON_wString MYON_FileSystemPath::CleanPath(
  bool bTrimLeft,
  bool bTrimRight,
  bool bAllowWindowsUNCHostNameOrDiskLetter,
  bool bDeleteWindowsUNCHostNameOrDiskLetter,
  const wchar_t directory_separator,
  const wchar_t* dirty_path
)
{
  return MYON_FileSystemPath::CleanPath(
    bTrimLeft,
    bTrimRight,
    bAllowWindowsUNCHostNameOrDiskLetter,
    bDeleteWindowsUNCHostNameOrDiskLetter,
    false,
    directory_separator,
    dirty_path
  );
}

const MYON_wString MYON_FileSystemPath::CleanPath(
  bool bTrimLeft,
  bool bTrimRight,
  bool bAllowWindowsUNCHostNameOrDiskLetter,
  bool bDeleteWindowsUNCHostNameOrDiskLetter,
  bool bExpandUser,
  const wchar_t directory_separator,
  const wchar_t* dirty_path
  )
{
  MYON_wString local_dirty_path(dirty_path);
  if ( bTrimLeft )
    local_dirty_path.TrimLeft();
  if ( bTrimRight )
    local_dirty_path.TrimRight();
  if ( local_dirty_path.IsEmpty() )
    return MYON_wString_CleanPathFailed();
  if (bExpandUser)
    local_dirty_path = MYON_FileSystemPath::ExpandUser(local_dirty_path);

  dirty_path = local_dirty_path;

  const bool bIsUNCHostName 
    = bAllowWindowsUNCHostNameOrDiskLetter 
    && local_dirty_path.Length() >= 3
    && MYON_wString::Backslash == local_dirty_path[0]
    && MYON_wString::Backslash == local_dirty_path[1]
    && IsPermittedInHostName(local_dirty_path[2])
    && (IsAtoZ(local_dirty_path[2]) || Is0to9(local_dirty_path[2]) || local_dirty_path[2] > 127)
    ;

  const bool bIsWindowsDrive 
    = bAllowWindowsUNCHostNameOrDiskLetter
    && (false == bIsUNCHostName)
    && local_dirty_path.Length() >= 3
    && ':' == local_dirty_path[1]
    && IsAtoZ(local_dirty_path[0])
    ;

  MYON_wString clean_path(dirty_path);
  wchar_t* clean_head = clean_path.Array();
  wchar_t* clean_start = clean_head;
  if (bIsUNCHostName)
  {
    clean_start += 3; // skip \\ and first charater of host name
    
    // skip rest of host name
    while ( IsPermittedInHostName(*clean_start) )
      clean_start++;    
    if (false == IsDirSep(*clean_start))
      return MYON_wString_CleanPathFailed();
  }
  else if (bIsWindowsDrive)
  {
    // Windows drive letter = capital letter
    *clean_start = MYON_wString::MapCharacterOrdinal(MYON_StringMapOrdinalType::UpperOrdinal,*clean_start);
    clean_start += 2; // Skip drive letter and colon
    if (false == IsDirSep(*clean_start))
      return MYON_wString_CleanPathFailed();
  }

  if (bDeleteWindowsUNCHostNameOrDiskLetter && (bIsUNCHostName || bIsWindowsDrive))
  {
    // Delete Windows UNC host name or drive letter
    local_dirty_path = clean_start;
    dirty_path = local_dirty_path;
    clean_path = dirty_path;
    clean_head = clean_path.Array();
    clean_start = clean_head;
  }

  const size_t clean_start_offset = (clean_start - clean_head);

  wchar_t* dst = clean_start;
  wchar_t* src = dst;
  for (;;)
  {
    wchar_t c;
    // advance to directory separator
    for (c = *src; false == IsDirSep(c) && 0 != c; c = *(++src))
    {
      *dst++ = c;
    }
    if ( 0 == c )
      break;
    // normalize directory separator
    *dst++ = (0 != directory_separator) ? directory_separator : c;
    // Condense /./ and //
    for (src++; (IsDirSep(*src) || IsDotDir(src)); src++)
    {
      // empty body;
    }
  }
  *dst = 0;
  if (dst > clean_head)
  {
    clean_path.SetLength(dst - clean_head);
    clean_head = clean_path.Array();
    clean_start = clean_head + clean_start_offset;
  }
  else
  {
    return MYON_wString_CleanPathFailed();
  }

  dst = clean_start;
  if (IsDirSep(*dst))
  {
    // Skip over root directory separator
    dst++;
  }
  else
  {
    // Skip over initial ../../../ ... at start of a relative path
    while (IsDotDotDir(dst))
      dst += 3;
  }
  if ( 0 == *dst )
    return clean_path;

  src = dst;
  bool bDirty = false;
  while (*src)
  {
    if (IsDotDotDir(src))
    {
      if (!bDirty)
      {
        MYON_ERROR("Bug.");
        return clean_path;
      }
      // will be dealt with in the recursive clean
      *dst++ = *src++;
      *dst++ = *src++;
      *dst++ = *src++;
      continue;
    }
    wchar_t* dir = dst;
    while (false == IsDirSep(*src) && 0 != *src)
    {
      *dst++ = *src++;
    }
    if (dir == dst)
    {
      MYON_ERROR("Bug.");
      return clean_path;
    }
    if (0 == *src)
    {
      break;
    }
    if (IsDotDotDir(src + 1))
    {
      // replace dir/../ with ./ and recursively clean 
      dst = dir;
      dst[0] = '.';
      dst[1] = src[3];
      dst += 2;
      src += 4;
      bDirty = true;
      while (IsDotDotDir(src))
      {
        // will be dealt with in the recursive clean
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
      }
    }
    else if (IsDirSep(*src))
    {
      *dst++ = *src++;
    }
    else
    {
      MYON_ERROR("Bug");
      return clean_path;
    }
  }

  if (dst > clean_head)
  {
    *dst = 0;
    clean_path.SetLength(dst - clean_head);
    clean_head = clean_path.Array();
    clean_start = clean_head + clean_start_offset;
  }
  else
  {
    MYON_ERROR("Bug.");
    return clean_path;
  }

  if ( false == bDirty )
    return clean_path;

  if (dst >= src)
  {
    MYON_ERROR("Bug.");
    return clean_path;
  }

  // recursively clean 
  const MYON_wString clean_tail = MYON_FileSystemPath::CleanPath(false,false,false,false,0,clean_start);
  if (clean_head < clean_start)
  {
    clean_path.SetLength(clean_start - clean_head);
    clean_path += clean_tail;
  }
  else
    clean_path = clean_tail;

  return clean_path;
}

const MYON_wString MYON_FileSystemPath::ExpandUser(
  const char* dirty_path
)
{
  const MYON_wString dirty_local_path(dirty_path);
  return MYON_FileSystemPath::ExpandUser(static_cast<const wchar_t*>(dirty_local_path));
}

const MYON_wString MYON_FileSystemPath::ExpandUser(
  const wchar_t* dirty_path
)
{
  for(;;)
  {
    if (nullptr == dirty_path)
      break;
    if (MYON_wString::Tilde != dirty_path[0])
      break;
    if (false == MYON_FileSystemPath::IsDirectorySeparator(dirty_path[1], true))
      break;
    MYON_wString expanduser_path = MYON_FileSystemPath::PlatformPath(MYON_FileSystemPath::PathId::HomeDirectory);
    if (expanduser_path.IsEmpty())
      break;
    const wchar_t dir_seps[3]
      =
    {
      MYON_FileSystemPath::DirectorySeparator,
      MYON_FileSystemPath::AlternateDirectorySeparator,
      0
    };
    expanduser_path.TrimRight(dir_seps);
    if (expanduser_path.IsEmpty())
      break;
    expanduser_path += MYON_wString(dirty_path + 1);
    return expanduser_path;
  }
  return MYON_wString(dirty_path);
}

bool MYON_FileSystem::PathExists(
  const char* path
  )
{
#if defined(MYON_COMPILER_MSC) && defined(MYON_RUNTIME_WIN)
  return ::PathFileExistsA(path) ? true : false;
#else
  struct stat s;
  if (0 == stat(path, &s))
  {
    if (0 != (s.st_mode & (S_IFDIR|S_IFREG)))
      return true;
  }
  return false;
#endif
}

bool MYON_FileSystem::PathExists(
  const wchar_t* path
  )
{
#if defined(MYON_COMPILER_MSC) && defined(MYON_RUNTIME_WIN)
  return ::PathFileExistsW(path) ? true : false;
#else
  const MYON_String pathUTF8(path);
  return MYON_FileSystem::PathExists(static_cast<const char*>(pathUTF8));
#endif
}

bool MYON_FileSystem::IsDirectory(
  const char* path
  )
{
  if (0 == path || 0 == path[0])
    return false;
#if defined(MYON_COMPILER_MSC) && defined(MYON_RUNTIME_WIN)
  return ::PathIsDirectoryA(path) ? true : false;
#else
  struct stat s;
  if (0 == stat(path, &s))
  {
    if (0 != (s.st_mode & S_IFDIR))
      return true;
  }
  return false;
#endif
}

bool MYON_FileSystem::IsDirectory(
  const wchar_t* path
  )
{
  if (0 == path || 0 == path[0])
    return false;
#if defined(MYON_COMPILER_MSC) && defined(MYON_RUNTIME_WIN)
  return ::PathIsDirectoryW(path) ? true : false;
#else
  const MYON_String pathUTF8(path);
  return MYON_FileSystem::IsDirectory(static_cast<const char*>(pathUTF8));
#endif
}

bool MYON_FileSystem::IsFile(
  const char* path
  )
{
  if (0 == path || 0 == path[0])
    return false;
#if defined(MYON_COMPILER_MSC) && defined(MYON_RUNTIME_WIN)
  return ::PathFileExistsA(path) && 0 == ::PathIsDirectoryA(path);
#else
  struct stat s;
  if (0 == stat(path, &s))
  {
    if (0 == (s.st_mode & S_IFDIR) && 0 != (s.st_mode & S_IFREG))
      return true;
  }
  return false;
#endif
}

bool MYON_FileSystem::IsFile(
  const wchar_t* path
  )
{
  if (0 == path || 0 == path[0])
    return false;
#if defined(MYON_COMPILER_MSC) && defined(MYON_RUNTIME_WIN)
  return ::PathFileExistsW(path) && 0 == ::PathIsDirectoryW(path);
#else
  const MYON_String pathUTF8(path);
  return MYON_FileSystem::IsFile(static_cast<const char*>(pathUTF8));
#endif
}


bool MYON_FileSystem::RemoveFile(
  const char* file_path
)
{
  for (;;)
  {
    if ( false == MYON_FileSystem::IsFile(file_path) )
      break;
    int rc;
#if defined(MYON_RUNTIME_WIN)
    rc = ::_unlink(file_path);
#elif defined(MYON_RUNTIME_APPLE)
    rc = ::unlink(file_path);
#else
    rc = std::remove(file_path);
#endif
    if (0 == rc)
      return true;
    break;
  }

  return false;
}

bool MYON_FileSystem::RemoveFile(
  const wchar_t* file_path
)
{
  for (;;)
  {
    if ( false == MYON_FileSystem::IsFile(file_path) )
      break;
    int rc;
#if defined(MYON_RUNTIME_WIN)
    rc = ::_wunlink(file_path);
#elif defined(MYON_RUNTIME_APPLE)
    const MYON_String utf8_file_path(file_path);
    rc = ::unlink(static_cast<const char*>(utf8_file_path));
#else
    const MYON_String utf8_file_path(file_path);
    rc = std::remove(static_cast<const char*>(utf8_file_path));
#endif
    if (0 == rc)
      return true;
    break;
  }

  return false;
}

bool MYON_FileSystem::IsDirectoryWithWriteAccess(
  const char* path
)
{
  const MYON_wString wide_path(path);
  return MYON_FileSystem::IsDirectoryWithWriteAccess(static_cast<const wchar_t*>(wide_path));
}

bool MYON_FileSystem::IsDirectoryWithWriteAccess(
  const wchar_t* path
)
{
  if (false == MYON_FileSystem::IsDirectory(path))
    return false;

  // native OS tools that query file systems do not
  // work on some network drives.
  // According to Microsoft techs, the only failsafe way
  // is to attempt to write a file.
  // https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/f57928d3-d89b-426d-a174-d06d97355afc/how-to-check-if-a-filefolder-is-writable-or-not?forum=windowssdk


  // try 2 uuids to get a file name that is not in use.
  for (int attempt = 0; attempt < 2; attempt++)
  {
    const MYON_UUID id = MYON_CreateId();
    wchar_t s[41];
    memset(s, 0, sizeof(s));
    MYON_UuidToString(id, s);
    s[36] = '.';
    s[37] = 't';
    s[38] = 'm';
    s[39] = 'p';
    s[40] = 0;
    const MYON_wString tmpfilename = MYON_FileSystemPath::CombinePaths(
      path, false, s, true, false
    );
    if (MYON_FileSystem::PathExists(tmpfilename))
      continue;

    FILE* fp = MYON_FileStream::Open(tmpfilename, L"wb");
    if (nullptr == fp)
      break; // cannot open a file in path
    bool bIsDirectoryWithWriteAccess = false;
    for ( ;; )
    {
      char c = 0;
      const MYON__UINT64 sizeof_c = sizeof(c);
      const MYON__UINT64 count = MYON_FileStream::Write(fp, sizeof_c, &c);
      if (0 != MYON_FileStream::Close(fp))
        break; // cannot close the file.
      fp = nullptr;
      if (count != sizeof_c)
        break; // cannot write to the file in path
      bIsDirectoryWithWriteAccess = MYON_FileSystem::PathExists(tmpfilename);
      // The purpose of this function is to test if a file can be opened and
      // written to using the same tools that write .3dm files.
      //
      // It is possible to have create and write permissions but not have 
      // read permissions. For this reason, we do not attempt to read the tmp file.
      break;
    }
    if (nullptr == fp)
    {
      // The purpose of this function is to test if a file can be opened and
      // written to using the same tools that write .3dm files.
      //
      // There is speculation that when a directory is managed by dropbox 
      // or perhaps other network storage devices, there may be significant 
      // latency in the file systems that results in a time lag between calling 
      // unlink() and having MYON_FileSystem::IsFile() report false.
      // For that reason, we do not check success codes on unlink 
      // or verify the tmp file is gone.
      MYON_FileSystem::RemoveFile(tmpfilename);
    }
    return bIsDirectoryWithWriteAccess;
  }

  return false;
}

const MYON_wString MYON_FileSystemPath::FullPathFromRelativePath(
  const wchar_t* base_path,
  bool bBasePathIncludesFileName,
  const wchar_t* relative_path
  )
{
  if ( nullptr == relative_path || 0 == relative_path )
    return MYON_wString::EmptyString;

  if ( nullptr == base_path || 0 == base_path[0] )
    return MYON_wString::EmptyString;

  const wchar_t* base_path_end = nullptr;
  if (bBasePathIncludesFileName)
  {
    on_wsplitpath(base_path, nullptr, nullptr, &base_path_end, nullptr);
  }
  else
  {
    base_path_end = base_path + MYON_wString::Length(base_path);
  }
  if (nullptr == base_path_end)
    return MYON_wString::EmptyString;
  if (!(base_path < base_path_end))
    return MYON_wString::EmptyString;
  
  MYON_wString dirty_full_path;
  dirty_full_path.Append(base_path,(int)(base_path_end - base_path));
  if ( false == MYON_FileSystemPath::IsDirectorySeparator(base_path_end[-1],true) )
    dirty_full_path += MYON_FileSystemPath::DirectorySeparator;
  dirty_full_path += relative_path;
  return MYON_FileSystemPath::CleanPath(dirty_full_path);
}

static bool CleanAndRemoveFileName(
  const wchar_t* dirty_path,
  bool bPathIncludesFileName,
  MYON_wString& volume,
  MYON_wString& clean_path,
  MYON_wString* file_name
  )
{
  MYON_wString path = MYON_FileSystemPath::CleanPath(dirty_path);
  for (;;)
  {
    if (path.IsEmpty())
      break;

    if (false == bPathIncludesFileName && false == IsDirSep(path[path.Length() - 1]))
      path += MYON_FileSystemPath::DirectorySeparator;

    const wchar_t* p = static_cast<const wchar_t*>(path);
    const wchar_t* v = nullptr;
    const wchar_t* d = nullptr;
    const wchar_t* f = nullptr;
    on_wsplitpath(p, &v, &d, bPathIncludesFileName ? &f : nullptr, nullptr);

    if (nullptr == d || 0 == d[0])
      break;
    
    clean_path = d;

    if (bPathIncludesFileName)
    {
      // remove trailing file name from base_path.
      if (nullptr == f || 0 == f[0])
        break;
      const size_t path_length = (size_t)path.Length();
      if  ( path_length <= 0 )
        break;
      if (!(p <= d && d < f && f < p + path_length))
        break;
      if (!IsDirSep(f[-1]))
        break;
      size_t len = (f - d);
      if (len <= 1 || len >= (size_t)clean_path.Length())
        break;
      if ( nullptr != file_name )
        *file_name = f;
      clean_path.SetLength(len);
    }
    else
    {
      if ( nullptr != file_name )
        *file_name = MYON_wString::EmptyString;
    }

    return true;
  }

  volume = MYON_wString::EmptyString;
  clean_path = MYON_wString::EmptyString;
  if ( nullptr != file_name )
    *file_name = MYON_wString::EmptyString;
  return false;
}

const MYON_wString MYON_FileSystemPath::RelativePath(
  const wchar_t* full_path,
  bool bFullPathIncludesFileName,
  const wchar_t* base_path,
  bool bBasePathIncludesFileName
  )
{
  MYON_wString best_answer(full_path);

  MYON_wString full_volume;
  MYON_wString local_full;
  MYON_wString file_name;
  if (false == CleanAndRemoveFileName(full_path,bFullPathIncludesFileName,full_volume,local_full,&file_name))
    return best_answer;

  best_answer = local_full;
  best_answer += file_name;

  MYON_wString base_volume;
  MYON_wString local_base;
  if (false == CleanAndRemoveFileName(base_path,bBasePathIncludesFileName,base_volume,local_base,nullptr))
    return best_answer;

  if (full_volume.IsNotEmpty() || base_volume.IsNotEmpty() )
  {
    if (false == MYON_wString::EqualPath(full_volume,base_volume))
      return best_answer;
  }

  const wchar_t* full_tail = static_cast<const wchar_t*>(local_full);
  const wchar_t* base_tail = static_cast<const wchar_t*>(local_base);
  if (false == IsDirSep(*full_tail) || false == IsDirSep(*base_tail))
  {
    // A double directory separator after the initial CleanAndRemoveFileName()
    // calls indicates invalid file path informtion.
    return best_answer;
  }

  // Skip initial directory separator
  full_tail++;
  base_tail++;
  if (0 == *full_tail || 0 == *base_tail)
  {
    return best_answer;
  }
  if (IsDirSep(*full_tail) || IsDirSep(*base_tail))
  {
    // A double directory separator after the initial MYON_FileSystemPath::CleanPath()
    // calls indicates invalid file path informtion.
    return best_answer;
  }

  int overlap_directory_count = 0;
  if (0 != *full_tail && 0 != *base_tail)
  {
    const wchar_t* full1 = full_tail;
    const wchar_t* base1 = base_tail;
    while (0 != *full1 && 0 != *base1 )
    {
      if (IsDotDir(base1) || IsDotDotDir(base1))
      {
        overlap_directory_count = 0;
        break;
      }
      bool bFullDirSep = IsDirSep(*full1);
      bool bBaseDirSep = IsDirSep(*base1);
      if (false == bFullDirSep && false == bBaseDirSep)
      {
        // skipping an element of a directory name 
        base1++;
        full1++;
        continue;
      }
      if (bFullDirSep && bBaseDirSep)
      {
        if (false == MYON_wString::EqualPath(full_tail, (int)(full1 - full_tail), base_tail, (int)(base1 - base_tail)))
        {
         // directory names have identical lengths and different content
          break;
        }

        // matching directory names

        // skip directory separator
        base1++;
        full1++;
        if (IsDirSep(*base1) || IsDirSep(*full1))
        {
          // damaged input
          break;
        }
        base_tail = base1;
        full_tail = full1;
        overlap_directory_count++;
        continue;
      }
      // directory names have different lengths
      break;
    }
  }

  if (overlap_directory_count < 1)
    return best_answer;

  // It is reasonable for base_tail to be nullptr
  if (nullptr == full_tail && IsDirSep(*full_tail) )
    return best_answer;

  // It is reasonable for base_tail to be nullptr
  if (nullptr != base_tail && IsDirSep(*base_tail) )
    return best_answer;
  
  // set dotdot_count to number of directories left in base_tail
  int dotdot_count = 0;
  while (0 != *base_tail)
  {
    if (IsDotDir(base_tail) || IsDotDotDir(base_tail))
      return best_answer;
    if (IsDirSep(*base_tail))
    {
      if (IsDirSep(base_tail[1]))
        return best_answer; // shouldn't be double dir seps after the initial clean
      dotdot_count++;
    }
    base_tail++;
  }

  // buid relative path
  MYON_wString relative_path;
  if (0 == dotdot_count)
  {
    relative_path = L".";
    relative_path += MYON_FileSystemPath::DirectorySeparator;
  }
  else
  {
    for (int i = 0; i < dotdot_count; i++)
    {
      relative_path += L"..";
      relative_path += MYON_FileSystemPath::DirectorySeparator;
    }
  }
  if (nullptr != full_tail && 0 != full_tail[0] )
    relative_path += full_tail;
  if (file_name.IsNotEmpty())
    relative_path += file_name;

  return relative_path;
}


//////////////////////////////////////////////////////////////////////////////
//
// MYON_FileStream implementation
//

FILE* MYON_FileStream::Open( const wchar_t* filename, const wchar_t* mode )
{
  FILE* fp = 0;

  if ( 0 == filename || 0 == filename[0] || 0 == mode || 0 == mode[0] )
    return fp;

#if defined(MYON_COMPILER_MSC) && defined(MYON_RUNTIME_WIN)
  errno_t e = _wfopen_s(&fp,filename,mode); 
  if ( 0 != e && 0 == fp )
    fp = 0; // reference e to keep lint quiet.
#else
  // I can't find an wfopen() or _wfopen() in
  // gcc version egcs-2.91.66 19990314/Linux (egcs-1.1.2 release)
  MYON_String fnameUTF8(filename);
  MYON_String modeUTF8(mode);
  fp = fopen(fnameUTF8,modeUTF8);
#endif

  return fp;
}

FILE* MYON_FileStream::Open( const char* filename, const char* mode )
{
  FILE* fp = 0;

  if ( 0 == filename || 0 == filename[0] || 0 == mode || 0 == mode[0] )
    return fp;

#if defined(MYON_COMPILER_MSC) && defined(MYON_RUNTIME_WIN)
  errno_t e = fopen_s(&fp,filename,mode); 
  if ( 0 != e && 0 == fp )
    fp = 0; // reference e to keep lint quiet.
#else
  fp = fopen(filename,mode);
#endif

  return fp;
}

int MYON_FileStream::Close( FILE* fp )
{
  return ( ( 0 != fp ) ? fclose(fp) : -1 );
}

bool MYON_FileStream::Is3dmFile(
  const wchar_t* file_path,
  bool bAllow3dmbakExtension
)
{
  for (;;)
  {
    if (false == MYON_FileSystemPath::FilePathHas3dmExtension(file_path, bAllow3dmbakExtension))
      break;
    FILE* fp = MYON_FileStream::Open3dmToRead(file_path);
    if (nullptr == fp)
      break;
    MYON_FileStream::Close(fp);
    return true;
  }
  return false;
}

bool MYON_FileStream::Is3dmFile(
  const char* file_path,
  bool bAllow3dmbakExtension
)
{
  for (;;)
  {
    if (false == MYON_FileSystemPath::FilePathHas3dmExtension(file_path, bAllow3dmbakExtension))
      break;
    FILE* fp = MYON_FileStream::Open3dmToRead(file_path);
    if (nullptr == fp)
      break;
    MYON_FileStream::Close(fp);
    return true;
  }
  return false;
}

bool MYON_FileSystemPath::FilePathHas3dmExtension(
  const wchar_t* file_path,
  bool bAllow3dmbakExtension
)
{
  for (;;)
  {
    // test file name
    const wchar_t* e = nullptr;
    on_wsplitpath(file_path, nullptr, nullptr, nullptr, &e);
    if (nullptr == e)
      break;
    if ('.' != e[0])
      break;
    if ('3' != e[1])
      break;
    if ('d' != e[2] && 'D' != e[2])
      break;
    if ('m' != e[3] && 'M' != e[3])
      break;
    if (0 == e[4])
      return true;
    if (false == bAllow3dmbakExtension)
      break;
    if ('b' != e[4] && 'B' != e[4])
      break;
    if ('a' != e[5] && 'A' != e[5])
      break;
    if ('k' != e[6] && 'K' != e[6])
      break;
    if (0 != e[7])
      break;
    return true;
  }
  return false;
}

bool MYON_FileSystemPath::FilePathHas3dmExtension(
  const char* file_path,
  bool bAllow3dmbakExtension
)
{
  for (;;)
  {
    // test file name
    const char* e = nullptr;
    on_splitpath(file_path, nullptr, nullptr, nullptr, &e);
    if (nullptr == e)
      break;
    if ('.' != e[0])
      break;
    if ('3' != e[1])
      break;
    if ('d' != e[2] && 'D' != e[2])
      break;
    if ('m' != e[3] && 'M' != e[3])
      break;
    if (0 == e[4])
      return true;
    if (false == bAllow3dmbakExtension)
      break;
    if ('b' != e[4] && 'B' != e[4])
      break;
    if ('a' != e[5] && 'A' != e[5])
      break;
    if ('k' != e[6] && 'K' != e[6])
      break;
    if (0 != e[7])
      break;
    return true;
  }
  return false;
}

bool MYON_FileSystemPath::IsValidFileName(
  const char* file_name,
  bool bAllPlatforms
)
{
  const MYON_wString wide_file_name(file_name);
  return MYON_FileSystemPath::IsValidFileName(static_cast<const wchar_t*>(wide_file_name), bAllPlatforms);
}

bool MYON_FileSystemPath::IsValidFileName(
  const wchar_t* file_name,
  bool bAllPlatforms
)
{
  if (nullptr == file_name || 0 == file_name[0])
    return false;

  bool bDoubleDot = false;
  wchar_t prev_c = 0;
  int file_name_length;
  for (file_name_length = 0; 0 != file_name[file_name_length]; file_name_length++)
  {
    if (file_name_length > 256)
      return false;

    // note that all illegal symbols currently tested for have 
    // UNICODE code points <= U+07F, so we can simply test c
    const wchar_t c = file_name[file_name_length];
    if (MYON_FileSystemPath::IsDirectorySeparator(c, bAllPlatforms))
      return false;

    switch (c)
    {
    case '.':
      bDoubleDot = ('.' == prev_c);
      break;

    case ':':
#if defined(MYON_RUNTIME_WIN)
      return false;
#else
      if (bAllPlatforms)
        return false;
#endif
      break;

      //// Most windows apps have these restrictions, but the file system supports 
      //// names with these characters.
      ////    case ':':
      ////    case '~':
      ////    case '#':
      ////    case '%':
      ////    case '&':
      ////    case '*':
      ////    case '{':
      ////    case '}':
      ////    case '<':
      ////    case '>':
      ////    case '?':
      ////    case '|':
      ////    case '"':
      ////#if defined(MYON_RUNTIME_WIN)
      ////      return false;
      ////#else
      ////      if (bAllPlatforms)
      ////        return false;
      ////#endif
      ////    
    }

    prev_c = c;
  }

  switch(prev_c)
  {
  case '.':
    if (1 == file_name_length)
      return false;
    if (2 == file_name_length && bDoubleDot)
      return false; 
    break;

  case '~':
    if (1 == file_name_length)
      return false;
    break;
  }

  return true;
}

const MYON_wString MYON_FileSystemPath::PlatformPath(MYON_FileSystemPath::PathId path_id)
{
#if defined(MYON_RUNTIME_WIN)
  KNOWNFOLDERID platform_path_id;
#define MYON_INTERNAL_SET_LOCAL_DIRECTORY_ID(win_fid,apple_fid) platform_path_id = win_fid
#elif defined(MYON_RUNTIME_COCOA_AVAILABLE)
  NSSearchPathDirectory platform_path_id;
#define MYON_INTERNAL_SET_LOCAL_DIRECTORY_ID(win_fid,apple_fid) platform_path_id = apple_fid
#endif

#if defined(MYON_INTERNAL_SET_LOCAL_DIRECTORY_ID)
  switch (path_id)
  {
  case MYON_FileSystemPath::PathId::DesktopDirectory:
    MYON_INTERNAL_SET_LOCAL_DIRECTORY_ID(FOLDERID_Desktop, NSDesktopDirectory);
    break;
  case MYON_FileSystemPath::PathId::DocumentsDirectory:
    MYON_INTERNAL_SET_LOCAL_DIRECTORY_ID(FOLDERID_Documents, NSDocumentDirectory);
    break;
  case MYON_FileSystemPath::PathId::DownloadsDirectory:
    MYON_INTERNAL_SET_LOCAL_DIRECTORY_ID(FOLDERID_Downloads, NSDownloadsDirectory);
    break;
  case MYON_FileSystemPath::PathId::HomeDirectory:
//#if defined(MYON_RUNTIME_WIN)
//    platform_path_id = FOLDERID_Profile;
//#elif defined(MYON_RUNTIME_APPLE_OBJECTIVE_C_AVAILABLE)
//     platform_path_id = NSUserDirectory;
//#endif
    MYON_INTERNAL_SET_LOCAL_DIRECTORY_ID(FOLDERID_Profile, NSUserDirectory);
    //MYON_INTERNAL_SET_LOCAL_DIRECTORY_ID(FOLDERID_Profile, NSHomeDirectory);
    break;
  default:
    return MYON_wString::EmptyString;
  }
#undef MYON_INTERNAL_SET_LOCAL_DIRECTORY_ID
#endif

  MYON_wString path;

#if defined(MYON_RUNTIME_WIN)

  const DWORD dwFlags = KF_FLAG_DEFAULT;
  const HANDLE hToken = nullptr; // current user
  wchar_t* windows_path = nullptr;
  const HRESULT hr = ::SHGetKnownFolderPath(
    platform_path_id,
    dwFlags,
    hToken,
    &windows_path
  );
  if (nullptr != windows_path)
  {
    if (S_OK == hr)
      path = windows_path;
    CoTaskMemFree(windows_path);
  }

#elif defined(MYON_RUNTIME_COCOA_AVAILABLE)

  NSArray *apple_paths = NSSearchPathForDirectoriesInDomains(platform_path_id, NSUserDomainMask, YES);
  if ([apple_paths count] > 0)
  {
    NSString* apple_path = [apple_paths objectAtIndex : 0];
    if (nullptr != apple_path)
    {
      MYON_wString s;
      const int len = (int)apple_path.length;
      s.SetLength(len);
      int idx;
      for (idx = 0; idx < len; idx++)
        s[idx] = [apple_path characterAtIndex : idx];
      s[idx] = 0;
      path = s;
    }
  }

#else

  if (MYON_FileSystemPath::PathId::HomeDirectory != path_id)
  {
    MYON_ERROR("Function not implemented.");
  }

#endif

  path.TrimLeftAndRight();

  // See if environment variables will
  if (MYON_FileSystemPath::PathId::HomeDirectory == path_id && path.IsEmpty())
  {
    const wchar_t dir_seps[4] = {
      MYON_FileSystemPath::DirectorySeparator,
      MYON_FileSystemPath::AlternateDirectorySeparator,
      MYON_wString::Space,
      0
    };
    for (;;)
    {
      path = MYON_wString(getenv("HOME"));
      path.TrimLeftAndRight();
      path.TrimRight(dir_seps);
      if ( MYON_FileSystem::IsDirectory(path) )
        break;

#if defined(MYON_RUNTIME_WIN)
      path = MYON_wString(getenv("USERPROFILE"));
      path.TrimLeftAndRight();
      path.TrimRight(dir_seps);
      if ( MYON_FileSystem::IsDirectory(path) )
        break;

      path = MYON_wString(getenv("HOMEDRIVE")) + MYON_wString(getenv("HOMEPATH"));
      path.TrimLeftAndRight();
      path.TrimRight(dir_seps);
      if ( MYON_FileSystem::IsDirectory(path) )
        break;
#endif
      path = MYON_wString::EmptyString;
      break;
    }
  }

  return path;
}

static unsigned int MYON_Internal_SeekTo3DGeometryFileFormatMark(
  FILE* fp
)
{
  const char* tag = "3D Geometry File Format ";
  char buffer[33] = {};

  for (;;)
  {
    if (32 != MYON_FileStream::Read(fp, 32, buffer))
      break;

    if (0 != MYON_String::CompareOrdinal(tag, 24, buffer, 24, false))
    {
      // it's not a "pure" .3DM file 
      // - see if we have a .3DM file with MS OLE-goo at the start
      // (generally, there is around 6kb of goo.  I keep looking
      // for up to 32mb just in case.)
      unsigned int offset = 0;
      for (unsigned int n = 0; n < 33554432; n++)
      {
        for (int j = 0; j < 31; j++)
          buffer[j] = buffer[j + 1];
        if (!MYON_FileStream::Read(fp, 1, &buffer[31]))
          break;
        if (0 == MYON_String::CompareOrdinal(tag, 24, buffer, 24, false))
        {
          offset = n + 1;
          break;
        }
      }
      if (0 == offset)
        break;
    }

    // get version
    //char* sVersion = s3d+24;
    // skip leading spaces
    int ver = 0;
    int i = 24;
    while (i < 32 && buffer[i] == ' ')
      i++;
    while (i < 32)
    {
      // TEMPORARY 2 = X
      if (i == 31 && buffer[i] == 'X')
      {
        buffer[i] = '2';
      }

      if (buffer[i] < '0' || buffer[i] > '9')
      {
        // it's not a valid .3DM file version
        break;
      }
      ver = ver * 10 + ((int)(buffer[i] - '0'));
      i++;
    }
    if (ver <= 0)
      break;

    if (false == MYON_FileStream::SeekFromCurrentPosition(fp, -32))
      break;

    return (unsigned int)ver;
  }
  MYON_FileStream::SeekFromStart(fp, 0);
  return false;
}

FILE* MYON_FileStream::Open3dmToRead(
  const wchar_t* file_path
)
{
  FILE* fp = nullptr;

  for (;;)
  {
    fp = MYON_FileStream::Open(file_path,L"rb");
    if (nullptr == fp)
      break;

    if ( 0 == MYON_Internal_SeekTo3DGeometryFileFormatMark(fp))
      break;

    return fp;
  }

  if ( nullptr != fp )
    MYON::CloseFile(fp);

  return nullptr;
}

FILE* MYON_FileStream::MYON_FileStream::Open3dmToRead(
  const char* file_path
)
{
  FILE* fp = nullptr;

  for (;;)
  {
    fp = MYON_FileStream::Open(file_path,"rb");
    if (nullptr == fp)
      break;

    if ( 0 == MYON_Internal_SeekTo3DGeometryFileFormatMark(fp))
      break;

    return fp;
  }

  if ( nullptr != fp )
    MYON::CloseFile(fp);

  return nullptr;
}


MYON__INT64 MYON_FileStream::CurrentPosition( FILE* fp )
{
  if ( 0 == fp )
    return -1;
#if defined(MYON_COMPILER_MSC) && defined(MYON_RUNTIME_WIN)
  return _ftelli64(fp);
#else
  return ftell(fp);
#endif
}

bool MYON_FileStream::SeekFromCurrentPosition( FILE* fp, MYON__INT64 offset )
{
  return MYON_FileStream::Seek(fp,offset,SEEK_CUR);
}

bool MYON_FileStream::SeekFromStart( FILE* fp, MYON__INT64 offset )
{
  return MYON_FileStream::Seek(fp,offset,SEEK_SET);
}

bool MYON_FileStream::SeekFromEnd( FILE* fp, MYON__INT64 offset )
{
  return MYON_FileStream::Seek(fp,offset,SEEK_END);
}

bool MYON_FileStream::Seek( FILE* fp, MYON__INT64 offset, int origin )
{
  if ( 0 == fp )
    return false;

  if ( origin < 0 || origin > 2 )
    return false;

  if ( 0 == offset && SEEK_CUR == origin )
    return true;
  
#if defined(MYON_COMPILER_MSC) && defined(MYON_RUNTIME_WIN)
  if (0 != _fseeki64(fp, offset, origin))
    return false;
#else

  const int i = 2147483646;
  const MYON__INT64 i64 = i;

  while ( offset > i64 )
  {
    if ( 0 != fseek( fp, i, origin ) )
      return false;
    if (SEEK_CUR != origin)
      origin = SEEK_CUR;
    offset -= i64;
  }

  while ( offset < -i64 )
  {
    if ( 0 != fseek( fp, -i, origin ) )
      return false;
    if (SEEK_CUR != origin)
      origin = SEEK_CUR;
    offset += i64;
  }

  if (0 != offset || SEEK_CUR != origin)
  {
    int ioffset = (int)offset;
    if (0 != fseek(fp, ioffset, origin))
      return false;
  }

#endif

  return true;
}

MYON__UINT64 MYON_FileStream::Read( FILE* fp, MYON__UINT64 count, void* buffer )
{
  MYON__UINT64 rc = 0;
  if ( 0 == fp || count <= 0 || 0 == buffer )
    return rc;

  if ( count <= MYON_MAX_SIZE_T )
  {
    rc = (MYON__UINT64)fread(buffer,1,(size_t)count,fp);
  }
  else
  {
    size_t sz, szread;
    while ( count > 0 )
    {
      sz = ( count > MYON_MAX_SIZE_T ) ? MYON_MAX_SIZE_T : ((size_t)count);
      szread = fread(buffer,1,sz,fp);
      rc += szread;
      if ( szread != sz )
        break;
      count -= sz;
      buffer = ((unsigned char*)buffer) + sz;
    }
  }

 return rc;
}

MYON__UINT64 MYON_FileStream::Write( FILE* fp, MYON__UINT64 count, const void* buffer )
{
  MYON__UINT64 rc = 0;
  if ( 0 == fp || count <= 0 || 0 == buffer )
    return rc;

  if ( count <= MYON_MAX_SIZE_T )
  {
    rc = fwrite(buffer,1,(size_t)count,fp);
  }
  else
  {
    size_t sz, szwrite;
    while ( count > 0 )
    {
      sz = ( count > MYON_MAX_SIZE_T ) ? MYON_MAX_SIZE_T : ((size_t)count);
      szwrite = fwrite(buffer,1,sz,fp);
      rc += szwrite;
      if ( szwrite != sz )
        break;
      count -= sz;
      buffer = ((unsigned char*)buffer) + sz;
    }
  }

 return rc;
}

bool MYON_FileStream::Flush( FILE* fp )
{
  if ( 0 == fp )
    return false;
  if ( 0 != fflush(fp) )
    return false;
  return true;
}

bool MYON_FileStream::GetFileInformation(
  const wchar_t* file_name,
  MYON__UINT64* file_size,
  MYON__UINT64* file_metadata_last_modified_time,
  MYON__UINT64* file_contents_last_modified_time
  )
{
  FILE* fp = MYON::OpenFile(file_name, L"rb");
  bool rc = MYON_FileStream::GetFileInformation(fp,file_size,file_metadata_last_modified_time,file_contents_last_modified_time);
  MYON::CloseFile(fp);
  return rc;
}

bool MYON_FileStream::GetFileInformation(
  const char* file_name,
  MYON__UINT64* file_size,
  MYON__UINT64* file_metadata_last_modified_time,
  MYON__UINT64* file_contents_last_modified_time
  )
{
  FILE* fp = MYON::OpenFile(file_name, "rb");
  bool rc = MYON_FileStream::GetFileInformation(fp,file_size,file_metadata_last_modified_time,file_contents_last_modified_time);
  MYON::CloseFile(fp);
  return rc;
}

bool MYON_FileStream::GetFileInformation( 
    FILE* fp,
    MYON__UINT64* file_size,
    MYON__UINT64* file_metadata_last_modified_time,
    MYON__UINT64* file_contents_last_modified_time
    )
{
  bool rc = false;

  if (file_size)
    *file_size = 0;
  if (file_metadata_last_modified_time)
    *file_metadata_last_modified_time = 0;
  if (file_contents_last_modified_time)
    *file_contents_last_modified_time = 0;

  if ( fp )
  {

#if defined(MYON_COMPILER_MSC)

    // Microsoft compilers
#if (_MSC_VER >= 1400)
    // VC 8 (2005) 
    // works for file sizes > 4GB 
    // when size_t is a 64 bit integer
    struct _stat64 sb;
    memset(&sb,0,sizeof(sb));
    int fd = _fileno(fp);    
    int fstat_rc = _fstat64(fd, &sb);
#else
    // VC6 compiler
    struct _stat sb;
    memset(&sb,0,sizeof(sb));
    int fd = _fileno(fp);    
    int fstat_rc = _fstat(fd, &sb);
#endif

#else
    // works on most compilers
    int fd = fileno(fp);
    struct stat sb;
    memset(&sb,0,sizeof(sb));
    int fstat_rc = fstat(fd, &sb);
#endif


    if (0 == fstat_rc)
    {
      if (file_size)
        *file_size = (MYON__UINT64)sb.st_size;
      if (file_metadata_last_modified_time)
        *file_metadata_last_modified_time = (MYON__UINT64)sb.st_ctime;
      if (file_contents_last_modified_time)
        *file_contents_last_modified_time = (MYON__UINT64)sb.st_mtime;
      rc = true;
    }
  }

  return rc;
}

//////////////////////////////////////////////////////////////////////////////
//
// MYON_FileIterator implementation
//

class MYON_DirectoryIteratorImpl
{
private:
  friend class MYON_FileIterator;

  MYON_DirectoryIteratorImpl();
  ~MYON_DirectoryIteratorImpl();
  MYON_DirectoryIteratorImpl(const MYON_DirectoryIteratorImpl&) = delete;
  MYON_DirectoryIteratorImpl& operator=(const MYON_DirectoryIteratorImpl&) = delete;

  const wchar_t* CurrentFileNameAsPointer() const;

#if defined(MYON_COMPILER_MSC)
  // Used by Windows ::Find
  //MYON__UINT32 m_file_attributes_mask = 0;
  HANDLE m_h = 0;
  WIN32_FIND_DATA m_fd;
#else
  // Apple and gcc
  MYON_wString m_ws_file_name_filter;
  MYON_String m_utf8_file_name_filter;
  DIR* m_dir = nullptr;
  struct dirent m_dirent;
  char m_dirent_name_buffer[NAME_MAX+1]; // < this field provide storage for m_dirent.d_name[]

  // information about the current file
  wchar_t m_current_name[1024];
  MYON__UINT64 m_current_file_attributes = 0; // 1 = regular file, 2 = directory
  MYON__UINT64 m_current_file_size = 0;
  MYON__UINT64 m_current_content_last_modified_time = 0;
#endif
};

MYON_DirectoryIteratorImpl::MYON_DirectoryIteratorImpl()
{
#if defined(MYON_COMPILER_MSC)
  memset(&m_fd,0,sizeof(m_fd));
#else
  memset(&m_dirent,0,sizeof(m_dirent));
  memset(&m_dirent_name_buffer[0],0,sizeof(m_dirent_name_buffer));
  memset(&m_current_name[0],0,sizeof(m_current_name));
#endif
}

MYON_DirectoryIteratorImpl::~MYON_DirectoryIteratorImpl()
{
#if defined(MYON_COMPILER_MSC)
  if ( 0 != m_h )
    ::FindClose(m_h);
#else
  if ( 0 != m_dir )
    closedir(m_dir);
#endif
}

void MYON_FileIterator::Reset()
{
  m_state = 0;
  m_directory = MYON_wString::EmptyString;
  m_item_name_filter = MYON_wString::EmptyString;
  m_item_name = MYON_wString::EmptyString;
  m_full_path_name = MYON_wString::EmptyString;
  m_count = 0;
  if (nullptr != m_impl)
  {
    delete m_impl;
    m_impl = nullptr;
  }
}


MYON__UINT64 MYON_FileIterator::CurrentItemCount() const
{
  return m_count;
}

#if defined(MYON_COMPILER_MSC)
static bool IsDotOrDotDotDir( const wchar_t* s )
#else
static bool IsDotOrDotDotDir( const char* s )
#endif
{
  bool rc = false;
  for (;;)
  {
    if ( 0 == s )
      break;
    if ( '.' != s[0] )
      break;
    if ( 0 != s[1] )
    {
      if ( '.' != s[1] )
        break;
      if ( 0 != s[2] )
        break;
    }
    rc = true; // s = "." or s = ".."
    break;
  }
  return rc;
}

bool MYON_FileIterator::Initialize( 
  const wchar_t* directory_name
  )
{
  const wchar_t* item_name_filter = nullptr;
  return Initialize(directory_name,item_name_filter);
}

bool MYON_FileIterator::Initialize( 
  const wchar_t* directory_name, 
  const wchar_t* item_name_filter
  )
{
  const MYON_wString local_item_name_filter(item_name_filter);
  item_name_filter = local_item_name_filter;

  MYON_wString local_directory_name(directory_name);
  {
    const wchar_t* dir_seps = L"/\\";
    local_directory_name.TrimRight(dir_seps);
    if ( local_directory_name.Length() <= 0 || local_directory_name.IsEmpty() )
      local_directory_name = directory_name;
  }

  Reset();
  m_directory = local_directory_name;
  m_item_name_filter = local_item_name_filter;
  if (m_directory.IsEmpty())
    return false;
  m_state = 1;
  return true;
}

bool MYON_FileIterator::Initialize( 
  const char* directory_name
  )
{
  const char* item_name_filter = nullptr;
  return Initialize(directory_name,item_name_filter);
}

bool MYON_FileIterator::Initialize(
  const char* directory_name,
  const char* item_name_filter
  )
{
  const MYON_wString local_directory_name(directory_name);
  const MYON_wString local_item_name_filter(item_name_filter);
  return Initialize(
    static_cast<const wchar_t*>(local_directory_name),
    static_cast<const wchar_t*>(local_item_name_filter)
    );  
}

bool MYON_FileIterator::FirstItem()
{
  const MYON_wString saved_directory(m_directory);
  const MYON_wString saved_item_name_filter(m_item_name_filter);
  if (m_state > 1)
  {
    Reset();
    m_directory = saved_directory;
    m_item_name_filter = saved_item_name_filter;
    if (saved_directory.IsEmpty())
      return false;
    m_state = 1;
  }

  if ( 1 != m_state  || nullptr != m_impl )
    return false;

  m_impl = new MYON_DirectoryIteratorImpl();
  m_state = 2;

  const wchar_t* item_name_filter = static_cast<const wchar_t*>(m_item_name_filter);
  if ( nullptr != item_name_filter && 0 == item_name_filter[0] )
    item_name_filter = nullptr;

#if defined(MYON_COMPILER_MSC)
  for (;;)
  {
    MYON_wString s(m_directory);

    if (0 == item_name_filter)
    {
      // A null file file_name_filter means iterate 
      // through all items in the directory.  To do
      // this using Windows' ::FindFirstFile, set the
      // filter to "*.*", even though some items will
      // not contain a "dot".
      item_name_filter = L"*.*";
    }

    if (0 != item_name_filter[0] && s.IsNotEmpty())
    {
      s += MYON_FileSystemPath::DirectorySeparator;
      s += item_name_filter;
    }

    m_impl->m_h = ::FindFirstFile(static_cast<const wchar_t*>(s), &m_impl->m_fd);
    if (0 == m_impl->m_h || INVALID_HANDLE_VALUE == m_impl->m_h || 0 == m_impl->m_fd.cFileName[0])
    {
      // Happens on "fake" directories like "My Music" and "My Videos"
      m_impl->m_h = 0;
      break;
    }

    if (IsDotOrDotDotDir(m_impl->m_fd.cFileName))
    {
      return NextItem();
    }

    m_count++;
    m_impl->m_fd.cFileName[(sizeof(m_impl->m_fd.cFileName) / sizeof(m_impl->m_fd.cFileName[0])) - 1] = 0;
    m_item_name = m_impl->m_fd.cFileName;
    m_full_path_name = MYON_wString::EmptyString;
    return true;
  }

#else
  // gcc code
  m_impl->m_utf8_file_name_filter = item_name_filter;
  const MYON_String utf8_str(m_directory); // convert wchar_t to utf8 string
  const char* s = utf8_str;
  m_impl->m_dir = (0 != s && 0 != s[0]) ? opendir(s) : 0;
  if ( 0 != m_impl->m_dir )
  {
    return NextItem();
  }
#endif

  Reset();
  m_directory = saved_directory;
  m_item_name_filter = saved_item_name_filter;
  m_state = 3;
  return false;
}

bool MYON_FileIterator::NextItem()
{
  m_item_name = MYON_wString::EmptyString;
  m_full_path_name = MYON_wString::EmptyString;
  if ( 1 == m_state )
    return FirstItem();
  if ( 2 != m_state )
    return false;

#if defined(MYON_COMPILER_MSC)
  for (;;)
  {
    if (0 == m_impl->m_h || INVALID_HANDLE_VALUE == m_impl->m_h || 0 == m_impl->m_fd.cFileName[0])
    {
      break;
    }

    for (;;)
    {
      if (!::FindNextFile(m_impl->m_h, &m_impl->m_fd) || 0 == m_impl->m_fd.cFileName[0])
        break;

      if (IsDotOrDotDotDir(m_impl->m_fd.cFileName))
        continue;

      m_count++;
      m_impl->m_fd.cFileName[(sizeof(m_impl->m_fd.cFileName) / sizeof(m_impl->m_fd.cFileName[0])) - 1] = 0;
      m_item_name = m_impl->m_fd.cFileName;
      m_full_path_name = MYON_wString::EmptyString;
      return true;
    }

    break;
  }
#else

  // gcc code
  MYON__UINT64 current_file_attributes = 0;
  wchar_t current_name[ sizeof(m_impl->m_current_name)/sizeof(m_impl->m_current_name[0]) ];
  for(;;)
  {
    current_file_attributes = 0;
    struct dirent* dp = 0;
    int readdir_errno = readdir_r(m_impl->m_dir, &m_impl->m_dirent, &dp);
    if ( 0 !=  readdir_errno )
      break;
    if ( 0 == dp )
      break;
    if ( 0 == m_impl->m_dirent.d_name[0] )
      break;

    if ( IsDotOrDotDotDir(m_impl->m_dirent.d_name) )
      continue;

    memset( current_name, 0, sizeof(current_name) );
    MYON_ConvertUTF8ToWideChar(
      false, // no BOM in input file name as utf8 string
      &m_impl->m_dirent.d_name[0],
      -1, // null terminated utf8 string
      &current_name[0], ((int)(sizeof(current_name)/sizeof(current_name[0]))) - 1, // output wchar_t string
      0, // null output error status
      (4|8|16), // mask common conversion errors
      0, // error_code_point = null terminator inserted at point of conversion error
      0  // null ouput end-of-string pointer
      );
    // TODO
    //   Test m_dirent.d_name to make sure it passes m_ws/utf8_file_name_filter

    MYON_wString wpath = m_directory;
    wpath += '/';
    wpath += current_name;

    // get a utf8 version of the full path to pass to stat
    const MYON_String utf8buffer(wpath);
    const char* utf8path = utf8buffer;
    if ( 0 == utf8path )
      continue;

    struct stat buf;
    memset(&buf,0,sizeof(buf));
    int stat_errno = stat( utf8path, &buf);
    if ( 0 != stat_errno )
      continue;

    if ( S_ISDIR(buf.st_mode) )
    {
      current_file_attributes = 2;
    }
    else if ( S_ISREG(buf.st_mode) )
    {
      // Only *.ext filters work at this time for non-windows
      const wchar_t* file_name_filter = m_impl->m_ws_file_name_filter;
      if (   0 != file_name_filter
            && '*' == file_name_filter[0] 
            && '.' == file_name_filter[1]
            && 0 != file_name_filter[2]
            && '*' != file_name_filter[2] )
      {
        // assume this is a *.extension filter
        const wchar_t* current_name_ext = 0;
        on_wsplitpath(current_name,0,0,0,&current_name_ext);
        if (   0 == current_name_ext 
            || 0 != wcscmp(file_name_filter+1,current_name_ext) 
           )
        {
          // current_name does pass match file_name_filter
          continue;
        }
      }
      current_file_attributes = 1;
    }
    else
      continue;

    // save current item information
    memcpy( m_impl->m_current_name, current_name, sizeof(m_impl->m_current_name) );
    m_impl->m_current_file_attributes = current_file_attributes;
    m_impl->m_current_file_size = buf.st_size;
    m_impl->m_current_content_last_modified_time = buf.st_mtime;

    m_item_name = m_impl->m_current_name;
    m_full_path_name = MYON_wString::EmptyString;
    return true;
  }

#endif
  const MYON__UINT64 saved_count = m_count;
  Reset();
  m_count = saved_count;
  m_state = 3;
  return false;
}

const wchar_t* MYON_DirectoryIteratorImpl::CurrentFileNameAsPointer() const
{
#if defined(MYON_COMPILER_MSC)
  return ( 0 != m_h && 0 != m_fd.cFileName[0] ) ? m_fd.cFileName : nullptr;
#else
  return ( 0 != m_current_name[0] ) ? m_current_name : nullptr;
#endif
}

const MYON_wString MYON_FileIterator::CurrentItemName() const
{
  return m_item_name;
}

MYON__UINT64 MYON_FileIterator::CurrentItemSize() const
{
  MYON__UINT64 file_size = 0;

  if (nullptr != m_impl)
  {

#if defined(MYON_COMPILER_MSC)
    if (0 != m_impl->CurrentFileNameAsPointer())
    {
      file_size = m_impl->m_fd.nFileSizeHigh;
      file_size *= ((MYON__UINT64)0xFFFFFFFF);
      file_size += m_impl->m_fd.nFileSizeLow;
    }
#else
    file_size = m_impl->m_current_file_size;
#endif
  }

  return file_size;
}

bool MYON_FileIterator::CurrentItemIsDirectory() const
{
  bool rc = false;
  if (nullptr != m_impl)
  {
    const wchar_t* current_file_name = m_impl->CurrentFileNameAsPointer();
    if (0 != current_file_name && 0 != current_file_name[0])
    {
#if defined(MYON_COMPILER_MSC)
      if (0 != (FILE_ATTRIBUTE_DIRECTORY & m_impl->m_fd.dwFileAttributes))
      {
        rc = true;
      }
#else
      if ( 2 == m_impl->m_current_file_attributes)
      {
        rc = true;
      }
#endif
    }
  }
  return rc;
}

bool MYON_FileIterator::CurrentItemIsFile() const
{
  bool rc = false;
  if (nullptr != m_impl)
  {
    const wchar_t* current_file_name = m_impl->CurrentFileNameAsPointer();
    if (0 != current_file_name && 0 != current_file_name[0])
    {
#if defined(MYON_COMPILER_MSC)
      if (0 == (FILE_ATTRIBUTE_DIRECTORY & m_impl->m_fd.dwFileAttributes))
      {
        rc = true;
      }
#else
      if ( 1 == m_impl->m_current_file_attributes)
      {
        rc = true;
      }
#endif
    }
  }
  return rc;
}
bool MYON_FileIterator::CurrentItemIsHidden() const
{
  bool rc = false;
  if (nullptr != m_impl)
  {
    const wchar_t* current_file_name = m_impl->CurrentFileNameAsPointer();
    if (0 != current_file_name && 0 != current_file_name[0])
    {
      if ('.' == current_file_name[0])
      {
        rc = true;
      }
#if defined(MYON_COMPILER_MSC)
      else if (0 != (FILE_ATTRIBUTE_HIDDEN & m_impl->m_fd.dwFileAttributes))
      {
        rc = true;
      }
#endif
    }
  }
  return rc;
}


const MYON_wString MYON_FileIterator::CurrentItemFullPathName() const
{
  if (m_full_path_name.IsEmpty() && m_item_name.IsNotEmpty())
  {
    if (m_directory.IsNotEmpty())
    {
      m_full_path_name = m_directory;
      m_full_path_name += MYON_FileSystemPath::DirectorySeparator;
      m_full_path_name += m_item_name;
    }
  }
  return m_full_path_name;
}

MYON__UINT64 MYON_SecondsSinceJanOne1970UTC()
{
#if defined(MYON_COMPILER_MSC)

  __time64_t t = _time64(nullptr);
  return (MYON__UINT64)t;  

#elif defined(MYON_COMPILER_CLANG) || defined(MYON_COMPILER_GNU)

  //__time64_t t = _time64(nullptr);
  time_t t = time(nullptr);
  return (MYON__UINT64)t;

#else

  __time64_t t = _time64(nullptr);
  return (MYON__UINT64)t;  

#endif
}

const MYON_wString SecondsSinceJanOne1970UTCToString(
  MYON__UINT64 seconds_since_epoch
  )
{
  int year = 0;
  int month = 0;
  int mday = 0;
  int hour = 0;
  int min = 0;
  int sec = 0;

#if defined(MYON_COMPILER_MSC)

  const time_t t = (time_t)seconds_since_epoch;
  const struct tm* ptr = _gmtime64( &t );
  if (nullptr != ptr)
  {
    const struct tm uct = *ptr;
    year = uct.tm_year;
    month = uct.tm_mon;
    mday = uct.tm_mday;
    hour = uct.tm_hour;
    min = uct.tm_min;
    sec = uct.tm_sec;
  }

#elif defined(MYON_COMPILER_CLANG) || defined(MYON_COMPILER_GNU)

  const time_t t = (time_t)seconds_since_epoch;
  const struct tm* ptr = gmtime( &t );
  if (nullptr != ptr)
  {
    const struct tm uct = *ptr;
    year = uct.tm_year;
    month = uct.tm_mon;
    mday = uct.tm_mday;
    hour = uct.tm_hour;
    min = uct.tm_min;
    sec = uct.tm_sec;
  }

#else

  const time_t t = (time_t)seconds_since_epoch;
  const struct tm* ptr = _gmtime64( &t );
  if (nullptr != ptr)
  {
    const struct tm uct = *ptr;
    year = uct.tm_year;
    month = uct.tm_mon;
    mday = uct.tm_mday;
    hour = uct.tm_hour;
    min = uct.tm_min;
    sec = uct.tm_sec;
  }

#endif

  if (
    year >= 1970
    && month >= 1 && month <= 12
    && mday >= 1 && mday <= 31
    && hour >= 0 && hour <= 24
    && min >= 0 && min <= 60
    && sec >= 0 && sec <= 60
    )
  {
    MYON_wString sUTC;
    // yyyy-mm-dd hh:mm:ss
    sUTC.Format(L"%04d-%02d-%02d %02d:%02d:%02d UTC",year,month,mday,hour,min,sec);
    return sUTC;
  }

  return MYON_wString::EmptyString;
}

#if defined(MYON_COMPILER_MSC)
static MYON__UINT64 SecondsSinceJanOne1970( FILETIME ft )
{
  // The FILETIME is in 100-nanosecond intervals since January 1, 1601 UCT.
  //
  // Between midnight January 1, 1601 and midnight January 1, 1970 there 
  // were 134774 days = 11644473600 seconds. Each second has 10^7 intervals
  // that are one hundred nanoseconds long.  So, if N = number of one hundred
  // nanosecond intervals since midnight January 1, 1601, then
  // (N / 10000000) - 11644473600 = number of seconds since midnight
  // January 1, 1970.
  //
  // January 1, 1601 was the start of a Gregorian calendary 400 year cycle
  // and "the internet" sometimes cites that as the reason that date is 
  // the "beginning of time" for Windows' FILETIME values.  This convention
  // would slightly simplify the formulae used to account for leap years, 
  // so it is plausable this might might even be true.

  MYON__UINT64 ft_since_jan_1_1601 = ft.dwHighDateTime;
  ft_since_jan_1_1601 *= 0xFFFFFFFF;
  ft_since_jan_1_1601 += ft.dwLowDateTime;

  MYON__UINT64 hundrednanoseconds_per_second = 10000000;

  MYON__UINT64 seconds_since_jan_1_1601 = ft_since_jan_1_1601 / hundrednanoseconds_per_second;
                                                   
  MYON__UINT64 seconds_since_jan_1_1970 = seconds_since_jan_1_1601 - 11644473600;

  return seconds_since_jan_1_1970;
}
#endif

MYON__UINT64 MYON_FileIterator::CurrentItemLastModifiedTime() const
{
  if ( nullptr == m_impl)
    return 0;
#if defined(MYON_COMPILER_MSC)
  return SecondsSinceJanOne1970(m_impl->m_fd.ftLastWriteTime);
#else
  return m_impl->m_current_content_last_modified_time;
#endif
}

MYON_FileIterator::~MYON_FileIterator()
{
  Reset();
}


MYON_ContentHash MYON_ContentHash::Create(
  MYON_SHA1_Hash sha1_name_hash,
  MYON__UINT64 byte_count,
  MYON_SHA1_Hash sha1_content_hash,
  MYON__UINT64 hash_time,
  MYON__UINT64 content_last_modified_time
  )
{
  MYON_ContentHash hash;
  
  if ( 0 == hash_time )
    hash_time = MYON_SecondsSinceJanOne1970UTC();
  hash.m_byte_count = (byte_count > 0) ? byte_count : 0;
  hash.m_hash_time = hash_time;
  
  hash.m_content_time 
    = (content_last_modified_time <= hash_time) 
    ? content_last_modified_time
    : 0;
  
  hash.m_sha1_name_hash = sha1_name_hash;

  hash.m_sha1_content_hash 
    = (hash.m_byte_count > 0)
    ? sha1_content_hash
    : MYON_SHA1_Hash::EmptyContentHash
    ;

  return hash;
}

MYON_ContentHash MYON_ContentHash::CreateFromBuffer( 
  MYON_SHA1_Hash sha1_name_hash,
  const void* buffer,
  size_t byte_count
  )
{
  MYON__UINT64 hash_time = MYON_SecondsSinceJanOne1970UTC();
  MYON__UINT64 hash_byte_count = (nullptr != buffer && byte_count >0) ? ((MYON__UINT64)byte_count) : 0;
  MYON__UINT64 content_last_modifed_time = 0;
  MYON_SHA1_Hash sha1_content_hash = MYON_SHA1_Hash::BufferContentHash(buffer,(size_t)hash_byte_count);
  return MYON_ContentHash::Create(sha1_name_hash,hash_byte_count,sha1_content_hash,hash_time,content_last_modifed_time);
}

MYON_ContentHash MYON_ContentHash::CreateFromFile( 
  MYON_SHA1_Hash sha1_file_name_hash,
  FILE* fp
  )
{
  MYON__UINT64 hash_time = MYON_SecondsSinceJanOne1970UTC();
  MYON__UINT64 file_byte_count = 0;
  MYON__UINT64 file_metadata_last_modified_time = 0;
  MYON__UINT64 file_contents_last_modified_time = 0;
  if ( false == MYON_FileStream::GetFileInformation(fp,&file_byte_count,&file_metadata_last_modified_time,&file_contents_last_modified_time) )
    return MYON_ContentHash::Create(sha1_file_name_hash,0,MYON_SHA1_Hash::EmptyContentHash,hash_time,0);

  MYON__UINT64 hash_byte_count = 0;
  MYON_SHA1_Hash sha1_hash = MYON_SHA1_Hash::FileContentHash(fp,hash_byte_count);
  return MYON_ContentHash::Create(sha1_file_name_hash,hash_byte_count,sha1_hash,hash_time,file_contents_last_modified_time);
}


MYON_ContentHash MYON_ContentHash::CreateFromFile( 
  const wchar_t* filename
  )
{
  MYON_SHA1_Hash sha1_file_name_hash = (nullptr == filename) ? MYON_SHA1_Hash::ZeroDigest : MYON_SHA1_Hash::FileSystemPathHash(filename);
  FILE* fp = MYON_FileStream::Open(filename, L"rb");
  MYON_ContentHash hash = MYON_ContentHash::CreateFromFile(sha1_file_name_hash,fp);
  MYON_FileStream::Close(fp);
  return hash;
}

MYON_ContentHash MYON_ContentHash::CreateFromFile( 
  const char* filename
  )
{
  MYON_SHA1_Hash sha1_file_name_hash = (nullptr == filename) ? MYON_SHA1_Hash::ZeroDigest : MYON_SHA1_Hash::FileSystemPathHash(filename);
  FILE* fp = MYON_FileStream::Open(filename, "rb");
  MYON_ContentHash hash = MYON_ContentHash::CreateFromFile(sha1_file_name_hash,fp);
  MYON_FileStream::Close(fp);
  return hash;
}


bool MYON_ContentHash::EqualContent(
  const MYON_ContentHash& a,
  const MYON_ContentHash& b
  )
{
  // Do not compare times
  return (a.m_byte_count == b.m_byte_count && a.m_sha1_content_hash == b.m_sha1_content_hash);
}


bool MYON_ContentHash::DifferentContent(
  const MYON_ContentHash& a,
  const MYON_ContentHash& b
  )
{
  // Do not compare times
  return (a.m_byte_count != b.m_byte_count || a.m_sha1_content_hash != b.m_sha1_content_hash);
}

int MYON_ContentHash::CompareContent(
  const MYON_ContentHash& a,
  const MYON_ContentHash& b
  )
{
  if (a.m_byte_count < b.m_byte_count)
    return -1;
  if (a.m_byte_count > b.m_byte_count)
    return 1;
  if (a.m_byte_count < b.m_byte_count)
    return -1;
  if (a.m_byte_count > b.m_byte_count)
    return 1;
  return MYON_SHA1_Hash::Compare(a.m_sha1_content_hash, b.m_sha1_content_hash);
}

int MYON_ContentHash::Compare(
  const MYON_ContentHash& a,
  const MYON_ContentHash& b
  )
{
  const int rc = MYON_ContentHash::CompareContent(a, b);
  if (0 != rc)
    return rc;

  if (a.m_hash_time < b.m_hash_time)
    return -1;
  if (a.m_hash_time > b.m_hash_time)
    return 1;

  if (a.m_content_time < b.m_content_time)
    return -1;
  if (a.m_content_time > b.m_content_time)
    return 1;

  return MYON_SHA1_Hash::Compare(a.m_sha1_name_hash, b.m_sha1_name_hash);
}

bool MYON_ContentHash::EqualFileNameSizeAndTime(
  const wchar_t* filename
  ) const
{
  if (IsNotSet())
    return false;
  if ( m_byte_count <= 0 || m_hash_time <= 0 || m_content_time < m_hash_time )
    return false; // content time is not reliable.
  if (nullptr == filename || 0 == filename[0])
    return false;
  const MYON_SHA1_Hash sha1_name_hash = MYON_SHA1_Hash::StringHash(filename);
  if ( sha1_name_hash != m_sha1_name_hash )
    return false;
  MYON__UINT64 file_byte_count = 0;
  MYON__UINT64 file_metadata_last_modified_time = 0;
  MYON__UINT64 file_contents_last_modified_time = 0;
  if ( false == MYON_FileStream::GetFileInformation(filename,&file_byte_count,&file_metadata_last_modified_time,&file_contents_last_modified_time) )
    return false;
  if ( file_contents_last_modified_time <= 0 )
    return false; // content time is not reliable
  return (file_byte_count == m_byte_count && file_contents_last_modified_time == m_content_time);
}

bool MYON_ContentHash::IsSet() const
{
  if ( 0 == m_hash_time )
    return false;
  return 
    (0 == m_byte_count)
    ? (MYON_SHA1_Hash::EmptyContentHash == m_sha1_content_hash)
    : (MYON_SHA1_Hash::EmptyContentHash != m_sha1_content_hash);
}


bool MYON_ContentHash::IsNotSet() const
{
  return (false == IsSet());
}

MYON__UINT64 MYON_ContentHash::ByteCount() const
{
  return m_byte_count;
}


MYON__UINT64 MYON_ContentHash::HashCalculationTime() const
{
  return m_hash_time;
}


MYON__UINT64 MYON_ContentHash::ContentLastModifiedTime() const
{
  return m_content_time;
}


MYON_SHA1_Hash MYON_ContentHash::ContentHash() const
{
  return m_sha1_content_hash;
}


MYON_SHA1_Hash MYON_ContentHash::NameHash() const
{
  return m_sha1_name_hash;
}
  

bool MYON_ContentHash::IsSameBufferContent(
  const void* buffer,
  size_t byte_count
  ) const
{
  return MYON_ContentHash::EqualContent(*this, MYON_ContentHash::CreateFromBuffer(MYON_SHA1_Hash::ZeroDigest,buffer,byte_count));
}


bool MYON_ContentHash::IsSameFileContent( 
  FILE* fp
  ) const
{
  return MYON_ContentHash::EqualContent(*this, MYON_ContentHash::CreateFromFile(MYON_SHA1_Hash::ZeroDigest,fp));
}

bool MYON_ContentHash::IsSameFileContent( 
  const wchar_t* filename
  ) const
{
  return MYON_ContentHash::EqualContent(*this, MYON_ContentHash::CreateFromFile(filename));
}

bool MYON_ContentHash::IsSameFileContent( 
  const char* filename
  ) const
{
  return MYON_ContentHash::EqualContent(*this, MYON_ContentHash::CreateFromFile(filename));
}

MYON_ContentHash::CompareResult MYON_ContentHash::CompareResultFromUnsigned(
  unsigned int compare_result_as_unsigned
  )
{
  switch (compare_result_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ContentHash::CompareResult::Unset);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ContentHash::CompareResult::EqualContent);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ContentHash::CompareResult::DifferentContent);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ContentHash::CompareResult::DifferentContentFileIsOlder);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ContentHash::CompareResult::ContentDifferentFileIsNewer);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ContentHash::CompareResult::FileDoesNotExist);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ContentHash::CompareResult::FileSystemFailure);
  }

  MYON_ERROR("Invalid compare_result_as_unsigned parameter.");
  return MYON_ContentHash::CompareResult::Unset;
}

MYON_ContentHash::CompareResult MYON_ContentHash::Compare(
  const wchar_t* file_name,
  bool bFastTest
  ) const
{
  if ( false ==  MYON_FileSystem::IsFile(file_name) )
    return MYON_ContentHash::CompareResult::FileDoesNotExist;

  if (IsNotSet())
    return MYON_ContentHash::CompareResult::DifferentContent;

  MYON_ContentHash file_content_hash = MYON_ContentHash::Unset;
  MYON__UINT64 file_metadata_last_modified_time = 0;
  if ( false == MYON_FileStream::GetFileInformation(file_name,&file_content_hash.m_byte_count,&file_metadata_last_modified_time,&file_content_hash.m_content_time) )
    return MYON_ContentHash::CompareResult::FileSystemFailure;
  if (0 == file_content_hash.m_byte_count && 0 == file_metadata_last_modified_time && 0 == file_content_hash.m_content_time)
  {
    return MYON_ContentHash::CompareResult::FileSystemFailure;
  }

  const MYON__UINT64 current_time = MYON_SecondsSinceJanOne1970UTC();
  bool bValidContentHashTime = (m_content_time > 0 && m_hash_time >= m_content_time && current_time >= m_hash_time);
  bool bValidFileTime = (file_content_hash.m_content_time > 0 && current_time >= file_content_hash.m_content_time);
  if (file_content_hash.m_byte_count != m_byte_count)
  {
    if (bValidContentHashTime &&  bValidFileTime)
    {
      // assume time values are accurate.
      if ( file_content_hash.m_content_time < m_content_time )
        return MYON_ContentHash::CompareResult::DifferentContentFileIsOlder;
      if ( file_content_hash.m_content_time > m_content_time )
        return MYON_ContentHash::CompareResult::ContentDifferentFileIsNewer;
    }
    return MYON_ContentHash::CompareResult::DifferentContent;
  }

  file_content_hash.m_sha1_name_hash = MYON_SHA1_Hash::StringHash(file_name);
  file_content_hash.m_sha1_content_hash = m_sha1_content_hash;

  if (bValidContentHashTime && bValidFileTime
    && m_content_time == file_content_hash.m_content_time
    && m_byte_count == file_content_hash.m_byte_count
    && m_sha1_name_hash == file_content_hash.m_sha1_name_hash
    )
  {
    if (bFastTest)
      return MYON_ContentHash::CompareResult::EqualContent;
  }

  // Have to calculate SHA1 content hash
  file_content_hash = MYON_ContentHash::CreateFromFile(file_name);
  return MYON_ContentHash::Compare(file_content_hash);
}

MYON_ContentHash::CompareResult MYON_ContentHash::Compare(
  MYON_ContentHash file_content_hash
  ) const
{
  if (file_content_hash.IsNotSet())
  {
    return ( m_sha1_name_hash == MYON_SHA1_Hash::EmptyContentHash)
      ? MYON_ContentHash::CompareResult::FileDoesNotExist
      : MYON_ContentHash::CompareResult::FileSystemFailure;
  }
  if ( IsNotSet() )
    return MYON_ContentHash::CompareResult::DifferentContent;
  if ( m_byte_count == file_content_hash.m_byte_count && m_sha1_content_hash == file_content_hash.m_sha1_content_hash )
    return MYON_ContentHash::CompareResult::EqualContent;
  const MYON__UINT64 current_time = MYON_SecondsSinceJanOne1970UTC();
  bool bValidTimes 
    =  m_content_time > 0 
    && m_hash_time >= m_content_time 
    && current_time >= m_hash_time
    && file_content_hash.m_content_time > 0 
    && file_content_hash.m_hash_time >= file_content_hash.m_content_time 
    && current_time >= file_content_hash.m_hash_time
    ;
  if (bValidTimes)
  {
    if ( file_content_hash.m_content_time < m_content_time )
      return MYON_ContentHash::CompareResult::DifferentContentFileIsOlder;
    if ( file_content_hash.m_content_time > m_content_time )
      return MYON_ContentHash::CompareResult::ContentDifferentFileIsNewer;
  }
  return MYON_ContentHash::CompareResult::DifferentContent;
}

bool MYON_ContentHash::Read(
  class MYON_BinaryArchive& archive
  )
{
  *this = MYON_ContentHash::Unset;
  bool rc = false;
  int major_version = 0;
  int minor_version = 0;
  if (!archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version))
    return rc;
  for (;;)
  {
    if ( 1 != major_version )
      break;
    if (!archive.ReadBigInt(&m_byte_count))
      break;
    if (!archive.ReadBigInt(&m_hash_time))
      break;
    if (!archive.ReadBigInt(&m_content_time))
      break;
    if (!m_sha1_name_hash.Read(archive))
      break;
    if (!m_sha1_content_hash.Read(archive))
      break;
    rc = true;
    break;
  }
  if (!archive.EndRead3dmChunk())
    rc = false;
  return rc;
}


bool MYON_ContentHash::Write(
  class MYON_BinaryArchive& archive
  ) const
{
  if (!archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0))
    return false;
  bool rc = false;
  for (;;)
  {
    if (!archive.WriteBigInt(m_byte_count))
      break;
    if (!archive.WriteBigInt(m_hash_time))
      break;
    if (!archive.WriteBigInt(m_content_time))
      break;
    if (!m_sha1_name_hash.Write(archive))
      break;
    if (!m_sha1_content_hash.Write(archive))
      break;
    rc = true;
    break;
  }
  if (!archive.EndWrite3dmChunk())
    rc = false;
  return rc;
}

void MYON_ContentHash::Dump(
  class MYON_TextLog& text_log
  ) const
{
  if (IsSet())
  {
    text_log.Print(L"MYON_ContentHash:\n");
    text_log.PushIndent();

    text_log.Print(L"Name hash: ");
    m_sha1_name_hash.Dump(text_log);
    text_log.Print(L"Content byte count = %llu\n",m_byte_count);
    text_log.Print(L"Content hash: ");
    m_sha1_content_hash.Dump(text_log);

    const MYON_wString content_time
      = ( m_content_time <= 0 )
      ? L"unknown"
      : SecondsSinceJanOne1970UTCToString(m_content_time);
    text_log.Print(L"Content last modified time = %ls\n",static_cast<const wchar_t*>(content_time));

    const MYON_wString hash_time
      = ( m_hash_time <= 0 )
      ? L"unknown"
      : SecondsSinceJanOne1970UTCToString(m_hash_time);
    text_log.Print(L"Content hash calculated time = %ls\n",static_cast<const wchar_t*>(content_time));

    text_log.PopIndent();
  }
  else
  {
    text_log.Print(L"MYON_ContentHash::Unset\n");
  }
}

int MYON_FileReference::Compare(
  const MYON_FileReference& a,
  const MYON_FileReference& b
  )
{
  int rc;
  for (;;)
  {
    // must compare every byte of every field.
    // If you don't like that, add another clearly named compare function.
    rc = MYON_wString::ComparePath(a.m_full_path,b.m_full_path);
    if (0 != rc)
      break;
    rc = MYON_wString::CompareOrdinal(a.m_full_path,b.m_full_path,false);
    if (0 != rc)
      break;
    rc = MYON_wString::ComparePath(a.m_relative_path,b.m_relative_path);
    if (0 != rc)
      break;
    rc = MYON_wString::CompareOrdinal(a.m_relative_path,b.m_relative_path,false);
    if (0 != rc)
      break;
    rc = MYON_ContentHash::CompareContent(a.m_content_hash, b.m_content_hash);
    if (0 != rc)
      break;

    break;
  }
  return rc;
}


MYON_FileReference::Status MYON_FileReference::StatusFromUnsigned(
  unsigned int full_path_status_as_unsigned
  )
{
  switch (full_path_status_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_FileReference::Status::Unknown);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_FileReference::Status::FullPathValid);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_FileReference::Status::FileNotFound);
  }
  MYON_ERROR("Invalid parameter.");
  return MYON_FileReference::Status::Unknown;
}


MYON_FileReference::MYON_FileReference(
  const wchar_t* full_path,
  const wchar_t* relative_path,
  MYON_ContentHash content_hash,
  MYON_FileReference::Status full_path_status
  )
  : m_full_path(full_path)
  , m_relative_path(relative_path)
  , m_content_hash(content_hash)
  , m_full_path_status(full_path_status)
{}

MYON_FileReference MYON_FileReference::CreateFromFullPath(
  const wchar_t* full_path,
  bool bSetContentHash,
  bool bSetFullPathStatus
  )
{
  MYON_wString local_full_path(full_path);
  local_full_path.TrimLeftAndRight();
  if (local_full_path.IsEmpty())
    return MYON_FileReference::Unset;
  full_path = local_full_path;
  MYON_FileReference::Status full_path_status = MYON_FileReference::Unset.m_full_path_status;
  MYON_ContentHash content_hash = MYON_FileReference::Unset.m_content_hash;
  const bool bFileExists =
    ( bSetFullPathStatus || bSetContentHash )
    ? MYON_FileSystem::IsFile(full_path)
    : false;
  if ( bSetFullPathStatus && bFileExists )
    full_path_status = MYON_FileReference::Status::FullPathValid;
  if ( bSetContentHash && bFileExists )
    content_hash = MYON_ContentHash::CreateFromFile(full_path);

  const wchar_t* relative_path = nullptr;
  const wchar_t* v = nullptr;
  const wchar_t* d = nullptr;
  const wchar_t* f = nullptr;
  const wchar_t* e = nullptr;
  on_wsplitpath(full_path,&v,&d,&f,&e);
  if (nullptr != d && nullptr != f && d < f && '.' == d[0])
  {
    relative_path = full_path;
    full_path = nullptr;
  }

  return MYON_FileReference(
    full_path,
    relative_path,
    content_hash,
    full_path_status
    );
}


MYON_FileReference::FindFilePreference MYON_FileReference::FindFile(
  const wchar_t* base_path,
  bool bBasePathIncludesFileName,
  MYON_wString& found_file_full_path
  ) const
{
  const MYON_FileReference::FindFilePreference* file_preference = nullptr;
  const unsigned int file_preference_count = 0;
  return Internal_FindFile(
    base_path,
    bBasePathIncludesFileName,
    file_preference,
    file_preference_count,
    found_file_full_path,
    nullptr
    );
}

MYON_FileReference::FindFilePreference MYON_FileReference::FindFile(
  const wchar_t* base_path,
  bool bBasePathIncludesFileName,
  MYON_FileReference::FindFilePreference first_choice,
  MYON_FileReference::FindFilePreference second_choice,
  MYON_FileReference::FindFilePreference third_choice,
  MYON_FileReference::FindFilePreference forth_choice,
  MYON_FileReference::FindFilePreference fifth_choice,
  MYON_wString& found_file_full_path
  ) const
{
  const MYON_FileReference::FindFilePreference file_preference[] = { first_choice, second_choice, third_choice, forth_choice, fifth_choice };
  const unsigned int file_preference_count = (unsigned int)(sizeof(file_preference)/sizeof(file_preference[0]));
  return Internal_FindFile(
    base_path,
    bBasePathIncludesFileName,
    file_preference,
    file_preference_count,
    found_file_full_path,
    nullptr
    );
}

MYON_FileReference::FindFilePreference MYON_FileReference::FindFileAndUpdateReference(
  const wchar_t* base_path,
  bool bBasePathIncludesFileName,
  bool bUpdateContentHash
  )
{
  MYON_wString found_file_full_path;
  return FindFileAndUpdateReference(base_path,bBasePathIncludesFileName,bUpdateContentHash,found_file_full_path);
}

MYON_FileReference::FindFilePreference MYON_FileReference::FindFileAndUpdateReference(
  const wchar_t* base_path,
  bool bBasePathIncludesFileName,
  bool bUpdateContentHash,
  MYON_wString& found_file_full_path
  )
{
  return FindFileAndUpdateReference(
    base_path,
    bBasePathIncludesFileName,
    MYON_FileReference::FindFilePreference::None,
    MYON_FileReference::FindFilePreference::None,
    MYON_FileReference::FindFilePreference::None,
    MYON_FileReference::FindFilePreference::None,
    MYON_FileReference::FindFilePreference::None,
    bUpdateContentHash,
    found_file_full_path
    );
}

MYON_FileReference::FindFilePreference MYON_FileReference::FindFileAndUpdateReference(
  const wchar_t* base_path,
  bool bBasePathIncludesFileName,
  MYON_FileReference::FindFilePreference first_choice,
  MYON_FileReference::FindFilePreference second_choice,
  MYON_FileReference::FindFilePreference third_choice,
  MYON_FileReference::FindFilePreference forth_choice,
  MYON_FileReference::FindFilePreference fifth_choice,
  bool bUpdateContentHash,
  MYON_wString& found_file_full_path
  )
{
  const MYON_FileReference::FindFilePreference file_preference[] = { first_choice, second_choice, third_choice, forth_choice, fifth_choice };
  const unsigned int file_preference_count = (unsigned int)(sizeof(file_preference)/sizeof(file_preference[0]));
  MYON_ContentHash found_file_content_hash = MYON_ContentHash::Unset;
  MYON_FileReference::FindFilePreference rc = Internal_FindFile(
    base_path,
    bBasePathIncludesFileName,
    file_preference,
    file_preference_count,
    found_file_full_path,
    &found_file_content_hash
    );
  if (rc != MYON_FileReference::FindFilePreference::None && found_file_full_path.IsNotEmpty())
  {
    m_full_path = found_file_full_path;
    m_relative_path = MYON_wString::EmptyString;
    m_full_path_hash = MYON_SHA1_Hash::EmptyContentHash;
    m_embedded_file_id = MYON_nil_uuid;
    if ( bUpdateContentHash && found_file_content_hash.IsNotSet() )
      found_file_content_hash = MYON_ContentHash::CreateFromFile(m_full_path);
    if ( found_file_content_hash.IsSet() )
      m_content_hash = found_file_content_hash;
  }
  return rc;
}

static MYON_FileReference::FindFilePreference Internal_FindFileResult(
  const MYON_wString& file_name_result,
  const MYON_ContentHash& content_hash_result,
  MYON_FileReference::FindFilePreference rc,
  MYON_wString& found_file_full_path,
  MYON_ContentHash* found_file_content_hash
  )
{
  found_file_full_path = file_name_result;
  if (nullptr != found_file_content_hash)
    *found_file_content_hash = content_hash_result;
  return rc;
}

MYON_FileReference::FindFilePreference MYON_FileReference::Internal_FindFile(
  const wchar_t* base_path,
  bool bBasePathIncludesFileName,
  const MYON_FileReference::FindFilePreference* file_preference,
  unsigned int file_preference_count,
  MYON_wString& found_file_full_path,
  MYON_ContentHash* found_file_content_hash
  ) const
{
  for (;;)
  {
    if (m_full_path.IsEmpty())
      break;

    const wchar_t* filename = nullptr;
    on_wsplitpath(static_cast<const wchar_t*>(m_full_path), nullptr, nullptr, &filename, nullptr);
    if (nullptr == filename || 0 == filename[0])
      break;

    // Clean up base_path
    MYON_wString local_base_path(base_path);
    base_path = nullptr;
    local_base_path.TrimLeftAndRight();

    if ( local_base_path.IsNotEmpty() )
    {
      // When the caller is confused and local_base_path identifies an existing file system element,
      // the set the bBasePathIncludesFileName parameter correctly.
      if (bBasePathIncludesFileName)
      {
        if (MYON_FileSystem::IsDirectory(base_path))
          bBasePathIncludesFileName = false;
      }
      else
      {
        if (MYON_FileSystem::IsFile(base_path))
          bBasePathIncludesFileName = true;
      }
    }

    if (local_base_path.IsNotEmpty() && bBasePathIncludesFileName)
    {
      bBasePathIncludesFileName = false;
      const wchar_t* start = static_cast<const wchar_t*>(local_base_path);
      const wchar_t* end_mark = nullptr;
      on_wsplitpath(start, nullptr, nullptr, &end_mark, nullptr);
      if (nullptr != start && nullptr != end_mark && start < end_mark)
      {
        local_base_path.SetLength(end_mark - start);
        if (false == MYON_FileSystemPath::IsDirectorySeparator(local_base_path[local_base_path.Length() - 1], true))
          local_base_path += MYON_FileSystemPath::DirectorySeparator;
        base_path = local_base_path;
      }
    }
    

    // Clean up file preferences and append defaults
    MYON_FileReference::FindFilePreference default_pref[] =
    {
      MYON_FileReference::FindFilePreference::RelativePath,
      MYON_FileReference::FindFilePreference::FullPath,
      MYON_FileReference::FindFilePreference::ContentMatch,
      MYON_FileReference::FindFilePreference::BasePath,
      MYON_FileReference::FindFilePreference::MostRecent
    };
    MYON_FileReference::FindFilePreference pref[10 + (sizeof(default_pref) / sizeof(default_pref[0]))];
    unsigned int pref_capacity = (unsigned int)(sizeof(pref) / sizeof(pref[0]));
    unsigned int pref_count = 0;
    for (unsigned int pass = 0; pass < 2; pass++)
    {
      const MYON_FileReference::FindFilePreference* pref_source = nullptr;
      unsigned int pref_source_count = 0;
      if (0 == pass)
      {
        pref_source = file_preference;
        pref_source_count = file_preference_count;
      }
      else if (1 == pass)
      {
        pref_source = default_pref;
        pref_source_count = (unsigned int)(sizeof(default_pref) / sizeof(default_pref[0]));
      }
      if (nullptr != pref_source)
        continue;
      for (unsigned int i = 0; i < pref_source_count && pref_count < pref_capacity; i++)
      {
        if (MYON_FileReference::FindFilePreference::None == pref_source[i])
          continue;
        unsigned int j;
        for (j = 0; j < i; j++)
        {
          if (pref[j] == pref_source[i])
            break;
        }
        if (j < i)
          continue; // don't add duplicate
        if (pref_count < i)
          pref[pref_count] = pref[i];
        pref_count++;
      }
    }


    MYON_wString candidate_file_name[3];  // full path, base path + relative path, base path + file_name
    MYON_FileReference::FindFilePreference candidate_file_pref[3] = { MYON_FileReference::FindFilePreference::None, MYON_FileReference::FindFilePreference::None, MYON_FileReference::FindFilePreference::None };
    unsigned int candidate_count = 0;
    for (unsigned int pass = 0; pass < 3; pass++)
    {
      MYON_FileReference::FindFilePreference ffp = MYON_FileReference::FindFilePreference::None;
      MYON_wString name;
      switch (pass)
      {
      case 0:
        name = m_full_path;
        ffp = MYON_FileReference::FindFilePreference::FullPath;
        break;
      case 1:
        if (nullptr != base_path && m_relative_path.IsNotEmpty())
        {
          name = MYON_FileSystemPath::FullPathFromRelativePath(base_path,false,static_cast<const wchar_t*>(m_relative_path));
          ffp = MYON_FileReference::FindFilePreference::RelativePath;
        }
        break;
      case 2:
        if (nullptr != base_path)
        {
          name = MYON_FileSystemPath::FullPathFromRelativePath(base_path,false,filename);
          ffp = MYON_FileReference::FindFilePreference::BasePath;
        }
        break;
      }
      if (name.IsEmpty() || MYON_FileReference::FindFilePreference::None == ffp)
        continue;
      if ( false == MYON_FileSystem::IsFile(name) )
        continue;
      if (ffp == pref[0])
      {
        // got lucky
        return Internal_FindFileResult(
          name,
          MYON_ContentHash::Unset,
          ffp,
          found_file_full_path,
          found_file_content_hash);
      }
      candidate_file_name[candidate_count] = name;
      candidate_file_pref[candidate_count] = ffp;
      candidate_count++;
    }


    if (0 == candidate_count)
      break;

    if ( 1 == candidate_count )
    {
      return Internal_FindFileResult(
        candidate_file_name[0],
        MYON_ContentHash::Unset,
        candidate_file_pref[0],
        found_file_full_path,
        found_file_content_hash);
    }


    MYON_ContentHash candidate_file_content[3] = { MYON_ContentHash::Unset, MYON_ContentHash::Unset, MYON_ContentHash::Unset };
    MYON__UINT64 candidate_file_time[3] = { 0 };

    for (unsigned int i = 0; i < pref_count; i++)
    {
      switch (pref[i])
      {
      case MYON_FileReference::FindFilePreference::None:
        break;

      case MYON_FileReference::FindFilePreference::FullPath:
      case MYON_FileReference::FindFilePreference::RelativePath:
      case MYON_FileReference::FindFilePreference::BasePath:
        for (unsigned int j = 0; j < candidate_count; j++)
        {
          if (pref[i] == candidate_file_pref[j])
          {
            return Internal_FindFileResult(
              candidate_file_name[j],
              candidate_file_content[j],
              candidate_file_pref[j],
              found_file_full_path,
              found_file_content_hash);
          }
        }
        break;

      case MYON_FileReference::FindFilePreference::ContentMatch:
        for (unsigned int j = 0; j < candidate_count; j++)
        {
          if (candidate_file_content[j].IsNotSet())
          {
            for (unsigned int k = 0; k < j; k++)
            {
              if (MYON_wString::EqualPath(candidate_file_name[j], candidate_file_name[k]))
              {
                candidate_file_content[j] = candidate_file_content[k];
                break;
              }
            }
            if (candidate_file_content[j].IsNotSet())
            {
              // Use EqualFileNameSizeAndTime() to avoid expensive content calculation.
              if (MYON_FileReference::FindFilePreference::FullPath == candidate_file_pref[j]
                && m_content_hash.EqualFileNameSizeAndTime(candidate_file_name[j]))
                candidate_file_content[j] = m_content_hash;
              else
                candidate_file_content[j] = MYON_ContentHash::CreateFromFile(candidate_file_name[j]);
            }
          }
          if (candidate_file_content[j].IsSet())
          {
            if (MYON_ContentHash::EqualContent(m_content_hash, candidate_file_content[j]))
              return Internal_FindFileResult(
                candidate_file_name[j],
                candidate_file_content[j],
                MYON_FileReference::FindFilePreference::ContentMatch,
                found_file_full_path,
                found_file_content_hash);
            candidate_file_time[j] = candidate_file_content[j].ContentLastModifiedTime();
          }
        }
        break;

      case MYON_FileReference::FindFilePreference::MostRecent:
        {
          unsigned int most_recent_dex = candidate_count;
          MYON__UINT64 most_recent_time = 0;
          for (unsigned int j = 0; j < candidate_count; j++)
          {
            if (candidate_file_time[j] <= 0)
            {
              MYON__UINT64 t = 0;
              for (unsigned int k = 0; k < j; k++)
              {
                if (MYON_wString::EqualPath(candidate_file_name[j], candidate_file_name[k]))
                {
                  t = candidate_file_time[k];
                  break;
                }
              }
              if (t <= 0)
                MYON_FileStream::GetFileInformation(candidate_file_name[j], nullptr, nullptr, &t);
              candidate_file_time[j] = t;
            }
            if (candidate_file_time[j] > most_recent_time)
            {
              most_recent_dex = j;
              most_recent_time = candidate_file_time[j];
            }
          }
          if (most_recent_time > 0 && most_recent_dex < candidate_count)
            return Internal_FindFileResult(
            candidate_file_name[most_recent_dex],
            candidate_file_content[most_recent_dex],
            MYON_FileReference::FindFilePreference::MostRecent,
            found_file_full_path,
            found_file_content_hash);
        }
        break;

      default:
        break;
      }
    }

    return Internal_FindFileResult(
      candidate_file_name[0],
      candidate_file_content[0],
      candidate_file_pref[0],
      found_file_full_path,
      found_file_content_hash);
    break;
  }

  // file not found
  return Internal_FindFileResult(
    MYON_wString::EmptyString,
    MYON_ContentHash::Unset,
    MYON_FileReference::FindFilePreference::None,
    found_file_full_path,
    found_file_content_hash);
}

bool MYON_FileReference::IsSet() const
{
  return m_full_path.IsNotEmpty();
}

bool MYON_FileReference::IsNotSet() const
{
  return m_full_path.IsEmpty();
}


bool MYON_FileReference::Write(
  bool bUseArchiveBasePath,
  MYON_BinaryArchive& archive
  ) const
{
  const wchar_t* base_path
    = bUseArchiveBasePath
    ? archive.ArchiveDirectoryNameAsPointer()
    : nullptr;
  bool bBasePathIncludesFileName = false;
  return Write( base_path, bBasePathIncludesFileName, archive );
}

bool MYON_FileReference::Write(
  const wchar_t* base_path,
  bool bBasePathIncludesFileName,
  MYON_BinaryArchive& archive
  ) const
{
  const int major_version = 1;

  // the embedded file id was added minor version 1
  const int minor_version = archive.Archive3dmVersion() >= 60 ? 1 : 0;

  if (!archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,major_version,minor_version))
    return false;

  bool rc = false;
  for (;;)
  {
    const bool bBasePathIsEmpty = (nullptr == base_path || 0 == base_path[0]);

    const MYON_wString full_path
      = m_full_path.IsEmpty() && m_relative_path.IsNotEmpty() && false == bBasePathIsEmpty
      ? MYON_FileSystemPath::CombinePaths(base_path, bBasePathIncludesFileName, m_relative_path, true, false)
      : m_full_path;
    if (!archive.WriteString(full_path))
      break;

    const MYON_wString relative_path 
      = (bBasePathIsEmpty || m_full_path.IsEmpty() )
      ? m_relative_path
      : MYON_FileSystemPath::RelativePath(m_full_path,true,base_path,bBasePathIncludesFileName);
    if (!archive.WriteString(relative_path))
      break;

    if (!m_content_hash.Write(archive))
      break;
    unsigned int i = static_cast<unsigned int>(m_full_path_status);
    if (!archive.WriteInt(i))
      break;

    // embedded file id added at chunk version 1.1
    MYON_UUID embedded_file_id = m_embedded_file_id;

    if ( IsSet() && archive.Active3dmTable() >  MYON_3dmArchiveTableType::bitmap_table )
    {
      const MYON_ComponentManifestItem& embedded_file_item = archive.Manifest().ItemFromNameHash(
        MYON_ModelComponent::Type::Image,
        MYON_NameHash::CreateFilePathHash(*this)
        );
      if (embedded_file_item.IsValid())
      {
        // A file with identical full path is embedded in this archive.
        // The embedded file can be used if the referenced file cannot
        // be found when this archive is read.
        embedded_file_id = embedded_file_item.Id();
      }
    }

    if (!archive.WriteUuid(embedded_file_id))
      break;

    rc = true;
    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;
  return rc;
}

bool MYON_FileReference::Read(
  MYON_BinaryArchive& archive
  )
{
  *this = MYON_FileReference::Unset;
  int major_version = 0;
  int minor_version = 0;
  if (!archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version))
    return false;
  bool rc = false;
  for (;;)
  {
    if ( 1 != major_version)
      break;
    if (!archive.ReadString(m_full_path))
      break;
    if (!archive.ReadString(m_relative_path))
      break;
    if (!m_content_hash.Read(archive))
      break;
    
    unsigned int full_path_status_as_unsigned = 0;
    if (!archive.ReadInt(&full_path_status_as_unsigned))
      break;
    //m_full_path_status = MYON_FileReference::StatusFromUnsigned(full_path_status_as_unsigned);
    // The full path status must be validated after each read.
    m_full_path_status = MYON_FileReference::Status::Unknown; 

    if (minor_version >= 1)
    {
      if (!archive.ReadUuid(m_embedded_file_id))
        break;
    }
    rc = true;
    break;
  }

  if (!archive.EndRead3dmChunk())
    rc = false;
  return rc;

}

void MYON_FileReference::Dump(
  class MYON_TextLog& text_log
  ) const
{
  text_log.Print("Full path: \"%ls\"\n", static_cast<const wchar_t*>(m_full_path));
  text_log.PushIndent();
  text_log.Print("Relative path: \"%ls\"\n", static_cast<const wchar_t*>(m_relative_path));
  m_content_hash.Dump(text_log);
  text_log.PopIndent();
}

unsigned int MYON_FileReference::SizeOf() const
{
  return m_full_path.SizeOf() + m_relative_path.SizeOf() + (unsigned int)(sizeof(*this) - sizeof(m_full_path) - sizeof(m_relative_path));
}

const MYON_wString& MYON_FileReference::FullPath() const
{
  return m_full_path;
}

const wchar_t* MYON_FileReference::FullPathAsPointer() const
{
  return static_cast<const wchar_t*>(m_full_path);
}

void MYON_FileReference::SetFullPath(
  const wchar_t* full_path,
  bool bSetContentHash
  )
{
  MYON_wString local_full_path(full_path);
  local_full_path.TrimLeftAndRight();
  if (local_full_path.IsEmpty())
    *this = MYON_FileReference::Unset;
  else
  {
    const MYON_wString clean_full_path = MYON_FileSystemPath::CleanPath(local_full_path);
    const bool bFullPathChanged = (0 != MYON_wString::CompareOrdinal(clean_full_path, m_full_path, false));
    m_full_path = clean_full_path;
    m_full_path_hash = MYON_SHA1_Hash::EmptyContentHash;
    m_embedded_file_id = MYON_nil_uuid;
    m_relative_path = MYON_wString::EmptyString;
    if (bSetContentHash)
    {
      if (m_content_hash.IsNotSet() || false == m_content_hash.EqualFileNameSizeAndTime(m_full_path))
        m_content_hash = MYON_ContentHash::CreateFromFile(m_full_path);
      m_full_path_status
        =  (m_content_hash.IsSet())
        ? MYON_FileReference::Status::FullPathValid
        : MYON_FileReference::Status::FileNotFound;
    }
    else if (bFullPathChanged)
    {
      m_content_hash = MYON_ContentHash::Unset;
      m_full_path_status 
        = MYON_FileSystem::IsFile(m_full_path)
        ? MYON_FileReference::Status::FullPathValid
        : MYON_FileReference::Status::FileNotFound;
    }
  }
}

void MYON_FileReference::SetFullPath(
  const char* full_path,
  bool bSetContentHash
  )
{
  const MYON_wString local_full_path(full_path);
  SetFullPath(static_cast<const wchar_t*>(local_full_path),bSetContentHash);
}



void MYON_FileReference::ClearFullPath()
{
  m_full_path = MYON_wString::EmptyString;
  m_full_path_hash = MYON_SHA1_Hash::EmptyContentHash;
  m_embedded_file_id = MYON_nil_uuid;
  m_full_path_status = MYON_FileReference::Status::Unknown;
}

const MYON_wString& MYON_FileReference::RelativePath() const
{
  return m_relative_path;
}

const wchar_t* MYON_FileReference::RelativePathAsPointer() const
{
  return static_cast<const wchar_t*>(m_relative_path);
}

void MYON_FileReference::SetRelativePath(
  const wchar_t* relative_path
  )
{
  m_relative_path = relative_path;
  m_relative_path.TrimLeftAndRight();
}

void MYON_FileReference::SetRelativePath(
  const char* relative_path
  )
{
  m_relative_path = relative_path;
  m_relative_path.TrimLeftAndRight();
}

void MYON_FileReference::SetRelativePathFromBasePath(
  const wchar_t* base_path,
  bool bBasePathContainsFileName
  )
{
  const MYON_wString relative_path = MYON_FileSystemPath::RelativePath(
    m_full_path,
    true,
    base_path,
    bBasePathContainsFileName
    );
}

void MYON_FileReference::SetRelativePathFromBasePath(
  const char* base_path,
  bool bBasePathContainsFileName
  )
{
  const MYON_wString local_base_path(base_path);
  SetRelativePathFromBasePath(static_cast<const wchar_t*>(local_base_path),bBasePathContainsFileName);
}

void MYON_FileReference::ClearRelativePath()
{
  m_relative_path = MYON_wString::EmptyString;
}

const MYON_ContentHash& MYON_FileReference::ContentHash() const
{
  return m_content_hash;
}

void MYON_FileReference::SetContentHash(
  MYON_ContentHash content_hash
  )
{
  m_content_hash = content_hash;
}

void MYON_FileReference::ClearContentHash()
{
  m_content_hash = MYON_ContentHash::Unset;
}

bool MYON_FileReference::UpdateContentHash()
{
  if (m_full_path.IsEmpty())
  {
    m_content_hash = MYON_FileReference::Unset.ContentHash();
    return true;
  }
  m_content_hash = MYON_ContentHash::CreateFromFile(m_full_path);
  m_recent_content_hash = m_content_hash;
  return m_content_hash.IsSet();
}

const MYON_ContentHash& MYON_FileReference::RecentContentHash(
  MYON__UINT64 recent_time
) const
{
  const MYON__UINT64 current_time = MYON_SecondsSinceJanOne1970UTC();
  if (0 == recent_time || recent_time > current_time)
    recent_time = current_time;
  if (m_recent_content_hash.IsNotSet() || m_recent_content_hash.HashCalculationTime() < recent_time)
  {
    if (m_content_hash.IsSet() && m_content_hash.HashCalculationTime() >= recent_time)
      m_recent_content_hash = m_content_hash;
    else
      m_recent_content_hash = MYON_ContentHash::CreateFromFile(m_full_path);
  }
  return m_recent_content_hash;
}

const MYON_SHA1_Hash& MYON_FileReference::FullPathHash() const
{
  if (m_full_path.IsNotEmpty() && m_full_path_hash == MYON_SHA1_Hash::EmptyContentHash)
  {
    m_full_path_hash = MYON_SHA1_Hash::FileSystemPathHash(m_full_path);
  }
  return m_full_path_hash;
}

MYON_FileReference::Status MYON_FileReference::FullPathStatus() const
{
  return m_full_path_status;
}

void MYON_FileReference::SetFullPathStatus(
  MYON_FileReference::Status full_path_status
  )
{
  m_full_path_status = full_path_status;
}

MYON_UUID MYON_FileReference::EmbeddedFileId() const
{
  return m_embedded_file_id;
}

void MYON_FileReference::SetEmbeddedFileId(
  MYON_UUID embedded_file_id
  )
{
  m_embedded_file_id = embedded_file_id;
}

// deprecated
bool MYON_FileSystemPath::PathExists(
  const char* path
)
{
  return MYON_FileSystem::PathExists(path);
}

// deprecated
bool MYON_FileSystemPath::PathExists(
  const wchar_t* path
)
{
  return MYON_FileSystem::PathExists(path);
}

// deprecated
bool MYON_FileSystemPath::IsFile(
  const char* path
)
{
  return MYON_FileSystem::IsFile(path);
}

// deprecated
bool MYON_FileSystemPath::IsFile(
  const wchar_t* path
)
{
  return MYON_FileSystem::IsFile(path);
}

// deprecated
bool MYON_FileSystemPath::IsDirectory(
  const char* path
)
{
  return MYON_FileSystem::IsDirectory(path);
}

// deprecated
bool MYON_FileSystemPath::IsDirectory(
  const wchar_t* path
)
{
  return MYON_FileSystem::IsDirectory(path);
}

// MYON_UnicodeTextFile

class MYON_File
{
public:
  virtual ~MYON_File() { }

  bool Open(const wchar_t* filename, const wchar_t* mode) { m_pFile = MYON_FileStream::Open(filename, mode); return nullptr != m_pFile; }
  bool Close(void)                                       const { return MYON_FileStream::Close(m_pFile) == 0; }
  bool SeekFromCurrentPosition(MYON__INT64 offset)         const { return MYON_FileStream::SeekFromCurrentPosition(m_pFile, offset); }
  bool SeekFromStart(MYON__INT64 offset)                   const { return MYON_FileStream::SeekFromStart(m_pFile, offset); }
  bool SeekFromEnd(MYON__INT64 offset)                     const { return MYON_FileStream::SeekFromEnd(m_pFile, offset); }
  bool Seek(MYON__INT64 offset, int origin)                const { return MYON_FileStream::Seek(m_pFile, offset, origin); }
  MYON__INT64  CurrentPosition(void)                       const { return MYON_FileStream::CurrentPosition(m_pFile); }
  MYON__UINT64 Read(MYON__UINT64 count, void* buffer)        const { return MYON_FileStream::Read(m_pFile, count, buffer); }
  MYON__UINT64 Write(MYON__UINT64 count, const void* buffer) const { return MYON_FileStream::Write(m_pFile, count, buffer); }

  MYON__UINT64 GetLength(void) const
  {
    const auto cur = CurrentPosition();
    SeekFromEnd(0);
    const auto end = CurrentPosition();
    SeekFromStart(cur);

    return end;
  }

private:
  FILE* m_pFile = nullptr;
};

class MYON_UnicodeTextFile::CImpl final
{
public:
  ~CImpl() { Close(); }

  bool Open(const wchar_t* wszFullPath, Modes mode);
  bool Close(void);
  bool ReadString(MYON_wString& s);
  bool WriteString(const wchar_t* wsz);
  bool ReadHeader(Types& t);
  bool WriteHeader(void);
  bool ReadStringFromUTF8(MYON_wString& s);
  bool ReadStringFromUTF16(MYON_wString& s);
  bool WriteStringToUTF8(const wchar_t* wsz);
  bool WriteStringToUTF16(const wchar_t* wsz);
  size_t ReadData(void* buf, size_t bytes_to_read);
  size_t WriteData(const void* buf, size_t bytes_to_write);

public:
  MYON_File m_File;
  Types m_Type = Types::Unknown;
};

size_t MYON_UnicodeTextFile::CImpl::ReadData(void* buf, size_t bytes_to_read)
{
  return m_File.Read(bytes_to_read, buf);
}

size_t MYON_UnicodeTextFile::CImpl::WriteData(const void* buf, size_t bytes_to_write)
{
  return m_File.Write(bytes_to_write, buf);
}

static const wchar_t* FileStreamMode(MYON_UnicodeTextFile::Modes m)
{
  if (m == MYON_UnicodeTextFile::Modes::Read)
    return L"rb";

  if (m == MYON_UnicodeTextFile::Modes::Write)
    return L"wb";

  MYON_ASSERT(false);
  return L"";
}

bool MYON_UnicodeTextFile::CImpl::Open(const wchar_t* wszFullPath, Modes mode)
{
  bool ok = false;
  int attemptsCounter = 0;

  while (!ok && (attemptsCounter < 100))
  {
    if (m_File.Open(wszFullPath, FileStreamMode(mode)))
    {
      ok = true;
    }
    else
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      attemptsCounter++;
    }
  }

  if (ok)
  {
    if (Modes::Write == mode)
    {
      ok = WriteHeader();
    }
    else
    {
      ok = ReadHeader(m_Type);
    }
  }

  return ok;
}

bool MYON_UnicodeTextFile::CImpl::Close(void)
{
  return m_File.Close();
}

bool MYON_UnicodeTextFile::CImpl::ReadHeader(Types& t)
{
  if (0 != m_File.CurrentPosition())
    return false;

  MYON__UINT8 pBuf[3] = { 0 };

  if (2 != ReadData(pBuf, 2))
    return false;

  if (pBuf[0] == MYON__UINT8(0xFF))
  {
    if (pBuf[1] == MYON__UINT8(0xFE))
    {
      t = Types::UTF16;
      return true;
    }
  }

  if (pBuf[0] == MYON__UINT8(0xEF))
  {
    if (pBuf[1] == MYON__UINT8(0xBB))
    {
      if (1 == ReadData(pBuf + 2, 1))
      {
        if (pBuf[2] == MYON__UINT8(0xBF))
        {
          t = Types::UTF8;
          return true;
        }
      }
    }
  }

  // No BOM was found so rewind and assume UTF8. This allows testing with ASCII files.
  m_File.SeekFromStart(0);
  t = Types::UTF8;

  return true;
}

bool MYON_UnicodeTextFile::CImpl::WriteHeader(void)
{
  MYON__UINT8 pBuf[3] = { 0 };

  size_t sizeBOM = 2;
  if (Types::UTF8 == m_Type)
  {
    sizeBOM = 3;
    pBuf[0] = MYON__UINT8(0xEF);
    pBuf[1] = MYON__UINT8(0xBB);
    pBuf[2] = MYON__UINT8(0xBF);
  }
  else
  if (Types::UTF16 == m_Type)
  {
    pBuf[0] = MYON__UINT8(0xFF);
    pBuf[1] = MYON__UINT8(0xFE);
  }
  else MYON_ASSERT(false); // Did you forget to set the type in the constructor?

  if (!WriteData(pBuf, sizeBOM))
    return false;

  return true;
}

bool MYON_UnicodeTextFile::CImpl::ReadString(MYON_wString& s)
{
  switch (m_Type)
  {
  case Types::UTF8:
    return ReadStringFromUTF8(s);

  case Types::UTF16:
    return ReadStringFromUTF16(s);
          
  case Types::Unknown:
  default:
    return false;
  }
}

bool MYON_UnicodeTextFile::CImpl::WriteString(const wchar_t* wsz)
{
  switch (m_Type)
  {
  case Types::UTF8:
    return WriteStringToUTF8(wsz);

  case Types::UTF16:
    return WriteStringToUTF16(wsz);
          
  case Types::Unknown:
  default:
    return false;
  }
}

bool MYON_UnicodeTextFile::CImpl::ReadStringFromUTF8(MYON_wString& s)
{
  const auto size_in_bytes = m_File.GetLength() - m_File.CurrentPosition();
  auto p = std::unique_ptr<MYON__UINT8[]>(new MYON__UINT8[size_in_bytes + 1]);
  auto* pBuffer = p.get();

  ReadData(pBuffer, size_in_bytes);
  pBuffer[size_in_bytes] = 0;

  const char* pUTF8 = reinterpret_cast<const char*>(pBuffer);

  const auto num_chars = MYON_ConvertUTF8ToWideChar(false, pUTF8, -1, nullptr, 0, nullptr, 0, 0, nullptr);

  auto* string_buf = s.SetLength(num_chars);
  if (nullptr == string_buf)
    return false;

  MYON_ConvertUTF8ToWideChar(false, pUTF8, -1, string_buf, num_chars+1, nullptr, 0, 0, nullptr);

  return !s.IsEmpty();
}

bool MYON_UnicodeTextFile::CImpl::ReadStringFromUTF16(MYON_wString& s)
{
  const auto char_size = sizeof(MYON__UINT16);
  const auto size_in_bytes = m_File.GetLength() - m_File.CurrentPosition();
  const auto size_in_chars = size_in_bytes / char_size;

#ifdef MYON_RUNTIME_WIN
  // On Windows, wchar_t is UTF16 so we can load the file directly into the MYON_wString.
  MYON_ASSERT(sizeof(wchar_t) == sizeof(MYON__UINT16));

  auto* buf = s.SetLength(size_in_chars);
  if (nullptr == buf)
    return false;

  if (ReadData(buf, size_in_bytes) != size_in_bytes)
    return false;

  buf[size_in_chars] = 0;
#else
  // On Mac wchar_t is UTF32 so we have to load the file into a buffer and then convert it to the MYON_wString.
  auto p = std::unique_ptr<MYON__UINT16[]>(new MYON__UINT16[size_in_chars + 1]);
  auto* pUTF16 = p.get();
  ReadData(pUTF16, size_in_bytes);
  pUTF16[size_in_chars] = 0;

  const auto num_chars = MYON_ConvertUTF16ToUTF32(false, pUTF16, -1, nullptr, 0, nullptr, 0, 0, nullptr);
  auto* string_buf = s.SetLength(num_chars);
  if (nullptr == string_buf)
    return false;

  MYON_ASSERT(sizeof(wchar_t) == sizeof(MYON__UINT32));
  auto* pWide = reinterpret_cast<MYON__UINT32*>(string_buf);
  MYON_ConvertUTF16ToUTF32(false, pUTF16, -1, pWide, num_chars+1, nullptr, 0, 0, nullptr);
#endif

  return true;
}

bool MYON_UnicodeTextFile::CImpl::WriteStringToUTF8(const wchar_t* wsz)
{
  const auto num_chars = MYON_ConvertWideCharToUTF8(false, wsz, -1, nullptr, 0, nullptr, 0, 0, nullptr);

  auto p = std::unique_ptr<char[]>(new char[size_t(num_chars) + 1]);
  auto* pBuffer = p.get();

  MYON_ConvertWideCharToUTF8(false, wsz, -1, pBuffer, num_chars + 1, nullptr, 0, 0, nullptr);

  if (WriteData(pBuffer, num_chars) != num_chars)
    return false;

  return true;
}

bool MYON_UnicodeTextFile::CImpl::WriteStringToUTF16(const wchar_t* wsz)
{
#ifdef MYON_RUNTIME_WIN
  // On Windows, wchar_t is UTF16 so we can save the file directly from 'wsz'.
  MYON_ASSERT(sizeof(wchar_t) == sizeof(MYON__UINT16));

  const auto size_in_bytes = wcslen(wsz) * sizeof(wchar_t);
  if (WriteData(wsz, size_in_bytes) != size_in_bytes)
    return false;
#else
  // On Mac wchar_t is UTF32 so we have to convert 'wsz' to UTF16 in a buffer and write the buffer to the file.
  MYON_ASSERT(sizeof(wchar_t) == sizeof(MYON__UINT32));
  auto* pWide = reinterpret_cast<const MYON__UINT32*>(wsz);

  const auto num_chars = MYON_ConvertUTF32ToUTF16(false, pWide, -1, nullptr, 0, nullptr, 0, 0, nullptr);

  const auto num_chars_inc_term = num_chars + 1;
  auto p = std::unique_ptr<MYON__UINT16[]>(new MYON__UINT16[num_chars_inc_term]);
  auto* pUTF16 = p.get();

  MYON_ConvertUTF32ToUTF16(false, pWide, -1, pUTF16, num_chars_inc_term, nullptr, 0, 0, nullptr);

  const auto size_in_bytes = num_chars * sizeof(MYON__UINT16);
  if (WriteData(pUTF16, size_in_bytes) != size_in_bytes)
    return false;
#endif

  return true;
}

MYON_UnicodeTextFile::MYON_UnicodeTextFile(Types t)
{
  m_impl = new CImpl;
  m_impl->m_Type = t;
}

MYON_UnicodeTextFile::~MYON_UnicodeTextFile()
{
  m_impl->~CImpl();
  m_impl = nullptr;
}

bool MYON_UnicodeTextFile::Open(const wchar_t* wszFullPath, Modes mode)
{
  return m_impl->Open(wszFullPath, mode);
}

bool MYON_UnicodeTextFile::Close(void)
{
  return m_impl->Close();
}

bool MYON_UnicodeTextFile::ReadString(MYON_wString& s)
{
  return m_impl->ReadString(s);
}

bool MYON_UnicodeTextFile::WriteString(const wchar_t* wsz)
{
  return m_impl->WriteString(wsz);
}
