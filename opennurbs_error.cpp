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


// openNURBS Geometry Library Errors and Warnings
//
//   If an error condition occurs during a openNURBS Geometry Library
//   computation, the MYON_Error() function is called, the computation is
//   stopped, and an error code (negative integer ) is returned.  If a
//   warning condition occurs during a Trout Lake Geometry Library 
//   computation, the MYON_Warning() function is called and the computation
//   continues.
//
//   MYON_GetErrorCount()
//   MYON_GetWarningCount()
//   MYON_Error()
//   MYON_Warning()
//

#define MYON_MAX_ERROR_MESSAGE_COUNT ((int)MYON_ErrorLog::MaximumEventCount)

static int MYON_ERROR_COUNT = 0;
static int MYON_WARNING_COUNT = 0;
static int MYON_MATH_ERROR_COUNT = 0;
class MYON_ErrorLog* MYON_ACTIVE_ERROR_LOG = nullptr;



static int MYON_DEBUG_ERROR_MESSAGE_OPTION = 0;


int MYON_GetErrorCount(void)
{
  return MYON_ERROR_COUNT;	
}	

int MYON_GetWarningCount(void)
{
  return MYON_WARNING_COUNT;	
}	

int MYON_GetMathErrorCount(void)
{
  return MYON_MATH_ERROR_COUNT;	
}	


int MYON_GetDebugErrorMessage(void)
{
  return MYON_DEBUG_ERROR_MESSAGE_OPTION?true:false;
}


void MYON_EnableDebugErrorMessage( int bEnableDebugErrorMessage )
{
  MYON_DEBUG_ERROR_MESSAGE_OPTION = bEnableDebugErrorMessage ? 1 : 0;
}


void MYON_MathError( 
        const char* sModuleName,
        const char* sErrorType,
        const char* sFunctionName
        )
{
  MYON_MATH_ERROR_COUNT++;

  if ( 0 == sModuleName)
    sModuleName = "";
  if ( 0 == sErrorType )
    sErrorType = "";
  if ( 0 == sFunctionName )
    sFunctionName = "";

  if ( 0 != sModuleName[0] || 0 != sErrorType[0] || 0 != sFunctionName[0] )
  {
    MYON_ErrorEx(__FILE__,__LINE__,sFunctionName,
              "Math library or floating point ERROR # %d module=%s type=%s function=%s",
              MYON_MATH_ERROR_COUNT, 
              sModuleName, // rhino.exe, opennurbs.dll, etc.
              sErrorType,   
              sFunctionName
              );
  }
  else
  {
    MYON_ErrorEx(__FILE__,__LINE__,sFunctionName,
              "Math library or floating point ERROR # %d",
              MYON_MATH_ERROR_COUNT
              );
  }
}	

static void MYON_IncrementErrorCount()
{
  MYON_ERROR_COUNT++;
}

static void MYON_IncrementWarningCount()
{
  MYON_WARNING_COUNT++;
}

void MYON_BrepIncrementErrorCount()
{
  MYON_ERROR_COUNT++;
  MYON_Brep::ErrorCount++;
} 

void MYON_SubDIncrementErrorCount()
{
  MYON_ERROR_COUNT++;
  MYON_SubD::ErrorCount++;
} 


bool MYON_IsNotValid()
{
  return false;
}

static bool MYON_Internal_PrintErrorMessage()
{
  return (0 != MYON_DEBUG_ERROR_MESSAGE_OPTION && MYON_ERROR_COUNT <= MYON_MAX_ERROR_MESSAGE_COUNT);
}

static bool MYON_Internal_PrintWarningMessage()
{
  return (0 != MYON_DEBUG_ERROR_MESSAGE_OPTION && MYON_WARNING_COUNT <= MYON_MAX_ERROR_MESSAGE_COUNT);
}


static bool MYON_Internal_LogError()
{
  return (nullptr != MYON_ACTIVE_ERROR_LOG);
}

