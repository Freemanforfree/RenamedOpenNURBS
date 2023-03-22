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

#if !defined(MYON_FSP_DEFS_INC_)
#define MYON_FSP_DEFS_INC_

template <class T> 
MYON_SimpleFixedSizePool<T>::MYON_SimpleFixedSizePool()
: MYON_FixedSizePool()
{}

template <class T>
MYON_SimpleFixedSizePool<T>::~MYON_SimpleFixedSizePool()
{ 
  MYON_FixedSizePool::Destroy();
}

template <class T>
bool MYON_SimpleFixedSizePool<T>::Create( 
  size_t element_count_estimate,
  size_t block_element_count
  )
{
  return MYON_FixedSizePool::Create(sizeof(T),element_count_estimate,block_element_count);
}

template <class T>
size_t MYON_SimpleFixedSizePool<T>::SizeofElement() const
{
  return MYON_FixedSizePool::SizeofElement();
}

template <class T>
T* MYON_SimpleFixedSizePool<T>::AllocateElement()
{
  return (T *)MYON_FixedSizePool::AllocateElement();
}

template <class T>
void MYON_SimpleFixedSizePool<T>::ReturnElement(T* p)
{
  MYON_FixedSizePool::ReturnElement(p);
}

template <class T>
void MYON_SimpleFixedSizePool<T>::ReturnAll()
{
  MYON_FixedSizePool::ReturnAll();
}

template <class T>
void MYON_SimpleFixedSizePool<T>::Destroy()
{
  MYON_FixedSizePool::Destroy();
}

template <class T>
size_t MYON_SimpleFixedSizePool<T>::ActiveElementCount() const
{
  return MYON_FixedSizePool::ActiveElementCount();
}

template <class T>
size_t MYON_SimpleFixedSizePool<T>::TotalElementCount() const
{
  return MYON_FixedSizePool::TotalElementCount();
}

template <class T>
T* MYON_SimpleFixedSizePool<T>::Element(size_t element_index) const
{
  return (T *)MYON_FixedSizePool::Element(element_index);
}

template <class T>
size_t MYON_SimpleFixedSizePool<T>::ElementIndex(T* element_ptr) const
{
  return MYON_FixedSizePool::ElementIndex(element_ptr);
}

template <class T> 
MYON_SimpleFixedSizePoolIterator<T>::MYON_SimpleFixedSizePoolIterator(const class MYON_SimpleFixedSizePool<T>& fsp)
: MYON_FixedSizePoolIterator((MYON_FixedSizePool&)fsp)
{}

template <class T> 
MYON_SimpleFixedSizePoolIterator<T>::MYON_SimpleFixedSizePoolIterator(const class MYON_SimpleFixedSizePoolIterator<T>& fsp_it)
: MYON_FixedSizePoolIterator(fsp_it)
{}

template <class T>
T* MYON_SimpleFixedSizePoolIterator<T>::FirstElement()
{
  return (T *)MYON_FixedSizePoolIterator::FirstElement();
}


template <class T>
T* MYON_SimpleFixedSizePoolIterator<T>::FirstElement(size_t element_index)
{
  return (T *)MYON_FixedSizePoolIterator::FirstElement(element_index);
}

template <class T>
T* MYON_SimpleFixedSizePoolIterator<T>::NextElement()
{
  return (T *)MYON_FixedSizePoolIterator::NextElement();
}

template <class T>
T* MYON_SimpleFixedSizePoolIterator<T>::CurrentElement()
{
  return (T *)MYON_FixedSizePoolIterator::CurrentElement();
}


template <class T>
void MYON_SimpleFixedSizePoolIterator<T>::Reset()
{
  MYON_FixedSizePoolIterator::Reset();
}

template <class T>
T* MYON_SimpleFixedSizePoolIterator<T>::FirstBlock( size_t* block_element_count )
{
  return (T *)MYON_FixedSizePoolIterator::FirstBlock(block_element_count);
}

template <class T>
T* MYON_SimpleFixedSizePoolIterator<T>::NextBlock( size_t* block_element_count )
{
  return (T *)MYON_FixedSizePoolIterator::NextBlock(block_element_count);
}

#endif
