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

//////////////////////////////////////////////////////////////////////////
//
// MYON_ComponentStatus bits
//

#define SELECTED_BIT (0x01U)
#define SELECTED_PERSISTENT_BIT (0x02U)
#define SELECTED_MASK   (SELECTED_BIT | SELECTED_PERSISTENT_BIT)
#define HIGHLIGHTED_BIT (0x04U)
#define LOCKED_BIT     (0x08U)
#define HIDDEN_BIT     (0x10U)

// A mark is a tool used in a wide variety of ways by calculations. 
// Its value is unpredictable outside the scope of a specific code block. 
// High quality calculations will save and restore mark state, 
// but this is not always the case.
// This state is not saved in archives.
#define RUNTIME_MARK_BIT (0x20U)


#define DELETED_BIT    (0x40U)

#define DAMAGED_BIT    (0x80U)

// Do NOT include RUNTIME_MARK_BIT in ALL_MASK
#define ALL_MASK (SELECTED_MASK|HIGHLIGHTED_BIT|LOCKED_BIT|HIDDEN_BIT|DELETED_BIT|DAMAGED_BIT)

MYON_ComponentState MYON_ComponentStateFromUnsigned(
  unsigned int state_as_unsigned
  )
{
  switch (state_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ComponentState::Unset);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ComponentState::Clear);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ComponentState::NotSelected);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ComponentState::Selected);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ComponentState::SelectedPersistent);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ComponentState::NotHighlighted);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ComponentState::Highlighted);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ComponentState::NotHidden);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ComponentState::Hidden);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ComponentState::NotLocked);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ComponentState::Locked);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ComponentState::NotDamaged);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ComponentState::Damaged);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ComponentState::NotDeleted);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ComponentState::Deleted);
  }

  return MYON_ComponentState::Unset;
}


//enum STATE_FILTER : unsigned int
//{
//  selected = 0,
//  highlighted = 1,
//  hidden = 2,
//  locked = 4,
//  damaged = 8
//};

MYON_ComponentStatus::MYON_ComponentStatus(
  MYON_ComponentState state
  )
{
  switch ( state)
  {
  case MYON_ComponentState::Selected:
    m_status_flags = SELECTED_BIT;
    break;
  case MYON_ComponentState::SelectedPersistent:
    m_status_flags = SELECTED_BIT | SELECTED_PERSISTENT_BIT;
    break;
  case MYON_ComponentState::Highlighted:
    m_status_flags = HIGHLIGHTED_BIT;
    break;
  case MYON_ComponentState::Hidden:
    m_status_flags = HIDDEN_BIT;
    break;
  case MYON_ComponentState::Locked:
    m_status_flags = LOCKED_BIT;
    break;
  case MYON_ComponentState::RuntimeMarkSet:
    m_status_flags = RUNTIME_MARK_BIT;
    break;
  case MYON_ComponentState::Deleted:
    m_status_flags = DELETED_BIT;
    break;
  case MYON_ComponentState::Damaged:
    m_status_flags = DAMAGED_BIT;
    break;
  default:
    m_status_flags = 0U;
    break;
  }
}

bool MYON_ComponentStatus::operator==(MYON_ComponentStatus b)
{
  return (m_status_flags&ALL_MASK) == (b.m_status_flags&ALL_MASK);
}

bool MYON_ComponentStatus::operator!=(MYON_ComponentStatus b)
{
  return (m_status_flags&ALL_MASK) != (b.m_status_flags&ALL_MASK);
}

const MYON_ComponentStatus MYON_ComponentStatus::LogicalAnd(MYON_ComponentStatus lhs, MYON_ComponentStatus rhs)
{
  MYON_ComponentStatus x;
  x.m_status_flags = (lhs.m_status_flags & rhs.m_status_flags);
  return x;
}

const MYON_ComponentStatus MYON_ComponentStatus::LogicalOr(MYON_ComponentStatus lhs, MYON_ComponentStatus rhs)
{
  MYON_ComponentStatus x;
  x.m_status_flags = (lhs.m_status_flags | rhs.m_status_flags);
  return x;
}