static void MYON_Internal_RecordErrorEvent(
  const MYON_ErrorEvent& error_event,
  bool bPrint
  )
{
  if (nullptr != MYON_ACTIVE_ERROR_LOG)
    MYON_ACTIVE_ERROR_LOG->Append(error_event);


  if ( 0 == MYON_DEBUG_ERROR_MESSAGE_OPTION )
    return;

  if (false == bPrint)
    return;

  
  const MYON_ErrorEvent::Type error_type = error_event.EventType();
  MYON_String s;      
  if ( MYON_ErrorEvent::Type::Warning == error_type )
  {
    if (MYON_WARNING_COUNT == MYON_MAX_ERROR_MESSAGE_COUNT)
      s = MYON_String::FormatToString("MYON_WARNING # %d: ... suspending warning messages.", MYON_WARNING_COUNT);
    else
      s = MYON_String::FormatToString("MYON_WARNING # %d: ", MYON_WARNING_COUNT) + error_event.ToString();
  }
  else
  {
    if (MYON_ERROR_COUNT == MYON_MAX_ERROR_MESSAGE_COUNT)
      s = MYON_String::FormatToString("MYON_ERROR # %d: ... suspending error messages.", MYON_ERROR_COUNT);
    else
      s = MYON_String::FormatToString("MYON_ERROR # %d: ", MYON_ERROR_COUNT) + error_event.ToString();
  }

  if ( s.IsNotEmpty() )
  {
    int obsolete_type;
    switch (error_type)
    {
    case MYON_ErrorEvent::Type::Unset:
      obsolete_type = 0;
      break;
    case MYON_ErrorEvent::Type::Warning:
      obsolete_type = 0;
      break;
    case MYON_ErrorEvent::Type::Error:
      obsolete_type = 1;
      break;
    case MYON_ErrorEvent::Type::Assert:
      obsolete_type = 2;
      break;
    case MYON_ErrorEvent::Type::Custom:
      obsolete_type = 0;
      break;
    case MYON_ErrorEvent::Type::SubDError:
      obsolete_type = 0;
      break;
    case MYON_ErrorEvent::Type::NotValid:
      obsolete_type = 0;
      break;
    default:
      obsolete_type = 0;
      break;
    }
    MYON_ErrorMessage(obsolete_type,static_cast<const char*>(s));
  }
}

void MYON_VARGS_FUNC_CDECL MYON_Error(
  const char* sFileName, 
  int line_number,
  const char* sFormat,
  ...
)
{
  MYON_IncrementErrorCount();

  const bool bPrintErrorMessage = MYON_Internal_PrintErrorMessage();
  if ( bPrintErrorMessage || MYON_Internal_LogError() )
  {
    MYON_String description;
    if (sFormat && sFormat[0]) 
    {
      va_list args;
      va_start(args, sFormat);
      description.FormatVargs(sFormat, args);
      va_end(args);
    }

    const MYON_ErrorEvent error_event(
      MYON_ErrorEvent::Type::Error,
      sFileName,
      (unsigned int)line_number,
      nullptr,
      description
    );

    MYON_Internal_RecordErrorEvent(error_event, bPrintErrorMessage);
  }

}

void MYON_VARGS_FUNC_CDECL MYON_ErrorEx(
  const char* sFileName,
  int line_number, 
  const char* sFunctionName,
  const char* sFormat,
  ...
)
{
  MYON_IncrementErrorCount();

  const bool bPrintErrorMessage = MYON_Internal_PrintErrorMessage();
  if (bPrintErrorMessage || MYON_Internal_LogError())
  {
    MYON_String description;
    if (sFormat && sFormat[0])
    {
      va_list args;
      va_start(args, sFormat);
      description.FormatVargs(sFormat, args);
      va_end(args);
    }

    const MYON_ErrorEvent error_event(
      MYON_ErrorEvent::Type::Error,
      sFileName,
      (unsigned int)line_number,
      sFunctionName,
      description
    );

    MYON_Internal_RecordErrorEvent(error_event, bPrintErrorMessage);
  }

}

