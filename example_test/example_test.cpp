/*
//
// Copyright (c) 1993-2018 Robert McNeel & Associates. All rights reserved.
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
*/		

////////////////////////////////////////////////////////////////
//
//  example_test.cpp  
// 
//  Example program using the Rhino file IO toolkit. The program reads  
//  Rhino 3dm model files and verifies they can be saved and reread with
//  no changes to core model content. The program is a console application
//  that takes options, file names, and directory names as command line
//   arguments.
//
////////////////////////////////////////////////////////////////////////

#include "../opennurbs_public_examples.h"

class Internal_CTestContext
{
public:
  enum : unsigned int
  {
    sizeof_a = 4088
  };

  Internal_CTestContext()
  {
    m_a = onmalloc(sizeof_a);
  }

  ~Internal_CTestContext()
  {
    onfree(m_a);
  }

  void SetInitialDirecory(
    const char* initial_directory,
    unsigned int counter
  )
  {
    m_initial_directory = Internal_CleanPath(initial_directory);
    m_initial_directory_length = m_initial_directory.Length();
    m_initial_directory_counter = counter;
  }

  void* m_a = nullptr;
  MYON__INT64 m_min_delta_time = 0;
  MYON__INT64 m_max_delta_time = 0;

  unsigned int m_file_count = 0;
  unsigned int m_directory_count = 0;
  unsigned int m_file_open_error_count = 0;
  unsigned int m_max_directory_tree_depth = 0;
  unsigned int m_max_file_count = 0;

  const MYON_String TextLogPathFromFullPath(
    const char* full_path
  ) const;

private:
  MYON_String m_initial_directory;
  unsigned int m_initial_directory_counter = 0;
  int m_initial_directory_length = 0;
  Internal_CTestContext(const Internal_CTestContext&) = delete;
  Internal_CTestContext operator=(const Internal_CTestContext&) = delete;
  const MYON_String Internal_CleanPath(const char* dirty_path) const
  {
    const char* volume = 0;
    const char* path = 0;

    // Use local path in case drive, dir, file_name_stem or ext are being reused.
    on_splitpath(dirty_path, &volume, &path, nullptr, nullptr);
    MYON_String clean_path(path);
    if (clean_path.IsEmpty())
      return MYON_String(dirty_path);
    clean_path.Replace(MYON_String::Backslash, MYON_String::Slash);
    
    if (nullptr != volume && volume < path)
    {
      MYON_String clean_volume(volume, (int)(path - volume));
      return (clean_volume + clean_path);
    }

    return clean_path;
  }

};

const MYON_String Internal_CTestContext::TextLogPathFromFullPath(const char* full_path) const
{
  // replace initial directory with <Initial Folder> and use / for
  // the directory separator so that output files are standardized.
  MYON_String text_log_path;
  MYON_String path1 = Internal_CleanPath(full_path);
  if (m_initial_directory_length > 0 &&
    MYON_String::EqualOrdinal(m_initial_directory, m_initial_directory_length, path1, m_initial_directory_length, true)
    )
  {
    text_log_path 
      = (m_initial_directory_counter>0)
      ? MYON_String::FormatToString("<initial folder %u>", m_initial_directory_counter)
      : MYON_String("<initial folder>");
    text_log_path += static_cast<const char*>(path1) + m_initial_directory_length;
  }
  else
  {
    text_log_path = path1;
  }
  return text_log_path;
}