bool MYON_ComponentStatus::StatusCheck(
  MYON_ComponentStatus candidate,
  MYON_ComponentStatus status_pass,
  MYON_ComponentStatus status_fail
)
{
  if (0 != MYON_ComponentStatus::LogicalAnd(candidate, status_pass).m_status_flags)
    return true;
  if (0 != MYON_ComponentStatus::LogicalAnd(candidate, status_fail).m_status_flags)
    return false;
  if (0 == status_fail.m_status_flags)
    return true;
  if (0 != status_pass.m_status_flags)
    return false;
  return true;
}

bool MYON_ComponentStatus::IsClear() const
{
  return (0 == (m_status_flags&ALL_MASK));
}

bool MYON_ComponentStatus::IsNotClear() const
{
  return (0 != (m_status_flags&ALL_MASK));
}

unsigned int MYON_ComponentStatus::SetStatus(
  MYON_ComponentStatus status_to_copy
  )
{
  unsigned char s1 = (status_to_copy.m_status_flags&ALL_MASK);
  if ( 0 == (SELECTED_BIT & s1) )
    s1 &= ~SELECTED_PERSISTENT_BIT;
  if (s1 != (m_status_flags&ALL_MASK))
  {
    const unsigned char mark = (m_status_flags&RUNTIME_MARK_BIT);
    m_status_flags = (s1|mark);
    return 1;
  }
  return 0;
}

unsigned int MYON_ComponentStatus::SetStates(
  MYON_ComponentStatus states_to_set
  )
{
  unsigned char s1 = (m_status_flags&ALL_MASK);
  unsigned char mask = (ALL_MASK & states_to_set.m_status_flags);

  if (0 == (SELECTED_BIT & mask))
  {
    // no changes to s1's selected status
    mask &= ~SELECTED_PERSISTENT_BIT;
  }
  else
  {
    // states_to_set specifies setting selected or selected_persistent.
    //
    // Clear the persistent bit on s1.
    // If s1's SELECTED_PERSISTENT_BIT it is supposed to remain set, 
    // then mask's SELECTED_PERSISTENT_BIT is set and the 
    // s1 bit will get set by the s1 |= mask line below.
    s1 &= ~SELECTED_PERSISTENT_BIT;
  }

  s1 |= mask;
  if (s1 != (m_status_flags&ALL_MASK))
  {
    const unsigned char mark = (m_status_flags&RUNTIME_MARK_BIT);
    m_status_flags = (s1|mark);
    return 1;
  }
  return 0;
}

unsigned int MYON_ComponentStatus::ClearStates(
  MYON_ComponentStatus states_to_clear
  )
{
  unsigned char s1 = (m_status_flags&ALL_MASK);

  unsigned char mask = ~(ALL_MASK & states_to_clear.m_status_flags);

  switch (SELECTED_MASK & mask)
  {
  case 0: 
    // states_to_clear.IsSelectedPersistent() is true.
    // This means all selection states must be cleared from s1.
    // We are in this case because 0 = (SELECTED_MASK & mask)
    // so the s1 &= mask line below will remove any set selection
    // states from s1.
    break;

  case SELECTED_PERSISTENT_BIT:
    // states_to_clear.IsSelected() is true.
    // states_to_clear.IsSelectedPersistent() is false.
    // That is SELECTED_BIT = (SELECTED_MASK & states_to_clear.m_status_flags)
    // If s1 is selected persistent, it must stay selected.
    // If s1 is selected but not persistent, its selection state must be cleared.
    if (SELECTED_MASK == (SELECTED_MASK & s1))
    {
      // s1 is selected persistent and must stay that way
      mask |= SELECTED_MASK;
    }
    else
    {
      // s1 is not selected persistent and must end up not selected.
      mask &= ~SELECTED_MASK;
    }
    break;

  default:
    // states_to_clear.IsSelected() is false.
    // No changes to s1's selection state.
    mask |= SELECTED_MASK;
  }

  s1 &= mask;
  if (s1 != (m_status_flags & ALL_MASK))
  {
    // 29 Dec 2022, Mikko, RH-71974:
    // This is wrong, selected and highlighted are separate states. A thing can be not selected but
    // still highlighted.
    /*
    // If input was selected and highlighted,
    // and output is not selected,
    // and hightlight and not explictily cleared,
    // then preserve highlight sync by auto-clearing highlight.
    if ( 0 == (SELECTED_MASK & s1)
      && 0 != (SELECTED_MASK & m_status_flags)
      && 0 != (HIGHLIGHTED_BIT & m_status_flags)
      && 0 != (HIGHLIGHTED_BIT & s1)
      )
    { 
      // Input was selected and hightlighted,
      // output is not selected.
      // Clear highlight automatically.
      s1 &= ~HIGHLIGHTED_BIT;
    }
    */

    // preserve value of runtime mark bit on m_status_flags
    const unsigned char mark = (m_status_flags&RUNTIME_MARK_BIT);

    // change m_status_flags to new value
    m_status_flags = (s1|mark);

    return 1;
  }

  return 0;
}


