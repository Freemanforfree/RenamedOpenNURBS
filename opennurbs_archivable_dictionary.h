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

#if !defined(OPENNURBS_ARCHIVABLE_DICTIONARY_INC_)
#define OPENNURBS_ARCHIVABLE_DICTIONARY_INC_

class MYON_CLASS MYON_ArchivableDictionary
{
public:
  static const MYON_ArchivableDictionary Empty;

  MYON_ArchivableDictionary();
  virtual ~MYON_ArchivableDictionary();

  MYON_ArchivableDictionary(const MYON_ArchivableDictionary& src);
  MYON_ArchivableDictionary& operator=(const MYON_ArchivableDictionary& src);

#if defined(MYON_HAS_RVALUEREF)
  MYON_ArchivableDictionary(MYON_ArchivableDictionary&& src) MYON_NOEXCEPT;
  MYON_ArchivableDictionary& operator=(MYON_ArchivableDictionary&& src) MYON_NOEXCEPT;
#endif

  virtual bool Write(MYON_BinaryArchive& binary_archive) const;
  virtual bool Read(MYON_BinaryArchive& binary_archive);

  int Count() const;      // Number of elements in dictionary.
  void Clear();           // Sets count to 0.

  MYON_ClassArray<MYON_wString> GetKeys() const;
  bool ContainsKey(const wchar_t* key) const;
  bool Remove(const wchar_t* key);

  bool CopyValue(const wchar_t* key, const MYON_ArchivableDictionary& source, const wchar_t* source_key = nullptr);
  void CopyValues(const MYON_ArchivableDictionary& source);

#pragma region Getters-Setters Primitives

  bool TrySetBool(const wchar_t* key, bool value);
  bool TryGetBool(const wchar_t* key, bool& value) const;
  void SetBool(const wchar_t* key, bool value);
  bool GetBool(const wchar_t* key) const;

  bool TrySetUInt8(const wchar_t* key, MYON__UINT8 value);
  bool TryGetUInt8(const wchar_t* key, MYON__UINT8& value) const;
  void SetUInt8(const wchar_t* key, MYON__UINT8 value);
  MYON__UINT8 GetUInt8(const wchar_t* key) const;

  bool TrySetInt8(const wchar_t* key, MYON__INT8 value);
  bool TryGetInt8(const wchar_t* key, MYON__INT8& value) const;
  void SetInt8(const wchar_t* key, MYON__INT8 value);
  MYON__INT8 GetInt8(const wchar_t* key) const;

  bool TrySetInt16(const wchar_t* key, MYON__INT16 value);
  bool TryGetInt16(const wchar_t* key, MYON__INT16& value) const;
  void SetInt16(const wchar_t* key, MYON__INT16 value);
  MYON__INT16 GetInt16(const wchar_t* key) const;

  bool TrySetInt32(const wchar_t* key, MYON__INT32 value);
  bool TryGetInt32(const wchar_t* key, MYON__INT32& value) const;
  void SetInt32(const wchar_t* key, MYON__INT32 value);
  MYON__INT32 GetInt32(const wchar_t* key) const;

  bool TrySetInt64(const wchar_t* key, MYON__INT64 value);
  bool TryGetInt64(const wchar_t* key, MYON__INT64& value) const;
  void SetInt64(const wchar_t* key, MYON__INT64 value);
  MYON__INT64 GetInt64(const wchar_t* key) const;

  bool TrySetFloat(const wchar_t* key, float value);
  bool TryGetFloat(const wchar_t* key, float& value) const;
  void SetFloat(const wchar_t* key, float value);
  float GetFloat(const wchar_t* key) const;

  bool TrySetDouble(const wchar_t* key, double value);
  bool TryGetDouble(const wchar_t* key, double& value) const;
  void SetDouble(const wchar_t* key, double value);
  double GetDouble(const wchar_t* key) const;

  bool TrySetUuid(const wchar_t* key, MYON_UUID value);
  bool TryGetUuid(const wchar_t* key, MYON_UUID& value) const;
  void SetUuid(const wchar_t* key, MYON_UUID value);
  MYON_UUID GetUuid(const wchar_t* key) const;

  bool TrySetString(const wchar_t* key, const MYON_wString& value);
  bool TryGetString(const wchar_t* key, MYON_wString& value) const;
  void SetString(const wchar_t* key, const MYON_wString& value);
  MYON_wString GetString(const wchar_t* key) const;

#pragma endregion

#pragma region Getters-Setters Primitive Arrays

