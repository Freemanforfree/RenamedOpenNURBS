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

class MYON_Value
{
public:

  // The VALUE_TYPE enum values must never be changed
  // because the values are used to determine the parameter
  // type during file reading.  Additions can be made.
  enum VALUE_TYPE
  {
    no_value_type         =  0,

    bool_value            =  1,
    int_value             =  2,
    double_value          =  3,
    color_value           =  4,
    point_value           =  5,
    vector_value          =  6,
    xform_value           =  7,
    string_value          =  8,
    objref_value          =  9,
    geometry_value        = 10,
    uuid_value            = 11,
    point_on_object_value = 12,
    polyedge_value        = 13,
    subd_edge_chain_value = 14,

    // each value type must have a case in MYON_Value::CreateValue().

    force_32bit_enum = 0xFFFFFFFF
  };

  static
  MYON_Value* CreateValue( int value_type );

  void Report(
    MYON_TextLog& text_log
    ) const;

  // The valid id is a nonzero integer the developer
  // assigns to this value.  Developers are responsible
  // for ensuring the 
  int m_value_id = -1;
  
  const VALUE_TYPE m_value_type;

  MYON_Value(VALUE_TYPE);
  MYON_Value(const MYON_Value& src);
  virtual ~MYON_Value();

  virtual MYON_Value* Duplicate() const=0;
  virtual int  Count() const=0;
  virtual bool ReadHelper(MYON_BinaryArchive& )=0;
  virtual bool WriteHelper(MYON_BinaryArchive& ) const=0;
  virtual bool ReportHelper(MYON_TextLog& ) const=0;

  virtual int  GetBools( const bool*& ) const;
  virtual int  GetInts( const int*& ) const;
  virtual int  GetColors( const MYON_Color*& ) const;
  virtual int  GetDoubles( const double*& ) const;
  virtual int  Get3dPoints( const MYON_3dPoint*& ) const;
  virtual int  Get3dVectors( const MYON_3dVector*& ) const;
  virtual int  GetXforms( const MYON_Xform*& ) const;
  virtual int  GetUuids( const MYON_UUID*& ) const;
  virtual int  GetObjRefs( MYON_ClassArray<MYON_ObjRef>& ) const;
  virtual int  GetGeometryPointers( const MYON_Geometry* const*& ) const;
  virtual int  GetSubDEdgeChainPointers(const MYON_SubDEdgeChain* const*&) const;
  virtual int  GetStrings( MYON_ClassArray<MYON_wString>& ) const;
  virtual int  GetPolyEdgePointers(MYON_ClassArray<MYON_PolyEdgeHistory>&) const;

private:
  // no implementation
  MYON_Value() = delete; 
  MYON_Value& operator=(const MYON_Value&) = delete; 
};

MYON_Value::MYON_Value( MYON_Value::VALUE_TYPE value_type )
         : m_value_type(value_type)
{}

MYON_Value::MYON_Value(const MYON_Value& src)
  : m_value_id(src.m_value_id)
  , m_value_type(src.m_value_type)
{}

MYON_Value::~MYON_Value()
{}

// base class virtuals do nothing.
int  MYON_Value::GetBools( const bool*& ) const {return 0;}
int  MYON_Value::GetInts( const int*& ) const {return 0;}
int  MYON_Value::GetColors( const MYON_Color*& ) const {return 0;}
int  MYON_Value::GetDoubles( const double*& ) const {return 0;}
int  MYON_Value::Get3dPoints( const MYON_3dPoint*& ) const {return 0;}
int  MYON_Value::Get3dVectors( const MYON_3dVector*& ) const {return 0;}
int  MYON_Value::GetXforms( const MYON_Xform*& ) const {return 0;}
int  MYON_Value::GetUuids( const MYON_UUID*& ) const {return 0;}
int  MYON_Value::GetObjRefs( MYON_ClassArray<MYON_ObjRef>& ) const {return 0;}
int  MYON_Value::GetGeometryPointers( const MYON_Geometry* const*& ) const {return 0;}
int  MYON_Value::GetSubDEdgeChainPointers(const MYON_SubDEdgeChain* const*&) const { return 0; }
int  MYON_Value::GetStrings( MYON_ClassArray<MYON_wString>& ) const {return 0;}
int  MYON_Value::GetPolyEdgePointers(MYON_ClassArray<MYON_PolyEdgeHistory>&) const { return 0; }

class MYON_DummyValue : public MYON_Value
{
public:
  MYON_DummyValue();
  ~MYON_DummyValue();
  MYON_Value* Duplicate() const;
  int  Count() const;
  bool ReadHelper(MYON_BinaryArchive& );
  bool WriteHelper(MYON_BinaryArchive& ) const;
  bool ReportHelper(MYON_TextLog& ) const;
};

MYON_DummyValue::MYON_DummyValue() : MYON_Value(MYON_Value::no_value_type)
{
}

MYON_DummyValue::~MYON_DummyValue()
{
}

MYON_Value* MYON_DummyValue::Duplicate() const {return 0;}
int  MYON_DummyValue::Count() const {return 0;}
bool MYON_DummyValue::ReadHelper(MYON_BinaryArchive& ) {return 0;}
bool MYON_DummyValue::WriteHelper(MYON_BinaryArchive& ) const {return 0;}
bool MYON_DummyValue::ReportHelper(MYON_TextLog& ) const {return 0;}
///////////////////////////////////////////////////////////////////////
//
// MYON_BoolValue saves bool values in the MYON_HistoryRecord::m_value[] array
//

class MYON_BoolValue : public MYON_Value
{
public:
  MYON_BoolValue();
  ~MYON_BoolValue();

  MYON_SimpleArray<bool> m_value;

  // virtual 
  class MYON_Value* Duplicate() const;

  // virtual
  int Count() const;

  // virtual 
  bool ReadHelper(MYON_BinaryArchive& archive );

  // virtual 
  bool WriteHelper(MYON_BinaryArchive& archive ) const;

  // virtual 
  bool ReportHelper(MYON_TextLog& text_log ) const;

  // virtual 
  int GetBools( const bool*& b ) const;
};

MYON_BoolValue::MYON_BoolValue() 
             : MYON_Value(MYON_Value::bool_value) 
{
}

MYON_BoolValue::~MYON_BoolValue()
{
}

// virtual 
class MYON_Value* MYON_BoolValue::Duplicate() const
{
  return new MYON_BoolValue(*this);
}

// virtual
int MYON_BoolValue::Count() const 
{
  return m_value.Count();
}

// virtual 
bool MYON_BoolValue::ReadHelper(MYON_BinaryArchive& archive )
{
  return archive.ReadArray(m_value);
}

// virtual 
bool MYON_BoolValue::WriteHelper(MYON_BinaryArchive& archive ) const
{
  return archive.WriteArray(m_value);
}

// virtual 
bool MYON_BoolValue::ReportHelper(MYON_TextLog& text_log ) const
{
  int i, count = m_value.Count();
  text_log.Print("bool value\n");
  text_log.PushIndent();
  for ( i = 0; i < count; i++ )
  {
    text_log.Print(m_value[i]?"true":"false");
  }
  text_log.PopIndent();
  return true;
}

// virtual 
int MYON_BoolValue::GetBools( const bool*& b ) const
{
  b = m_value.Array();
  return m_value.Count();
}


///////////////////////////////////////////////////////////////////////
//
// MYON_IntValue saves int values in the MYON_HistoryRecord::m_value[] array
//

class MYON_IntValue : public MYON_Value
{
public:
  MYON_IntValue();
  ~MYON_IntValue();

  MYON_SimpleArray<int> m_value;

  // virtual 
  class MYON_Value* Duplicate() const;

  // virtual
  int Count() const;

  // virtual 
  bool ReadHelper(MYON_BinaryArchive& archive );

  // virtual 
  bool WriteHelper(MYON_BinaryArchive& archive ) const;

  // virtual 
  bool ReportHelper(MYON_TextLog& text_log ) const;

  // virtual 
  int GetInts( const int*& b ) const;
};

MYON_IntValue::MYON_IntValue() 
             : MYON_Value(MYON_Value::int_value) 
{
}

MYON_IntValue::~MYON_IntValue()
{
}

// virtual 
class MYON_Value* MYON_IntValue::Duplicate() const
{
  return new MYON_IntValue(*this);
}

// virtual
int MYON_IntValue::Count() const 
{
  return m_value.Count();
}

// virtual 
bool MYON_IntValue::ReadHelper(MYON_BinaryArchive& archive )
{
  return archive.ReadArray(m_value);
}

// virtual 
bool MYON_IntValue::WriteHelper(MYON_BinaryArchive& archive ) const
{
  return archive.WriteArray(m_value);
}

// virtual 
bool MYON_IntValue::ReportHelper(MYON_TextLog& text_log ) const
{
  int i, count = m_value.Count();
  text_log.Print("integer value\n");
  text_log.PushIndent();
  for ( i = 0; i < count; i++ )
  {
    text_log.Print("%d",m_value[i]);
  }
  text_log.PopIndent();
  return true;
}

// virtual 
int MYON_IntValue::GetInts( const int*& b ) const
{
  b = m_value.Array();
  return m_value.Count();
}


///////////////////////////////////////////////////////////////////////
//
// MYON_DoubleValue saves bool values in the MYON_HistoryRecord::m_value[] array
//

class MYON_DoubleValue : public MYON_Value
{
public:
  MYON_DoubleValue();
  ~MYON_DoubleValue();

  MYON_SimpleArray<double> m_value;

  // virtual 
  class MYON_Value* Duplicate() const;

  // virtual
  int Count() const;

  // virtual 
  bool ReadHelper(MYON_BinaryArchive& archive );

  // virtual 
  bool WriteHelper(MYON_BinaryArchive& archive ) const;

  // virtual 
  bool ReportHelper(MYON_TextLog& text_log ) const;

  // virtual 
  int GetDoubles( const double*& a ) const;
};

MYON_DoubleValue::MYON_DoubleValue() 
             : MYON_Value(MYON_Value::double_value) 
{
}

MYON_DoubleValue::~MYON_DoubleValue()
{
}

// virtual 
class MYON_Value* MYON_DoubleValue::Duplicate() const
{
  return new MYON_DoubleValue(*this);
}

// virtual
int MYON_DoubleValue::Count() const 
{
  return m_value.Count();
}

// virtual 
bool MYON_DoubleValue::ReadHelper(MYON_BinaryArchive& archive )
{
  return archive.ReadArray(m_value);
}

// virtual 
bool MYON_DoubleValue::WriteHelper(MYON_BinaryArchive& archive ) const
{
  return archive.WriteArray(m_value);
}

// virtual 
bool MYON_DoubleValue::ReportHelper(MYON_TextLog& text_log ) const
{
  int i, count = m_value.Count();
  text_log.Print("number value\n");
  text_log.PushIndent();
  for ( i = 0; i < count; i++ )
  {
    text_log.Print(m_value[i]);
  }
  text_log.PopIndent();
  return true;
}