void MYON_VARGS_FUNC_CDECL MYON_Warning(
  const char* sFileName,
  int line_number,
  const char* sFormat,
  ...
)
{
  MYON_IncrementWarningCount();

  const bool bPrintErrorMessage = MYON_Internal_PrintWarningMessage();
  if ( bPrintErrorMessage || MYON_Internal_LogError())
  {
    MYON_String description;
    if (sFormat && sFormat[0]) 
    {
      va_list args;
      va_start(args, sFormat);
      description.FormatVargs(sFormat, args);
      va_end(args);
    }

    const MYON_ErrorEvent error_event(
      MYON_ErrorEvent::Type::Warning,
      sFileName,
      (unsigned int)line_number,
      nullptr,
      description
    );

    MYON_Internal_RecordErrorEvent(error_event, bPrintErrorMessage);
  }

}

void MYON_VARGS_FUNC_CDECL MYON_WarningEx(
  const char* sFileName, 
  int line_number,
  const char* sFunctionName,
  const char* sFormat,
  ...
)
{
  MYON_IncrementWarningCount();

  const bool bPrintErrorMessage = MYON_Internal_PrintWarningMessage();
  if ( bPrintErrorMessage || MYON_Internal_LogError())
  {
    MYON_String description;
    if (sFormat && sFormat[0]) 
    {
      va_list args;
      va_start(args, sFormat);
      description.FormatVargs(sFormat, args);
      va_end(args);
    }

    const MYON_ErrorEvent error_event(
      MYON_ErrorEvent::Type::Warning,
      sFileName,
      (unsigned int)line_number,
      sFunctionName,
      description
    );

    MYON_Internal_RecordErrorEvent(error_event, bPrintErrorMessage);
  }

}

void MYON_VARGS_FUNC_CDECL MYON_REMOVE_ASAP_AssertEx(
  int bCondition,
  const char* sFileName,
  int line_number,
  const char* sFunctionName,
  const char* sFormat,
  ...
)
{
  if ( false == bCondition)
  {

    const bool bPrintErrorMessage = MYON_Internal_PrintErrorMessage();
    if ( bPrintErrorMessage || MYON_Internal_LogError())
    {
      MYON_String description;
      if (sFormat && sFormat[0]) 
      {
        va_list args;
        va_start(args, sFormat);
        description.FormatVargs(sFormat, args);
        va_end(args);
      }

      const MYON_ErrorEvent error_event(
        MYON_ErrorEvent::Type::Warning,
        sFileName,
        (unsigned int)line_number,
        sFunctionName,
        description
      );

      MYON_Internal_RecordErrorEvent(error_event, bPrintErrorMessage);
    }
  }
}

////////////////////////////////////////////////////////////////////////
//
// MYON_ErrorEvent
//
MYON_ErrorEvent::MYON_ErrorEvent(const MYON_ErrorEvent& src)
{
  Internal_CopyFrom(src);
}

MYON_ErrorEvent& MYON_ErrorEvent::operator=(const MYON_ErrorEvent& src)
{
  if (this != &src)
  {
    Internal_CopyFrom(src);
  }
  return *this;
}