static const MYONX_ErrorCounter Internal_TestModelRead(
  MYON_TextLog& text_log,
  const MYON_String text_log_3dm_file_name,
  MYON_BinaryArchive& source_archive,
  bool bVerbose
  )
{
  MYONX_ModelTest::Type test_type = MYONX_ModelTest::Type::ReadWriteReadCompare;
  
  /////
  //
  // Read the orginal file
  //
  text_log.PushIndent();
  MYONX_ModelTest test;
  const MYON_wString wide_text_log_3dm_file_name(text_log_3dm_file_name);
  test.ReadTest(source_archive, test_type, true, wide_text_log_3dm_file_name, &text_log);
  text_log.PrintNewLine();
  text_log.Print("Test Results:\n");
  text_log.PushIndent();
  test.Dump(text_log);
  text_log.PopIndent();

  MYONX_ErrorCounter error_counter = test.ErrorCounter();

  const MYONX_Model* source_model = test.SourceModel().get();
  if (nullptr == source_model)
  {
    text_log.PopIndent();
    return error_counter;
  }

  const bool bCompareTestFailed = MYONX_ModelTest::Result::Fail == test.TestResult(MYONX_ModelTest::Type::ReadWriteReadCompare);

  if ( bVerbose || bCompareTestFailed )
  {
    for (int i = 0; i < 2; i++)
    {
      if (0 == i)
        test.DumpSourceModel();
      else
        test.DumpReadWriteReadModel();
      if (false == bCompareTestFailed)
        break;
    }
  }
  
  text_log.PrintNewLine();

  const MYON_ModelComponent::Type component_types[] =
  {
    // TODO uncomment types as components are derived from MYON_ModelComponent
    MYON_ModelComponent::Type::Image,
    //MYON_ModelComponent::Type::TextureMapping,
    MYON_ModelComponent::Type::RenderMaterial,
    MYON_ModelComponent::Type::LinePattern,
    MYON_ModelComponent::Type::Layer,
    //MYON_ModelComponent::Type::Group,
    MYON_ModelComponent::Type::TextStyle,
    MYON_ModelComponent::Type::DimStyle,
    MYON_ModelComponent::Type::RenderLight,
    MYON_ModelComponent::Type::HatchPattern,
    MYON_ModelComponent::Type::InstanceDefinition,
    MYON_ModelComponent::Type::ModelGeometry,
    //MYON_ModelComponent::Type::HistoryRecord
  };

  const unsigned int component_type_count = (unsigned int)(sizeof(component_types) / sizeof(component_types[0]));

  const MYONX_Model& model = *source_model;

  error_counter.ClearLibraryErrorsAndWarnings();

  const MYON_ComponentManifest& manifest = model.Manifest();
  MYON_SerialNumberMap it_map;

  for (unsigned int i = 0; i < component_type_count; i++)
  {
    const MYON_ModelComponent::Type component_type = component_types[i];
    const bool bUniqueNameRequired = MYON_ModelComponent::UniqueNameRequired(component_type);
    const bool bEmbeddedFileComponent = (MYON_ModelComponent::Type::Image == component_type);
    MYONX_ModelComponentIterator it(model,component_type);
    unsigned int it_count = 0;
    for (MYON_ModelComponentReference mcr = it.FirstComponentReference(); false == mcr.IsEmpty(); mcr = it.NextComponentReference())
    {
      const MYON_ModelComponent* model_component = mcr.ModelComponent();
      if (nullptr == model_component)
      {
        MYON_ERROR("Iterator returned nullptr mcr.ModelComponent()");
        continue; 
      }
      if (model_component->ComponentType() != component_type)
      {
        MYON_ERROR("Iterator returned wrong mcr.ModelComponent()->ComponentType()");
        continue; 
      }
      const MYON__UINT64 model_component_sn = model_component->RuntimeSerialNumber();
      const MYON_UUID model_component_id = model_component->Id();
      if (0 == model_component_sn)
      {
        MYON_ERROR("Iterator mcr.ModelComponent()->RuntimeSerialNumber() is zero.");
        continue; 
      }
      if (MYON_nil_uuid == model_component_id)
      {
        MYON_ERROR("Iterator mcr.ModelComponent()->Id() is nil.");
        continue; 
      }
      const MYON_SerialNumberMap::SN_ELEMENT* e_sn = it_map.FindSerialNumber(model_component_sn);
      if (nullptr != e_sn)
      {
        MYON_ERROR("Iterator returned component serial number twice.");
        continue; 
      }
      const MYON_SerialNumberMap::SN_ELEMENT* e_id = it_map.FindId(model_component_id);
      if (nullptr != e_id)
      {
        MYON_ERROR("Iterator returned component id twice.");
        continue; 
      }
      MYON_SerialNumberMap::SN_ELEMENT* e = it_map.AddSerialNumberAndId(model_component_sn,model_component_id);
      if (nullptr == e)
      {
        MYON_ERROR("MYON_SerialNumberMap failed to add sn and id.");
        continue; 
      }
      if (e->m_value.m_u_type != 0)
      {
        MYON_ERROR("MYON_SerialNumberMap error.");
        continue; 
      }
      e->m_value.m_u_type = 1;
      e->m_value.m_u.ptr = (void*)model_component;

      const MYON_ComponentManifestItem& item = manifest.ItemFromComponentRuntimeSerialNumber(model_component_sn);
      if (item.IsUnset())
      {
        MYON_ERROR("Iterator returned item not in manifest.");
        continue; 
      }
      if (model_component_sn != item.ComponentRuntimeSerialNumber())
      {
        MYON_ERROR("manifest.ItemFromComponentRuntimeSerialNumber() error.");
        continue; 
      }
      if (model_component_id != item.Id())
      {
        MYON_ERROR("item has wrong id.");
        continue; 
      }
 
      const MYON_ComponentManifestItem& item_id = manifest.ItemFromId(model_component_id);
      if (&item != &item_id)
      {
        MYON_ERROR("manifest.ItemFromId() failed.");
        continue; 
      }

      if (bUniqueNameRequired || bEmbeddedFileComponent)
      {
        const MYON_Bitmap* embedded_file_reference = bEmbeddedFileComponent ? MYON_Bitmap::Cast(model_component) : nullptr;
        const MYON_NameHash full_path_hash 
          = (nullptr != embedded_file_reference) 
          ? MYON_NameHash::CreateFilePathHash(embedded_file_reference->FileReference()) 
          : MYON_NameHash::EmptyNameHash;
        const MYON_NameHash name_hash 
          =bEmbeddedFileComponent
          ? full_path_hash
          : model_component->NameHash();
        if (false == name_hash.IsValidAndNotEmpty())
        {
          MYON_ERROR("model compoent name is not valid.");
          continue; 
        }
        if (name_hash != item.NameHash())
        {
          MYON_ERROR("item has wrong name hash.");
          continue; 
        }
        const MYON_ComponentManifestItem& item_name = manifest.ItemFromNameHash(component_type,name_hash);
        if (&item != &item_name)
        {
          MYON_ERROR("manifest.ItemFromNameHash() failed.");
          continue; 
        }
      }
      it_count++;
    }

    if (it_count != manifest.ActiveAndDeletedComponentCount(component_type))
    {
      MYON_ERROR("iterator count != manifest.ActiveAndDeletedComponentCount(component_type)");
      continue; 
    }
  }

  text_log.PopIndent();
  error_counter.AddLibraryErrorsAndWarnings();
  return error_counter;
}