  bool TrySetBoolArray(const wchar_t* key, const MYON_SimpleArray<bool>& value);
  bool TryGetBoolArray(const wchar_t* key, MYON_SimpleArray<bool>& value) const;
  void SetBoolArray(const wchar_t* key, const MYON_SimpleArray<bool>& value);
  MYON_SimpleArray<bool> GetBoolArray(const wchar_t* key) const;

  bool TrySetUInt8Array(const wchar_t* key, const MYON_SimpleArray<MYON__UINT8>& value);
  bool TryGetUInt8Array(const wchar_t* key, MYON_SimpleArray<MYON__UINT8>& value) const;
  void SetUInt8Array(const wchar_t* key, const MYON_SimpleArray<MYON__UINT8>& value);
  MYON_SimpleArray<MYON__UINT8> GetUInt8Array(const wchar_t* key) const;

  bool TrySetInt8Array(const wchar_t* key, const MYON_SimpleArray<MYON__INT8>& value);
  bool TryGetInt8Array(const wchar_t* key, MYON_SimpleArray<MYON__INT8>& value) const;
  void SetInt8Array(const wchar_t* key, const MYON_SimpleArray<MYON__INT8>& value);
  MYON_SimpleArray<MYON__INT8> GetInt8Array(const wchar_t* key) const;

  bool TrySetInt16Array(const wchar_t* key, const MYON_SimpleArray<MYON__INT16>& value);
  bool TryGetInt16Array(const wchar_t* key, MYON_SimpleArray<MYON__INT16>& value) const;
  void SetInt16Array(const wchar_t* key, const MYON_SimpleArray<MYON__INT16>& value);
  MYON_SimpleArray<MYON__INT16> GetInt16Array(const wchar_t* key) const;

  bool TrySetInt32Array(const wchar_t* key, const MYON_SimpleArray<MYON__INT32>& value);
  bool TryGetInt32Array(const wchar_t* key, MYON_SimpleArray<MYON__INT32>& value) const;
  void SetInt32Array(const wchar_t* key, const MYON_SimpleArray<MYON__INT32>& value);
  MYON_SimpleArray<MYON__INT32> GetInt32Array(const wchar_t* key) const;

  bool TrySetFloatArray(const wchar_t* key, const MYON_SimpleArray<float>& value);
  bool TryGetFloatArray(const wchar_t* key, MYON_SimpleArray<float>& value) const;
  void SetFloatArray(const wchar_t* key, const MYON_SimpleArray<float>& value);
  MYON_SimpleArray<float> GetFloatArray(const wchar_t* key) const;

  bool TrySetDoubleArray(const wchar_t* key, const MYON_SimpleArray<double>& value);
  bool TryGetDoubleArray(const wchar_t* key, MYON_SimpleArray<double>& value) const;
  void SetDoubleArray(const wchar_t* key, const MYON_SimpleArray<double>& value);
  MYON_SimpleArray<double> GetDoubleArray(const wchar_t* key) const;

  bool TrySetUuidArray(const wchar_t* key, const MYON_SimpleArray<MYON_UUID>& value);
  bool TryGetUuidArray(const wchar_t* key, MYON_SimpleArray<MYON_UUID>& value) const;
  void SetUuidArray(const wchar_t* key, const MYON_SimpleArray<MYON_UUID>& value);
  MYON_SimpleArray<MYON_UUID> GetUuidArray(const wchar_t* key) const;

  bool TrySetStringArray(const wchar_t* key, const MYON_ClassArray<MYON_wString>& value);
  bool TryGetStringArray(const wchar_t* key, MYON_ClassArray<MYON_wString>& value) const;
  void SetStringArray(const wchar_t* key, const MYON_ClassArray<MYON_wString>& value);
  MYON_ClassArray<MYON_wString> GetStringArray(const wchar_t* key) const;

#pragma endregion

#pragma region Getters-Setters Geometry

  bool TrySetMeshParameters(const wchar_t* key, const MYON_MeshParameters& value);
  bool TryGetMeshParameters(const wchar_t* key, MYON_MeshParameters& value) const;
  void SetMeshParameters(const wchar_t* key, const MYON_MeshParameters& value);
  MYON_MeshParameters GetMeshParameters(const wchar_t* key) const;

#pragma endregion

private:
  class MYON_ArchivableDictionaryPrivate* m_private = nullptr;
  MYON__UINT_PTR m_reserved = 0;
};

////////////////////////////////////////////////////////////////

#endif