void MYON_ErrorEvent::Internal_CopyFrom(const MYON_ErrorEvent& src)
{
  m_event_type = src.m_event_type;
  m_line_number = src.m_line_number;
  m_file_name = nullptr;
  m_function_name = nullptr;
  m_description = nullptr;

  const size_t sizeof_buffer = sizeof(m_buffer);
  memcpy(m_buffer, src.m_buffer, sizeof_buffer);

  // buffer_capacity keeps code working if char changes to wchar_t
  const size_t buffer_capacity = sizeof_buffer/sizeof(m_buffer[0]); 
  const char* src_buffer = &src.m_buffer[0];

  if (nullptr != src.m_file_name && src.m_file_name >= src_buffer)
  {
    const size_t i = (src.m_file_name - src_buffer);
    if (i >= 0 && i < buffer_capacity)
      m_file_name = &m_buffer[i];
  }
  if (nullptr != src.m_function_name && src.m_function_name >= src_buffer)
  {
    const size_t i = (src.m_function_name - src_buffer);
    if (i >= 0 && i < buffer_capacity)
      m_function_name = &m_buffer[i];
  }
  if (nullptr != src.m_description && src.m_description >= src_buffer)
  {
    const size_t i = (src.m_description - src_buffer);
    if (i >= 0 && i < buffer_capacity)
      m_description = &m_buffer[i];
  }
}


MYON_ErrorEvent::MYON_ErrorEvent(
  MYON_ErrorEvent::Type event_type,
  const char* file_name,
  unsigned int line_number,
  const char* function_name,
  const char* description
)
  : m_event_type(event_type)
  , m_line_number(line_number)
{
  const size_t buffer_capacity = sizeof(m_buffer) / sizeof(m_buffer[0]);
  size_t file_name_length = MYON_String::Length(file_name);
  size_t function_name_length = MYON_String::Length(function_name);
  size_t description_length = MYON_String::Length(description);
  if (file_name_length + function_name_length + description_length + 3 > buffer_capacity)
  {
    const size_t cap = buffer_capacity / 4;
    if (file_name_length > cap )
    {
      file_name += (file_name_length - cap);
      file_name_length = cap;
    }
    if (file_name_length + function_name_length + description_length + 3 > buffer_capacity)
    {
      if (function_name_length > cap)
      {
        function_name += (function_name_length - cap);
        function_name_length = cap;
      }
    }
  }

  char* c = m_buffer;
  char* c1 = m_buffer + (buffer_capacity - 1);

  if (file_name_length > 0 && c < c1)
  {
    m_file_name = c;
    for (size_t i = 0; i < file_name_length && c < c1; i++)
      *c++ = file_name[i];
    if (c < c1)
      *c++ = 0;
  }

  if (function_name_length > 0 && c < c1)
  {
    m_function_name = c;
    for (size_t i = 0; i < function_name_length && c < c1; i++)
      *c++ = function_name[i];
    if (c < c1)
      *c++ = 0;
  }

  if (description_length > 0 && c < c1)
  {
    m_description = c;
    for (size_t i = 0; i < description_length && c < c1; i++)
      *c++ = description[i];
    if (c < c1)
      *c++ = 0;
  }

  *c1 = 0;
}

const MYON_ErrorEvent MYON_ErrorEvent::Create(
  MYON_ErrorEvent::Type event_type,
  const char* file_name,
  unsigned int line_number,
  const char* function_name,
  const char* description
)
{
  return MYON_ErrorEvent(
    event_type,
    file_name,
    line_number,
    function_name,
    description
  );
}

const char* MYON_ErrorEvent::FileName() const
{
  return (nullptr != m_file_name || 0 != m_file_name) ? m_file_name : "";
}

const char* MYON_ErrorEvent::FunctionName() const
{
  return (nullptr != m_function_name || 0 != m_function_name) ? m_function_name : "";
}

const char* MYON_ErrorEvent::Description() const
{
  return (nullptr != m_description || 0 != m_description) ? m_description : "";
}

unsigned int MYON_ErrorEvent::LineNumber() const
{
  return m_line_number;
}

MYON_ErrorEvent::Type MYON_ErrorEvent::EventType() const
{
  return m_event_type;
}