static const MYONX_ErrorCounter Internal_TestFileRead(
  MYON_TextLog& text_log,
  const MYON_String fullpath,
  const MYON_String text_log_path,
  bool bVerbose
  )
{
  FILE* fp = nullptr;

  fp = MYON_FileStream::Open3dmToRead(fullpath);

  MYONX_ErrorCounter error_counter;
  error_counter.ClearLibraryErrorsAndWarnings();

  const MYON_String path_to_print
    = (text_log_path.IsNotEmpty())
    ? text_log_path
    : fullpath;
  
  for (;;)
  {
    if (nullptr == fp)
    {
      text_log.Print(
        "Skipped file: %s\n",
        static_cast<const char*>(path_to_print)
      );
      error_counter.IncrementFailureCount();
      break;
    }

    text_log.Print(
      "File name: %s\n",
      static_cast<const char*>(path_to_print)
    );

    MYON_BinaryFile source_archive(MYON::archive_mode::read3dm, fp);
    const MYON_wString wide_full_path(fullpath);
    source_archive.SetArchiveFullPath(static_cast<const wchar_t*>(wide_full_path));

    error_counter += Internal_TestModelRead(text_log, path_to_print, source_archive, bVerbose);
    break;
  }

  if ( nullptr != fp )
  {
    MYON_FileStream::Close(fp);
    fp = nullptr;
  }

  return error_counter;
}

static int Internal_ComparePath(const MYON_String* lhs, const MYON_String* rhs)
{
  // sort nullptr to end of list.
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  int rc = MYON_String::CompareOrdinal(static_cast<const char*>(*lhs), static_cast<const char*>(*rhs), true);
  if ( 0 == rc )
    rc = MYON_String::CompareOrdinal(static_cast<const char*>(*lhs), static_cast<const char*>(*rhs), false);
  return rc;
}