MYON_ComponentState MYON_ComponentStatus::SelectedState() const
{
  switch ((m_status_flags & SELECTED_MASK))
  {
  case 0U:
    return MYON_ComponentState::NotSelected;

  case SELECTED_BIT:
    return MYON_ComponentState::Selected;

  case (SELECTED_BIT|SELECTED_PERSISTENT_BIT):
    return MYON_ComponentState::SelectedPersistent;
  }

  // error
  return MYON_ComponentState::NotSelected;
}

unsigned int MYON_ComponentStatus::SetSelectedState(
    bool bSelectedState,
    bool bPersistent,
    bool bSynchronizeHighlight
  )
{
  if (bSelectedState)
  {
    return 
      bPersistent
      ? SetSelectedState(MYON_ComponentState::SelectedPersistent, bSynchronizeHighlight)
      : SetSelectedState(MYON_ComponentState::Selected, bSynchronizeHighlight)
      ;
  }
  return SetSelectedState(MYON_ComponentState::NotSelected, bSynchronizeHighlight);
}

unsigned int MYON_ComponentStatus::SetSelectedState(
  MYON_ComponentState selected_state,
  bool bSynchronizeHighlight
  )
{
  bool bChanged = false;
  switch (selected_state)
  {
  case MYON_ComponentState::NotSelected:
    if ( 0 != ClearStates(MYON_ComponentStatus::Selected) )
      bChanged = true;
    if ( bSynchronizeHighlight && 0 != ClearStates(MYON_ComponentStatus::Highlighted) )
      bChanged = true;
    break;

  case MYON_ComponentState::Selected:
    if ( 0 != SetStates(MYON_ComponentStatus::Selected) )
      bChanged = true;
    if ( bSynchronizeHighlight && 0 != SetStates(MYON_ComponentStatus::Highlighted) )
      bChanged = true;
    break;

  case MYON_ComponentState::SelectedPersistent:
    if ( 0 != SetStates(MYON_ComponentStatus::SelectedPersistent) )
      bChanged = true;
    if ( bSynchronizeHighlight && 0 != SetStates(MYON_ComponentStatus::Highlighted) )
      bChanged = true;
    break;
  default:
    break;
  }

  return bChanged ? 1U : 0U;
}

bool MYON_ComponentStatus::IsSelected() const
{
  return 0 != (m_status_flags & SELECTED_BIT);
}

bool MYON_ComponentStatus::IsSelectedPersistent() const
{
  return ( (SELECTED_BIT | SELECTED_PERSISTENT_BIT) == (m_status_flags & SELECTED_MASK) );
}

unsigned int MYON_ComponentStatus::SetHighlightedState(
  bool bIsHighlighted
  )
{
  return bIsHighlighted ? SetStates(MYON_ComponentStatus::Highlighted) : ClearStates(MYON_ComponentStatus::Highlighted);
}

