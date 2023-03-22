#if !defined(OPENNURBS_EXAMPLE_UD_INC_)
#define OPENNURBS_EXAMPLE_UD_INC_

class CExampleWriteUserData : public MYON_UserData
{
  static int m__sn;
  MYON_OBJECT_DECLARE(CExampleWriteUserData);

public:
  static MYON_UUID Id();

  CExampleWriteUserData();
  virtual ~CExampleWriteUserData();

  CExampleWriteUserData( const char* s);
  CExampleWriteUserData(const CExampleWriteUserData& src);
  CExampleWriteUserData& operator=(const CExampleWriteUserData& src);

  void Dump( MYON_TextLog& text_log ) const override;
  bool GetDescription( MYON_wString& description ) override;
  bool Archive() const override;
  bool Write(MYON_BinaryArchive& file) const override;
  bool Read(MYON_BinaryArchive& file) override;

  MYON_wString m_str;
  int m_sn;
};

#endif