// virtual 
int MYON_DoubleValue::GetDoubles( const double*& a ) const
{
  a = m_value.Array();
  return m_value.Count();
}

///////////////////////////////////////////////////////////////////////
//
// MYON_PointValue saves bool values in the MYON_HistoryRecord::m_value[] array
//

class MYON_PointValue : public MYON_Value
{
public:
  MYON_PointValue();
  ~MYON_PointValue();

  MYON_SimpleArray<MYON_3dPoint> m_value;

  // virtual 
  class MYON_Value* Duplicate() const;

  // virtual
  int Count() const;

  // virtual 
  bool ReadHelper(MYON_BinaryArchive& archive );

  // virtual 
  bool WriteHelper(MYON_BinaryArchive& archive ) const;

  // virtual 
  bool ReportHelper(MYON_TextLog& text_log ) const;

  // virtual 
  int Get3dPoints( const MYON_3dPoint*& a ) const;
};

MYON_PointValue::MYON_PointValue() 
             : MYON_Value(MYON_Value::point_value) 
{
}

MYON_PointValue::~MYON_PointValue()
{
}

// virtual 
class MYON_Value* MYON_PointValue::Duplicate() const
{
  return new MYON_PointValue(*this);
}

// virtual
int MYON_PointValue::Count() const 
{
  return m_value.Count();
}

// virtual 
bool MYON_PointValue::ReadHelper(MYON_BinaryArchive& archive )
{
  return archive.ReadArray(m_value);
}

// virtual 
bool MYON_PointValue::WriteHelper(MYON_BinaryArchive& archive ) const
{
  return archive.WriteArray(m_value);
}

// virtual 
bool MYON_PointValue::ReportHelper(MYON_TextLog& text_log ) const
{
  int i, count = m_value.Count();
  text_log.Print("point value\n");
  text_log.PushIndent();
  for ( i = 0; i < count; i++ )
  {
    text_log.Print(m_value[i]);
  }
  text_log.PopIndent();
  return true;
}

// virtual 
int MYON_PointValue::Get3dPoints( const MYON_3dPoint*& a ) const
{
  a = m_value.Array();
  return m_value.Count();
}


///////////////////////////////////////////////////////////////////////
//
// MYON_VectorValue saves bool values in the MYON_HistoryRecord::m_value[] array
//

class MYON_VectorValue : public MYON_Value
{
public:
  MYON_VectorValue();
  ~MYON_VectorValue();

  MYON_SimpleArray<MYON_3dVector> m_value;

  // virtual 
  class MYON_Value* Duplicate() const;

  // virtual
  int Count() const;

  // virtual 
  bool ReadHelper(MYON_BinaryArchive& archive );

  // virtual 
  bool WriteHelper(MYON_BinaryArchive& archive ) const;

  // virtual 
  bool ReportHelper(MYON_TextLog& text_log ) const;

  // virtual 
  int Get3dVectors( const MYON_3dVector*& a ) const;
};

MYON_VectorValue::MYON_VectorValue() 
             : MYON_Value(MYON_Value::vector_value) 
{
}

MYON_VectorValue::~MYON_VectorValue()
{
}

// virtual 
class MYON_Value* MYON_VectorValue::Duplicate() const
{
  return new MYON_VectorValue(*this);
}

// virtual
int MYON_VectorValue::Count() const 
{
  return m_value.Count();
}

// virtual 
bool MYON_VectorValue::ReadHelper(MYON_BinaryArchive& archive )
{
  return archive.ReadArray(m_value);
}

// virtual 
bool MYON_VectorValue::WriteHelper(MYON_BinaryArchive& archive ) const
{
  return archive.WriteArray(m_value);
}

// virtual 
bool MYON_VectorValue::ReportHelper(MYON_TextLog& text_log ) const
{
  text_log.Print("vector value\n");
  text_log.PushIndent();
  int i, count = m_value.Count();
  for ( i = 0; i < count; i++ )
  {
    text_log.Print(m_value[i]);
  }
  text_log.PopIndent();
  return true;
}

// virtual 
int MYON_VectorValue::Get3dVectors( const MYON_3dVector*& a ) const
{
  a = m_value.Array();
  return m_value.Count();
}


///////////////////////////////////////////////////////////////////////
//
// MYON_XformValue saves bool values in the MYON_HistoryRecord::m_value[] array
//

class MYON_XformValue : public MYON_Value
{
public:
  MYON_XformValue();
  ~MYON_XformValue();

  MYON_SimpleArray<MYON_Xform> m_value;

  // virtual 
  class MYON_Value* Duplicate() const;

  // virtual
  int Count() const;

  // virtual 
  bool ReadHelper(MYON_BinaryArchive& archive );

  // virtual 
  bool WriteHelper(MYON_BinaryArchive& archive ) const;

  // virtual 
  bool ReportHelper(MYON_TextLog& text_log ) const;

  // virtual 
  int GetXforms( const MYON_Xform*& a ) const;
};

MYON_XformValue::MYON_XformValue() 
             : MYON_Value(MYON_Value::xform_value) 
{
}

MYON_XformValue::~MYON_XformValue()
{
}

// virtual 
class MYON_Value* MYON_XformValue::Duplicate() const
{
  return new MYON_XformValue(*this);
}

// virtual
int MYON_XformValue::Count() const 
{
  return m_value.Count();
}

// virtual 
bool MYON_XformValue::ReadHelper(MYON_BinaryArchive& archive )
{
  return archive.ReadArray(m_value);
}

// virtual 
bool MYON_XformValue::WriteHelper(MYON_BinaryArchive& archive ) const
{
  return archive.WriteArray(m_value);
}

// virtual 
bool MYON_XformValue::ReportHelper(MYON_TextLog& text_log ) const
{
  text_log.Print("xform value\n");
  text_log.PushIndent();
  int i, count = m_value.Count();
  for ( i = 0; i < count; i++ )
  {
    text_log.Print(m_value[i]);
  }
  text_log.PopIndent();
  return true;
}

// virtual 
int MYON_XformValue::GetXforms( const MYON_Xform*& a ) const
{
  a = m_value.Array();
  return m_value.Count();
}


///////////////////////////////////////////////////////////////////////
//
// MYON_ColorValue saves bool values in the MYON_HistoryRecord::m_value[] array
//

class MYON_ColorValue : public MYON_Value
{
public:
  MYON_ColorValue();
  ~MYON_ColorValue();

  MYON_SimpleArray<MYON_Color> m_value;

  // virtual 
  class MYON_Value* Duplicate() const;

  // virtual
  int Count() const;

  // virtual 
  bool ReadHelper(MYON_BinaryArchive& archive );

  // virtual 
  bool WriteHelper(MYON_BinaryArchive& archive ) const;

  // virtual 
  bool ReportHelper(MYON_TextLog& text_log ) const;

  // virtual 
  int GetColors( const MYON_Color*& a ) const;
};

MYON_ColorValue::MYON_ColorValue() 
             : MYON_Value(MYON_Value::color_value) 
{
}

MYON_ColorValue::~MYON_ColorValue()
{
}

// virtual 
class MYON_Value* MYON_ColorValue::Duplicate() const
{
  return new MYON_ColorValue(*this);
}

// virtual
int MYON_ColorValue::Count() const 
{
  return m_value.Count();
}

// virtual 
bool MYON_ColorValue::ReadHelper(MYON_BinaryArchive& archive )
{
  return archive.ReadArray(m_value);
}

// virtual 
bool MYON_ColorValue::WriteHelper(MYON_BinaryArchive& archive ) const
{
  return archive.WriteArray(m_value);
}

// virtual 
bool MYON_ColorValue::ReportHelper(MYON_TextLog& text_log ) const
{
  MYON_Color c;
  text_log.Print("color value\n");
  text_log.PushIndent();
  int i, count = m_value.Count();
  for ( i = 0; i < count; i++ )
  {
    c = m_value[i];
    text_log.Print("rbg(%d,%d,%d)",c.Red(),c.Green(),c.Blue());
  }
  text_log.PopIndent();
  return true;
}

// virtual 
int MYON_ColorValue::GetColors( const MYON_Color*& a ) const
{
  a = m_value.Array();
  return m_value.Count();
}


///////////////////////////////////////////////////////////////////////
//
// MYON_UuidValue saves bool values in the MYON_HistoryRecord::m_value[] array
//

class MYON_UuidValue : public MYON_Value
{
public:
  MYON_UuidValue();
  ~MYON_UuidValue();

  MYON_SimpleArray<MYON_UUID> m_value;

  // virtual 
  class MYON_Value* Duplicate() const;

  // virtual
  int Count() const;

  // virtual 
  bool ReadHelper(MYON_BinaryArchive& archive );

  // virtual 
  bool WriteHelper(MYON_BinaryArchive& archive ) const;

  // virtual 
  bool ReportHelper(MYON_TextLog& text_log ) const;

  // virtual 
  int GetUuids( const MYON_UUID*& a ) const;
};

MYON_UuidValue::MYON_UuidValue() 
             : MYON_Value(MYON_Value::uuid_value) 
{
}

MYON_UuidValue::~MYON_UuidValue()
{
}

// virtual 
class MYON_Value* MYON_UuidValue::Duplicate() const
{
  return new MYON_UuidValue(*this);
}

// virtual
int MYON_UuidValue::Count() const 
{
  return m_value.Count();
}

// virtual 
bool MYON_UuidValue::ReadHelper(MYON_BinaryArchive& archive )
{
  return archive.ReadArray(m_value);
}

// virtual 
bool MYON_UuidValue::WriteHelper(MYON_BinaryArchive& archive ) const
{
  return archive.WriteArray(m_value);
}

// virtual 
bool MYON_UuidValue::ReportHelper(MYON_TextLog& text_log ) const
{
  text_log.Print("uuid value\n");
  text_log.PushIndent();
  int i, count = m_value.Count();
  for ( i = 0; i < count; i++ )
  {
    text_log.Print(m_value[i]);
  }
  text_log.PopIndent();
  return true;
}

// virtual 
int MYON_UuidValue::GetUuids( const MYON_UUID*& a ) const
{
  a = m_value.Array();
  return m_value.Count();
}

///////////////////////////////////////////////////////////////////////
//
// MYON_StringValue saves string values in the MYON_HistoryRecord::m_value[] array
//

class MYON_StringValue : public MYON_Value
{
public:
  MYON_StringValue();
  ~MYON_StringValue();

  MYON_ClassArray<MYON_wString> m_value;

  // virtual 
  class MYON_Value* Duplicate() const;

  // virtual
  int Count() const;

  // virtual 
  bool ReadHelper(MYON_BinaryArchive& archive );

  // virtual 
  bool WriteHelper(MYON_BinaryArchive& archive ) const;

  // virtual 
  bool ReportHelper(MYON_TextLog& text_log ) const;

  // virtual 
  int GetStrings( MYON_ClassArray<MYON_wString>& s ) const;
};

MYON_StringValue::MYON_StringValue() 
             : MYON_Value(MYON_Value::string_value) 
{
}

MYON_StringValue::~MYON_StringValue()
{
}