static const MYONX_ErrorCounter Internal_TestReadFolder(
  MYON_TextLog& text_log,
  const char* directory_path,
  unsigned int directory_tree_depth,
  bool bVerbose,
  Internal_CTestContext& test_context
  )
{
  MYONX_ErrorCounter error_counter;
  error_counter.ClearLibraryErrors();

  if (nullptr == directory_path || 0 == directory_path[0])
  {
    text_log.Print("Empty directory name.\n");
  }

  MYON_FileIterator fit;
  if (false == fit.Initialize(directory_path))
  {
    text_log.Print(
      "Invalid directory name: %s\n",
      directory_path
      );
    error_counter.IncrementFailureCount();
    return error_counter;
  }

  const MYON_String text_log_directory_name
    = (directory_tree_depth <= 1)
    ? MYON_String(directory_path)
    : test_context.TextLogPathFromFullPath(directory_path);
  text_log.Print(
    "Directory name: %s\n",
    static_cast<const char*>(text_log_directory_name)
    );
  text_log.PushIndent();

  MYON_ClassArray< MYON_String > sub_directories(32);
  MYON_ClassArray< MYON_String > skipped_files(32);
  MYON_ClassArray< MYON_String > tested_files(32);

  for ( bool bHaveItem = fit.FirstItem(); bHaveItem; bHaveItem = fit.NextItem() )
  {
    if ( test_context.m_max_file_count > 0 && test_context.m_file_count >= test_context.m_max_file_count)
      break;

    if (fit.CurrentItemIsDirectory())
    {
      if (directory_tree_depth < test_context.m_max_directory_tree_depth)
      {
        sub_directories.Append(fit.CurrentItemFullPathName());
      }
      continue;
    }

    MYON_String fullpath( fit.CurrentItemFullPathName() );

    if (MYON_FileStream::Is3dmFile(fullpath, false))
      tested_files.Append(fullpath);
    else
      skipped_files.Append(fullpath);
  }

  // sort file and folder names so test order depends only on content.
  // This is required so results from different computers can be compared.
  sub_directories.QuickSort(Internal_ComparePath);
  skipped_files.QuickSort(Internal_ComparePath);
  tested_files.QuickSort(Internal_ComparePath);

  if (skipped_files.Count() > 0)
  {
    text_log.PrintNewLine();
    for (int i = 0; i < skipped_files.Count(); i++)
    {
      const MYON_String path_to_print = test_context.TextLogPathFromFullPath(skipped_files[i]);
      text_log.Print(
        "Skipped file: %s\n",
        static_cast<const char*>(path_to_print)
      );
    }
    text_log.PrintNewLine();
  }
     
  for ( int i = 0; i < tested_files.Count(); i++ )
  {
    const MYON_String full_path = tested_files[i];
    const MYON_String path_to_print = test_context.TextLogPathFromFullPath(full_path);
    test_context.m_file_count++;
    const MYONX_ErrorCounter file_error_counter = Internal_TestFileRead(text_log, full_path, path_to_print, bVerbose);
    error_counter += file_error_counter;
  }

  for (int i = 0; i < sub_directories.Count(); i++)
  {
    if (test_context.m_max_file_count > 0 && test_context.m_file_count >= test_context.m_max_file_count)
      break;
    const MYON_String sub_directory_path = sub_directories[i];
    test_context.m_directory_count++;
    error_counter += Internal_TestReadFolder(text_log, sub_directory_path, directory_tree_depth + 1, bVerbose, test_context);
  }

  text_log.PopIndent();

  return error_counter;
}


static MYONX_ErrorCounter Internal_InvalidArg(const MYON_String arg, MYON_TextLog& text_log)
{
  MYONX_ErrorCounter err = MYONX_ErrorCounter::Zero;
  text_log.Print("Invalid command line parameter: \"%s\"\n", static_cast<const char*>(arg));
  err.IncrementFailureCount();
  return err;
}

static MYONX_ErrorCounter Internal_Test( 
  unsigned int max_directory_tree_depth,
  MYON_String full_path,
  bool bVerbose,
  MYON_TextLog& text_log,
  unsigned int& directory_counter,
  unsigned int& file_count,
  unsigned int& folder_count
)
{

  MYONX_ErrorCounter err = MYONX_ErrorCounter::Zero;

  if (MYON_FileSystem::IsFile(full_path))
  {
    if (MYON_FileStream::Is3dmFile(full_path, false))
    {
      text_log.Print("Testing 3dm file: %s\n", static_cast<const char*>(full_path));
      err = Internal_TestFileRead(text_log, full_path, MYON_String::EmptyString, bVerbose);
      file_count++;
    }
  }
  else if (MYON_FileSystem::IsDirectory(full_path))    
  {
    if ( max_directory_tree_depth > 0 )
    {
      text_log.Print("Testing 3dm files in folder: %s\n", static_cast<const char*>(full_path));
      Internal_CTestContext test_context;
      directory_counter++;
      test_context.SetInitialDirecory(full_path,directory_counter);
      test_context.m_max_directory_tree_depth = max_directory_tree_depth;

      err = Internal_TestReadFolder(text_log, full_path, 1, bVerbose, test_context);
      file_count += test_context.m_file_count;
      folder_count += test_context.m_directory_count;
    }
  }
  else
  {
    err += Internal_InvalidArg(full_path,text_log);
  }

  return err;
}

