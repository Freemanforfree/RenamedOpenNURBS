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

#if !defined(OPENNURBS_3DM_PROPERTIES_INC_)
#define OPENNURBS_3DM_PROPERTIES_INC_

//////////////////////////////////////////////////////////////////////////////////////////

class MYON_CLASS MYON_3dmRevisionHistory
{
public:
  /*
    Default construction sets this = MYON_3dmRevisionHistory::Empty
  */
  MYON_3dmRevisionHistory();

  ~MYON_3dmRevisionHistory() = default;

  MYON_3dmRevisionHistory(const MYON_3dmRevisionHistory&) = default;
  MYON_3dmRevisionHistory& operator=(const MYON_3dmRevisionHistory&) = default;

  /*
  Description:
    The Empty revision has a revision number zero,
    all time values set to zero and all string
    values empty.
  */
  static const MYON_3dmRevisionHistory Empty;

  /*
  Returns:
    A revision history with
      m_revision_count = 1
      m_create_time = now
      m_last_edit_time = now
      m_sCreatedBy = current user
      m_sLastEditedBy = current user
  */
  static MYON_3dmRevisionHistory FirstRevision();

  int NewRevision(); // returns updated revision count

  bool IsValid() const;
  bool IsEmpty() const;
  bool Read( MYON_BinaryArchive& );
  bool Write( MYON_BinaryArchive& ) const;
  void Dump( MYON_TextLog& ) const;

  /*
  Returns:
    true 
      if m_create_time is >= January 1, 1970
  */
  bool CreateTimeIsSet() const;

  /*
  Returns:
    true 
      if m_last_edit_time is >= January 1, 1970
  */
  bool LastEditedTimeIsSet() const;

  MYON_wString m_sCreatedBy;
  MYON_wString m_sLastEditedBy;
  struct tm  m_create_time;        // UCT create time
  struct tm  m_last_edit_time;     // UCT las edited time
  int        m_revision_count = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////

class MYON_CLASS MYON_3dmNotes
{
public:
  MYON_3dmNotes();
  ~MYON_3dmNotes();

  static const MYON_3dmNotes Empty;

  bool IsValid() const;
  bool IsEmpty() const;

  bool Read( MYON_BinaryArchive& );
  bool Write( MYON_BinaryArchive& ) const;

  void Dump(MYON_TextLog&) const;

  ////////////////////////////////////////////////////////////////
  //
  // Interface - this information is serialized.  Applications
  // may want to derive a runtime class that has additional
  // window and font information.
  MYON_wString m_notes;

  bool m_bVisible;    // true if notes window is showing
  bool m_bHTML;       // true if notes are in HTML

  // last window position
  int m_window_left;
  int m_window_top;
  int m_window_right;
  int m_window_bottom;
};

//////////////////////////////////////////////////////////////////////////////////////////

class MYON_CLASS MYON_3dmApplication
{
  // application that created the 3dm file
public:
  MYON_3dmApplication();
  ~MYON_3dmApplication();

  static const MYON_3dmApplication Empty;

  bool IsValid() const;

  bool IsEmpty() const;

  bool Read( MYON_BinaryArchive& );
  bool Write( MYON_BinaryArchive& ) const;

  void Dump( MYON_TextLog& ) const;

  MYON_wString m_application_name;    // short name like "Rhino 2.0"
  MYON_wString m_application_URL;     // URL
  MYON_wString m_application_details; // whatever you want
};

//////////////////////////////////////////////////////////////////////////////////////////

class MYON_CLASS MYON_3dmProperties
{
public:
  MYON_3dmProperties() = default;
  ~MYON_3dmProperties() = default;
  MYON_3dmProperties(const MYON_3dmProperties&) = default;
  MYON_3dmProperties& operator=(const MYON_3dmProperties&) = default;

  static const MYON_3dmProperties Empty;

  bool IsEmpty() const;

  bool Read(
    MYON_BinaryArchive& archive
    );

  /*
  Remarks:
    If archive.ArchiveFileName() is not empty, that value is 
    written in place of m_3dmArchiveFullPathName in the 3dm archive.
    If archive.ArchiveFileName() is empty, then m_3dmArchiveFullPathName
    is written in the 3dm archive.
  */
  bool Write(
    MYON_BinaryArchive& archive
    ) const;

  void Dump( MYON_TextLog& ) const;

  MYON_3dmRevisionHistory  m_RevisionHistory;
  MYON_3dmNotes            m_Notes;
  MYON_WindowsBitmap       m_PreviewImage;     // preview image of model
  MYON_3dmApplication      m_Application;      // application that created 3DM file
  
  // name of .3dm archive when it was written.  Used to find referenced files
  // when the archive is moved or copied and then read.
  MYON_wString             m_3dmArchiveFullPathName; 
};

//////////////////////////////////////////////////////////////////////////////////////////

#endif