// virtual 
class MYON_Value* MYON_StringValue::Duplicate() const
{
  return new MYON_StringValue(*this);
}

// virtual
int MYON_StringValue::Count() const 
{
  return m_value.Count();
}

// virtual 
bool MYON_StringValue::ReadHelper(MYON_BinaryArchive& archive )
{
  return archive.ReadArray(m_value);
}

// virtual 
bool MYON_StringValue::WriteHelper(MYON_BinaryArchive& archive ) const
{
  return archive.WriteArray(m_value);
}

// virtual 
bool MYON_StringValue::ReportHelper(MYON_TextLog& text_log ) const
{
  text_log.Print("string value\n");
  text_log.PushIndent();
  int i, count = m_value.Count();
  for ( i = 0; i < count; i++ )
  {
    text_log.Print(m_value[i]);
  }
  text_log.PopIndent();
  return true;
}

// virtual 
int MYON_StringValue::GetStrings( MYON_ClassArray<MYON_wString>& s ) const
{
  s = m_value;
  return m_value.Count();
}


///////////////////////////////////////////////////////////////////////
//
// MYON_ObjRefValue saves objref values in the MYON_HistoryRecord::m_value[] array
//

class MYON_ObjRefValue : public MYON_Value
{
public:
  MYON_ObjRefValue();
  ~MYON_ObjRefValue();

  MYON_ClassArray<MYON_ObjRef> m_value;

  // virtual 
  class MYON_Value* Duplicate() const;

  // virtual
  int Count() const;

  // virtual 
  bool ReadHelper(MYON_BinaryArchive& archive );

  // virtual 
  bool WriteHelper(MYON_BinaryArchive& archive ) const;

  // virtual 
  bool ReportHelper(MYON_TextLog& text_log ) const;

  // virtual 
  int GetObjRefs( MYON_ClassArray<MYON_ObjRef>& oref ) const;
};

MYON_ObjRefValue::MYON_ObjRefValue() 
             : MYON_Value(MYON_Value::objref_value) 
{
}

MYON_ObjRefValue::~MYON_ObjRefValue()
{
}

// virtual 
class MYON_Value* MYON_ObjRefValue::Duplicate() const
{
  return new MYON_ObjRefValue(*this);
}

// virtual
int MYON_ObjRefValue::Count() const 
{
  return m_value.Count();
}

// virtual 
bool MYON_ObjRefValue::ReadHelper(MYON_BinaryArchive& archive )
{
  return archive.ReadArray(m_value);
}

// virtual 
bool MYON_ObjRefValue::WriteHelper(MYON_BinaryArchive& archive ) const
{
  return archive.WriteArray(m_value);
}

// virtual 
bool MYON_ObjRefValue::ReportHelper(MYON_TextLog& text_log ) const
{
  text_log.Print("objref value\n");
  text_log.PushIndent();
  int i, count = m_value.Count();
  for ( i = 0; i < count; i++ )
  {
    text_log.Print("object id: ");
    text_log.Print(m_value[i].m_uuid);
    text_log.Print("\n");
  }
  text_log.PopIndent();
  return true;
}

// virtual 
int MYON_ObjRefValue::GetObjRefs( MYON_ClassArray<MYON_ObjRef>& s ) const
{
  s = m_value;
  return m_value.Count();
}


///////////////////////////////////////////////////////////////////////
//
// MYON_GeometryValue saves geometry values in the MYON_HistoryRecord::m_value[] array
//

class MYON_GeometryValue : public MYON_Value
{
public:
  MYON_GeometryValue();
  ~MYON_GeometryValue();
  MYON_GeometryValue(const MYON_GeometryValue& src);
  MYON_GeometryValue& operator=(const MYON_GeometryValue& src);

  MYON_SimpleArray<MYON_Geometry*> m_value;

  // virtual 
  class MYON_Value* Duplicate() const;

  // virtual
  int Count() const;

  // virtual 
  bool ReadHelper(MYON_BinaryArchive& archive );

  // virtual 
  bool WriteHelper(MYON_BinaryArchive& archive ) const;

  // virtual 
  bool ReportHelper(MYON_TextLog& text_log ) const;

  // virtual 
  int GetGeometryPointers( const MYON_Geometry* const*& ) const;
};

MYON_GeometryValue::MYON_GeometryValue() 
             : MYON_Value(MYON_Value::geometry_value) 
{
}

MYON_GeometryValue::~MYON_GeometryValue()
{
  int i, count = m_value.Count();
  for ( i = 0; i < count; i++ )
  {
    MYON_Geometry* p = m_value[i];
    m_value[i] = 0;
    if (p)
    {
      delete p;
    }
  }
}

MYON_GeometryValue::MYON_GeometryValue(const MYON_GeometryValue& src) : MYON_Value(src)
{
  *this = src;
}

MYON_GeometryValue& MYON_GeometryValue::operator=(const MYON_GeometryValue& src)
{
  if ( this != &src )
  {
    int i, count = m_value.Count();
    for ( i = 0; i < count; i++ )
    {
      MYON_Geometry* p = m_value[i];
      m_value[i] = 0;
      if (p)
      {
        delete p;
      }
    }
    m_value.Destroy();

    m_value_id = src.m_value_id;

    count = src.m_value.Count();
    m_value.Reserve(count);
    for ( i = 0; i < count; i++ )
    {
      const MYON_Geometry* src_ptr = src.m_value[i];
      if ( !src_ptr )
        continue;
      MYON_Geometry* ptr = src_ptr->Duplicate();
      if ( ptr )
        m_value.Append(ptr);
    }
  }
  return *this;
}

// virtual 
class MYON_Value* MYON_GeometryValue::Duplicate() const
{
  return new MYON_GeometryValue(*this);
}

// virtual
int MYON_GeometryValue::Count() const 
{
  return m_value.Count();
}

// virtual 
bool MYON_GeometryValue::ReadHelper(MYON_BinaryArchive& archive )
{
  int i, count = m_value.Count();
  for ( i = 0; i < count; i++ )
  {
    delete m_value[i];
  }
  m_value.SetCount(0);

  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
  if (!rc)
    return false;

  for(;;)
  {
    rc = archive.ReadInt(&count);
    if (!rc) break;
    m_value.Reserve(count);
    
    for( i = 0; i < count && rc; i++ )
    {
      MYON_Object* p=0;
      rc = archive.ReadObject(&p) > 0;
      if (rc)
      {
        MYON_Geometry* g = MYON_Geometry::Cast(p);
        if (g)
        {
          p = 0;
          m_value.Append(g);
        }
      }
      if ( p )
        delete p;
    }
    if (!rc) break;

    break;
  }

  if ( !archive.EndRead3dmChunk() )
    rc = false;
  return rc;
}

// virtual 
bool MYON_GeometryValue::WriteHelper(MYON_BinaryArchive& archive ) const
{
  bool rc = archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0);
  if (!rc)
    return false;

  for(;;)
  {
    rc = archive.WriteInt(m_value.Count());
    if (!rc) break;
    
    int i, count = m_value.Count();
    for( i = 0; i < count && rc; i++ )
    {
      rc = archive.WriteObject(m_value[i]);
    }
    if (!rc) break;

    break;
  }

  if ( !archive.EndWrite3dmChunk() )
    rc = false;
  return rc;
}

// virtual 
bool MYON_GeometryValue::ReportHelper(MYON_TextLog& text_log ) const
{
  text_log.Print("geometry value\n");
  text_log.PushIndent();
  int i, count = m_value.Count();
  for ( i = 0; i < count; i++ )
  {
    const MYON_Geometry* p = m_value[i];
    if ( p )
      p->Dump(text_log);
  }
  text_log.PopIndent();
  return true;
}

// virtual 
int MYON_GeometryValue::GetGeometryPointers( const MYON_Geometry* const*&a ) const
{
  a = m_value.Array();
  return m_value.Count();
}


///////////////////////////////////////////////////////////////////////
//
// MYON_PolyEdgeHistoryValue saves geometry values in the MYON_HistoryRecord::m_value[] array
//

class MYON_PolyEdgeHistoryValue : public MYON_Value
{
public:
  MYON_PolyEdgeHistoryValue();
  ~MYON_PolyEdgeHistoryValue();

  MYON_ClassArray<MYON_PolyEdgeHistory> m_value;

  // virtual 
  class MYON_Value* Duplicate() const;

  // virtual
  int Count() const;

  // virtual 
  bool ReadHelper(MYON_BinaryArchive& archive );

  // virtual 
  bool WriteHelper(MYON_BinaryArchive& archive ) const;

  // virtual 
  bool ReportHelper(MYON_TextLog& text_log ) const;

  // virtual 
  int  GetPolyEdgePointers( MYON_ClassArray<MYON_PolyEdgeHistory>& ) const;
};

MYON_PolyEdgeHistoryValue::MYON_PolyEdgeHistoryValue() 
             : MYON_Value(MYON_Value::polyedge_value) 
{
}

MYON_PolyEdgeHistoryValue::~MYON_PolyEdgeHistoryValue()
{
  m_value.Destroy();
}

//MYON_PolyEdgeHistoryValue::MYON_PolyEdgeHistoryValue(const MYON_PolyEdgeHistoryValue& src) : MYON_Value(src)
//{
//  *this = src;
//}
//
//MYON_PolyEdgeHistoryValue& MYON_PolyEdgeHistoryValue::operator=(const MYON_PolyEdgeHistoryValue& src)
//{
//  if ( this != &src )
//  {
//    int i, count = m_value.Count();
//    for ( i = 0; i < count; i++ )
//    {
//      MYON_Geometry* p = m_value[i];
//      m_value[i] = 0;
//      if (p)
//      {
//        delete p;
//      }
//    }
//    m_value.Destroy();
//
//    m_value_id = src.m_value_id;
//
//    count = src.m_value.Count();
//    m_value.Reserve(count);
//    for ( i = 0; i < count; i++ )
//    {
//      const MYON_Geometry* src_ptr = src.m_value[i];
//      if ( !src_ptr )
//        continue;
//      MYON_Geometry* ptr = src_ptr->Duplicate();
//      if ( ptr )
//        m_value.Append(ptr);
//    }
//  }
//  return *this;
//}

// virtual 
class MYON_Value* MYON_PolyEdgeHistoryValue::Duplicate() const
{
  return new MYON_PolyEdgeHistoryValue(*this);
}

// virtual
int MYON_PolyEdgeHistoryValue::Count() const 
{
  return m_value.Count();
}

// virtual 
bool MYON_PolyEdgeHistoryValue::ReadHelper(MYON_BinaryArchive& archive )
{
  m_value.Destroy();

  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
  if (!rc)
    return false;

  for(;;)
  {
    int count = 0;
    rc = archive.ReadInt(&count);
    if (!rc) break;
    m_value.Reserve(count);
    
    for( int i = 0; i < count && rc; i++ )
    {
      if ( !m_value.AppendNew().Read(archive) )
      {
        m_value.Destroy();
        rc = false;
        break;
      }
    }
    if (!rc) break;

    break;
  }

  if ( !archive.EndRead3dmChunk() )
    rc = false;
  return rc;
}