static MYON_String Internal_PlatformId(bool bVerbose)
{
  const MYON_String runtime =
#if defined(MYON_RUNTIME_WIN)
    bVerbose ? "Windows" : "Win"
#elif defined(MYON_RUNTIME_APPLE_IOS)
    bVerbose ? "Apple iOS" : "iOS"
#elif defined(MYON_RUNTIME_APPLE_MACOS)
    bVerbose ? "Apple Mac OS" : "MacOS"
#elif defined(MYON_RUNTIME_APPLE)
    "Apple"
#elif defined(MYON_RUNTIME_ANDROID)
    "Android"
#elif defined(MYON_RUNTIME_LINUX)
    "Linux"
#else
    "Runtime"
#endif
    ;

  const MYON_String platform =
#if defined(MYON_64BIT_RUNTIME)
    bVerbose ? " 64 bit" : "64"
#elif defined(MYON_32BIT_RUNTIME)
    bVerbose ? " 32 bit" : "32"
#else
    ""
#endif
    ;

  const MYON_String configuration =
#if defined(MYON_DEBUG)
    bVerbose ? " Debug" : "Debug"
#else
    bVerbose ? " Release" : "Release"
#endif
    ;

  MYON_String platform_id(runtime+platform+configuration);

  return platform_id;
}


static MYON_String Internal_DefaultOutFileName(
  const MYON_String exe_stem
  )
{
  MYON_String default_file_name(exe_stem);
  default_file_name.TrimLeftAndRight();
  if (default_file_name.IsEmpty())
    default_file_name = "example_test";
  default_file_name += "_log";

  const MYON_String platform_id = Internal_PlatformId(false);
  if (platform_id.IsNotEmpty())
    default_file_name += MYON_String(MYON_String("_") + platform_id);
  default_file_name += ".txt";

  return default_file_name;
}

static void Internal_PrintHelp(
  const MYON_String example_test_exe_stem,
  MYON_TextLog& text_log
)
{
  const MYON_String default_out_filename = Internal_DefaultOutFileName(example_test_exe_stem);

  text_log.Print("\n");
  text_log.Print("SYNOPSIS:\n");
  text_log.Print("  %s [-out[:outfilename.txt]] [-recursive[:N]] [-verbose] <file or folder names>\n", static_cast<const char*>(example_test_exe_stem) );
  text_log.Print("\n");
  text_log.Print("DESCRIPTION:\n");
  text_log.Print("  If a file is listed, it is read as an opennurbs model file.\n");
  text_log.Print("  If a folder is listed, all .3dm files in that folder are read\n");
  text_log.Print("  as opennurbs model files.\n");
  text_log.Print("\n");
  text_log.Print("  Available options:\n");
  text_log.Print("    -help\n");
  text_log.Print("      Displays this message.\n");
  text_log.Print("    -out\n");
  text_log.Print("      If -out is not present, output is written to stdout.\n");
  text_log.Print("      If :outfilename.txt is appended to -out, output is written to the specifed file.\n");
  text_log.Print("      If :stdout is appended to -out, output is sent to stdout.\n");
  text_log.Print("      If :null is appended to -out, no output is written.\n");
  text_log.Print("      Otherise output is written to %s.\n",static_cast<const char*>(default_out_filename));
  text_log.Print("    -recursive\n");
  text_log.Print("    -r\n");
  text_log.Print("      Recursivly reads files in subfolders.\n");
  text_log.Print("      If a :N is appended to the option, N limits the recursion depth.\n");
  text_log.Print("      -recursive:1 reads all 3dm files in the folder and does not recurse.\n");
  text_log.Print("    -verbose\n");
  text_log.Print("    -v\n");
  text_log.Print("      For each 3dm file, a text summary of the 3dm file contents is created\n");
  text_log.Print("      in the folder containing the 3dm file.\n");
  text_log.Print("\n");
  text_log.Print("RETURNS:\n");
  text_log.Print("  0: All tested files passed.\n");
  text_log.Print("  >0: Number of failures, errors, or warnings. See output for details.\n");
  text_log.Print("\n");
  text_log.Print("EXAMPLE:\n");
  text_log.Print("  %s -out:mytestresults.txt -resursive:2 .../example_files\n",static_cast<const char*>(example_test_exe_stem));
  text_log.Print("  with read all the opennurbs .3dm files in the .../example_files folder\n");
  text_log.Print("  and immediate subfolders of .../example_files. Output will be saved in mytestresults.txt.\n");
}

