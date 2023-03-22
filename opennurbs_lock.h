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

#if !defined(OPENNURBS_LOCK_INC_)
#define OPENNURBS_LOCK_INC_

/*
Description:
  MYON_Lock is a thread safe lock semephore. It is implemented using
  platform specific compare and set functions.
*/
class MYON_CLASS MYON_Lock
{
public:
#if defined(MYON_COMPILER_CLANG)
  MYON_Lock() MYON_NOEXCEPT;
#else
  MYON_Lock() = default;
#endif
  ~MYON_Lock() = default;

  // Copy constructor and operator= are implicitly deleted because
  // __atomic_base<int, false> has a deleted copy constructor
  // MYON_Lock(const MYON_Lock&) = default;
  // MYON_Lock& operator=(const MYON_Lock&) = default;

  // MYON_Lock::InvalidLockValue (= -1) may never be used as a lock value.
  enum : int
  {
    UnlockedValue = 0,
    DefaultLockedValue = 1,
    InvalidLockValue = -1
  };

  /*
  Returns:
    Current lock value
    MYON_Lock::UnlockedValue indicates the the resource protected by the lock is available.
  */
  int IsLocked();

  /*
  Description:
    Calls GetLock(MYON_Lock::DefaultLockedValue);
  Returns:
    True if the lock state was unlocked
    and the current lock value was changed from MYON_Lock::UnlockedValue to MYON_Lock::DefaultLockedValue.
    False otherwise.
  */
  bool GetDefaultLock();

  /*
  Description:
    Calls ReturnLock(MYON_Lock::DefaultLockedValue);
  Returns:
    True if the lock state was locked with a locak value = MYON_Lock::DefaultLockedValue
    and the current lock value was changed from MYON_Lock::DefaultLockedValue to MYON_Lock::UnlockedValue.
    False otherwise.
  */
  bool ReturnDefaultLock();

  /*
  Parameters:
    lock_value - [in]
      any value except MYON_Lock::UnlockedValue or MYON_Lock::InvalidLockValue.
      Typically MYON_Lock::DefaultLockedValue is used.
  Returns:
    True if the lock_value parameter was valid and the current
    lock value was changed from MYON_Lock::UnlockedValue to lock_value.
    False otherwise.
  */
  bool GetLock(int lock_value);

  /*
  Parameters:
    lock_value - [in]
      any value except MYON_Lock::UnlockedValue or MYON_Lock::InvalidLockValue.
      Typically this is the value that was passed to GetLock().
  Returns:
    True if the lock_value parameter was valid and the current
    lock value was changed from that value to zero.
    False otherwise.
  */
  bool ReturnLock(int lock_value);

  /*
  Description:
    Unconditionally sets the lock value to MYON_Lock::UnlockedValue.
  Returns:
    previous value of the lock.
      MYON_Lock::UnlockedValue indicates the lock was available
      otherwise the lock passed to GetLock() is returned
  */
  int BreakLock();

private:
  // It is important that sizeof(MYON_Lock) = sizeof(int)
  // and that m_lock_value be an int.
#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  // C4251: 'MYON_Lock::m_lock_value': struct 'std::atomic<int>' 
  //         needs to have dll-interface to be used by clients of class 'MYON_Lock'
  // m_lock_value is private and all code that manages m_lock_value is explicitly implemented in the DLL.
private:
#if defined(MYON_COMPILER_CLANG) || defined(MYON_RUNTIME_LINUX)
    std::atomic<int> m_lock_value;
#else
  std::atomic<int> m_lock_value = MYON_Lock::UnlockedValue;
#endif
#pragma MYON_PRAGMA_WARNING_POP
};

#endif