// virtual 
bool MYON_PolyEdgeHistoryValue::WriteHelper(MYON_BinaryArchive& archive ) const
{
  bool rc = archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0);
  if (!rc)
    return false;

  for(;;)
  {
    rc = archive.WriteInt(m_value.Count());
    if (!rc) break;
    
    int i, count = m_value.Count();
    for( i = 0; i < count && rc; i++ )
    {
      rc = m_value[i].Write(archive);
    }
    if (!rc) break;

    break;
  }

  if ( !archive.EndWrite3dmChunk() )
    rc = false;
  return rc;
}

// virtual 
bool MYON_PolyEdgeHistoryValue::ReportHelper(MYON_TextLog& text_log ) const
{
  text_log.Print("polyedge value\n");
  text_log.PushIndent();
  int i, count = m_value.Count();
  for ( i = 0; i < count; i++ )
  {
    m_value[i].Dump(text_log);
  }
  text_log.PopIndent();
  return true;
}

// virtual 
int MYON_PolyEdgeHistoryValue::GetPolyEdgePointers( MYON_ClassArray<MYON_PolyEdgeHistory>& a ) const
{
  a = m_value;
  return m_value.Count();
}


///////////////////////////////////////////////////////////////////////
//
// MYON_SubDEdgeChainHistoryValue saves geometry values in the MYON_HistoryRecord::m_value[] array
//

class MYON_SubDEdgeChainHistoryValue : public MYON_Value
{
public:
  MYON_SubDEdgeChainHistoryValue();
  ~MYON_SubDEdgeChainHistoryValue();
  MYON_SubDEdgeChainHistoryValue(const MYON_SubDEdgeChainHistoryValue&);
  MYON_SubDEdgeChainHistoryValue& operator=(const MYON_SubDEdgeChainHistoryValue&);

  MYON_SimpleArray<MYON_SubDEdgeChain*> m_value;

  class MYON_Value* Duplicate() const override;
  int Count() const override;
  bool ReadHelper(MYON_BinaryArchive& archive) override;
  bool WriteHelper(MYON_BinaryArchive& archive) const override;
  bool ReportHelper(MYON_TextLog& text_log) const override;
  int GetSubDEdgeChainPointers(const MYON_SubDEdgeChain* const*&) const override;
};

MYON_SubDEdgeChainHistoryValue::MYON_SubDEdgeChainHistoryValue()
  : MYON_Value(MYON_Value::subd_edge_chain_value)
{
}

MYON_SubDEdgeChainHistoryValue::~MYON_SubDEdgeChainHistoryValue()
{
  int i, count = m_value.Count();
  for (i = 0; i < count; i++)
  {
    MYON_SubDEdgeChain* p = m_value[i];
    if (nullptr != p)
    {
      m_value[i] = nullptr;
      delete p;
    }
  }
}

MYON_SubDEdgeChainHistoryValue::MYON_SubDEdgeChainHistoryValue(const MYON_SubDEdgeChainHistoryValue& src) : MYON_Value(src)
{
  *this = src;
}

MYON_SubDEdgeChainHistoryValue& MYON_SubDEdgeChainHistoryValue::operator=(const MYON_SubDEdgeChainHistoryValue& src)
{
  if (this != &src)
  {
    int i, count = m_value.Count();
    for (i = 0; i < count; i++)
    {
      MYON_SubDEdgeChain* p = m_value[i];
      if (nullptr != p)
      {
        m_value[i] = nullptr;
        delete p;
      }
    }
    m_value.Destroy();

    m_value_id = src.m_value_id;

    count = src.m_value.Count();
    m_value.Reserve(count);
    for (i = 0; i < count; i++)
    {
      const MYON_SubDEdgeChain* src_ptr = src.m_value[i];
      if (!src_ptr)
        continue;
      MYON_SubDEdgeChain* ptr = new MYON_SubDEdgeChain(*src_ptr);
      if (ptr)
        m_value.Append(ptr);
    }
  }
  return *this;
}

// virtual 
class MYON_Value* MYON_SubDEdgeChainHistoryValue::Duplicate() const
{
  return new MYON_SubDEdgeChainHistoryValue(*this);
}

// virtual
int MYON_SubDEdgeChainHistoryValue::Count() const
{
  return m_value.Count();
}