static void Internal_PrintHelp(
  const MYON_String example_test_exe_stem,
  MYON_TextLog* text_log_ptr
)
{
  MYON_TextLog print_to_stdout;
  if (nullptr == text_log_ptr)
  {
    print_to_stdout.SetIndentSize(2);
    text_log_ptr = &print_to_stdout;
  }
  return Internal_PrintHelp(example_test_exe_stem, *text_log_ptr);
}

static char Internal_ParseOptionTailSeparator()
{
  return ':';
}

static const char* Internal_ParseOptionHead(
  const char* s,
  const char* option_name0,
  const char* option_name1 = nullptr,
  const char* option_name2 = nullptr
)
{
  if (nullptr == s || 0 == s[0])
    return nullptr;
  if ('-' != s[0] )
  {
#if defined(MYON_OS_WINDOWS)
    if ( '/' != s[0] )
#endif
    return nullptr;
  }

  s++;
  if (0 == s[0])
    return nullptr;
  
  const char* option_names[] = { option_name0, option_name1, option_name2 };
  const size_t option_name_count = sizeof(option_names) / sizeof(option_names[0]);
  for (size_t i = 0; i < option_name_count; i++)
  {
    const char* option_name = option_names[i];
    const int option_name_length = MYON_String::Length(option_name);
    if (option_name_length < 1)
      continue;
    if (MYON_String::EqualOrdinal(option_name, option_name_length, s, option_name_length, true))
    {
      const char* tail = s + option_name_length;
      if (0 == tail[0] || Internal_ParseOptionTailSeparator() == tail[0])
        return tail;
    }
  }

  return nullptr;
}

static const MYON_String Internal_ParseOptionTail(
  const char* s
)
{
  for (;;)
  {
    if (nullptr == s || Internal_ParseOptionTailSeparator() != s[0] || s[1] <= MYON_String::Space)
      break;
    MYON_String tail(s+1);
    tail.TrimRight();
    int len = tail.Length();
    if (len < 1)
      break;
    if (len >= 2)
    {
      bool bTrim = false;
      if ('"' == tail[0] && '"' == tail[len - 1])
        bTrim = true;
      else if ('\'' == tail[0] && '\'' == tail[len - 1])
        bTrim = true;
      if (bTrim)
      {
        tail = MYON_String(s + 2, len - 2);
        if (tail.Length() < 1)
          break;
      }
    }
    return tail;
  }
  return MYON_String::EmptyString;
}


static bool Internal_ParseArg_HELP(const MYON_String arg)
{
  const char* tail = Internal_ParseOptionHead(static_cast<const char*>(arg), "help", "h", "?" );
  return (nullptr != tail && 0 == tail[0]);
}

static bool Internal_ParseArg_VERBOSE(const MYON_String arg)
{
  const char* tail = Internal_ParseOptionHead(static_cast<const char*>(arg), "verbose", "v" );
  return (nullptr != tail && 0 == tail[0]);
}

static bool Internal_ParseArg_OUT(const MYON_String exe_stem, const MYON_String arg, MYON_String& output_file_path)
{
  const char* tail = Internal_ParseOptionHead(static_cast<const char*>(arg), "out" );
  if (nullptr == tail)
    return false;

  if (0 == tail[0])
  {
    output_file_path = Internal_DefaultOutFileName(exe_stem);
  }
  else if (Internal_ParseOptionTailSeparator() == tail[0])
  {
    output_file_path = MYON_FileSystemPath::ExpandUser(Internal_ParseOptionTail(tail));
    output_file_path.TrimLeftAndRight();
    if (
      false == output_file_path.EqualOrdinal("stdout",true)
      && false == output_file_path.EqualOrdinal("stderr",true)
      && false == output_file_path.EqualOrdinal("null",true)
      && false == output_file_path.EqualOrdinal("dev/null",true)
      && MYON_FileSystem::IsDirectory(output_file_path)
      )
    {
      const MYON_wString wide_dir(output_file_path);
      const MYON_wString wide_fname(Internal_DefaultOutFileName(exe_stem));
      output_file_path = MYON_String(MYON_FileSystemPath::CombinePaths(wide_dir, false, wide_fname, true, false));
    }
  }
  else
  {
    output_file_path = MYON_String::EmptyString;
  }

  return true;
}

