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

#if !defined(OPENNURBS_ERROR_INC_)
#define OPENNURBS_ERROR_INC_

/*
// Macros used to log errors and warnings.  The MYON_Warning() and MYON_Error()
// functions are defined in opennurbs_error.cpp.
*/
#define MYON_ERROR(msg) MYON_ErrorEx(__FILE__,__LINE__,OPENNURBS__FUNCTION__,msg)
#define MYON_WARNING(msg) MYON_WarningEx(__FILE__,__LINE__,OPENNURBS__FUNCTION__,msg)
#define MYON_ASSERT_OR_RETURN(cond,returncode) do{if (!(cond)) {MYON_ErrorEx(__FILE__,__LINE__,OPENNURBS__FUNCTION__, #cond " is false");return(returncode);}}while(0)
#define MYON_ASSERT_OR_RETURNVOID(cond) do{if (!(cond)) {MYON_ErrorEx(__FILE__,__LINE__,OPENNURBS__FUNCTION__, #cond " is false");return;}}while(0)

// Do not use MYON_ASSERT. If a condition can be checked by MYON_ASSERT, then the
// code must be written detect and respond to that condition. This define will
// be deleted ASAP. It is being used to detect situations where a crash will
// occur and then letting the crash occur. 
#define MYON_ASSERT(cond) MYON_REMOVE_ASAP_AssertEx(cond,__FILE__,__LINE__,OPENNURBS__FUNCTION__, #cond " is false")


MYON_BEGIN_EXTERNC

/*
// All error/warning messages are sent to MYON_ErrorMessage().  Replace the
// default handler (defined in opennurbs_error_message.cpp) with something
// that is appropriate for debugging your application.
*/
MYON_DECL
void MYON_ErrorMessage( 
       int,         /* 0 = warning message, 1 = serious error message, 2 = assert failure */
       const char*  
       ); 

/*
Returns:
  Number of opennurbs errors since program started.
*/
MYON_DECL
int MYON_GetErrorCount(void);

/*
Returns:
  Number of opennurbs warnings since program started.
*/
MYON_DECL
int MYON_GetWarningCount(void);

/*
Returns:
  Number of math library or floating point errors that have 
  been handled since program started.
*/
MYON_DECL
int MYON_GetMathErrorCount(void);


MYON_DECL
int MYON_GetDebugErrorMessage(void);

MYON_DECL
void MYON_EnableDebugErrorMessage( int bEnableDebugErrorMessage );

MYON_DECL
void MYON_VARGS_FUNC_CDECL MYON_Error(
  const char* file_name, /* __FILE__ will do fine */
  int line_number,       /* __LINE__ will do fine */
  const char* format,    /* format string */
  ...                    /* format ags */
  );

MYON_DECL
void MYON_VARGS_FUNC_CDECL MYON_ErrorEx(
  const char* file_name,      /* __FILE__ will do fine */
  int line_number,            /* __LINE__ will do fine */
  const char* function_name,  /* OPENNURBS__FUNCTION__ will do fine */
  const char* format,         /* format string */
  ...                         /* format ags */
  );

MYON_DECL
void MYON_VARGS_FUNC_CDECL MYON_Warning(
  const char* file_name, /* __FILE__ will do fine */
  int line_number,       /* __LINE__ will do fine */
  const char* format,    /* format string */
  ...                    /* format ags */
  );

MYON_DECL
void MYON_VARGS_FUNC_CDECL MYON_WarningEx(
  const char* file_name,      /* __FILE__ will do fine */
  int line_number,            /* __LINE__ will do fine */
  const char* function_name,  /*OPENNURBS__FUNCTION__ will do fine */
  const char* format,         /* format string */
  ...                         /* format ags */
  );

// Ideally - these "assert" functions will be deleted when the SDK can be changed.
MYON_DECL
void MYON_VARGS_FUNC_CDECL MYON_REMOVE_ASAP_AssertEx(
  int,        // if false, error is flagged
  const char* file_name,      /* __FILE__ will do fine */
  int line_number,            /* __LINE__ will do fine */
  const char* function_name,  /* OPENNURBS__FUNCTION__ will do fine */
  const char* format,         /* format string */
  ...                         /* format ags */
  );