// virtual 
bool MYON_SubDEdgeChainHistoryValue::ReadHelper(MYON_BinaryArchive& archive)
{
  m_value.Destroy();

  int chunk_version = 0;
  if (false == archive.BeginRead3dmAnonymousChunk(&chunk_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (chunk_version < 1)
      break;
    int count = 0;
    if (false == archive.ReadInt(&count))
      break;

    m_value.Reserve(count);
    for (int i = 0; i < count; i++)
    {
      MYON_SubDEdgeChain* c = new MYON_SubDEdgeChain();
      if (false == c->Read(archive))
        break;
      m_value.Append(c);
    }
    if (count == m_value.Count())
      rc = true;
    else
      m_value.Destroy();

    break;
  }

  if (!archive.EndRead3dmChunk())
    rc = false;
  return rc;
}

// virtual 
bool MYON_SubDEdgeChainHistoryValue::WriteHelper(MYON_BinaryArchive& archive) const
{
  if (false == archive.BeginWrite3dmAnonymousChunk(1))
    return false;

  bool rc = false;
  for (;;)
  {
    int count = m_value.Count();
    for (int i = 0; i < count; ++i)
    {
      if (nullptr == m_value[i])
        count = 0;
    }
    if (false == archive.WriteInt(count))
      break;

    rc = true;
    for (int i = 0; i < count && rc; i++)
      rc = m_value[i]->Write(archive);

    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;
  return rc;
}

// virtual 
bool MYON_SubDEdgeChainHistoryValue::ReportHelper(MYON_TextLog& text_log) const
{
  text_log.Print("SubD edge chain value\n");
  text_log.PushIndent();
  int i, count = m_value.Count();
  for (i = 0; i < count; i++)
  {
    if( nullptr != m_value[i])
      m_value[i]->Dump(text_log);
  }
  text_log.PopIndent();
  return true;
}

// virtual 
int MYON_SubDEdgeChainHistoryValue::GetSubDEdgeChainPointers(const MYON_SubDEdgeChain* const*& a) const
{
  a = m_value.Array();
  return m_value.Count();
}


///////////////////////////////////////////////////////////////////////
//

// static
MYON_Value* MYON_Value::CreateValue( int value_type )
{
  MYON_Value* value = 0;
  switch((unsigned int)value_type)
  {
  case no_value_type:
    break;
  case bool_value:
    value = new MYON_BoolValue();
    break;
  case int_value:
    value = new MYON_IntValue();
    break;
  case double_value:
    value = new MYON_DoubleValue();
    break;
  case color_value:
    value = new MYON_ColorValue();
    break;
  case point_value:
    value = new MYON_PointValue();
    break;
  case vector_value:
    value = new MYON_VectorValue();
    break;
  case xform_value:
    value = new MYON_XformValue();
    break;
  case string_value:
    value = new MYON_StringValue();
    break;
  case objref_value:
    value = new MYON_ObjRefValue();
    break;
  case geometry_value:
    value = new MYON_GeometryValue();
    break;
  case uuid_value:
    value = new MYON_UuidValue();
    break;
  case point_on_object_value:
    //value = new MYON_PointOnObjectValue();
    break;
  case polyedge_value:
    value = new MYON_PolyEdgeHistoryValue();
    break;
  case subd_edge_chain_value:
    value = new MYON_SubDEdgeChainHistoryValue();
    break;
  case force_32bit_enum:
    break;
  default:
    break;
  }
  return value;
}

///////////////////////////////////////////////////////////////////////
//
// MYON_HistoryRecord implementation
//

MYON_OBJECT_IMPLEMENT(MYON_HistoryRecord,MYON_ModelComponent,"ECD0FD2F-2088-49dc-9641-9CF7A28FFA6B");

MYON_HistoryRecord::MYON_HistoryRecord() MYON_NOEXCEPT
  : MYON_ModelComponent(MYON_ModelComponent::Type::HistoryRecord)
{}

MYON_HistoryRecord::~MYON_HistoryRecord()
{
  Internal_Destroy();
}

MYON_HistoryRecord::MYON_HistoryRecord(const MYON_HistoryRecord& src)
  : MYON_ModelComponent(MYON_ModelComponent::Type::HistoryRecord,src)
{
  Internal_Copy(src);
}

MYON_HistoryRecord& MYON_HistoryRecord::operator=(const MYON_HistoryRecord& src)
{
  if ( this != &src && false == this->IsSystemComponent() )
  {
    MYON_ModelComponent::operator=(*this);
    Internal_Destroy();
    MYON_Object::operator=(src);
    Internal_Copy(src);
  }
  return *this;
}

void MYON_HistoryRecord::Internal_Copy(const MYON_HistoryRecord& src)
{
  // input value of this->m_value[] is known to be empty
  m_command_id = src.m_command_id;
  m_version = src.m_version;
  m_record_type = src.m_record_type;
  m_descendants = src.m_descendants;
  m_antecedents = src.m_antecedents;
  m_bValuesSorted = true;
  m_bCopyOnReplaceObject = src.m_bCopyOnReplaceObject;

  const unsigned int count = src.m_value.UnsignedCount();
  m_value.SetCapacity(count);
  const MYON_Value* prev_v = 0;
  for (unsigned int i = 0; i < count; i++)
  {
    const MYON_Value* src_v = src.m_value[i];
    if (src_v)
    {
      MYON_Value* v = src_v->Duplicate();
      if (v)
      {
        m_value.Append(v);
        if (m_bValuesSorted && prev_v && prev_v->m_value_id > v->m_value_id)
          m_bValuesSorted = false;
        prev_v = v;
      }
    }
  }
}

bool MYON_HistoryRecord::IsValid( MYON_TextLog* text_log ) const
{
  return true;
}

void MYON_HistoryRecord::Internal_Destroy()
{
  const unsigned int count = m_value.UnsignedCount();
  for ( unsigned int i = 0; i < count; i++ )
  {
    MYON_Value* v = m_value[i];
    if (nullptr != v)
    {
      m_value[i] = nullptr;
      delete v;
    }
  }
  m_value.Empty();
}

MYON_HistoryRecord::RECORD_TYPE MYON_HistoryRecord::RecordType(int i)
{
  switch (i)
  {
  case (int)MYON_HistoryRecord::RECORD_TYPE::history_parameters:
    return MYON_HistoryRecord::RECORD_TYPE::history_parameters;

  case (int)MYON_HistoryRecord::RECORD_TYPE::feature_parameters:
    return MYON_HistoryRecord::RECORD_TYPE::feature_parameters;
  }

  return MYON_HistoryRecord::RECORD_TYPE::history_parameters;
}

bool MYON_HistoryRecord::SetBoolValue( int value_id, bool b)
{
  return ( 1 == SetBoolValues(value_id, 1, &b) );
}

bool MYON_HistoryRecord::SetIntValue( int value_id, int i)
{
  return ( 1 == SetIntValues(value_id, 1, &i) );
}

bool MYON_HistoryRecord::SetDoubleValue( int value_id, double x)
{
  return ( 1 == SetDoubleValues(value_id, 1, &x) );
}

bool MYON_HistoryRecord::SetPointValue( int value_id, MYON_3dPoint p)
{
  return ( 1 == SetPointValues(value_id, 1, &p) );
}

bool MYON_HistoryRecord::SetVectorValue( int value_id, MYON_3dVector v)
{
  return ( 1 == SetVectorValues(value_id, 1, &v) );
}

bool MYON_HistoryRecord::SetXformValue( int value_id, MYON_Xform xform)
{
  return ( 1 == SetXformValues(value_id, 1, &xform) );
}

bool MYON_HistoryRecord::SetColorValue( int value_id, MYON_Color c)
{
  return ( 1 == SetColorValues(value_id, 1, &c) );
}

bool MYON_HistoryRecord::SetObjRefValue( int value_id, const MYON_ObjRef& oref)
{
  return ( 1 == SetObjRefValues(value_id, 1, &oref) );
}

bool MYON_HistoryRecord::SetPointOnObjectValue( int value_id, const MYON_ObjRef& oref, MYON_3dPoint point )
{
  MYON_ObjRef poo = oref;
  poo.m_point = point;
  return SetObjRefValue(value_id,poo);
}

bool MYON_HistoryRecord::GetPointOnObjectValue( int value_id, MYON_ObjRef& oref ) const
{
  bool rc = GetObjRefValue(value_id,oref);
  return (rc && oref.m_point.IsValid());
}

bool MYON_HistoryRecord::SetGeometryValue( int value_id, MYON_Geometry* g)
{
  MYON_SimpleArray<MYON_Geometry*> a(1);
  a.Append(g);
  return SetGeometryValues(value_id, a);
}


bool MYON_HistoryRecord::SetPolyEdgeValue( int value_id, const MYON_PolyEdgeHistory& polyedge )
{
  return ( 1 == SetPolyEdgeValues(value_id, 1, &polyedge) );
}

bool MYON_HistoryRecord::SetUuidValue( int value_id, MYON_UUID uuid )
{
  return ( 1 == SetUuidValues(value_id, 1, &uuid) );
}

static
int CompareValueIdHelper(const MYON_Value* a, const MYON_Value* b )
{
  if (!a)
  {
    return b ? -1 : 0;
  }
  if (!b)
  {
    return 1;
  }
  return (a->m_value_id - b->m_value_id);
}

static int CompareValueId( MYON_Value * const * a, MYON_Value * const * b )
{
  // handle NULLs in case somebody messes up the m_value[] array.
  if ( !a )
  {
    return b ? -1 : 0;
  }
  if (!b)
    return 1;

  return CompareValueIdHelper(*a,*b);
}

MYON_Value* MYON_HistoryRecord::FindValueHelper( int value_id, int value_type, bool bCreateOne ) const
{
  MYON_HistoryRecord* vp = const_cast<MYON_HistoryRecord*>(this);
  if ( m_value.Count() > 0 )
  {
    if ( !m_bValuesSorted )
    {
      vp->m_value.QuickSort(CompareValueId);
      vp->m_bValuesSorted = true;
    }

    MYON_DummyValue dummy_value;
    dummy_value.m_value_id = value_id;
    MYON_Value* p = &dummy_value;
    int i = m_value.BinarySearch(&p,CompareValueId);

    if ( i >= 0 )
    {
      // m_value[i]->m_value_id == value_id

      if ( value_type == ((int)m_value[i]->m_value_type) )
      {
        // type matches
        return m_value[i];
      }

      if ( bCreateOne )
      {
        // type does not match - replace the existing one
        MYON_Value* new_value = MYON_Value::CreateValue(value_type);
        if ( new_value )
        {
          new_value->m_value_id = value_id;
          delete m_value[i];
          vp->m_value[i] = new_value;
          return new_value;
        }
      }
    }
    else if ( bCreateOne )
    {
      // no value in m_value[] array with a matching value_id
      MYON_Value* new_value = MYON_Value::CreateValue(value_type);
      if ( new_value )
      {
        new_value->m_value_id = value_id;
        if ( m_bValuesSorted && (*m_value.Last())->m_value_id > value_id )
          vp->m_bValuesSorted = false;
        vp->m_value.Append(new_value);
        return new_value;
      }
    }
  }
  else if ( bCreateOne )
  {
    MYON_Value* new_value = MYON_Value::CreateValue(value_type);
    if ( new_value )
    {
      new_value->m_value_id = value_id;
      vp->m_bValuesSorted = true;
      vp->m_value.Append(new_value);
      return new_value;
    }
  }
  return 0;
}


bool MYON_HistoryRecord::SetBoolValues( int value_id, int count, const bool* b)
{
  MYON_BoolValue* v = static_cast<MYON_BoolValue*>(FindValueHelper(value_id,MYON_Value::bool_value,true));
  if ( v )
  {
    v->m_value.SetCount(0);
    v->m_value.SetCapacity(count);
    v->m_value.Append(count,b);
  }
  return (0 != v);
}

bool MYON_HistoryRecord::SetIntValues( int value_id, int count, const int* i)
{
  MYON_IntValue* v = static_cast<MYON_IntValue*>(FindValueHelper(value_id,MYON_Value::int_value,true));
  if ( v )
  {
    v->m_value.SetCount(0);
    v->m_value.SetCapacity(count);
    v->m_value.Append(count,i);
  }
  return (0 != v);
}

bool MYON_HistoryRecord::SetDoubleValues( int value_id, int count, const double* x)
{
  MYON_DoubleValue* v = static_cast<MYON_DoubleValue*>(FindValueHelper(value_id,MYON_Value::double_value,true));
  if ( v )
  {
    v->m_value.SetCount(0);
    v->m_value.SetCapacity(count);
    v->m_value.Append(count,x);
  }
  return (0 != v);
}

bool MYON_HistoryRecord::SetPointValues( int value_id, int count, const MYON_3dPoint* P)
{
  MYON_PointValue* v = static_cast<MYON_PointValue*>(FindValueHelper(value_id,MYON_Value::point_value,true));
  if ( v )
  {
    v->m_value.SetCount(0);
    v->m_value.SetCapacity(count);
    v->m_value.Append(count,P);
  }
  return (0 != v);
}

bool MYON_HistoryRecord::SetVectorValues( int value_id, int count, const MYON_3dVector* V)
{
  MYON_VectorValue* v = static_cast<MYON_VectorValue*>(FindValueHelper(value_id,MYON_Value::vector_value,true));
  if ( v )
  {
    v->m_value.SetCount(0);
    v->m_value.SetCapacity(count);
    v->m_value.Append(count,V);
  }
  return (0 != v);
}

bool MYON_HistoryRecord::SetXformValues( int value_id, int count, const MYON_Xform* xform)
{
  MYON_XformValue* v = static_cast<MYON_XformValue*>(FindValueHelper(value_id,MYON_Value::xform_value,true));
  if ( v )
  {
    v->m_value.SetCount(0);
    v->m_value.SetCapacity(count);
    v->m_value.Append(count,xform);
  }
  return (0 != v);
}

bool MYON_HistoryRecord::SetColorValues( int value_id, int count, const MYON_Color* c)
{
  MYON_ColorValue* v = static_cast<MYON_ColorValue*>(FindValueHelper(value_id,MYON_Value::color_value,true));
  if ( v )
  {
    v->m_value.SetCount(0);
    v->m_value.SetCapacity(count);
    v->m_value.Append(count,c);
  }
  return (0 != v);
}

bool MYON_HistoryRecord::SetUuidValues( int value_id, int count, const MYON_UUID* u )
{
  MYON_UuidValue* v = static_cast<MYON_UuidValue*>(FindValueHelper(value_id,MYON_Value::uuid_value,true));
  if ( v )
  {
    v->m_value.SetCount(0);
    v->m_value.SetCapacity(count);
    v->m_value.Append(count,u);
  }
  return (0 != v);
}

bool MYON_HistoryRecord::SetStringValues( int value_id, int count, const wchar_t* const* s )
{
  MYON_StringValue* v = static_cast<MYON_StringValue*>(FindValueHelper(value_id,MYON_Value::string_value,true));
  if ( v )
  {
    v->m_value.Destroy();
    v->m_value.Reserve(count);
    int i;
    for( i = 0; i < count; i++ )
    {
      v->m_value.AppendNew() = s[i];
    }
  }
  return (0 != v);
}

bool MYON_HistoryRecord::SetStringValues( int value_id, const MYON_ClassArray<MYON_wString>& s )
{
  MYON_StringValue* v = static_cast<MYON_StringValue*>(FindValueHelper(value_id,MYON_Value::string_value,true));
  if ( v )
  {
    v->m_value = s;
  }
  return (0 != v);
}

bool MYON_HistoryRecord::SetStringValue( int value_id, const wchar_t* s )
{
  MYON_StringValue* v = static_cast<MYON_StringValue*>(FindValueHelper(value_id,MYON_Value::string_value,true));
  if ( v )
  {
    v->m_value.Destroy();
    v->m_value.AppendNew() = s;
  }
  return (0 != v);
}


bool MYON_HistoryRecord::SetObjRefValues( int value_id, int count, const MYON_ObjRef* oref)
{
  MYON_ObjRefValue* v = static_cast<MYON_ObjRefValue*>(FindValueHelper(value_id,MYON_Value::objref_value,true));
  if ( v )
  {
    v->m_value.Destroy();
    v->m_value.Reserve(count);

    if(count)
    {
      // 2019-01-23 - kike@mcneel.com
      // Objects in instance definitions can not be modified in that case
      // I add the root instance reference and all the instance definitions as 'antecedents'
      const bool idef_geometry = oref && (oref->m__iref.Count() > 0);

      for(int i = 0; i < count; i++)
      {
        // The call to DecrementProxyReferenceCount() is critical.
        // It makes sure there are no active runtime pointers 
        // saved in the history record.  If this call is not here,
        // you will eventually crash and history update will never
        // work right even when it doesn't crash.
        MYON_ObjRef& vor = v->m_value.AppendNew();
        vor = oref[i];
        vor.DecrementProxyReferenceCount();
        // Feb 12 2010 - Fixing bug in ExtrudeCrv history
        //  and probably lots of other subtle history bugs.
        //  History must lookup by UUID and not by runtime serial number.
        vor.m_runtime_sn = 0;

        // 2019-01-23 - kike@mcneel.com
        if(!idef_geometry)
        {
          MYON_UUID object_id = v->m_value[i].m_uuid;
          if(!MYON_UuidIsNil(object_id))
          {
            m_antecedents.AddUuid(object_id);
          }
        }
      }

      // 2019-01-23 - kike@mcneel.com
      if(idef_geometry)
      {
        if(auto iref = oref->m__iref.Last())
          m_antecedents.AddUuid(iref->m_iref_uuid);

        for(int r = 0; r < oref->m__iref.Count(); ++r)
          m_antecedents.AddUuid(oref->m__iref[r].m_idef_uuid);
      }
    }
  }
  return (0 != v);
}


bool MYON_HistoryRecord::SetGeometryValues(int value_id, const MYON_SimpleArray<MYON_Geometry*> a)
{
  MYON_GeometryValue* v = static_cast<MYON_GeometryValue*>(FindValueHelper(value_id, MYON_Value::geometry_value, true));
  if (v)
  {
    v->m_value = a;
  }
  return (0 != v);
}

bool MYON_HistoryRecord::SetSubDEdgeChainValue(int value_id, const MYON_SubDEdgeChain& edge_chain)
{
  MYON_SimpleArray<const MYON_SubDEdgeChain*> a;
  a.Append(&edge_chain);
  return MYON_HistoryRecord::SetSubDEdgeChainValues(value_id, a);
}

bool MYON_HistoryRecord::SetSubDEdgeChainValues(int value_id, const MYON_ClassArray<MYON_SubDEdgeChain>& edge_chains)
{
  const unsigned count = edge_chains.UnsignedCount();
  MYON_SimpleArray<const MYON_SubDEdgeChain*> a(count);
  for (unsigned i = 0; i < count; ++i)
    a.Append(&edge_chains[i]);
  return MYON_HistoryRecord::SetSubDEdgeChainValues(value_id, a);
}

bool MYON_HistoryRecord::SetSubDEdgeChainValues(int value_id, const MYON_SimpleArray<const MYON_SubDEdgeChain*>& edge_chains)
{
  // validate
  const unsigned count = edge_chains.UnsignedCount();
  if (count <= 0)
    return false;

  for (unsigned i = 0; i < count; ++i)
  {
    const MYON_SubDEdgeChain* c = edge_chains[i];
    if (nullptr == c)
      return false;
    const MYON_UUID parent_subd_id = c->PersistentSubDId();
    if (MYON_nil_uuid == parent_subd_id)
      return false; // a persistent id is reqiured so that update history can find the new subd and update the runtime MYON_SubDEdgePtr values.
    if (c->EdgeCount() <= 0)
      return false;
    if (false == c->HasPersistentEdgeIds())
    {
      c->SetPersistentEdgeIdsFromRuntimeEdgePtrs();
      if (false == c->HasPersistentEdgeIds())
        return false;
    }
    m_antecedents.AddUuid(parent_subd_id, true);
  }

  // copy edge chains and add
  MYON_SubDEdgeChainHistoryValue* v = static_cast<MYON_SubDEdgeChainHistoryValue*>(FindValueHelper(value_id, MYON_Value::subd_edge_chain_value, true));
  if ( nullptr != v )
  {
    v->m_value.Reserve(count);
    for (unsigned i = 0; i < count; ++i)
      v->m_value.Append(new MYON_SubDEdgeChain(*edge_chains[i]));
  }

  return (nullptr != v);
}

bool MYON_HistoryRecord::SetPolyEdgeValues( int value_id,  int count, const MYON_PolyEdgeHistory* a )
{
  MYON_PolyEdgeHistoryValue* v = static_cast<MYON_PolyEdgeHistoryValue*>(FindValueHelper(value_id,MYON_Value::polyedge_value,true));
  if ( v )
  {
    v->m_value.Destroy();
    v->m_value.Append(count,a);

    for ( int i = 0; i < count; i++ )
    {
      const MYON_PolyEdgeHistory& pe_history = a[i];
      for ( int j = 0; j < pe_history.m_segment.Count(); j++ )
      {
        const MYON_CurveProxyHistory& segment = pe_history.m_segment[j];
        m_antecedents.AddUuid(segment.m_curve_ref.m_uuid);
      }
    }
  }
  return (0 != v);
}

bool MYON_HistoryRecord::GetBoolValue( int value_id, bool* b ) const
{
  bool rc = false;
  const MYON_BoolValue* v = static_cast<MYON_BoolValue*>(FindValueHelper(value_id,MYON_Value::bool_value,0));
  if ( v && 1 == v->m_value.Count())
  {
    *b = v->m_value[0];
    rc = true;
  }
  return rc;
}

bool MYON_HistoryRecord::GetIntValue( int value_id, int* i ) const
{
  bool rc = false;
  const MYON_IntValue* v = static_cast<MYON_IntValue*>(FindValueHelper(value_id,MYON_Value::int_value,0));
  if ( v && 1 == v->m_value.Count())
  {
    *i = v->m_value[0];
    rc = true;
  }
  return rc;
}

bool MYON_HistoryRecord::GetDoubleValue( int value_id, double* number ) const
{
  bool rc = false;
  const MYON_DoubleValue* v = static_cast<MYON_DoubleValue*>(FindValueHelper(value_id,MYON_Value::double_value,0));
  if ( v && 1 == v->m_value.Count())
  {
    *number = v->m_value[0];
    rc = true;
  }
  return rc;
}

bool MYON_HistoryRecord::GetPointValue( int value_id, MYON_3dPoint& point ) const
{
  bool rc = false;
  const MYON_PointValue* v = static_cast<MYON_PointValue*>(FindValueHelper(value_id,MYON_Value::point_value,0));
  if ( v && 1 == v->m_value.Count())
  {
    point = v->m_value[0];
    rc = true;
  }
  return rc;
}

bool MYON_HistoryRecord::GetVectorValue( int value_id, MYON_3dVector& vector ) const
{
  bool rc = false;
  const MYON_VectorValue* v = static_cast<MYON_VectorValue*>(FindValueHelper(value_id,MYON_Value::vector_value,0));
  if ( v && 1 == v->m_value.Count())
  {
    vector = v->m_value[0];
    rc = true;
  }
  return rc;
}

bool MYON_HistoryRecord::GetXformValue( int value_id, MYON_Xform& xform ) const
{
  bool rc = false;
  const MYON_XformValue* v = static_cast<MYON_XformValue*>(FindValueHelper(value_id,MYON_Value::xform_value,0));
  if ( v && 1 == v->m_value.Count())
  {
    xform = v->m_value[0];
    rc = true;
  }
  return rc;
}

bool MYON_HistoryRecord::GetColorValue( int value_id, MYON_Color* color ) const
{
  bool rc = false;
  const MYON_ColorValue* v = static_cast<MYON_ColorValue*>(FindValueHelper(value_id,MYON_Value::color_value,0));
  if ( v && 1 == v->m_value.Count())
  {
    *color = v->m_value[0];
    rc = true;
  }
  return rc;
}

bool MYON_HistoryRecord::GetObjRefValue( int value_id, MYON_ObjRef& oref ) const
{
  bool rc = false;
  const MYON_ObjRefValue* v = static_cast<MYON_ObjRefValue*>(FindValueHelper(value_id,MYON_Value::objref_value,0));
  if ( v && 1 == v->m_value.Count())
  {
    oref = v->m_value[0];
    rc = true;
  }
  return rc;
}

bool MYON_HistoryRecord::GetStringValue( int value_id, MYON_wString& str ) const
{
  bool rc = false;
  MYON_StringValue* v = static_cast<MYON_StringValue*>(FindValueHelper(value_id,MYON_Value::string_value,0));
  if ( v && 1 == v->m_value.Count())
  {
    str = v->m_value[0];
    rc = true;
  }
  return rc;
}

bool MYON_HistoryRecord::GetGeometryValue( int value_id, const MYON_Geometry*& g ) const
{
  bool rc = false;
  g = 0;
  const MYON_GeometryValue* v = static_cast<MYON_GeometryValue*>(FindValueHelper(value_id,MYON_Value::geometry_value,0));
  if ( v && 1 == v->m_value.Count())
  {
    g = v->m_value[0];
    rc = true;
  }
  return rc;
}

bool MYON_HistoryRecord::GetSubDEdgeChainValue(int value_id, const MYON_SubDEdgeChain*& edge_chain) const
{
  bool rc = false;
  edge_chain = 0;
  const MYON_SubDEdgeChainHistoryValue* v = static_cast<MYON_SubDEdgeChainHistoryValue*>(FindValueHelper(value_id, MYON_Value::subd_edge_chain_value, 0));
  if (v && 1 == v->m_value.Count())
  {
    edge_chain = v->m_value[0];
    rc = true;
  }
  return rc;
}

bool MYON_HistoryRecord::GetPolyEdgeValue( int value_id, const MYON_PolyEdgeHistory*& polyedge ) const
{
  bool rc = false;
  polyedge = 0;
  const MYON_PolyEdgeHistoryValue* v = static_cast<MYON_PolyEdgeHistoryValue*>(FindValueHelper(value_id,MYON_Value::polyedge_value,0));
  if ( v && 1 == v->m_value.Count())
  {
    polyedge = &v->m_value[0];
    rc = true;
  }
  return rc;
}

bool MYON_HistoryRecord::GetCurveValue( int value_id, const MYON_Curve*& c ) const
{
  c = 0;
  const MYON_Geometry* g = 0;
  if (GetGeometryValue( value_id, g ))
  {
    c = MYON_Curve::Cast(g);
  }
  return (0 != c);
}

bool MYON_HistoryRecord::GetSurfaceValue( int value_id, const MYON_Surface*& s ) const
{
  s = 0;
  const MYON_Geometry* g = 0;
  if (GetGeometryValue( value_id, g ))
  {
    s = MYON_Surface::Cast(g);
  }
  return (0 != s);
}

bool MYON_HistoryRecord::GetBrepValue( int value_id, const MYON_Brep*& b ) const
{
  b = 0;
  const MYON_Geometry* g = 0;
  if (GetGeometryValue( value_id, g ))
  {
    b = MYON_Brep::Cast(g);
  }
  return (0 != b);
}

bool MYON_HistoryRecord::GetMeshValue( int value_id, const MYON_Mesh*& m ) const
{
  m = 0;
  const MYON_Geometry* g = 0;
  if (GetGeometryValue( value_id, g ))
  {
    m = MYON_Mesh::Cast(g);
  }
  return (0 != m);
}

bool MYON_HistoryRecord::GetUuidValue( int value_id, MYON_UUID* uuid ) const
{
  bool rc = false;
  const MYON_UuidValue* v = static_cast<MYON_UuidValue*>(FindValueHelper(value_id,MYON_Value::uuid_value,0));
  if ( v && 1 == v->m_value.Count())
  {
    *uuid = v->m_value[0];
    rc = true;
  }
  return rc;
}

int MYON_HistoryRecord::GetBoolValues( int value_id, MYON_SimpleArray<bool>& a ) const
{
  a.SetCount(0);
  const MYON_BoolValue* v = static_cast<MYON_BoolValue*>(FindValueHelper(value_id,MYON_Value::bool_value,0));
  if ( v )
  {
    a = v->m_value;
  }
  return a.Count();
}


int MYON_HistoryRecord::GetStringValues( int value_id, MYON_ClassArray<MYON_wString>& a ) const
{
  a.SetCount(0);
  const MYON_StringValue* v = static_cast<MYON_StringValue*>(FindValueHelper(value_id,MYON_Value::string_value,0));
  if ( v )
  {
    a = v->m_value;
  }
  return a.Count();
}


int MYON_HistoryRecord::GetIntValues( int value_id, MYON_SimpleArray<int>& a ) const
{
  a.SetCount(0);
  const MYON_IntValue* v = static_cast<MYON_IntValue*>(FindValueHelper(value_id,MYON_Value::int_value,0));
  if ( v )
  {
    a = v->m_value;
  }
  return a.Count();
}

int MYON_HistoryRecord::GetDoubleValues( int value_id, MYON_SimpleArray<double>& a) const
{
  a.SetCount(0);
  const MYON_DoubleValue* v = static_cast<MYON_DoubleValue*>(FindValueHelper(value_id,MYON_Value::double_value,0));
  if ( v )
  {
    a = v->m_value;
  }
  return a.Count();
}

int MYON_HistoryRecord::GetPointValues( int value_id, MYON_SimpleArray<MYON_3dPoint>& a ) const
{
  a.SetCount(0);
  const MYON_PointValue* v = static_cast<MYON_PointValue*>(FindValueHelper(value_id,MYON_Value::point_value,0));
  if ( v )
  {
    a = v->m_value;
  }
  return a.Count();
}

int MYON_HistoryRecord::GetVectorValues( int value_id, MYON_SimpleArray<MYON_3dVector>& a ) const
{
  a.SetCount(0);
  const MYON_VectorValue* v = static_cast<MYON_VectorValue*>(FindValueHelper(value_id,MYON_Value::vector_value,0));
  if ( v )
  {
    a = v->m_value;
  }
  return a.Count();
}

int MYON_HistoryRecord::GetXformValues( int value_id, MYON_SimpleArray<MYON_Xform>& a ) const
{
  a.SetCount(0);
  const MYON_XformValue* v = static_cast<MYON_XformValue*>(FindValueHelper(value_id,MYON_Value::xform_value,0));
  if ( v )
  {
    a = v->m_value;
  }
  return a.Count();
}

int MYON_HistoryRecord::GetColorValues( int value_id, MYON_SimpleArray<MYON_Color>& a ) const
{
  a.SetCount(0);
  const MYON_ColorValue* v = static_cast<MYON_ColorValue*>(FindValueHelper(value_id,MYON_Value::color_value,0));
  if ( v )
  {
    a = v->m_value;
  }
  return a.Count();
}

int MYON_HistoryRecord::GetObjRefValues( int value_id, MYON_ClassArray<MYON_ObjRef>& a ) const
{
  a.SetCount(0);
  const MYON_ObjRefValue* v = static_cast<MYON_ObjRefValue*>(FindValueHelper(value_id,MYON_Value::objref_value,0));
  if ( v )
  {
    a = v->m_value;
  }
  return a.Count();
}

int MYON_HistoryRecord::GetGeometryValues( int value_id, MYON_SimpleArray<const MYON_Geometry*>& a) const
{
  a.SetCount(0);
  const MYON_GeometryValue* v = static_cast<MYON_GeometryValue*>(FindValueHelper(value_id,MYON_Value::geometry_value,0));
  if ( v )
  {
    int i, count = v->m_value.Count();
    a.Reserve(count);
    for ( i = 0; i < count; i++ )
      a.Append(v->m_value[i]);
  }
  return a.Count();
}

int MYON_HistoryRecord::GetSubDEdgeChainValues(int value_id, MYON_SimpleArray<const MYON_SubDEdgeChain*>& a) const
{
  a.SetCount(0);
  const MYON_SubDEdgeChainHistoryValue* v = static_cast<MYON_SubDEdgeChainHistoryValue*>(FindValueHelper(value_id, MYON_Value::subd_edge_chain_value, 0));
  if (v)
  {
    int i, count = v->m_value.Count();
    a.Reserve(count);
    for (i = 0; i < count; i++)
      a.Append(v->m_value[i]);
  }
  return a.Count();
}

int MYON_HistoryRecord::GetPolyEdgeValues( int value_id, MYON_SimpleArray<const MYON_PolyEdgeHistory*>& a) const
{
  a.SetCount(0);
  const MYON_PolyEdgeHistoryValue* v = static_cast<MYON_PolyEdgeHistoryValue*>(FindValueHelper(value_id,MYON_Value::polyedge_value,0));
  if ( v )
  {
    int i, count = v->m_value.Count();
    a.Reserve(count);
    for ( i = 0; i < count; i++ )
      a.Append(&v->m_value[i]);
  }
  return a.Count();
}

int MYON_HistoryRecord::GetUuidValues( int value_id, MYON_SimpleArray<MYON_UUID>& a) const
{
  a.SetCount(0);
  const MYON_UuidValue* v = static_cast<MYON_UuidValue*>(FindValueHelper(value_id,MYON_Value::uuid_value,0));
  if ( v )
  {
    a = v->m_value;
  }
  return a.Count();
}


bool MYON_HistoryRecord::IsAntecedent( MYON_UUID object_uuid ) const
{
  return m_antecedents.FindUuid(object_uuid);
}

int MYON_HistoryRecord::ValueReport( MYON_TextLog& text_log ) const
{
  int value_count = 0;
  int i, vi, count = m_value.Count();

  // list values
  MYON_SimpleArray<int> vi_list(count);
  vi_list.SetCount(count);
  vi_list.Zero();

  m_value.Sort( MYON::sort_algorithm::quick_sort, vi_list.Array(), CompareValueId );

  for ( i = 0; i < count; i++ )
  {
    vi = vi_list[i];
    const MYON_Value* v = m_value[vi];
    if (!v)
      continue;
    text_log.Print("Value ID %d:\n",v->m_value_id);
    text_log.PushIndent();
    m_value[i]->ReportHelper(text_log);
    text_log.PopIndent();
    value_count++;
  }
  return value_count;
}

void MYON_HistoryRecord::Dump( MYON_TextLog& text_log ) const
{
  MYON_ModelComponent::Dump(text_log);

  int i, count;
  MYON_SimpleArray<MYON_UUID> uuid_list;

  text_log.Print("Command ID: ");
  text_log.Print(m_command_id);
  text_log.Print("\n");

  text_log.Print("Version %d\n",m_version);

  text_log.Print("Record ID: ");
  text_log.Print(Id());
  text_log.Print("\n");

  text_log.Print("Record type: %s\n",
                 (m_record_type == MYON_HistoryRecord::RECORD_TYPE::feature_parameters) 
                 ? "feature parameters" : "history parameters");

  // list antededents
  uuid_list.SetCount(0);
  m_antecedents.GetUuids(uuid_list);
  count = uuid_list.Count();
  if ( count <= 0 )
  {
    text_log.Print("No antededents.\n");
  }
  else
  {
    text_log.Print("Antededent ID:\n");
    text_log.PushIndent();
    for ( i = 0; i < count; i++ )
    {
      text_log.Print(uuid_list[i]);
      text_log.Print("\n");
    }
    text_log.PopIndent();
  }

  // list descendants
  uuid_list.SetCount(0);
  m_descendants.GetUuids(uuid_list);
  count = uuid_list.Count();
  if ( count <= 0 )
  {
    text_log.Print("No descendants.\n");
  }
  else
  {
    text_log.Print("Descendant ID:\n");
    text_log.PushIndent();
    for ( i = 0; i < count; i++ )
    {
      text_log.Print(uuid_list[i]);
      text_log.Print("\n");
    }
    text_log.PopIndent();
  }

  text_log.Print("Values:\n");
  text_log.PushIndent();
  int value_count = ValueReport(text_log);
  if ( 0 == value_count )
    text_log.Print("none\n");
  text_log.PopIndent();
}

void MYON_HistoryRecord::DestroyValue( int value_id )
{
  if ( m_value.Count() > 0 )
  {
    if ( !m_bValuesSorted )
    {
      m_value.QuickSort(CompareValueId);
      m_bValuesSorted = true;
    }
    MYON_DummyValue dummy_value;
    dummy_value.m_value_id = value_id;
    MYON_Value* p = &dummy_value;
    int i = m_value.BinarySearch(&p,CompareValueId);
    if ( i >= 0 )
    {
      MYON_Value* v = m_value[i];
      m_value.Remove();
      delete v;
    }
  }
}

bool MYON_HistoryRecord::Read(MYON_BinaryArchive& archive)
{
  return Internal_ReadV5(archive);
}

bool MYON_HistoryRecord::Internal_ReadV5( MYON_BinaryArchive& archive )
{
  *this = MYON_HistoryRecord::Empty;

  // put entire history record in a chunk
  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
  if (!rc)
    return false;

  for(;;)
  {
    rc = (1 == major_version);
    if (!rc) break;

    MYON_UUID record_id = MYON_nil_uuid;
    rc = archive.ReadUuid(record_id);
    if(!rc) break;
    SetId(record_id);

    rc = archive.ReadInt(&m_version);
    if(!rc) break;

    rc = archive.ReadUuid(m_command_id);
    if(!rc) break;

    // 16 October 2012 Dale Lear
    //   Fixing http://dev.mcneel.com/bugtrack/?q=101403
    // Changing bSortDescendantsAferRead from true to false
    // per discussion in the bug report. 
    const bool bSortDescendantsAferRead = false;
    rc = m_descendants.Read(archive,bSortDescendantsAferRead);
    if(!rc) break;

    rc = m_antecedents.Read(archive);
    if(!rc) break;

    // all values are in a chunk
    int mjvs=0,mnvs=0;
    int value_id0 = 0;
    rc = archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&mjvs,&mnvs);
    if (rc)
    {
      rc = ( 1 == mjvs );

      int i, count = 0;
      if (rc) 
        rc = archive.ReadInt(&count);

      if (rc)
        m_value.Reserve(count);

      for ( i = 0; i < count && rc; i++ )
      {
        int mjv=0,mnv=0;
        rc = archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&mjv,&mnv);
        if ( !rc)
          break;
        for(;;)
        {
          rc = ( 1 == mjv);
          if (!rc) break;
          int value_type = MYON_Value::no_value_type;
          rc = archive.ReadInt(&value_type);
          if (!rc) 
            break;

          int value_id = 0;
          rc = archive.ReadInt(&value_id);
          if (!rc) 
            break;

          MYON_Value* value = MYON_Value::CreateValue(value_type);
          if ( value )
          {        
            value->m_value_id = value_id;
            rc = value->ReadHelper(archive);
            if (!rc) 
            {
              delete value;
              break;
            }
            m_value.Append(value);
            if ( value->m_value_id <= value_id0 )
              m_bValuesSorted = false;
            else
              value_id0 = value->m_value_id;
          }

          break;
        }
        if (!archive.EndRead3dmChunk())
          rc = false;
      }

      // end of all values
      if (!archive.EndRead3dmChunk())
        rc = false;
    }

    if ( rc && minor_version >= 1 )
    {
      // 1.1 fields added to opennurbs version 200603200
      int rec_type = (int)MYON_HistoryRecord::RECORD_TYPE::history_parameters;
      if (rc)
        rc = archive.ReadInt( &rec_type );
      if (rc )
        m_record_type = RecordType(rec_type);

      if (rc && minor_version >= 2)
      {
        archive.ReadBool(&m_bCopyOnReplaceObject);
      }

    }

    break;
  }

  // end of entire history record in a chunk
  if (!archive.EndRead3dmChunk())
    rc = false;
  return rc;
}