const char* MYON_ErrorEvent::TypeToString(
  MYON_ErrorEvent::Type event_type
)
{
  const char* s;
  switch (event_type)
  {
  case MYON_ErrorEvent::Type::Unset:
    s = "Unset";
    break;
  case MYON_ErrorEvent::Type::Warning:
    s = "Warning";
    break;
  case MYON_ErrorEvent::Type::Error:
    s = "Error";
    break;
  case MYON_ErrorEvent::Type::Assert:
    s = "Assert";
    break;
  case MYON_ErrorEvent::Type::Custom:
    s = "Custom";
    break;
  case MYON_ErrorEvent::Type::SubDError:
    s = "SubDError";
    break;
  case MYON_ErrorEvent::Type::NotValid:
    s = "NotValid";
    break;
  default:
    s = "Invalid";
    break;
  }

  return s;
}

const MYON_String MYON_ErrorEvent::ToString() const
{
  const char* function_name = FunctionName();
  const char* description = Description();
  if (0 != function_name[0])
  {
    return MYON_String::FormatToString(
      "%s.%u %s(): %s \"%s\"",
      FileName(), 
      LineNumber(), 
      function_name,
      MYON_ErrorEvent::TypeToString(m_event_type),
      description
    );
  }
  return MYON_String::FormatToString(
    "%s.%u: %s \"%s\"",
    FileName(), 
    LineNumber(), 
    MYON_ErrorEvent::TypeToString(m_event_type),
    description
  );
}

void MYON_ErrorEvent::Dump(
  class MYON_TextLog& text_log
) const
{
  const MYON_String s(ToString());
  text_log.Print("%s\n", static_cast<const char*>(s));
}


////////////////////////////////////////////////////////////////////////
//
// MYON_ErrorLog
//

bool MYON_ErrorLog::EnableLogging()
{
  if ( 0 == ((MYON__UINT_PTR)this) )
    return false;
  if (this == MYON_ACTIVE_ERROR_LOG)
    return true;
  if (nullptr != MYON_ACTIVE_ERROR_LOG)
    return false;
  if (Count() >= MYON_ErrorLog::MaximumEventCount)
    return false;

  MYON_ACTIVE_ERROR_LOG = this;
  return true;
}

void MYON_ErrorLog::DisableLogging()
{
  if (nullptr != MYON_ACTIVE_ERROR_LOG && this == MYON_ACTIVE_ERROR_LOG)
  {
    MYON_ACTIVE_ERROR_LOG = nullptr;
  }
}

void MYON_ErrorLog::Dump(
  class MYON_TextLog& text_log
) const
{
  const unsigned int count = Count();
  text_log.Print("Error log: %u events\n", count);
  MYON_TextLogIndent indent1(text_log);
  for (unsigned int i = 0; i < count; i++)
  {
    m_events[i].Dump(text_log);
  }
}


MYON_ErrorLog::~MYON_ErrorLog()
{
  if (MYON_ACTIVE_ERROR_LOG == this)
    MYON_ACTIVE_ERROR_LOG = nullptr;
}

unsigned int MYON_ErrorLog::Append(
  const MYON_ErrorEvent& error_event
)
{
  const unsigned int event_capacity = (unsigned int)(sizeof(m_events) / sizeof(m_events[0]));
  if (m_event_count >= event_capacity)
    return 0;

  if (m_event_count < event_capacity)
  {
    m_events[m_event_count++] = error_event;
    if (m_event_count == event_capacity && MYON_ACTIVE_ERROR_LOG == this)
    {
      // stop wasting time logging errors
      MYON_ACTIVE_ERROR_LOG = nullptr;
    }
  }
  return m_event_count;
}

unsigned int MYON_ErrorLog::Count() const
{
  return m_event_count <= MYON_ErrorLog::MaximumEventCount ? m_event_count : MYON_ErrorLog::MaximumEventCount;
}

const MYON_ErrorEvent& MYON_ErrorLog::Event(unsigned int i) const
{
  return (i < m_event_count) ? m_events[i] : MYON_ErrorEvent::Unset;
}

void MYON_ErrorLog::Clear()
{
  m_event_count = 0;
}
