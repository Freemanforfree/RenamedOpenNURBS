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

#if !defined(MYON_EMBEDDED_FILE_INC_)
#define MYON_EMBEDDED_FILE_INC_

class MYON_CLASS MYON_EmbeddedFile : public MYON_ModelComponent
{
  MYON_OBJECT_DECLARE(MYON_EmbeddedFile);

public:
  MYON_EmbeddedFile();
  MYON_EmbeddedFile(const MYON_EmbeddedFile&);
  virtual ~MYON_EmbeddedFile();

  const MYON_EmbeddedFile& operator = (const MYON_EmbeddedFile&);

  // Returns the fully-qualified filename of the embedded file. This filename may or may not refer to a
  // local file depending on the way the embedded file was loaded. For example, if it was loaded from an
  // archive, the filename could be that of a file on a different computer.
  MYON_wString Filename(void) const;

  // Sets the fully-qualified filename of the embedded file. This can be used to set a filename initially
  // when none has been loaded, or to change the filename prior to saving to an archive.
  void SetFilename(const wchar_t* filename);

  // Loads the embedded file from a local file. This also sets the filename. Returns true if successful, else false.
  bool LoadFromFile(const wchar_t* filename);

  // Saves the embedded file to a local file. The embedded file must have been loaded by a call to
  // one of either LoadFromFile(), LoadFromBuffer() or Read() prior to calling this method.
  // Returns true if successful, else false.
  bool SaveToFile(const wchar_t* filename) const;

  // Loads the embedded file from a buffer. This does not set the filename; it merely loads the contents
  // of the buffer into the embedded file object. If you intend to call Write() after this, you may need
  // to call SetFilename() first. Returns true if successful, else false.
  bool LoadFromBuffer(MYON_Buffer& buf);

  // Saves the embedded file to a buffer. The embedded file must have been loaded by a call to
  // one of either LoadFromFile(), LoadFromBuffer() or Read() prior to calling this method.
  // This does not save the filename to the buffer, it only saves the file contents.
  // Returns true if successful, else false.
  bool SaveToBuffer(MYON_Buffer& buf) const;

  // Reads an embedded file from an archive. The archive must be positioned at the start of suitable data
  // as created by the Write() method. It first loads the filename from the archive and then loads the file
  // contents. This method is an expert user tool. Incorrect use will have undefined results.
  // Returns true if successful, else false.
  bool Read(MYON_BinaryArchive& archive) override;

  // Writes the embedded file to an archive. The embedded file must have been loaded by a call to
  // one of either LoadFromFile(), LoadFromBuffer() or Read() prior to calling this method.
  // This writes both the filename and the file contents to the archive.
  // Returns true if successful, else false.
  bool Write(MYON_BinaryArchive& archive) const override;

  // Returns the length of the loaded data. If the object is not loaded, this method returns zero.
  size_t Length(void) const;

  // Returns the compressed length of the data loaded by Read(). If the object is not loaded or was
  // loaded by LoadFromFile() or LoadFromBuffer(), this method returns zero.
  size_t CompressedLength(void) const;

  // Clears the embedded file data. Returns true if successful, else false.
  bool Clear(void);

  // If MYON_EmbeddedFile::Cast(ref.ModelComponent()) is not null,
  // that pointer is returned. Otherwise, none_return_value is returned.
  static const MYON_EmbeddedFile* FromModelComponentRef(const MYON_ModelComponentReference& ref,
                                                      const MYON_EmbeddedFile* none_return_value);

private:
  class CImpl;
  CImpl* m_impl;
};

#endif
