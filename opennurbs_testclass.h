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

#if !defined(OPENNURBS_TEST_CLASS_INC_)
#define OPENNURBS_TEST_CLASS_INC_

#if defined(MYON_CPLUSPLUS)

#if !defined(OPENNURBS_NO_STD_MUTEX)

class MYON_CLASS MYON_TestClass
{
public:
  MYON_TestClass();
  ~MYON_TestClass();
  MYON_TestClass(const MYON_TestClass&);
  MYON_TestClass& operator=(const MYON_TestClass&);

public:

  /*
  The first instance of a MYON_TestClass has serial number 1.
  */
  const MYON__UINT64 SerialNumber;

  /*
  If this class was created by the copy constructor, then
  CopiedFrom is the serial number of the source class.
  Otherwise, CopiedFrom is zero.
  */
  const MYON__UINT64 CopiedFrom;

  /*
  Time this instance was constructed. 
  Seconds since Jan 1, 1970, UCT from C-runtime time(nullptr).
  */
  const MYON__UINT64 ConstructedAt;
  
  /*
  Returns:
    Number of MYON_TestClass instances that have been constructed.
  Remarks:
    Thread safe.
    If you need to know two or more of  ConstructedCount(), DestructedCount(),
    and CurrentCount() the same time, call GetCurrentAndConstructedCount().
  */
  static MYON__UINT64 ConstructedCount();

  /*
  Returns:
    Number of MYON_TestClass instances that have been destroyed.
  Remarks:
    Thread safe.
    If you need to know two or more of  ConstructedCount(), DestructedCount(),
    and CurrentCount() the same time, call GetCurrentAndConstructedCount().
  */
  static MYON__UINT64 DestructedCount();

  /*
  Returns:
    Number of MYON_TestClass instances that currently exist.
  Remarks:
    Thread safe.
    If you need to know two or more of  ConstructedCount(), DestructedCount(),
    and CurrentCount() the same time, call GetCurrentAndConstructedCount().
  */
  static MYON__UINT64 CurrentCount();

  /*
  Parameters:
    constructed_count - [out]
      Number of MYON_TestClass that have been constructed.
    current_count - [out]
      Number of MYON_TestClass that currently exist.
  Remarks:
    Thread safe.
  */
  static void GetCurrentAndConstructedCount(
    MYON__UINT64& constructed_count,
    MYON__UINT64& current_count
  );

  /*
  Returns:
    String value.
  Remarks:
    Thread safe.
  */
  const MYON_wString Value() const;

  /*
  Returns:
    String value.
  Remarks:
    Thread safe.
  */
  void SetValue(
    const MYON_wString s
    );

  /*
  Returns:
    String value.
  Remarks:
    Thread safe.
  */
  void Dump(class MYON_TextLog& text_log) const;

  /*
  Returns:
    A string with the format
    MYON_TestClass[<SERIAL_NUMBER>] \"<STRING_VALUE>\"
    where <SERIAL_NUMBER> is this->SerialNumber and <STRING_VALUE>
    is the string returned by this->Value().

  Remarks:
    Thread safe.
  */
  const MYON_wString ToString() const;

private:
#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_mutex is private and all code that manages m_mutex is explicitly implemented in the DLL.
  //
  // m_local_mutex is used for thread safe access to instance members
  mutable std::mutex m_local_mutex;
#pragma MYON_PRAGMA_WARNING_POP


  // MYON_wString is multi-thread safe
private:
  MYON_wString m_string;

private:
#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // These are private and all code that manages m_mutex is explicitly implemented in the DLL.
  // internal_counter_mutex protects access to internal_* counters.
  static std::mutex internal_counter_mutex;
  static MYON__UINT64 internal_CtorSerialNumberGenerator;
  static MYON__UINT64 internal_PopulationCounter;
  static MYON__UINT64 Internal_CounterHelper(
    int task, // 0 = construct, 1 = destruct, 2 = query
    MYON__UINT64* constructed_count,
    MYON__UINT64* current_count
  );
#pragma MYON_PRAGMA_WARNING_POP
};

#endif

#endif

#endif
