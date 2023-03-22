#include "opennurbs.h"
#include "opennurbs_testclass.h"

#if !defined(OPENNURBS_NO_STD_MUTEX)

MYON__UINT64 MYON_TestClass::CurrentCount()
{
  MYON__UINT64 constructed_count = 0;
  MYON__UINT64 current_count = 0;
  MYON_TestClass::GetCurrentAndConstructedCount(constructed_count, current_count);
  return current_count;
}

MYON__UINT64 MYON_TestClass::ConstructedCount()
{
  MYON__UINT64 constructed_count = 0;
  MYON__UINT64 current_count = 0;
  MYON_TestClass::GetCurrentAndConstructedCount(constructed_count, current_count);
  return constructed_count;
}

MYON__UINT64 MYON_TestClass::DestructedCount()
{
  MYON__UINT64 constructed_count = 0;
  MYON__UINT64 current_count = 0;
  MYON_TestClass::GetCurrentAndConstructedCount(constructed_count, current_count);
  return constructed_count - current_count;
}

MYON__UINT64 MYON_TestClass::Internal_CounterHelper(
  int task, // 0 = construct, 1 = destruct, 2 = query
  MYON__UINT64* constructed_count,
  MYON__UINT64* current_count
)
{
  std::lock_guard<std::mutex> lock(MYON_TestClass::internal_counter_mutex);
  MYON__UINT64 rc = 0;
  if (0 == task)
  {
    ++MYON_TestClass::internal_PopulationCounter;
    rc = ++MYON_TestClass::internal_CtorSerialNumberGenerator;
  }
  else if (1 == task)
  {
    --MYON_TestClass::internal_PopulationCounter;
  }
  else
  {
    *constructed_count = MYON_TestClass::internal_CtorSerialNumberGenerator;
    *current_count = MYON_TestClass::internal_PopulationCounter;
  }
  return rc;
}

void  MYON_TestClass::GetCurrentAndConstructedCount(
  MYON__UINT64& constructed_count,
  MYON__UINT64& current_count
)
{
  MYON_TestClass::Internal_CounterHelper(2, &constructed_count, &current_count);
}

MYON_TestClass::MYON_TestClass()
  : SerialNumber(MYON_TestClass::Internal_CounterHelper(0,nullptr,nullptr))
  , CopiedFrom(0)
  , ConstructedAt((MYON__UINT64)time(nullptr))
{}

MYON_TestClass::~MYON_TestClass()
{
  MYON_TestClass::Internal_CounterHelper(1, nullptr, nullptr);
}

MYON_TestClass::MYON_TestClass(const MYON_TestClass& src)
  : SerialNumber(MYON_TestClass::Internal_CounterHelper(0,nullptr,nullptr))
  , CopiedFrom(src.SerialNumber)
  , ConstructedAt((MYON__UINT64)time(nullptr))
  , m_string(src.Value())
{}

MYON_TestClass& MYON_TestClass::operator=(const MYON_TestClass& src)
{
  if (this != &src)
  {
    SetValue(src.Value());
  }
  return *this;
}

const MYON_wString MYON_TestClass::Value() const
{
  std::lock_guard<std::mutex> lock(m_local_mutex);
  return m_string;
}

void MYON_TestClass::SetValue(
  const MYON_wString s
)
{
  std::lock_guard<std::mutex> lock(m_local_mutex);
  m_string = s;
}

void MYON_TestClass::Dump(class MYON_TextLog& text_log) const
{
  const MYON_wString s(ToString());
  text_log.PrintString(s);
  text_log.PrintNewLine();
}

const MYON_wString MYON_TestClass::ToString() const
{
  const MYON_wString v = Value();

  const MYON_wString ymdhms = MYON_wString::FromSecondsSinceJanuaryFirst1970(
    ConstructedAt,
    MYON_DateFormat::YearMonthDay,
    MYON_TimeFormat::HourMinuteSecond24,
    0,0,0
  );


  return MYON_wString::FormatToString(
    L"MYON_TestClass[%llu] @ %s \"%ls\"",
    SerialNumber, 
    static_cast<const wchar_t*>(ymdhms),
    static_cast<const wchar_t*>(v)
  );
}

#endif