static bool Internal_ParseArg_RECURSE(const MYON_String arg, unsigned int& N)
{
  const char* tail = Internal_ParseOptionHead(static_cast<const char*>(arg), "recursive", "r" );
  if (nullptr == tail)
    return false;

  if (0 == tail[0])
  {
    N = 16; // sanity limit for default directory recursion depth; 
    return true;
  }
    
  if (Internal_ParseOptionTailSeparator() == tail[0])
  {
    const MYON_String num = Internal_ParseOptionTail(tail);
    if (num.IsNotEmpty())
    {
      unsigned int u = 0;
      const char* s1 = MYON_String::ToNumber(num, u, &u);
      if (nullptr != s1 && s1 > static_cast<const char*>(num) && u >= 1 && 0 == s1[0])
      {
        N = u;
        return true;
      }
    }
  }

  N = 0;
  return false;
}


static const MYON_String Internal_ParseArg_PATH(const MYON_String arg, unsigned int max_directory_tree_depth)
{
  MYON_String arg_full_path = MYON_FileSystemPath::ExpandUser(static_cast<const char*>(arg));
  arg_full_path.TrimLeftAndRight();

  if (MYON_FileSystem::IsFile(arg_full_path))
    return arg_full_path;

  if (max_directory_tree_depth > 0)
  {
    if (arg_full_path.Length() != 1 || false == MYON_FileSystemPath::IsDirectorySeparator(arg_full_path[0], true))
    {
      const char dir_seps[3] = { MYON_FileSystemPath::DirectorySeparatorAsChar, MYON_FileSystemPath::AlternateDirectorySeparatorAsChar, 0 };
      arg_full_path.TrimRight(dir_seps);
    }
    if (MYON_FileSystem::IsDirectory(arg_full_path))
      return arg_full_path;
  }

  return MYON_String::EmptyString;
}

static void Internal_PrintIntroduction(
  const MYON_String& example_test_exe_path,
  MYON_TextLog& text_log
)
{
  text_log.Print("Executable: %s\n", static_cast<const char*>(example_test_exe_path));
  text_log.Print("Compiled: " __DATE__ " " __TIME__ "\n");
  const MYON_String platform_name = Internal_PlatformId(true);
  MYON::Version();
  text_log.Print("Platform: %s\n", static_cast<const char*>(platform_name));
  text_log.Print(
    "Opennurbs version: %u.%u %04u-%02u-%02u %02u:%02u (%u)\n",
    MYON::VersionMajor(),
    MYON::VersionMinor(),
    MYON::VersionYear(),
    MYON::VersionMonth(),
    MYON::VersionDayOfMonth(),
    MYON::VersionHour(),
    MYON::VersionMinute(),
    MYON::VersionBranch()
  );
  text_log.Print("Current time: ");
  text_log.PrintCurrentTime();
  text_log.Print(" UCT\n");
}

