#include "opennurbs.h"

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

MYON_Terminator::MYON_Terminator()
{
  m_reserved1[0] = false;
  m_reserved1[1] = false;
  m_reserved1[2] = false;
  m_reserved1[3] = false;
  m_reserved1[4] = false;
  m_reserved1[5] = false;
  m_reserved2[0] = 0;
  m_reserved2[1] = 0;
}

MYON_Terminator::~MYON_Terminator()
{
  m_callback_function = 0;
  m_callback_context = 0;
}

void MYON_Terminator::SetTerminationQueryCallbackFunction(
  bool (*callback_function)(MYON__UINT_PTR context),
  MYON__UINT_PTR callback_context
  )
{
  if (    nullptr == callback_function 
       || callback_function != m_callback_function
       || callback_context != m_callback_context
     )
  {
    m_callback_context = callback_context;
    m_callback_function = callback_function;
    m_previous_query_clock = 0;
  }
}

MYON__UINT_PTR MYON_Terminator::CallbackFunction() const
{
  return reinterpret_cast<MYON__UINT_PTR>(m_callback_function);
}

MYON__UINT_PTR MYON_Terminator::CallbackContext() const
{
  return m_callback_context;
}

void MYON_Terminator::SetThreadId(
  MYON__UINT_PTR thread_id
  )
{
  m_thread_id = thread_id;
}

MYON__UINT_PTR MYON_Terminator::ThreadId() const
{
  return m_thread_id;
}

void MYON_Terminator::RequestTermination()
{
  m_bTerminationRequested = true;
}

void MYON_Terminator::RequestTermination(
  MYON_Terminator* terminator
  )
{
  if ( nullptr != terminator )
    terminator->RequestTermination();
}


bool MYON_Terminator::TerminationRequested( MYON_Terminator* terminator )
{
  return TerminationRequestedExpert( terminator, CLOCKS_PER_SEC/10 );
}

void MYON_Terminator::SetAllowDefaultCancellationHandling(bool bAllow)
{
  m_bAllowDefaultCancellationHandling = bAllow;
}

bool MYON_Terminator::AllowDefaultCancellationHandling(void) const
{
  return m_bAllowDefaultCancellationHandling;
}

bool MYON_Terminator::TerminationRequestedExpert(
  MYON_Terminator* terminator,
  MYON__UINT64 callback_delta
  )
{
  if ( nullptr != terminator )
  {
    if ( terminator->m_bTerminationRequested )
      return true;

    if ( nullptr != terminator->m_callback_function && callback_delta > 0 )
    {
      const MYON__UINT64 current_clock = clock();
      if ( 0 == terminator->m_previous_query_clock
           || current_clock < terminator->m_previous_query_clock // clock() rolled over
           || current_clock - terminator->m_previous_query_clock >= callback_delta
          )
      {
        terminator->m_previous_query_clock = current_clock;
        if ( terminator->m_callback_function(terminator->m_callback_context) )
        {
          terminator->m_bTerminationRequested = true;
          return true;
        }
      }
    }
  }

  return false;
}
 