bool MYON_ComponentStatus::IsHighlighted() const
{
  return 0 != (m_status_flags & HIGHLIGHTED_BIT);
}

bool MYON_ComponentStatus::RuntimeMark() const
{
  return ( 0 != (m_status_flags & RUNTIME_MARK_BIT) );

}

bool MYON_ComponentStatus::IsMarked(
  MYON__UINT8 mark_bits
) const
{
  return (0 == mark_bits) ? (0 != (m_status_flags & RUNTIME_MARK_BIT)) : (mark_bits == m_mark_bits);
}


bool MYON_ComponentStatus::SetRuntimeMark(
  bool bRuntimeMark
)
{
  return bRuntimeMark ? SetRuntimeMark() : ClearRuntimeMark();
}

bool MYON_ComponentStatus::SetRuntimeMark()
{
  const unsigned char c = (m_status_flags | RUNTIME_MARK_BIT);
  if (c != m_status_flags)
  {
    m_status_flags = c;
    return true;
  }
  return false;
}

bool MYON_ComponentStatus::ClearRuntimeMark()
{
  const unsigned char c = (m_status_flags & ~RUNTIME_MARK_BIT);
  if (c != m_status_flags)
  {
    m_status_flags = c;
    return true;
  }
  return false;
}

MYON__UINT8 MYON_ComponentStatus::MarkBits() const
{
  return m_mark_bits;
}

MYON__UINT8 MYON_ComponentStatus::SetMarkBits(MYON__UINT8 bits)
{
  const MYON__UINT8 rc = m_mark_bits;
  m_mark_bits = bits;
  return rc;
}


unsigned int MYON_ComponentStatus::SetHiddenState(
  bool bIsHidden
  )
{
  return bIsHidden ? SetStates(MYON_ComponentStatus::Hidden) : ClearStates(MYON_ComponentStatus::Hidden);
}

bool MYON_ComponentStatus::IsHidden() const
{
  return 0 != (m_status_flags & HIDDEN_BIT);
}

unsigned int MYON_ComponentStatus::SetLockedState(
  bool bIsLocked
  )
{
  return bIsLocked ? SetStates(MYON_ComponentStatus::Locked) : ClearStates(MYON_ComponentStatus::Locked);
}

bool MYON_ComponentStatus::IsLocked() const
{
  return 0 != (m_status_flags & LOCKED_BIT);
}

unsigned int MYON_ComponentStatus::SetDeletedState(
  bool bIsDeleted
  )
{
  return bIsDeleted ? SetStates(MYON_ComponentStatus::Deleted) : ClearStates(MYON_ComponentStatus::Deleted);
}

bool MYON_ComponentStatus::IsDeleted() const
{
  return 0 != (m_status_flags & DELETED_BIT);
}



unsigned int MYON_ComponentStatus::SetDamagedState(
  bool bIsDamaged
  )
{
  return bIsDamaged ? SetStates(MYON_ComponentStatus::Damaged) : ClearStates(MYON_ComponentStatus::Damaged);
}

bool MYON_ComponentStatus::IsDamaged() const
{
  return 0 != (m_status_flags & DAMAGED_BIT);
}

bool MYON_ComponentStatus::operator==(const MYON_ComponentStatus& other) const
{
  return m_status_flags == other.m_status_flags;
}

bool MYON_ComponentStatus::operator!=(const MYON_ComponentStatus& other) const
{
  return m_status_flags != other.m_status_flags;
}

bool MYON_ComponentStatus::AllEqualStates(
  MYON_ComponentStatus states_filter,
  MYON_ComponentStatus comparand
  ) const
{
  unsigned char mask = (states_filter.m_status_flags & ALL_MASK);
  mask &= ~SELECTED_PERSISTENT_BIT;
  if (0 == mask)
    return false;
  
  unsigned char s1 = mask & m_status_flags;
  unsigned char s2 = mask & comparand.m_status_flags;

  return (s1 == s2);
}

