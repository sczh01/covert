//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <AppEvnts.hpp>
#include <Clipbrd.hpp>
#include <IniFiles.hpp>
#include "MY_UTIL.H"
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
	TMemo *Memo1;
	TMemo *Memo2;
	TButton *BtnConvert1;
	TRadioGroup *rdoInputType;
	TRadioGroup *rdoOutputType;
	TLabeledEdit *LeCMD1;
	TLabeledEdit *LeDATA1;
	TLabeledEdit *LeDELAY1;
	TLabeledEdit *LeCMD2;
	TLabeledEdit *LeDATA2;
	TLabeledEdit *LeDELAY2;
	TApplicationEvents *MainAppEvents;
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TButton *BtnCopy2;
	TLinkLabel *LinkLabel1;
	TRadioGroup *if_out;
	TRadioGroup *if_in;
	TRadioGroup *if_type;
	TLabeledEdit *LeArrayHead;
	TLabeledEdit *LeArrayTrail;
	TLabeledEdit *LeArrayGap;
	TLabeledEdit *LeCmdPos;
	TLabeledEdit *LeDataPos;
	TLabeledEdit *LeDelayPos;
	TLabeledEdit *LeDataCntPos;
	TLabeledEdit *LeDataTypePos;
	TLabeledEdit *LeAnnotate;
	TLabeledEdit *LeExFunc;
	TButton *btnSave;
	TSaveDialog *saveDlgInitial;
	TCheckBox *MIPI_Data_Type;
	TEdit *Edit1;
	TEdit *Edit2;
	TEdit *Edit3;
	TEdit *Edit4;
	TEdit *Edit5;
	TEdit *Edit6;
	TCheckBox *AllGCS;
	TLinkLabel *LinkLabel_Other;
	TLabeledEdit *LeArrayGapOut;
	TOpenDialog *openDlgLogDir;
	TLabeledEdit *LabeledEdit1;
	TButton *btnOpenLogfile;
	TButton *btnSavePrj;
	TButton *btnOpenPrj;
	void __fastcall MainAppEventsMessage(tagMSG &Msg, bool &Handled);
	void __fastcall BtnConvert1Click(TObject *Sender);
	void __fastcall BtnCopy2Click(TObject *Sender);
	void __fastcall LinkLabel1Click(TObject *Sender);
	void __fastcall btnSaveClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Memo1Click(TObject *Sender);
	void __fastcall rdoInputTypeClick(TObject *Sender);
	void __fastcall OpenPrj(UnicodeString usPrjPath);
	void __fastcall SavePrj(UnicodeString usPrjPath);
	void __fastcall DoGSWAPForward( TStringList* psl );
	void __fastcall DoGSWAPBack( TStringList* psl );
	void __fastcall DoCIE( TStringList* psl );
	void __fastcall DoLineGenerate( TStringList* psl );
	void __fastcall LoadParser( );
private:	// User declarations
	struct CONVERT_SETTING
	{
		UnicodeString 	strCMD;
		UnicodeString 	strDATA;
		UnicodeString 	strAHead;
		UnicodeString 	strATrail;
		UnicodeString 	strAGap;
		UnicodeString 	strAHeadOut;
		UnicodeString 	strATrailOut;
		UnicodeString 	strAGapOut;
		int 			iCmdPos,iDataPos,iDTPos,iDelayPos,iDelaytime;
		unsigned char bRenesasFormat;
		unsigned char outIF_16bit;
		unsigned char inIF_16bit;
		unsigned char mipi_gcs_dcs_change;
		unsigned char mipi_always_gcs;
		unsigned char is_gcs;  //0: not gcs(dcs), 1: gcs
		unsigned char def_is_gcs;  //0: not gcs(dcs), 1: gcs
		unsigned char isShowDelay;
	};

	CONVERT_SETTING setting;
	UnicodeString S,S1,S2;
	int param_cnt;
	int param_cnt_16bit;
	int ex_param[64];
	LCD_Initial_Parser parser;
	void convert_16_8();


	void AddCMDPacket(TStringList* psl, DWORD cmd, vector<DWORD>* param, CONVERT_SETTING* setting);
	void AddCMDPacket(TStringList* psl, DWORD cmd, vector<DWORD>* param, CONVERT_SETTING* setting, DWORD type);
	void __fastcall TMainForm::FindDir(UnicodeString Dir, TStringList* FileList);
public:		// User declarations
	__fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