int main( int argc, const char *argv[] )
{
  // If you are using OpenNURBS as a Windows DLL, then you MUST use
  // MYON::OpenFile() to open the file.  If you are not using OpenNURBS
  // as a Windows DLL, then you may use either MYON::OpenFile() or fopen()
  // to open the file.

  MYON_ErrorLog verbose_log;
  verbose_log.EnableLogging();

  MYON_String arg(nullptr != argv ? argv[0] : ((const char*)nullptr));
  arg.TrimLeftAndRight();
  const MYON_String example_test_exe_path(arg);
  MYON_String example_test_exe_stem;
  MYON_FileSystemPath::SplitPath(example_test_exe_path, nullptr, nullptr, &example_test_exe_stem, nullptr);
  if (example_test_exe_stem.IsEmpty())
    example_test_exe_stem = "example_test";

  MYON_TextLog print_to_stdout;
  print_to_stdout.SetIndentSize(2);

  int argi;
  if ( argc < 2 ) 
  {
    Internal_PrintHelp(example_test_exe_stem, print_to_stdout);
    return 0;
  }

  // Call once in your application to initialze opennurbs library
  MYON::Begin();

  // default text_log is to stdout
  MYON_TextLog* text_log = &print_to_stdout;
  FILE* text_log_fp = nullptr;
  
  unsigned int maximum_directory_depth = 0;

  bool bVerbose = false; // true = output will include a full listing of all 3dm file contents.

  MYONX_ErrorCounter err;
  unsigned int file_count = 0;
  unsigned int folder_count = 0;
  unsigned int directory_arg_counter = 0;

  bool bPrintIntroduction = true;

  for ( argi = 1; argi < argc; argi++ ) 
  {
    arg = argv[argi];
    arg.TrimLeftAndRight();
    if (arg.IsEmpty())
      continue;

    if (argi + 1 == argc && Internal_ParseArg_HELP(arg))
    {
      Internal_PrintHelp(example_test_exe_stem, text_log);
      if ( 0 == folder_count && 0 == file_count && 0 == err.TotalCount() )
        return 0;
      continue;
    }

    // check for -out:<file name> option
    MYON_String output_file_name;
    if ( Internal_ParseArg_OUT(example_test_exe_stem, arg, output_file_name) )
    {
      // need a new introduction when output destination changes.
      bPrintIntroduction = true;

      // change destination of text_log file
      if ( text_log != &print_to_stdout && text_log != &MYON_TextLog::Null )
      {
        delete text_log;
      }
      if ( text_log_fp )
      {
        MYON::CloseFile(text_log_fp);
        text_log_fp = nullptr;
      }

      text_log = &print_to_stdout;

      if (output_file_name.IsEmpty() || output_file_name.EqualOrdinal("stdout", true))
        continue;

      if (output_file_name.EqualOrdinal("null", true) || output_file_name.EqualOrdinal("dev/null", true))
      {
        text_log = &MYON_TextLog::Null;
        continue;
      }

      FILE* text_fp = MYON::OpenFile(output_file_name,"w");
      if (nullptr == text_fp)
      {
        err += Internal_InvalidArg(arg, *text_log);
        break;
      }

      text_log_fp = text_fp;
      text_log = new MYON_TextLog(text_log_fp);
      text_log->SetIndentSize(2);

      continue;
    }

    // check for -recursive:<directory depth> option
    if (Internal_ParseArg_RECURSE(arg, maximum_directory_depth))
    {
      continue;
    }

    if (Internal_ParseArg_VERBOSE(arg))
    {
      bVerbose = true;
      continue;
    }

    if (bPrintIntroduction)
    {
      bPrintIntroduction = false;
      Internal_PrintIntroduction(example_test_exe_path, *text_log);
    }

    const MYON_String arg_full_path = Internal_ParseArg_PATH(arg,maximum_directory_depth);
    if (arg_full_path.IsEmpty())
    {
      err += Internal_InvalidArg(arg, *text_log);
      break;
    }

    err += Internal_Test(
      maximum_directory_depth,
      arg_full_path,
      bVerbose,
      *text_log,
      directory_arg_counter,
      file_count,
      folder_count
    );
  }

  if (bPrintIntroduction)
  {
    bPrintIntroduction = false;
    Internal_PrintIntroduction(example_test_exe_path, *text_log);
  }

  if (folder_count > 0)
  {
    text_log->Print(
      "Tested %u 3dm files in %u folders.",
      file_count,
      folder_count
    );
  }
  else
  {
    text_log->Print(
      "Tested %u 3dm files.",
      file_count
    );
  }

  if (err.FailureCount() > 0)
    text_log->Print(" Failures. ");
  else if (err.ErrorCount() > 0) {
    text_log->Print(" Errors:\n");
    for (int vbli = 0; vbli < verbose_log.Count(); vbli++) {
      text_log->Print("    !! ");
      text_log->Print(verbose_log.Event(vbli).Description());
      text_log->Print("\n");
    }
  }
  else if (err.FailureCount() > 0)
    text_log->Print(" Warnings. ");
  else
    text_log->Print(" All tests passed. ");
  err.Dump(*text_log);
  text_log->PrintNewLine();

  if ( text_log != &print_to_stdout && text_log != &MYON_TextLog::Null )
  {
    delete text_log;
  }

  text_log = nullptr;

  if ( text_log_fp )
  {
    // close the text text_log file
    MYON::CloseFile( text_log_fp );
    text_log_fp = 0;
  }
  
  // OPTIONAL: Call just before your application exits to clean
  //           up opennurbs class definition information.
  //           Opennurbs will not work correctly after MYON::End()
  //           is called.
  MYON::End();

  return err.TotalCount();
}