bool MYON_HistoryRecord::Write(MYON_BinaryArchive& archive) const
{
  return Internal_WriteV5(archive);
}

bool MYON_HistoryRecord::Internal_WriteV5( MYON_BinaryArchive& archive ) const
{
  // 2015-06-01 Dale Lear
  //   Save m_bCopyOnReplaceObject in the file in chunck version 1.2

  const int minor_version 
    = (archive.Archive3dmVersion() >= 60)
    ? 2  // V6 after 2015-06-01 or later file
    : 1; // V5 or earlier file

  bool rc = archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,minor_version);
  if (!rc)
    return false;

  for(;;)
  {
    rc = archive.WriteUuid(Id());
    if(!rc) break;

    rc = archive.WriteInt(m_version);
    if(!rc) break;

    rc = archive.WriteUuid(m_command_id);
    if(!rc) break;

    // 30 October 2012 Dale Lear
    //   Fixing http://dev.mcneel.com/bugtrack/?q=101403
    // Changing bSortDescendantsBeforeWrite from true to false
    // per discussion in the bug report. 
    const bool bSortDescendantsBeforeWrite = false;
    rc = m_descendants.Write(archive,bSortDescendantsBeforeWrite);
    if(!rc) break;

    rc = m_antecedents.Write(archive);
    if(!rc) break;


    // wrap all values in a chunk
    rc = archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0);
    if (rc)
    {
      // value count
      int i, count = m_value.Count();
      rc = archive.WriteInt(count);

      for ( i = 0; i < count && rc; i++ )
      {
        // put individual value in its own chunk
        rc = archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0);
        if ( !rc)
          break;
        for(;;)
        {
          const MYON_Value* value = m_value[i];
          rc = archive.WriteInt(value ? value->m_value_type : MYON_Value::no_value_type );
          if (!rc) break;

          rc = archive.WriteInt(value ? value->m_value_id : 0 );
          if (!rc) break;
          
          if ( value && value->m_value_type != MYON_Value::no_value_type )
            rc = value->WriteHelper(archive);
          if (!rc) break;

          break;
        }
        // end of individual value chunk
        if (!archive.EndWrite3dmChunk())
          rc = false;
      }

      // end of all values chunk
      if ( !archive.EndWrite3dmChunk() )
        rc = false;

    }

    // 1.1 fields added to opennurbs version 200603200
    if (rc)
    {
      int i = (int)m_record_type;
      rc = archive.WriteInt(i);
    }

    if (rc && minor_version >= 2)
    {
      rc = archive.WriteBool(m_bCopyOnReplaceObject);
    }

    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;
  return rc;
}

