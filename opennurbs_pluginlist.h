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

#if !defined(OPENNURBS_PLUGINLIST_INC_)
#define OPENNURBS_PLUGINLIST_INC_

/*
Description:
  The MYON_PluginRef class is used to store a list of
  application plug-ins that may have saved user data
  in a 3dm file so they can be loaded as needed for
  reading their user data.
*/
class MYON_CLASS MYON_PlugInRef
{
public:
  MYON_PlugInRef();

  // executable information
  MYON_UUID m_plugin_id;
  int m_plugin_type; // CRhinoPlugIn::plugin_type enum value
  int m_plugin_platform; // 0 = unknown, 1 = C++, 2 = .NET
  int m_plugin_sdk_version;
  int m_plugin_sdk_service_release;
  MYON_wString m_plugin_name;
  MYON_wString m_plugin_version;
  MYON_wString m_plugin_filename; // name of executable file

  // developer contact information
  MYON_wString m_developer_organization;
  MYON_wString m_developer_address;
  MYON_wString m_developer_country;
  MYON_wString m_developer_phone;
  MYON_wString m_developer_email;
  MYON_wString m_developer_website;
  MYON_wString m_developer_updateurl;
  MYON_wString m_developer_fax;

  void Default();

  bool Write( MYON_BinaryArchive& file ) const;
  bool Read( MYON_BinaryArchive& file );

  void Dump(MYON_TextLog& text_log) const;
};


#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ClassArray<MYON_PlugInRef>;
#endif

#endif

