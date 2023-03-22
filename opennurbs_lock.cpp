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


#if defined(MYON_COMPILER_CLANG)
MYON_Lock::MYON_Lock() MYON_NOEXCEPT
: m_lock_value(MYON_Lock::UnlockedValue)
{}
#endif

/*
Description:
  MYON_TestIntLock is used as the test function in sleep locks that
  set 4 byte integer values.  If the input value of lock is test_value,
  then it is set to sleep_value.
Parameters:
  test_value - [in]
  sleep_value - [in]
  lock - [in/out]
    4 byte integer to test. 
    If lock == test_value, then lock is set to sleep_value. 
    If lock != test_value, then the value of lock is not changed
Returns:
  The input value of lock.
*/

/*
Description:
  MYON_SetIntLock is used to unconditionally 
  lock = lock_value and return the original value of lock
  as an atomic operation.
  It is intended to be used when a sleep lock is employed to make
  setting the lock thread safe.
Parameters:
  lock - [in/out]
    4 byte integer to set
  lock_value - [in]
Returns:
  The input value of lock.
*/

int MYON_Lock::IsLocked()
{
  const int is_locked = m_lock_value;
  return is_locked;
}

bool MYON_Lock::GetDefaultLock()
{
  return GetLock(MYON_Lock::DefaultLockedValue);
}

bool MYON_Lock::ReturnDefaultLock()
{
  return ReturnLock(MYON_Lock::DefaultLockedValue);
}

bool MYON_Lock::GetLock(int lock_value)
{
  if ( MYON_Lock::UnlockedValue == lock_value || MYON_Lock::InvalidLockValue == lock_value )
    return false;
  int test_value = MYON_Lock::UnlockedValue;
  const bool rc = m_lock_value.compare_exchange_strong(test_value, lock_value);
  return (rc && MYON_Lock::UnlockedValue == test_value);
}

bool MYON_Lock::ReturnLock(int lock_value)
{
  if ( MYON_Lock::UnlockedValue == lock_value || MYON_Lock::InvalidLockValue == lock_value )
    return false;
  int test_value = lock_value;
  const bool rc = m_lock_value.compare_exchange_strong(test_value,MYON_Lock::UnlockedValue);
  return (rc && lock_value == test_value);
}

int MYON_Lock::BreakLock()
{
  const int previous_lock_value = m_lock_value;
  m_lock_value = MYON_Lock::UnlockedValue;
  return previous_lock_value;
}