void MYON_HistoryRecord::RemapObjectIds( const MYON_SimpleArray<MYON_UuidPair>& id_remap )
{
  if ( id_remap.Count() > 0 )
  {
    int i, j;
    m_antecedents.RemapUuids(id_remap);
    m_descendants.RemapUuids(id_remap);
    for ( i = 0; i < m_value.Count(); i++ )
    {
      MYON_Value* v = m_value[i];
      if ( v && MYON_Value::objref_value == v->m_value_type )
      {
        MYON_ObjRefValue* objrev_v = static_cast<MYON_ObjRefValue*>(v);
        for ( j = 0; j < objrev_v->m_value.Count(); j++ )
        {
          objrev_v->m_value[j].RemapObjectId(id_remap);
        }
      }
      // 24 May 2021, Mikko, RH-56171:
      // Some commands like Offset use an UUID list to map inputs and outputs.
      // Similar to remapping the objref ids, the uuid lists also need to be
      // updated to use new ids.
      // Other commands that currently use UUID lists are Divide, Slab, Ribbon, 
      // FilletSrf, ChamferSrf, ArrayCrv, Hatch.
      else if (v && MYON_Value::uuid_value == v->m_value_type)
      {
        MYON_UuidValue* uuid_v = static_cast<MYON_UuidValue*>(v);
        for (j = 0; j < uuid_v->m_value.Count(); j++)
        {
          int index = id_remap.BinarySearch((const MYON_UuidPair*)&uuid_v->m_value[j], MYON_UuidPair::CompareFirstUuid);
          if (index >= 0)
            uuid_v->m_value[j] = id_remap[index].m_uuid[1];
        }
      }
      // 27 Oct 2021, Mikko, RH-66102:
      // Polyedge values contain objrefs and also need remapping.
      // This fixes history replay not working properly on BlendSrf and others
      // when importing/pasting.
      else if (v && MYON_Value::polyedge_value == v->m_value_type)
      {
        MYON_PolyEdgeHistoryValue* objrev_v = static_cast<MYON_PolyEdgeHistoryValue*>(v);
        for (j = 0; j < objrev_v->m_value.Count(); j++)
        {
          int k;
          for (k = 0; k < objrev_v->m_value[j].m_segment.Count(); k++)
          {
            objrev_v->m_value[j].m_segment[k].m_curve_ref.RemapObjectId(id_remap);
          }
        }
      }
    }
  }
}