bool MYON_ComponentStatus::SomeEqualStates(
  MYON_ComponentStatus states_filter,
  MYON_ComponentStatus comparand
  ) const
{
  unsigned char mask = (states_filter.m_status_flags & ALL_MASK);
  mask &= ~SELECTED_PERSISTENT_BIT;
  if (0 == mask)
    return false;

  unsigned char s1 = mask & m_status_flags;
  unsigned char s2 = mask & comparand.m_status_flags;

  if (0 != (s1&s2))
    return true; // some set states are equal

  s1 = mask & ~s1;
  s2 = mask & ~s2;
  if (0 != (s1&s2))
    return true; // some clear states are equal

  return false;
}


bool MYON_ComponentStatus::NoEqualStates(
  MYON_ComponentStatus states_filter,
  MYON_ComponentStatus comparand
  ) const
{
  unsigned char mask = (states_filter.m_status_flags & ALL_MASK);
  mask &= ~SELECTED_PERSISTENT_BIT;
  if (0 == mask)
    return false;

  unsigned char s1 = mask & m_status_flags;
  unsigned char s2 = mask & comparand.m_status_flags;

  return (mask == (s1 ^ s2));
}

MYON_AggregateComponentStatus::MYON_AggregateComponentStatus(const MYON_AggregateComponentStatusEx& src)
{
  memcpy(this, &src, sizeof(*this));
}

MYON_AggregateComponentStatus& MYON_AggregateComponentStatus::operator=(const MYON_AggregateComponentStatusEx& src)
{
  memcpy(this, &src, sizeof(*this));
  return *this;
}

MYON_AggregateComponentStatusEx::MYON_AggregateComponentStatusEx(const MYON_AggregateComponentStatus& src)
  : MYON_AggregateComponentStatus(src)
{
  Internal_ChangeStatusSerialNumber();
}

MYON_AggregateComponentStatusEx& MYON_AggregateComponentStatusEx::operator=(const MYON_AggregateComponentStatus& src)
{
  MYON_AggregateComponentStatus::operator=(src);
  Internal_ChangeStatusSerialNumber();
  return *this;
}

MYON__UINT64 MYON_AggregateComponentStatusEx::ComponentStatusSerialNumber() const
{
  return m_component_status_serial_number;
}

void MYON_AggregateComponentStatusEx::Internal_ChangeStatusSerialNumber()
{
  m_component_status_serial_number = MYON_NextContentSerialNumber();
}

bool MYON_AggregateComponentStatus::ClearAllStates()
{
  if (m_current <= 1)
  {
    unsigned char c = m_current;
    unsigned int n = m_component_count;
    *this = MYON_AggregateComponentStatus::Empty;
    m_current = c;
    n = m_component_count;
    return true;
  }
  return false;
}

bool MYON_AggregateComponentStatusEx::ClearAllStates()
{
  Internal_ChangeStatusSerialNumber();
  return MYON_AggregateComponentStatus::ClearAllStates();
}

bool MYON_AggregateComponentStatus::ClearAggregateStatus(
  MYON_ComponentStatus states_to_clear
  )
{
  if (states_to_clear.m_status_flags == MYON_ComponentStatus::AllSet.m_status_flags || 0 == m_component_count)
    return ClearAllStates();

  if (1 == m_current)
  {
    m_aggregate_status.ClearStates(states_to_clear);
    const unsigned char s1 = m_aggregate_status.m_status_flags;
    if (0 == (SELECTED_BIT & s1))
    {
      m_selected_count = 0;
      m_selected_persistent_count = 0;
    }
    else if (0 == (SELECTED_PERSISTENT_BIT & s1))
    {
      m_selected_count -= m_selected_persistent_count;
      m_selected_persistent_count = 0;
    }
    if (0 == (HIGHLIGHTED_BIT & s1))
      m_highlighted_count = 0;
    if (0 == (LOCKED_BIT & s1))
      m_locked_count = 0;
    if (0 == (HIDDEN_BIT & s1))
      m_hidden_count = 0;
    if (0 == (DAMAGED_BIT & s1))
      m_damaged_count = 0;
    return true;
  }

  return false;
}