MYON_DECL
void MYON_MathError( 
  const char*, /* sModuleName */
  const char*, /* sErrorType */
  const char*  /* sFunctionName */
  );

MYON_END_EXTERNC

#if defined(MYON_CPLUSPLUS)

class MYON_CLASS MYON_ErrorEvent
{
public:
  enum class Type : unsigned char
  {
    Unset = 0,
    Warning = 1,   // call to MYON_WARNING / MYON_Warning / MYON_WarningEx
    Error = 2,     // call to MYON_ERROR / MYON_Error / MYON_ErrorEx
    Assert = 3,    // MYON_ASSERT (do not use MYON_ASSERT - write code that handles errors and calls MYON_ERROR)
    Custom = 4,
    SubDError = 5, // call to MYON_SubDIncrementErrorCount()
    BrepError = 6, // call to MYON_BrepIncrementErrorCount()
    NotValid = 7   // call to MYON_IsNotValid()
  };

  static const char* TypeToString(
    MYON_ErrorEvent::Type event_type
  );

  const MYON_String ToString() const;

public:
  MYON_ErrorEvent() = default;
  ~MYON_ErrorEvent() = default;
  MYON_ErrorEvent(const MYON_ErrorEvent&);
  MYON_ErrorEvent& operator=(const MYON_ErrorEvent&);

  MYON_ErrorEvent(
    MYON_ErrorEvent::Type event_type,
    const char* file_name,
    unsigned int line_number,
    const char* function_name,
    const char* description
  );

  static const MYON_ErrorEvent Create(
    MYON_ErrorEvent::Type event_type,
    const char* file_name,
    unsigned int line_number,
    const char* function_name,
    const char* description
  );

  static const MYON_ErrorEvent Unset;

  const char* FileName() const;
  const char* FunctionName() const;
  const char* Description() const;
  unsigned int LineNumber() const;
  MYON_ErrorEvent::Type EventType() const;

  void Dump(
    class MYON_TextLog& text_log
  ) const;

private:
  friend class MYON_ErrorLog;

  MYON_ErrorEvent::Type m_event_type = MYON_ErrorEvent::Type::Unset;
  unsigned char m_reserved1 = 0;
  unsigned short m_reserved2 = 0;
  unsigned int m_line_number = 0;
  const char* m_file_name = nullptr;
  const char* m_function_name = nullptr;
  const char* m_description = nullptr;
  char m_buffer[128] = {};

  void Internal_CopyFrom(const MYON_ErrorEvent& src);
};


class MYON_CLASS MYON_ErrorLog
{
public:
  enum : unsigned int
  {
    MaximumEventCount = 32
  };
public:
  MYON_ErrorLog() = default;
  virtual ~MYON_ErrorLog();
  MYON_ErrorLog(const MYON_ErrorLog&) = default;
  MYON_ErrorLog& operator=(const MYON_ErrorLog&) = default;

  /*
  Parameters:
    error_event - [in]
      event to add
  Returns:
    0: Event not added because maximum capacity reached.
    >0: Number of events after adding error_event.
  */
  virtual
  unsigned int Append(
    const MYON_ErrorEvent& error_event
  );

  /*
  Returns:
    Total number of error events.
  */
  unsigned int Count() const;

  /*
  Parameters:
    i - [in]
      zero based event index.
  Returns
    Event at specified index or MYON_ErrorEvent::Unset if the index is out of range.
  */
  const MYON_ErrorEvent& Event(unsigned int i) const;

  void Clear();

  /*
  Returns:
    True if up to MYON_ErrorLog::MaximumErrorCount error events will be saved in this to this error log.
    False if another error log is active.
  */
  bool EnableLogging();

  /*
  Description:
    Stop logging errors to this error log.
  */
  void DisableLogging();

  void Dump(
    class MYON_TextLog& text_log
  ) const;

protected:
  unsigned int m_event_count = 0;
  MYON_ErrorEvent m_events[MYON_ErrorLog::MaximumEventCount];
};

#endif

#endif