bool MYON_HistoryRecord::CopyOnReplaceObject() const
{
  return m_bCopyOnReplaceObject;
}
void MYON_HistoryRecord::SetCopyOnReplaceObject(
  bool bCopyOnReplaceObject
  )
{
  m_bCopyOnReplaceObject = (bCopyOnReplaceObject) ? true : false;
}


/////////////////////////////////////////////////////////////////////////////////
//
// MYON_CurveProxyHistory
//

MYON_CurveProxyHistory::MYON_CurveProxyHistory()
{
}

MYON_CurveProxyHistory::~MYON_CurveProxyHistory()
{
}

void MYON_CurveProxyHistory::Destroy()
{
  m_curve_ref.Destroy();
  m_bReversed = false;
  m_full_real_curve_domain = MYON_Interval::EmptyInterval;
  m_sub_real_curve_domain = MYON_Interval::EmptyInterval;
  m_proxy_curve_domain = MYON_Interval::EmptyInterval;
  m_segment_edge_domain = MYON_Interval::EmptyInterval;
  m_segment_trim_domain = MYON_Interval::EmptyInterval;
}


bool MYON_CurveProxyHistory::Write( MYON_BinaryArchive& file ) const
{
  if ( !file.BeginWrite3dmChunk( TCODE_ANONYMOUS_CHUNK,1,1) )
    return false;

  bool rc = false;
  for(;;)
  {
    if ( !m_curve_ref.Write(file) )
      break;
    if ( !file.WriteBool(m_bReversed) )
      break;
    if ( !file.WriteInterval(m_full_real_curve_domain) )
      break;
    if ( !file.WriteInterval(m_sub_real_curve_domain) )
      break;
    if (!file.WriteInterval(m_proxy_curve_domain))
      break;
    // Added in version 1,1
    if (!file.WriteInterval(m_segment_edge_domain))
      break;
    if (!file.WriteInterval(m_segment_trim_domain))
      break;
    rc = true;
    break;
  }

  if ( !file.EndWrite3dmChunk() )
    rc = false;
  return rc;
}

bool MYON_CurveProxyHistory::Read( MYON_BinaryArchive& file )
{
  int version_major = 0;
  int version_minor = 0;
  Destroy();

  if ( !file.BeginRead3dmChunk( TCODE_ANONYMOUS_CHUNK,&version_major,&version_minor) )
    return false;

  bool rc = false;
  for(;;)
  {
    if ( 1 != version_major )
      break;
    if ( !m_curve_ref.Read(file) )
      break;
    if ( !file.ReadBool(&m_bReversed) )
      break;
    if ( !file.ReadInterval(m_full_real_curve_domain) )
      break;
    if ( !file.ReadInterval(m_sub_real_curve_domain) )
      break;
    if (!file.ReadInterval(m_proxy_curve_domain))
      break;
    if (version_minor > 0)
    {
      if (!file.ReadInterval(m_segment_edge_domain))
        break;
      if (!file.ReadInterval(m_segment_trim_domain))
        break;
    }
    rc = true;
    break;
  }

  if ( !file.EndRead3dmChunk() )
    rc = false;

  return rc;
}

void MYON_CurveProxyHistory::Dump( MYON_TextLog& ) const
{
}

/////////////////////////////////////////////////////////////////////////////////
//
// MYON_PolyEdgeHistory
//

MYON_PolyEdgeHistory::MYON_PolyEdgeHistory()
: m_evaluation_mode(0)
{
}

MYON_PolyEdgeHistory::~MYON_PolyEdgeHistory()
{
}

void MYON_PolyEdgeHistory::Destroy()
{
  m_segment.Destroy();
  m_t.Destroy();
  m_evaluation_mode = 0;
}


bool MYON_PolyEdgeHistory::Write( MYON_BinaryArchive& file ) const
{
  int i;
  if ( !file.BeginWrite3dmChunk( TCODE_ANONYMOUS_CHUNK,1,0) )
    return false;

  bool rc = false;
  for(;;)
  {
    if ( !file.WriteInt(m_segment.Count()) )
      break;
    for ( i = 0; i < m_segment.Count(); i++ )
    {
      if ( !m_segment[i].Write(file) )
        break;
    }
    if ( i < m_segment.Count() )
      break;
    if ( !file.WriteInt(m_t.Count()) )
      break;
    if ( m_t.Count() > 0 )
    {
      if ( !file.WriteDouble(m_t.Count(),m_t.Array()) )
        break;
    }
    if ( !file.WriteInt(m_evaluation_mode) )
      break;
    rc = true;
    break;
  }

  if ( !file.EndWrite3dmChunk() )
    rc = false;
  return rc;
}

bool MYON_PolyEdgeHistory::Read( MYON_BinaryArchive& file )
{
  int count, i;
  int version_major = 0;
  int version_minor = 0;

  Destroy();

  if ( !file.BeginRead3dmChunk( TCODE_ANONYMOUS_CHUNK,&version_major,&version_minor) )
    return false;

  bool rc = false;
  for(;;)
  {
    if ( 1 != version_major )
      break;
    count = 0;
    if ( !file.ReadInt(&count) )
      break;
    m_segment.Reserve(count);
    for ( i = 0; i < count; i++ )
    {
      if ( !m_segment.AppendNew().Read(file) )
        break;
    }
    if ( i < count )
      break;
    count = 0;
    if ( !file.ReadInt(&count) )
      break;
    if ( count > 0 )
    {
      m_t.Reserve(count);
      m_t.SetCount(count);
      if ( !file.ReadDouble(count,m_t.Array()) )
        break;
    }
    if ( !file.ReadInt(&m_evaluation_mode) )
      break;
    rc = true;
    break;
  }

  if ( !file.EndRead3dmChunk() )
    rc = false;
  return rc;
}

void MYON_PolyEdgeHistory::Dump( MYON_TextLog& ) const
{
}