bool MYON_AggregateComponentStatusEx::ClearAggregateStatus(
  MYON_ComponentStatus states_to_clear
)
{
  Internal_ChangeStatusSerialNumber();
  return MYON_AggregateComponentStatus::ClearAggregateStatus(states_to_clear);
}

bool MYON_AggregateComponentStatus::IsEmpty() const
{
  return (0 == m_current);
}

bool MYON_AggregateComponentStatusEx::IsEmpty() const
{
  return MYON_AggregateComponentStatus::IsEmpty();
}

bool MYON_AggregateComponentStatus::IsCurrent() const
{
  return (1 == m_current);
}

bool MYON_AggregateComponentStatusEx::IsCurrent() const
{
  return MYON_AggregateComponentStatus::IsCurrent();
}

void MYON_AggregateComponentStatus::MarkAsNotCurrent()
{
  if (2 != m_current)
  {
    *this = MYON_AggregateComponentStatus::Empty;
    m_current = 2;
  }
}

void MYON_AggregateComponentStatusEx::MarkAsNotCurrent()
{
  Internal_ChangeStatusSerialNumber();
  return MYON_AggregateComponentStatus::MarkAsNotCurrent();
}

MYON_ComponentStatus MYON_AggregateComponentStatus::AggregateStatus() const
{
  return m_aggregate_status;
}

MYON_ComponentStatus MYON_AggregateComponentStatusEx::AggregateStatus() const
{
  return MYON_AggregateComponentStatus::AggregateStatus();
}

unsigned int MYON_AggregateComponentStatus::ComponentCount() const
{
  return m_component_count;
}

unsigned int MYON_AggregateComponentStatusEx::ComponentCount() const
{
  return MYON_AggregateComponentStatus::ComponentCount();
}

unsigned int MYON_AggregateComponentStatus::SelectedCount() const
{
  return m_selected_count;
}

unsigned int MYON_AggregateComponentStatusEx::SelectedCount() const
{
  return MYON_AggregateComponentStatus::SelectedCount();
}

unsigned int MYON_AggregateComponentStatus::SelectedPersistentCount() const
{
  return m_selected_persistent_count;
}

unsigned int MYON_AggregateComponentStatusEx::SelectedPersistentCount() const
{
  return MYON_AggregateComponentStatus::SelectedPersistentCount();
}

unsigned int MYON_AggregateComponentStatus::HighlightedCount() const
{
  return m_highlighted_count;
}

unsigned int MYON_AggregateComponentStatusEx::HighlightedCount() const
{
  return MYON_AggregateComponentStatus::HighlightedCount();
}

unsigned int MYON_AggregateComponentStatus::HiddenCount() const
{
  return m_hidden_count;
}

unsigned int MYON_AggregateComponentStatusEx::HiddenCount() const
{
  return MYON_AggregateComponentStatus::HiddenCount();
}

unsigned int MYON_AggregateComponentStatus::LockedCount() const
{
  return m_locked_count;
}

unsigned int MYON_AggregateComponentStatusEx::LockedCount() const
{
  return MYON_AggregateComponentStatus::LockedCount();
}

unsigned int MYON_AggregateComponentStatus::DamagedCount() const
{
  return m_damaged_count;
}

unsigned int MYON_AggregateComponentStatusEx::DamagedCount() const
{
  return MYON_AggregateComponentStatus::DamagedCount();
}

bool MYON_AggregateComponentStatus::Add(
  const MYON_AggregateComponentStatus& aggregate_status
  )
{
  if (0 == m_current )
  {
    *this = aggregate_status;
    return m_current <= 1;
  }

  if ( m_current >= 2 )
    return false;

  if ( 0 == aggregate_status.m_current )
    return true;

  if ( aggregate_status.m_current >= 2 )
  {
    MarkAsNotCurrent();
    return false;
  }

  m_component_count += aggregate_status.m_component_count;
  
  const unsigned char s1 = aggregate_status.m_aggregate_status.m_status_flags;
  if ( 0 == s1 )
    return true;

  if (0 != (SELECTED_BIT & s1))
  {
    m_selected_count += aggregate_status.m_selected_count;
    m_selected_persistent_count += aggregate_status.m_selected_persistent_count;
  }
  if ( 0 != (HIGHLIGHTED_BIT & s1) )
    m_highlighted_count += aggregate_status.m_highlighted_count;
  if ( 0 != (LOCKED_BIT & s1) )
    m_locked_count += aggregate_status.m_locked_count;
  if ( 0 != (HIDDEN_BIT & s1) )
    m_hidden_count += aggregate_status.m_hidden_count;
  if ( 0 != (DAMAGED_BIT & s1) )
    m_damaged_count += aggregate_status.m_damaged_count;

  m_aggregate_status.m_status_flags |= s1;

  return true;
}

bool MYON_AggregateComponentStatusEx::Add(
  const MYON_AggregateComponentStatus& aggregate_status
)
{
  Internal_ChangeStatusSerialNumber();
  return MYON_AggregateComponentStatus::Add(aggregate_status);
}

bool MYON_AggregateComponentStatus::Add(
  MYON_ComponentStatus component_status
  )
{
  if ( 0 == m_current )
    m_current = 1;
  else if ( 1 != m_current )
    return false;

  m_component_count++;

  const unsigned char s1 = component_status.m_status_flags;
  if ( 0 == s1 )
    return true;

  if (0 != (SELECTED_BIT & s1))
  {
    m_selected_count++;
    if (0 != (SELECTED_PERSISTENT_BIT & s1))
      m_selected_persistent_count++;
  }
  if ( 0 != (HIGHLIGHTED_BIT & s1) )
    m_highlighted_count++;
  if ( 0 != (LOCKED_BIT & s1) )
    m_locked_count++;
  if ( 0 != (HIDDEN_BIT & s1) )
    m_hidden_count++;
  if ( 0 != (DAMAGED_BIT & s1) )
    m_damaged_count++;

  m_aggregate_status.m_status_flags |= s1;

  return true;
}


bool MYON_AggregateComponentStatusEx::Add(
  MYON_ComponentStatus component_status
)
{
  Internal_ChangeStatusSerialNumber();
  return MYON_AggregateComponentStatus::Add(component_status);
}

/////////////////////////////////////////////////////////////////
//
// MYON_Object component status virtual interface
//
//
unsigned int MYON_Object::ClearAllComponentStates() const
{
  return ClearComponentStates(MYON_ComponentStatus::AllSet);
}

// virtual
unsigned int MYON_Object::ClearComponentStates(
  MYON_ComponentStatus states_to_clear
  ) const
{
  return 0U;
}

//virtual
unsigned int MYON_Object::GetComponentsWithSetStates(
  MYON_ComponentStatus states_filter,
  bool bAllEqualStates,
  MYON_SimpleArray< MYON_COMPONENT_INDEX >& components
  ) const
{
  components.SetCount(0);
  return 0U;
}
  

// virtual
unsigned int MYON_Object::SetComponentStates(
  MYON_COMPONENT_INDEX component_index,
  MYON_ComponentStatus states_to_set
  ) const
{
  return 0U;
}

// virtual
unsigned int MYON_Object::ClearComponentStates(
  MYON_COMPONENT_INDEX component_index,
  MYON_ComponentStatus states_to_clear
  ) const
{
  return 0U;
}
  
// virtual
unsigned int MYON_Object::SetComponentStatus(
  MYON_COMPONENT_INDEX component_index,
  MYON_ComponentStatus status_to_copy
  ) const
{
  return 0U;
}

// virtual
MYON_AggregateComponentStatus MYON_Object::AggregateComponentStatus() const
{
  return MYON_AggregateComponentStatus::Empty;
}

// virtual
void MYON_Object::MarkAggregateComponentStatusAsNotCurrent() const
{
  return;
}

// virtual
bool MYON_Object::DeleteComponents(
  const MYON_COMPONENT_INDEX* ci_list,
  size_t ci_count
  )
{
  return false;
}
