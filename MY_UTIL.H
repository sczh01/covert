//---------------------------------------------------------------------------

#ifndef MY_UTIL_H
#define MY_UTIL_H

#include <cstdio>
#include <string>
#include <vector>
using namespace std;

//---------------------------------------------------------------------------

/*
输入格式：
1.一行只查找第一个W_COM和第一个W_DATA
2.仅带 // 注释检查
3.支持的格式：(对应LoadInitial里的type)
   格式1: 指令和参数分开，每个8/16位，例如 W_COM(0x0013); W_DATA(0x1A00);
   格式2: 指令和参数一起，每个16位，例如 LCD_CtrlWrite(0x0013, 0x1A00);
	格式3: 指令和参数一起，每个8位，(CMD,DATA,DATA)格式，例如 LCD_CtrlWrite(0x13, 0x1A, 0x00);
*/

/*
Output格式(每一个数格式):
[0-15]: 实际数据

[16-23]: 保留

[24]: 0 数据 / 1 指令
[25]: 0 使用[16] / 1 延时，忽略[16]，data为参数
[26]:
[27]:
[28]:
[29]:
[30]:
[31]:
*/

#define LCD_DATA          0x01000000
#define LCD_CMD           0x10000000
#define LCD_DELAY         0x20000000
#define LCD_DCS           0x40000000
#define LCD_RGB_Interface 0x80000000
#define MAX_DATA_LEN	  128

const DWORD DCS_LIST[] = {
	0x00, 0x01, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13,
	0x20, 0x21, 0x26, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E,
	0x30, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3C, 0x3E,
	0x44, 0x45,
	0xA1
};

typedef struct _cie_pad_{
	float mL;
	int   reg;
	float tL;
	float x,y;
	float tx,ty;
}CIEPAD;

class R6900x{
public:
float FIC_Chip;
float FIC_Clock;
float FIC_TPC_DETECT;
float FIC_TPC_PRD;
float FIC_TPC_RXCHG;
float FIC_TPC_TICB;
float FIC_TPC_NUM_B;
float FIC_TPC_NUM_H;
float FIC_TPC_CINT;
float FIC_TPC_VCHG;
float FIC_TPC_VOFSC;
float FIC_TPC_VHSP2;
float FIC_TPC_ICALSET;
float FIC_TPC_CIIRB;
float FIC_TPC_CITG;
float FIC_TPC_IGTSTART;
float FIC_CC_f;
float FIC_Coarse_Cal;
float FIC_Fine_Cal;
float FIC_Detail_Cal;
float FIC_ADCToCself;
float FIC_dCselfTodADC;
float FIC_dADCTodCself;

};

#define TX_NUM 72
#define RX_NUM 54

class Test_LogFile{
public:
	Test_LogFile():outstr(new TStringList()),instr(new TStringList()),pList(new TStringList()),pList1(new TStringList()){
		file_num_all=file_num_OK=file_num_NG=0;
		memset( (void*)trx, 0, TX_NUM*RX_NUM );
		memset( (void*)limit, 0, TX_NUM*RX_NUM*2 );
		memset( (void*)out_limit, 0, TX_NUM*RX_NUM*2 );
		memset( (void*)ref_limit, 0, TX_NUM*RX_NUM*2 );
		isUpdateRefLimit = false;
		isUpdateGui = false;
	};
	~Test_LogFile(){delete outstr; delete instr;delete pList;};
	int file_num_all,file_num_OK, file_num_NG;
	TStringList* outstr,*instr, *pList, *pList1;
	UnicodeString S,S1,S2,S_out;
	wstring str,str1,str2;

	float trx[TX_NUM][RX_NUM];
	float limit[TX_NUM][RX_NUM*2];
	float out_limit[TX_NUM][RX_NUM*2];
	float ref_limit[TX_NUM][RX_NUM*2];
	bool  isUpdateRefLimit;
	bool  isUpdateGui;
	TStringList *Load(UnicodeString file, int *param){
		instr->Clear();

		if ( FileExists( file ) ){
			instr->LoadFromFile(file);
			file_num_all++;
		}
		else
			return NULL;

		//instr->Assign( in );
		wstring tempstr,tempstr1,tempstr2;

		int sX,eX,len;

		switch( param[0] ){
		case 03://calcuate the FRC, ex_param: 03 tx_NUM rx_NUM
			S2 = L"";
			file_num_NG ++;
			for( int k=0; k<param[1]; k++ ){
				str = instr->Strings[k].c_str();
				S1 = str.c_str();
				pList->Delimiter = ' ';
				pList->DelimitedText = S1;
				pList->Count;

				for( int j=0; j<param[2]; j++ ){
					trx[k][j] += StrToInt( pList->Strings[j])*1.0/param[5]/param[6];
				}
			}
			break;
		}
		return outstr;
	}
	TStringList *Comp( int file_line, int col, int row, int start, int comp_range, int p2_comp_range, int comp_type, int p2_comp_type ){
		S2 = L"";
		for( int k=0; k<col /*param[1]*/; k++ ){
			//phase 1
			str = instr->Strings[file_line+start/*param[7]*/+k].c_str();
			size_t temp1 = str.find( L":" );
			str1 = str.substr(temp1+1,str.length() );
			S1 = str1.c_str();
			pList->Delimiter = ',';
			pList->DelimitedText = S1;
			pList->Count;

			//phase 2
			str = instr->Strings[file_line+start+col/*param[7]*/+k].c_str();
			temp1 = str.find( L":" );
			str1 = str.substr(temp1+1,str.length() );
			S1 = str1.c_str();
			pList1->Delimiter = ',';
			pList1->DelimitedText = S1;
			pList1->Count;


			if(  row <= pList->Count ){
				for( int j=0; j<row ; j++ ){
					trx[k][j] = StrToFloat( pList->Strings[j]);
					trx[k+col][j] = StrToFloat( pList1->Strings[j]);

					if( 1== comp_range ){ //external from limit
						 if( comp_type/*param[9]*/ == 2 ){   // <>  AMP open phase 1
							if( trx[k+col][j] < ref_limit[k+col][j*2] || trx[k+col][j] > ref_limit[k+col][j*2+1] ) {
								//error
								S.sprintf( L"P2(%02d:%02d)=%3.1f  ", k, j, trx[k+col][j] );
								S2+=S;
								isUpdateGui = true;
							}
						 }else if( comp_type == 1 ){  //>   ex: noise
							if( trx[k+col][j] > ref_limit[k+col][j*2] ){
							}
						 }else if( comp_type == 3 ){   //< ex:AMP open phase 2
							if( trx[k+col][j] < ref_limit[k+col][j*2] ){
							}
						 }else if( comp_type == 4 ){   //< ex:
							if( trx[k+col][j] < ref_limit[k+col][j*2] ){
							}
						 }
					}else{

					}

					if( 1==p2_comp_range){
						 if( p2_comp_type/*param[9]*/ == 2 ){   // <>  AMP open phase 1
							if( trx[k][j] < ref_limit[k][j*2] || trx[k][j] > ref_limit[k][j*2+1] ) {
								//error
								S.sprintf( L"P1(%02d:%02d)=%1.2f  ", k, j, trx[k][j] );
								S2+=S;
								isUpdateGui = true;
							}
						 }else if( p2_comp_type == 1 ){  //>   ex: noise
							if( trx[k][j] > ref_limit[k][j*2] ){
							}
						 }else if( p2_comp_type == 3 ){   //< ex:AMP open phase 2
							if( trx[k][j] < ref_limit[k][j*2] ){
							}
						 }else if( p2_comp_type == 4 ){   //< ex:
							if( trx[k][j] < ref_limit[k][j*2] ){
							}
						 }
					}else{

                    }
				}
			}
		}
	}

	TStringList *Comp( int file_line, int col, int row, int start, int ext_from, int comp_type ){
		S2 = L"";
		for( int k=0; k<col /*param[1]*/; k++ ){
			str = instr->Strings[file_line+start/*param[7]*/+k].c_str();
			size_t temp1 = str.find( L":" );
			str1 = str.substr(temp1+1,str.length() );
			S1 = str1.c_str();
			pList->Delimiter = ',';
			pList->DelimitedText = S1;
			pList->Count;

			if(  row <= pList->Count ){
				for( int j=0; j<row /*param[2]*/; j++ ){
					trx[k][j] = StrToFloat( pList->Strings[j]);
					if( 1== ext_from/*param[11]*/ ){ //external from limit
						 if( comp_type/*param[9]*/ == 2 ){   // <>  AMP open phase 1
							if( trx[k][j] < ref_limit[k][j*2] || trx[k][j] > ref_limit[k][j*2+1] ) {
								//error
								S.sprintf( L"(%02d:%02d)=%04f  ", k, j, trx[k][j] );
								S2+=S;
								isUpdateGui = true;
							}
						 }else if( comp_type == 1 ){  //>   ex: noise
							if( trx[k][j] > ref_limit[k][j*2] ){
							}
						 }else if( comp_type == 3 ){   //< ex:AMP open phase 2
							if( trx[k][j] < ref_limit[k][j*2] ){
							}
						 }else if( comp_type == 4 ){   //< ex:
							if( trx[k][j] < ref_limit[k][j*2] ){
							}
						 }
					}else{

					}
				}
			}
		}
	};
	TStringList *Load(UnicodeString file, wstring str_item,wstring str_index,wstring str_rv, int *param){
		wstring strTestItem,strContent,strDetail;
		bool isEnd = false;

		instr->Clear();

		if ( FileExists( file ) ){
			instr->LoadFromFile(file);
			file_num_all++;
		}
		else
			return NULL;

		//instr->Assign( in );

		int sX,eX,len;
		for( int i=0; i<instr->Count; i++){

			if( isEnd )
				break;

			strTestItem=instr->Strings[i].UpperCase().c_str();
			len = strTestItem.length();
			if( strTestItem.find(str_item) < len ){
				strContent = instr->Strings[i+1].UpperCase().c_str(); //next string
				strDetail = instr->Strings[i+2].UpperCase().c_str();

				switch( param[0] ){
				case 00: //find some fail item ==> calibration data test
					if( strContent.find(str_index)<strContent.length() )  {
						file_num_NG++;
						S.sprintf( L"NG#%04d:", file_num_all );
						S1 = instr->Strings[i+2].c_str();
						S+=S1;
						//StringReplace( S1,str_rv.c_str(),"",TReplaceFlags()<<rfReplaceAll );
						outstr->Add( S );
						break;
					}
					else{
						S.sprintf( L"OK#%04d:", file_num_all );
						S1 = instr->Strings[i+2].c_str();
						S+=S1;
						//StringReplace( S1,str_rv.c_str(),"",TReplaceFlags()<<rfReplaceAll );
						outstr->Add( S );
					}
					break;
				case 01:
					if( strContent.find(str_index)<strContent.length() )  {
						file_num_NG++;
						S.sprintf( L"NG#%04d:", file_num_all );
						S1 = instr->Strings[i+2].c_str();
						S+=S1;
						//StringReplace( S1,str_rv.c_str(),"",TReplaceFlags()<<rfReplaceAll );
						outstr->Add( S );
						break;
					}
					else{
						S2 = L"";
						for( int k=0; k<param[2]; k++ ){
							str = instr->Strings[i+6+k].c_str();
							size_t temp1 = str.find( L":" );
							str1 = str.substr(temp1+1,str.length() );
							//S1.assign(str1.begin(),str1.end());
							S1 = str1.c_str();
							pList->Delimiter = ',';
							pList->DelimitedText = S1;
							pList->Count;

							for( int j=0; j<param[1]; j++ ){
								S2+=pList->Strings[j];
								S2+=L"	";
							}
						}
						outstr->Add( S2 );
					}
					break;
				case 02://calcuate the FRC, ex_param: 02 tx_NUM rx_NUM
					S2 = L"";
					if( strContent.find(str_index)<strContent.length() || strDetail.find(str_rv)<strDetail.length() ){
						for( int k=0; k<param[1]; k++ ){
							str = instr->Strings[i+param[7]+k].c_str();
							size_t temp1 = str.find( L":" );
							str1 = str.substr(temp1+1,str.length() );
							S1 = str1.c_str();
							pList->Delimiter = ',';
							pList->DelimitedText = S1;
							pList->Count;

							if(  param[2] <= pList->Count ){
								for( int j=0; j<param[2]; j++ ){
									trx[k][j] += StrToFloat( pList->Strings[j]);
								}
							}else{
								//error control
							}
						}
						file_num_NG ++;
						isEnd = true;
					}
					else{

					}
					break;
				case 04://satis the NG sample,
					S2 = L"";     //find the data
					//update the limit
					if( strContent.find(str_index)<strContent.length() && (false==isUpdateRefLimit) ){
						for( int k=0; k<param[1]; k++ ){
							str = instr->Strings[i+param[10]+k].c_str();
							size_t temp1 = str.find( L":" );
							str1 = str.substr(temp1+1,str.length() );
							S1 = str1.c_str();
							pList->Delimiter = ',';
							pList->DelimitedText = S1;
							pList->Count;

							if(  param[2] <= pList->Count ){
								for( int j=0; j<param[2]; j++ ){
									for( int l=0; l< param[9];l++ )
										ref_limit[k][j*param[9]+l] = StrToFloat( pList->Strings[j*param[9]+l]);
								}
							}
						}
						if( param[12] == 2 ){ // phase 2
							for( int k=param[1]; k<param[1]+param[1]; k++ ){
								str = instr->Strings[i+param[10]+k].c_str();
								size_t temp1 = str.find( L":" );
								str1 = str.substr(temp1+1,str.length() );
								S1 = str1.c_str();
								pList->Delimiter = ',';
								pList->DelimitedText = S1;
								pList->Count;

								if(  param[2] <= pList->Count ){
									for( int j=0; j<param[2]; j++ ){
										for( int l=0; l< param[9];l++ )
											ref_limit[k][j*param[9]+l] = StrToFloat( pList->Strings[j*param[9]+l]);
									}
								}
							}
						}
						isUpdateRefLimit = true;
					}

					if( strContent.find(str_index)<strContent.length() ){


						if( param[12] == 2 ){ // phase 2
							Comp( i, param[1], param[2], param[7], param[11], param[14],param[9],param[13] );
						}else if( param[12] == 1 ){
							Comp( i, param[1], param[2], param[7], param[11], param[9] );
						}
						if( isUpdateGui ){
							S = ExtractFileName( file );
							S +=L"==>";
							S +=S2;
							outstr->Add( S );
							isUpdateGui = false;
						}

						file_num_NG ++;
						isEnd = true;
					}
					break;
				}
			}
		}
		return outstr;
	}
};
class LCD_Initial_Parser{
public:
	Test_LogFile 	log;
	wstring 		S_COM,S_DATA,S_DELAY,S_AHead,S_ATrail,S_AGap,S_Annotate;
	UnicodeString 	S_ACmdPos,S_ADataPos,S_ADelayPos,S_ADTPos,S_ADataCntPos,S_ExFunc,S_LogFileDir;
	TStringList*  	S_LogFileList;
	int 			in_data_type; //0:8bit, 1:16bit, 2: 24bit
	int				out_data_type; //0:8bit, 1:16bit, 2: 24bit
	int				ex_func;     //00 New way handle data 000000 compatible before.
	int				ex_param[64];
	UnicodeString	S1,S2,S3;

	TStringList* 	StringsIn;
	vector<DWORD> 	Output;
	vector<CIEPAD> 	out;

	LCD_Initial_Parser(): S_COM(L"W_COM"), S_DATA(L"W_DATA"), S_DELAY(L"DELAYMS"), StringsIn(new TStringList()),StringsOut(new TStringList()){
		Output.reserve(300);out.reserve(260);memset(ex_param,0,64);};
	~LCD_Initial_Parser(){delete StringsIn; delete StringsOut;};
	void LoadInitial(TStrings* In, int type = 1);
	void Convert(wstring str, int type);
	TStringList* DebugOut(void);

private:
	void BlankCropper(wstring& str);//去掉所有空格
	void BlankCropper(wstring& str, int pos );//
	void BlankCropper(wstring& str, int pos, wstring ch_op );
	void RenesasFormatCropper(wstring& str);//去掉所有空格、R、h
	TStringList* StringsOut;//debug用
	bool IsDCSCommand(DWORD cmd);
};

bool LCD_Initial_Parser::IsDCSCommand(DWORD cmd){
#if 0
	static size_t count = sizeof(DCS_LIST);
	for( size_t i = 0; i < count; i++ ){
		if ( DCS_LIST[i] == cmd )
			return true;
	}
	return false;
#endif
	return (cmd<0xB0)?true:false;
}

void LCD_Initial_Parser::LoadInitial(TStrings* In, int type)
{
	int i=0,j=0;
	TStringList *pList = NULL;
	StringsIn->Assign( In );
	Output.clear();//只清除数据，reserve的不变
	unsigned int  cmd=0, dat=0;

	pList = new TStringList();
	log.outstr->Clear();
	log.isUpdateRefLimit = false;
	for ( i = 0; i < StringsIn->Count; i++ ){
		if( type == 17 ){
			switch( ex_param[0] ) {
			case 3:
			log.Load( StringsIn->Strings[i].UpperCase().c_str(), ex_param );
			break;
			case 0:
			case 1:
			case 2:
			case 4:
			log.Load( StringsIn->Strings[i].UpperCase().c_str(), S_COM, S_DATA, S_DELAY, ex_param );
			break;
			}
		}else if( type == 20 ){
			S1 = StringsIn->Strings[i].UpperCase().c_str();
			pList->Delimiter = ' ';
			pList->DelimitedText = S1;

			if ( 2 == pList->Count ){ //1st is cmd, 2nd is data
				swscanf( pList->Strings[0].c_str(), L"%x", &cmd );
				Output.push_back( ( cmd & 0xFF) | LCD_CMD );
				swscanf( pList->Strings[1].c_str(), L"%x", &dat );
				Output.push_back( ( dat & 0xFF) | LCD_DATA );
			}else if( 1 == pList->Count ){  // it's data
				swscanf( pList->Strings[0].c_str(), L"%x", &dat );
				Output.push_back( ( dat & 0xFF) | LCD_DATA );
			}
		}else{
			Convert( StringsIn->Strings[i].UpperCase().c_str(), type );
		}
	}

	if( type == 17 ){
		if(0 == ex_param[0]){
		//sats the NG inforamtion
			//log.outstr->Clear();
			log.S.sprintf( L"Total Test Sample %04d: NG Sample %04d, Fail ratio=%03f%", log.file_num_all, log.file_num_NG,log.file_num_NG*100.0/log.file_num_all );
			log.outstr->Add( log.S );
		}
		else if(1 == ex_param[0]) {
			log.outstr->Clear();
		}
		else if(2 == ex_param[0] || 3 == ex_param[0]){  //calculate FRC limit
			log.outstr->Clear();
			for( i=0; i<ex_param[1]; i++ ){
				S1 = L"";
				for( j=0; j<ex_param[2]; j++ ){
					log.trx[i][j] /= log.file_num_NG;
					log.out_limit[i][j*2] = log.trx[i][j]*ex_param[3]/100;
					log.out_limit[i][j*2+1] = log.trx[i][j]*ex_param[4]/100;
					S1+= FloatToStrF( log.out_limit[i][j*2], ffNumber, 10, 2 );
					S1+=L"	"; //L"W_COM"
					S1+= FloatToStrF( log.out_limit[i][j*2+1],ffNumber, 10, 2  );
					S1+=L"	";
					log.trx[i][j] = 0.0;
				}
				log.outstr->Add( S1 );
			}
		}
	}

	for ( i = 0; i < Output.size(); i++ ){
		if ( Output[i] & LCD_CMD ){
			if ( IsDCSCommand(Output[i] & 0xFFFF) )
			Output[i] |= LCD_DCS;
		}
	}
	log.file_num_all =0;
	log.file_num_OK = 0;
	log.file_num_NG =0 ;
	if( pList )
		delete pList;
}

void LCD_Initial_Parser::Convert(wstring str, int type)
//string is zero-base
{
	size_t com_start = str.find( S_COM );
	size_t data_start = str.find( S_DATA );
	size_t delay_start = str.find( S_DELAY );
	size_t slash_pos = str.find( S_Annotate );
	size_t cmd_pos =  StrToIntDef(S_ACmdPos,0);
	size_t data_pos =  StrToIntDef(S_ADataPos,0);
	size_t delay_pos =  StrToIntDef(S_ADelayPos,0);
	size_t cnt_pos =  StrToIntDef(S_ADataCntPos,0);
	size_t dt_pos =  StrToIntDef(S_ADTPos,0);

	size_t str_length = str.length();
	size_t l_bracket_pos, r_bracket_pos, l_braces_pos, r_braces_pos, comma_pos, gap_pos, temp_pos;
	static wstring s_value, str_next, str_split[MAX_DATA_LEN], str_temp;
	static DWORD value_cmd, value_data, value_delay,value_data_array[MAX_DATA_LEN];
	bool isEnd=true;
	int  parser_count=1;
	size_t i=0,j=0;
	value_cmd = value_data = value_delay = 0;

	if( slash_pos <  str_length ){
	   	str = str.substr( 0, slash_pos - 1 );
		str_length = str.length();
	}

	switch( type )
	{
	  case 1://格式1: 指令和参数分开，每个8/16位，例如 W_COM(0x0013); W_DATA(0x1A00);
		if( 0x10==(ex_func&0xf0) ){  //for all 8bit output
			if ( com_start < str_length && com_start < slash_pos )//有W_COM
			{
				l_bracket_pos = str.find( L'(', com_start );
				r_bracket_pos = str.find( L')', com_start );
				if ( l_bracket_pos < str_length && r_bracket_pos < str_length )
				{
					s_value = str.substr( l_bracket_pos + 1, r_bracket_pos - l_bracket_pos - 1 );
					BlankCropper( s_value );
					swscanf( s_value.c_str(), L"%i", &value_cmd );
					if( in_data_type == 0 ){
						Output.push_back( (value_cmd & 0xFF) | LCD_CMD );
					}else if( in_data_type == 1 ){
						Output.push_back( (value_cmd & 0xFF) | LCD_CMD );
						Output.push_back( ((value_cmd & 0xFF00)>>8) | LCD_CMD );
					}else if( in_data_type == 2 ){
						Output.push_back( (value_cmd & 0xFF) | LCD_CMD );
						Output.push_back( ((value_cmd & 0xFF00)>>8) | LCD_CMD );
						Output.push_back( ((value_cmd & 0xFF0000)>>16) | LCD_CMD );
					}else if( in_data_type == 3 ){
						Output.push_back( (value_cmd & 0xFF) | LCD_CMD );
						Output.push_back( ((value_cmd & 0xFF00)>>8) | LCD_CMD );
						Output.push_back( ((value_cmd & 0xFF0000)>>16) | LCD_CMD );
						Output.push_back( ((value_cmd & 0xFF000000)>>24) | LCD_CMD );
					}
				}
			}

			if ( data_start < str_length && data_start < slash_pos )//有W_DATA
			{
				l_bracket_pos = str.find( L'(', data_start );
				r_bracket_pos = str.find( L')', data_start );
				if ( l_bracket_pos < str_length && r_bracket_pos < str_length )
				{
					s_value = str.substr( l_bracket_pos + 1, r_bracket_pos - l_bracket_pos - 1 );
					BlankCropper( s_value );
					swscanf( s_value.c_str(), L"%i", &value_data );

					if( in_data_type == 0 ){
						Output.push_back( (value_data & 0xFF) | LCD_DATA );
					}else if( in_data_type == 1 ){
						Output.push_back( (value_data & 0xFF) | LCD_DATA );
						Output.push_back( ((value_data & 0xFF00)>>8) | LCD_DATA );
					}else if( in_data_type == 2 ){
						Output.push_back( (value_data & 0xFF) | LCD_DATA );
						Output.push_back( ((value_data & 0xFF00)>>8) | LCD_DATA );
						Output.push_back( ((value_data & 0xFF0000)>>16) | LCD_DATA );
					}else if( in_data_type == 3 ){
						Output.push_back( (value_data & 0xFF) | LCD_DATA );
						Output.push_back( ((value_data & 0xFF00)>>8) | LCD_DATA );
						Output.push_back( ((value_data & 0xFF0000)>>16) | LCD_DATA );
						Output.push_back( ((value_data & 0xFF000000)>>24) | LCD_DATA );
					}
				}
			}
		}else if( 0x20==(ex_func&0xF0) ){
			if ( com_start < str_length && com_start < slash_pos )//有W_COM
			{
				l_bracket_pos = str.find( L'(', com_start );
				r_bracket_pos = str.find( L')', com_start );
				if ( l_bracket_pos < str_length && r_bracket_pos < str_length )
				{
					s_value = str.substr( l_bracket_pos + 1, str_length );
					BlankCropper( s_value );
					swscanf( s_value.c_str(), L"%i", &value_cmd );
					Output.push_back( (value_cmd & 0xFFFF) | LCD_CMD );
				}
			}
			if ( data_start < str_length && data_start < slash_pos )//有W_DATA
			{
				l_bracket_pos = str.find( L'(', data_start );
				r_bracket_pos = str.find( L')', data_start );
				if ( l_bracket_pos < str_length && r_bracket_pos < str_length )
				{
					s_value = str.substr( l_bracket_pos + 1, r_bracket_pos - l_bracket_pos - 1 );
					BlankCropper( s_value );
					swscanf( s_value.c_str(), L"%i", &value_data );
					Output.push_back( (value_data & 0xFFFF) | LCD_DATA );
				}
			}
		}else if( 0x30==(ex_func&0xF0) ){
			if ( com_start < str_length && com_start < slash_pos )//有W_COM
			{
				l_bracket_pos = str.find( L'=', com_start );

				if ( l_bracket_pos < str_length )
				{
					s_value = str.substr( l_bracket_pos + 1, r_bracket_pos - l_bracket_pos - 1 );

					if( s_value.find( L"0x", 0 )== 0xffffffff && s_value.find( L"0X", 0 )== 0xffffffff )
						s_value = L"0x" + s_value ;

					BlankCropper( s_value );
					swscanf( s_value.c_str(), L"%i", &value_cmd );
					Output.push_back( (value_cmd & 0xFFFF) | LCD_CMD );
				}
			}
			if ( data_start < str_length && data_start < slash_pos )//有W_DATA
			{
				l_bracket_pos = str.find( L'=', data_start );

				if ( l_bracket_pos < str_length  )
				{
					s_value = str.substr( l_bracket_pos + 1, str_length );
					if( s_value.find( L"0x", 0 )== 0xffffffff && s_value.find( L"0X", 0 )== 0xffffffff )
						s_value = L"0x" + s_value ;
					BlankCropper( s_value );
					swscanf( s_value.c_str(), L"%i", &value_data );
					Output.push_back( (value_data & 0xFFFF) | LCD_DATA );
				}
			}
		}		break;

		case 2://指令和参数分开，每个8/16位，例如 {setc, [107], W, 0x000B0}
		 if ( com_start < str_length && com_start < slash_pos )//有setc
		 {
				l_braces_pos = str.find( L'{' );
				r_braces_pos = str.find( L'}', com_start );
			if ( l_braces_pos < com_start && r_braces_pos < str_length )
			{
					comma_pos = str.rfind( L',' );
			   s_value = str.substr( comma_pos + 1, r_braces_pos - comma_pos - 1 );
			   BlankCropper( s_value );
					swscanf( s_value.c_str(), L"%i", &value_cmd );
			   Output.push_back( (value_cmd & 0xFFFF) | LCD_CMD );
			}
		 }

		 if ( data_start < str_length && data_start < slash_pos )//有setp
		 {
				l_braces_pos = str.find( L'{' );
				r_braces_pos = str.find( L'}', data_start );
			if ( l_braces_pos < data_start && r_braces_pos < str_length )
			{
					comma_pos = str.rfind( L',' );
			   s_value = str.substr( comma_pos + 1, r_braces_pos - comma_pos - 1 );
			   BlankCropper( s_value );
					swscanf( s_value.c_str(), L"%i", &value_data );
			   Output.push_back( (value_data & 0xFFFF) | LCD_DATA );
			}
		 }
			break;
	  case 3://格式1: 指令和参数分开，每个8/16位，例如 Set(0x0013,0x1A00,0x2200,...);
		 if ( com_start < str_length && com_start < slash_pos )//有com_start
		 {
			//First is command
			l_bracket_pos = str.find( S_AHead, com_start );
			r_bracket_pos = str.find( S_ATrail, com_start );
			gap_pos =     str.find( S_AGap, com_start );
			if ( l_bracket_pos < str_length && r_bracket_pos < str_length && gap_pos < r_bracket_pos )
			{
			   s_value = str.substr( l_bracket_pos + 1, gap_pos - l_bracket_pos - 1 );
			   BlankCropper( s_value );
			   swscanf( s_value.c_str(), L"%i", &value_cmd );
			   Output.push_back( (value_cmd & 0xFFFF) | LCD_CMD );
			}
			else if( l_bracket_pos < str_length && r_bracket_pos < str_length ) //
			{
			   s_value = str.substr( l_bracket_pos + 1, r_bracket_pos - l_bracket_pos - 1 );
			   BlankCropper( s_value );
			   swscanf( s_value.c_str(), L"%i", &value_cmd );
			   Output.push_back( (value_cmd & 0xFFFF) | LCD_CMD );
			   break; //no parameter
			}
			else
			{
				break;
			}
			//Next is parameter
			str_next = str.substr(  gap_pos + 1, r_bracket_pos );
			while( isEnd ){
				l_bracket_pos = str_next.find( S_AGap, com_start );
				r_bracket_pos = str_next.find( S_ATrail, com_start );

				if(   l_bracket_pos <   r_bracket_pos ){
					s_value = str_next.substr( 0, l_bracket_pos  );
					BlankCropper( s_value );
					swscanf( s_value.c_str(), L"%i", &value_data );
					Output.push_back( (value_data & 0xFFFF) | LCD_DATA );
					str_next = str_next.substr(  l_bracket_pos + 1, r_bracket_pos );
				}
				else
				{
					s_value = str_next.substr( 0, r_bracket_pos  );
					BlankCropper( s_value );
					swscanf( s_value.c_str(), L"%i", &value_data );
					Output.push_back( (value_data & 0xFFFF) | LCD_DATA );
					str_next = str_next.substr(  l_bracket_pos + 1, r_bracket_pos );
					isEnd = false;
				}
			}
		 }
		 break;
	  case 5://格式1: 指令和参数分开，每个8/16位，例如 Set 0x13 0x1A 0x22 //xxxx
		 if ( com_start < str_length && com_start < slash_pos )//有com_start
		 {
			//First is Line start
			l_bracket_pos = str.find( S_AHead, com_start );
			//Next is Register setting
			str_next = str.substr(  l_bracket_pos + 1, str_length );
			BlankCropper( str_next );
			while( isEnd ){
				l_bracket_pos = str_next.find( S_AGap, 0 );
				r_bracket_pos = str_next.length();

				if(   l_bracket_pos <   r_bracket_pos ){
					if( parser_count == cmd_pos ){
						s_value = str_next.substr( 0, l_bracket_pos  );
						BlankCropper( s_value );
						swscanf( s_value.c_str(), L"%i", &value_data );
						Output.push_back( (value_data & 0xFFFF) | LCD_CMD );
						isEnd = false;
					}
					else if(  parser_count == delay_pos  )
					{
						s_value = str_next.substr( 0, l_bracket_pos  );
						BlankCropper( s_value );
						swscanf( s_value.c_str(), L"%i", &value_delay );
						Output.push_back( value_delay | LCD_DELAY );
				   }
				}
				else{
					isEnd = false;
				}
				str_next = str_next.substr(  l_bracket_pos + 1, r_bracket_pos );
				BlankCropper( str_next );
				parser_count ++;
			}

			isEnd = true;

			while( isEnd ){
				l_bracket_pos = str_next.find( S_AGap, 0 );
				r_bracket_pos = str_next.length();

				if(   l_bracket_pos <   r_bracket_pos ){
					if( parser_count >= data_pos ){
						s_value = str_next.substr( 0, l_bracket_pos  );
						BlankCropper( s_value );
						swscanf( s_value.c_str(), L"%i", &value_data );
						Output.push_back( (value_data & 0xFFFF) | LCD_DATA );
					}
					else if(  parser_count == delay_pos  )
					{
						s_value = str_next.substr( 0, l_bracket_pos  );
						BlankCropper( s_value );
						swscanf( s_value.c_str(), L"%i", &value_delay );
					   //	Output.push_back( value_delay | LCD_DELAY );
				   }
				}
				else
				{
					s_value = str_next.substr( 0, r_bracket_pos  );
					BlankCropper( s_value );
					swscanf( s_value.c_str(), L"%i", &value_data );
					Output.push_back( (value_data & 0xFFFF) | LCD_DATA );
					isEnd = false;
                }

				str_next = str_next.substr(  l_bracket_pos + 1, r_bracket_pos );
				BlankCropper( str_next );
				parser_count ++;
			}

			if( delay_pos > 0 )
			{
				Output.push_back( value_delay | LCD_DELAY );
            }
		 }
		 break;
	  case 6://格式1: 指令和参数分开，每个8/16位，例如 Set 0x13 0x1A 0x22 //xxxx
		 if ( com_start < str_length && com_start < slash_pos )//有com_start
		 {
			//First is Line start
			if( 0xffffffff !=(l_bracket_pos = str.find( L"REGFLAG_DELAY", com_start )) )
			{
				l_bracket_pos = str.find( S_AGap, com_start );
				r_bracket_pos = str.length();
				str_next = str.substr(  l_bracket_pos + 1, r_bracket_pos );
				l_bracket_pos = str_next.find( S_AGap, com_start );
				str_next = str_next.substr(  0, l_bracket_pos );

				BlankCropper( str_next, 1 );


				if( str_next.find( L"0x", 0 )== 0xffffffff && str_next.find( L"0X", 0 )== 0xffffffff )
					str_split[0] = L"0x";
				else
					str_split[0] = L"";

				str_split[0] += str_temp;

				swscanf( str_split[parser_count].c_str(), L"%i", &value_data_array[0] );
				if( value_data_array[0] > 0 )
					Output.push_back( value_data_array[0] | LCD_DELAY );

				break;

            }

			if( 0xffffffff != str.find( L"REGFLAG_END_OF_TABLE", com_start ) )
				break;

			l_bracket_pos = str.find( S_AHead, com_start );

			if(l_bracket_pos == 0xffffffff)
				break;

			//Next is Register setting
			str_next = str.substr(  l_bracket_pos + 1, str_length );
			BlankCropper( str_next );
			parser_count = 0;

			while( isEnd ){       //paraser a line
				l_bracket_pos = str_next.find( S_AGap, 0 );
				r_bracket_pos = str_next.length();
				if( str_next.find( S_Annotate )< r_bracket_pos )
					temp_pos =  str_next.find( S_Annotate );
				else
					temp_pos =  str_next.find( S_ATrail, 0 );

				if( l_bracket_pos > temp_pos )
				{
					l_bracket_pos = temp_pos;
					isEnd = false;
				}

				if( r_bracket_pos==0 )
				{
					parser_count--;
					break;
				}

				if( l_bracket_pos == 0xffffffff  )
				{
					l_bracket_pos = str_next.find( S_ATrail, 0 );
					isEnd = false;
				}

				if(   l_bracket_pos <  r_bracket_pos ){

					str_temp = str_next.substr( 0, l_bracket_pos  );
					BlankCropper( str_temp, 1 );
					BlankCropper( str_temp, 0, L"{" );
					if( str_temp.find( L"0x", 0 )== 0xffffffff && str_temp.find( L"0X", 0 )== 0xffffffff )
						str_split[parser_count] = L"0x";
					else
						str_split[parser_count] = L"";

					str_split[parser_count] += str_temp;

					swscanf( str_split[parser_count].c_str(), L"%i", &value_data_array[parser_count] );
					str_next = str_next.substr(  l_bracket_pos + 1, r_bracket_pos );
					BlankCropper( str_next, 0 );

					if( isEnd )
						parser_count++;
				}
				else
				{
					str_split[parser_count] = str_next.substr( 0, r_bracket_pos  );
					swscanf( str_split[parser_count].c_str(), L"%i", &value_data_array[parser_count] );
					isEnd = false;
				}
				if(   parser_count >= MAX_DATA_LEN )
				{
					//Error
					isEnd = false;
				}
			}
			isEnd = true;

			Output.push_back( (value_data_array[cmd_pos-1] & 0xFFFF) | LCD_CMD );

			if( data_pos > 0 && parser_count < (MAX_DATA_LEN - 4) ) {
				for( int i=data_pos;i<= parser_count+1;i++ ){
					Output.push_back( (value_data_array[i-1] & 0xFFFF) | LCD_DATA );
				}
			}
			else if( parser_count >= (MAX_DATA_LEN - 4))
			{
				for( int i=data_pos;i<MAX_DATA_LEN-data_pos;i++ ){
					Output.push_back( (value_data_array[i-1] & 0xFFFF) | LCD_DATA );
				}
			}

			if( delay_pos > 0 )
			{
				if( value_data_array[delay_pos-1] > 0 )
					Output.push_back( value_data_array[delay_pos-1] | LCD_DELAY );
			}
			else
			{
				Output.push_back( 0x1 | LCD_DELAY );
			}
		 }
		 break;
	  case 7://格式7: cmd 0x70xxxx, data 0x72yyyy
		 if ( com_start < str_length && com_start < slash_pos )//有W_COM
		 {
				//l_bracket_pos = str.find( L'(', com_start );
				//r_bracket_pos = str.find( L')', com_start );
				s_value = L"0x";
			   s_value+=str.substr( com_start + 4, 4 );
			   BlankCropper( s_value );
					swscanf( s_value.c_str(), L"%i", &value_cmd );
					Output.push_back( (value_cmd & 0xFFFF) | LCD_CMD );
		 }

		 if ( data_start < str_length && data_start < slash_pos )//有W_DATA
		 {
				s_value = L"0x";
			   s_value+=str.substr( data_start + 4, 4 );
			   BlankCropper( s_value );
					swscanf( s_value.c_str(), L"%i", &value_data );
			   Output.push_back( (value_data & 0xFFFF) | LCD_DATA );

		 }
		break;
	  case 8:
		if( in_data_type == 0 ){ //dec
			s_value=str.substr( com_start , 3 );
			BlankCropper( s_value );
			swscanf( s_value.c_str(), L"%i", &value_cmd );
			Output.push_back( value_cmd & 0xFF );
		}
		else{
			if( (str_next.find(L"x", 0 )==1) || (str_next.find(L"X", 0 )==1) )
				s_value = L"";
			else
				s_value = L"0x";

			s_value+=str.substr( com_start , 3 );
			BlankCropper( s_value );
			swscanf( s_value.c_str(), L"%i", &value_cmd );
			Output.push_back( value_cmd & 0xFF );
	   }

		break;
	  case 9:
		str_next = str;
		BlankCropper( str_next );
		parser_count = 0;

		while( isEnd ){       //paraser a line
			l_bracket_pos = str_next.find( S_AGap, 0 );
			r_bracket_pos = str_next.length();

			if(   l_bracket_pos <   r_bracket_pos ){
				if( in_data_type == 1) //hex
					if( (str_next.find(L"x", 0 )==1) || (str_next.find(L"X", 0 )==1) )
						str_split[parser_count] = L"";
					else
						str_split[parser_count] = L"0x";
				else
						str_split[parser_count] = L"";

				str_split[parser_count] += str_next.substr( 0, l_bracket_pos  );
				BlankCropper(str_split[parser_count]);
				swscanf( str_split[parser_count].c_str(), L"%i", &value_data_array[parser_count] );
				str_next = str_next.substr(  l_bracket_pos + 1, r_bracket_pos );
				BlankCropper( str_next );
				parser_count++;
			}
			else
			{
				if( in_data_type == 1) //hex
					if( (str_next.find(L"x", 0 )==1) || (str_next.find(L"X", 0 )==1) )
						str_split[parser_count] = L"";
					else
						str_split[parser_count] = L"0x";
				else   //Dec
						str_split[parser_count] = L"";

				str_split[parser_count] += str_next.substr( 0, r_bracket_pos  );
				BlankCropper(str_split[parser_count]);
				swscanf( str_split[parser_count].c_str(), L"%i", &value_data_array[parser_count] );
				isEnd = false;
			}
			if(   parser_count >= MAX_DATA_LEN )
			{
				//Error
				isEnd = false;
			}
		}

		isEnd = true;


		for( int i=0;i<= parser_count;i++ ){
			Output.push_back( value_data_array[i] & 0xFF );
		}

		break;
	  case 10://格式 10: Linux kernel .dtsi 文件 例如  39 01 00 00 00 00 05 c7 00 10 00 10
		 if ( com_start < str_length && com_start < slash_pos )//有com_start
		 {
			//First is Line start
			l_bracket_pos = str.find( S_AHead, com_start );
			//Next is Register setting
			if( S_AHead == L"" )
				str_next = str.substr(  l_bracket_pos , str_length );
			else
				str_next = str.substr(  l_bracket_pos + 1, str_length );
			//str_next.Rtrim();

			BlankCropper( str_next, 0 );
			parser_count = 0;

			while( isEnd ){       //paraser a line
				l_bracket_pos = str_next.find( S_AGap, 0 );
				r_bracket_pos = str_next.length();

				if(   (l_bracket_pos !=0) && (l_bracket_pos <   r_bracket_pos) ){
					str_temp = str_next.substr( 0, l_bracket_pos  );
					str_split[parser_count] = L"";

					//if( str_temp.find( L"0x", 0 )== 0xffffffff && str_temp.find( L"0X", 0 )== 0xffffffff )
					//	str_split[parser_count] = L"0x";
					//else
					//	str_split[parser_count] = L"";

					str_split[parser_count] +=  str_temp;
					BlankCropper(s_value);
					swscanf( str_split[parser_count].c_str(), L"%i", &value_data_array[parser_count] );
					str_next = str_next.substr(  l_bracket_pos + 1, r_bracket_pos );
					BlankCropper( str_next, 0 );
					parser_count++;
				}
				else
				{
					if(  l_bracket_pos ==0 )
					{
						parser_count--;
						isEnd = false;
					}
					else{
						str_temp = str_next.substr( 0, r_bracket_pos  );
						if( str_temp.find( L"0x", 0 )== 0xffffffff && str_temp.find( L"0X", 0 )== 0xffffffff )
							str_split[parser_count] = L"0x";
						else
							str_split[parser_count] = L"";
						str_split[parser_count] +=  str_temp;
						BlankCropper(s_value);
						swscanf( str_split[parser_count].c_str(), L"%i", &value_data_array[parser_count] );
						isEnd = false;
					}
				}

				if(   parser_count >= MAX_DATA_LEN )
				{
					//Error
					isEnd = false;
                }
			}

			isEnd = true;
			switch( ex_func&0xf ){
			case 0: //example: 39 01 00 00 00 00 05 c7 00 10 00 10
				Output.push_back( (value_data_array[cmd_pos-1] & 0xFFFF) | LCD_CMD );

				if( data_pos > 0 && parser_count < (MAX_DATA_LEN - 4) ) {
					for( int i=data_pos;i<= parser_count+1;i++ ){
						if( cnt_pos == 0 )
						{
							Output.push_back( (value_data_array[i-1] & 0xFFFF) | LCD_DATA );
						}
						else if( (value_data_array[cnt_pos-1] & 0xFFFF) > (i-data_pos))
							Output.push_back( (value_data_array[i-1] & 0xFFFF) | LCD_DATA );
						else
							break;
					}
				}
				else if( parser_count >= (MAX_DATA_LEN - 4))
				{
					for( int i=data_pos;i<MAX_DATA_LEN-data_pos;i++ ){
						if( cnt_pos == 0 )
						{
							Output.push_back( (value_data_array[i-1] & 0xFFFF) | LCD_DATA );
						}
						else if( (value_data_array[cnt_pos-1] & 0xFFFF) > (i-data_pos))
							Output.push_back( (value_data_array[i-1] & 0xFFFF) | LCD_DATA );
						else
							break;
					}
				}
				break;

			case 1:     //just for Syna Lilac2 format for TD4191
				if( cnt_pos == 0 ){
					Output.push_back( (value_data_array[data_pos-1] & 0xFFFF) | LCD_CMD );
					Output.push_back( ((value_data_array[data_pos-1]>>8) & 0xFFFF) | LCD_DATA );
				}
				else if( (value_data_array[cnt_pos-1] & 0xFFFF) > (i-data_pos))
					Output.push_back( (value_data_array[i-1] & 0xFFFF) | LCD_DATA );
				else
					break;
				Output.push_back( (value_data_array[cmd_pos-1] & 0xFFFF) | LCD_DATA );
				Output.push_back( ((value_data_array[cmd_pos-1]>>8) & 0xFFFF) | LCD_DATA );
				break;

			case 2:     //just for MTK register table {0x00082902,0x000155C3,0x00000001,0x01};
				int data_type  =  (value_data_array[0]&0x00ff00)>>8;
				int param_size =  (value_data_array[0]&0x00ff0000)>>16;

				if( data_type == 0x29 || data_type == 0x39 ){
					Output.push_back( (value_data_array[1] & 0xFF) | LCD_CMD );

					if( param_size/4 >= 1 ){
						Output.push_back( (value_data_array[1] & 0xFF00)>>8 | LCD_DATA );
						Output.push_back( (value_data_array[1] & 0xFF0000)>>16 | LCD_DATA );
						Output.push_back( (value_data_array[1] & 0xFF000000)>>24 | LCD_DATA );
						for( int i=2;i<= param_size/4;i++ ){
							Output.push_back( (value_data_array[i] & 0xFF) | LCD_DATA );
							Output.push_back( (value_data_array[i] & 0xFF00)>>8 | LCD_DATA );
							Output.push_back( (value_data_array[i] & 0xFF0000)>>16 | LCD_DATA );
							Output.push_back( (value_data_array[i] & 0xFF000000)>>24 | LCD_DATA );
						}
						switch( param_size%4 ){
						case 1:
							Output.push_back( (value_data_array[param_size/4+1] & 0xFF) | LCD_DATA );
						break;
						case 2:
							Output.push_back( (value_data_array[param_size/4+1] & 0xFF) | LCD_DATA );
							Output.push_back( (value_data_array[param_size/4+1] & 0xFF00)>>8 | LCD_DATA );
						break;
						case 3:
							Output.push_back( (value_data_array[param_size/4+1] & 0xFF) | LCD_DATA );
							Output.push_back( (value_data_array[param_size/4+1] & 0xFF00)>>8 | LCD_DATA );
							Output.push_back( (value_data_array[param_size/4+1] & 0xFF0000)>>16 | LCD_DATA );
						break;
						}
					}

					else if( ((param_size-1)/4) == 0 ){
						switch( (param_size-1)%4 ){
						case 1:
							Output.push_back( (value_data_array[1] & 0xFF00)>>8 | LCD_DATA );
						break;
						case 2:
							Output.push_back( (value_data_array[1] & 0xFF00)>>8 | LCD_DATA );
							Output.push_back( (value_data_array[1] & 0xFF0000)>>16 | LCD_DATA );
						break;
						case 3:
							Output.push_back( (value_data_array[1] & 0xFF00)>>8 | LCD_DATA );
							Output.push_back( (value_data_array[1] & 0xFF0000)>>16 | LCD_DATA );
							Output.push_back( (value_data_array[1] & 0xFF000000)>>24 | LCD_DATA );
						break;
						}
					}
					if(  value_data_array[(param_size-1)/4+2] > 0 )
						Output.push_back( value_data_array[(param_size-1)/4+2] | LCD_DELAY );
				}else if( data_type == 0x05){
					Output.push_back( param_size | LCD_CMD );
					Output.push_back( value_data_array[1] | LCD_DELAY );
				}
				break;
//				if( delay_pos > 0 ){
//					if(  value_data_array[delay_pos-1] > 0 )
//						Output.push_back( value_data_array[delay_pos-1] | LCD_DELAY );
//				}
			}
		 }
		 break;
	  case 11://Excel format
		 if ( com_start < str_length && com_start < slash_pos )//有com_start
		 {
			//First is Line start
			S_AGap = L"	";
			l_bracket_pos = str.find( S_AGap, com_start );   //cmd
			if( (l_bracket_pos != 0) && (l_bracket_pos<str_length) ){
				str_split[0] = L"0x";
				str_split[0] += str.substr( com_start, l_bracket_pos  );
				swscanf( str_split[0].c_str(), L"%i", &value_data_array[0] );
				str_split[1] = L"0x";
				str_split[1] += str.substr(  l_bracket_pos + 1, str_length );
				swscanf( str_split[1].c_str(), L"%i", &value_data_array[1] );
				Output.push_back( (value_data_array[0] & 0xFFFF) | LCD_CMD );
				Output.push_back( (value_data_array[1] & 0xFFFF) | LCD_DATA );
			}
			else if(  l_bracket_pos == 0 ){    //data
				str_split[0] = L"0x";
				str_split[0] += str.substr( l_bracket_pos+1, str_length );
				swscanf( str_split[0].c_str(), L"%i", &value_data_array[0] );
				Output.push_back( (value_data_array[0] & 0xFFFF) | LCD_DATA );
			}
		 }
		 break;
	  case 14:
		if( in_data_type == 0 ){ //dec
			s_value=str.substr( com_start , 3 );
			BlankCropper( s_value );
			swscanf( s_value.c_str(), L"%i", &value_cmd );
			Output.push_back( value_cmd | LCD_DATA );
		}
		else{
			s_value = L"";

			s_value+=str.substr( com_start , 4 );
			BlankCropper( s_value );
			swscanf( s_value.c_str(), L"%i", &value_cmd );
			Output.push_back( value_cmd |LCD_DATA );
	   }

		break;    //out

	  case 15:

			//First is Line start
		S_AGap = L"	";
		l_bracket_pos = str.find( S_AGap, com_start );   //cmd
		if( (l_bracket_pos != 0) && (l_bracket_pos<str_length) ){
			str_split[0] = L"0x";
			str_split[0] += str.substr( com_start, l_bracket_pos  );
			swscanf( str_split[0].c_str(), L"%i", &value_data_array[0] );
			str_split[1] = L"0x";
			str_split[1] += str.substr(  l_bracket_pos + 1, str_length );
			swscanf( str_split[1].c_str(), L"%i", &value_data_array[1] );
			Output.push_back( (value_data_array[0] & 0xFFFF) | LCD_CMD );
			Output.push_back( (value_data_array[1] & 0xFFFF) | LCD_DATA );
		}
	   break;
	  case 16://For ELPCS, ex E7422   mipi.write 0x39 0x51  0xFF
		 if ( com_start < str_length && com_start < slash_pos )//有com_start
		 {
			//First is Line start
			l_bracket_pos = str.find( S_AHead, com_start );
			//Next is Register setting
			if( S_AHead == L"" )
				str_next = str.substr(  l_bracket_pos , str_length );
			else
				str_next = str.substr(  l_bracket_pos + 1, str_length );
			//str_next.Rtrim();

			BlankCropper( str_next, 0 );
			parser_count = 0;

			while( isEnd ){       //paraser a line
				l_bracket_pos = str_next.find( S_AGap, 0 );
				r_bracket_pos = str_next.length();

				if(   (l_bracket_pos !=0) && (l_bracket_pos <   r_bracket_pos) ){
					str_split[parser_count] = L"0x";
					str_split[parser_count] += str_next.substr( 0, l_bracket_pos  );
					BlankCropper(s_value);
					swscanf( str_split[parser_count].c_str(), L"%i", &value_data_array[parser_count] );
					str_next = str_next.substr(  l_bracket_pos + 1, r_bracket_pos );
					BlankCropper( str_next, 0 );
					parser_count++;
				}
				else
				{
					if(  l_bracket_pos ==0 )
					{
						parser_count--;
						isEnd = false;
					}
					else{
						str_split[parser_count] = L"0x";
						str_split[parser_count] += str_next.substr( 0, r_bracket_pos  );
						BlankCropper(s_value);
						swscanf( str_split[parser_count].c_str(), L"%i", &value_data_array[parser_count] );
						isEnd = false;
					}
				}

				if(   parser_count >= MAX_DATA_LEN )
				{
					//Error
					isEnd = false;
                }
			}

			isEnd = true;
			switch( ex_func ){
				case 0:
				Output.push_back( (value_data_array[cmd_pos-1] & 0xFFFF) | LCD_CMD );

				if( data_pos > 0 && parser_count < (MAX_DATA_LEN - 4) ) {
					for( int i=data_pos;i<= parser_count+1;i++ ){
						if( cnt_pos == 0 )
						{
							Output.push_back( (value_data_array[i-1] & 0xFFFF) | LCD_DATA );
						}
						else if( (value_data_array[cnt_pos-1] & 0xFFFF) > (i-data_pos))
							Output.push_back( (value_data_array[i-1] & 0xFFFF) | LCD_DATA );
						else
							break;
					}
				}
				else if( parser_count >= (MAX_DATA_LEN - 4))
				{
					for( int i=data_pos;i<MAX_DATA_LEN-data_pos;i++ ){
						if( cnt_pos == 0 )
						{
							Output.push_back( (value_data_array[i-1] & 0xFFFF) | LCD_DATA );
						}
						else if( (value_data_array[cnt_pos-1] & 0xFFFF) > (i-data_pos))
							Output.push_back( (value_data_array[i-1] & 0xFFFF) | LCD_DATA );
						else
							break;
					}
				}
				break;

				case 1:     //just for Syna Lilac2 format

				//for( int i=data_pos;i<= parser_count+1;i++ ){
					if( cnt_pos == 0 )
					{
						//Output.push_back( (value_data_array[i-1] & 0xFFFF) | LCD_DATA );
						Output.push_back( (value_data_array[data_pos-1] & 0xFFFF) | LCD_CMD );
						Output.push_back( ((value_data_array[data_pos-1]>>8) & 0xFFFF) | LCD_DATA );
					}
					else if( (value_data_array[cnt_pos-1] & 0xFFFF) > (i-data_pos))
						Output.push_back( (value_data_array[i-1] & 0xFFFF) | LCD_DATA );
					else
						break;
				//}
				Output.push_back( (value_data_array[cmd_pos-1] & 0xFFFF) | LCD_DATA );
				Output.push_back( ((value_data_array[cmd_pos-1]>>8) & 0xFFFF) | LCD_DATA );


				break;
			}
			if( delay_pos > 0 )
			{
				Output.push_back( value_data_array[delay_pos-1] | LCD_DELAY );
			}
		 }
		 break;
	  case 19://格式1: 指令和参数分开，每个8/16位，例如 Set 0x13 0x1A 0x22 //xxxx
		 if ( com_start < str_length && com_start < slash_pos )//有com_start
		 {
			//First is Line start
			if( 0xffffffff !=(l_bracket_pos = str.find( L"REGFLAG_DELAY", com_start )) )
			{
				l_bracket_pos = str.find( S_AGap, com_start );
				r_bracket_pos = str.length();
				str_next = str.substr(  l_bracket_pos + 1, r_bracket_pos );
				l_bracket_pos = str_next.find( S_AGap, com_start );
				str_next = str_next.substr(  0, l_bracket_pos );

				BlankCropper( str_next, 1 );


				if( str_next.find( L"0x", 0 )== 0xffffffff && str_next.find( L"0X", 0 )== 0xffffffff )
					str_split[0] = L"0x";
				else
					str_split[0] = L"";

				str_split[0] += str_temp;

				swscanf( str_split[parser_count].c_str(), L"%i", &value_data_array[0] );
				if( value_data_array[0] > 0 )
					Output.push_back( value_data_array[0] | LCD_DELAY );

				break;

            }

			if( 0xffffffff != str.find( L"REGFLAG_END_OF_TABLE", com_start ) )
				break;

			l_bracket_pos = str.find( S_AHead, com_start );

			if(l_bracket_pos == 0xffffffff)
				break;

			//Next is Register setting
			str_next = str.substr(  l_bracket_pos + 1, str_length );
			BlankCropper( str_next );
			parser_count = 0;

			while( isEnd ){       //paraser a line
				l_bracket_pos = str_next.find( S_AGap, 0 );
				r_bracket_pos = str_next.length();
				if( str_next.find( S_Annotate )< r_bracket_pos )
					temp_pos =  str_next.find( S_Annotate );
				else
					temp_pos =  str_next.find( S_ATrail, 0 );

				if( l_bracket_pos > temp_pos )
				{
					l_bracket_pos = temp_pos;
					isEnd = false;
				}

				if( r_bracket_pos==0 )
				{
					parser_count--;
					break;
				}

				if( l_bracket_pos == 0xffffffff  )
				{
					l_bracket_pos = str_next.find( S_ATrail, 0 );
					isEnd = false;
				}

				if(   l_bracket_pos <  r_bracket_pos ){

					str_temp = str_next.substr( 0, l_bracket_pos  );
					BlankCropper( str_temp, 1 );
					BlankCropper( str_temp, 0, L"{" );
					if( str_temp.find( L"0x", 0 )== 0xffffffff && str_temp.find( L"0X", 0 )== 0xffffffff )
						str_split[parser_count] = L"0x";
					else
						str_split[parser_count] = L"";

					str_split[parser_count] += str_temp;

					swscanf( str_split[parser_count].c_str(), L"%i", &value_data_array[parser_count] );
					str_next = str_next.substr(  l_bracket_pos + 1, r_bracket_pos );
					BlankCropper( str_next, 0 );

					if( isEnd )
						parser_count++;
				}
				else
				{
					str_split[parser_count] = str_next.substr( 0, r_bracket_pos  );
					swscanf( str_split[parser_count].c_str(), L"%i", &value_data_array[parser_count] );
					isEnd = false;
				}
				if(   parser_count >= MAX_DATA_LEN )
				{
					//Error
					isEnd = false;
				}
			}
			isEnd = true;

			if( dt_pos > 0 ){
				value_data_array[cmd_pos-1] |=  (value_data_array[dt_pos-1]<<16);
			}
			Output.push_back( (value_data_array[cmd_pos-1] & 0xFFFF) | LCD_CMD );

			if( data_pos > 0 && parser_count < (MAX_DATA_LEN - 4) ) {
				for( int i=data_pos;i<= parser_count+1;i++ ){
					Output.push_back( (value_data_array[i-1] & 0xFFFF) | LCD_DATA );
				}
			}
			else if( parser_count >= (MAX_DATA_LEN - 4))
			{
				for( int i=data_pos;i<MAX_DATA_LEN-data_pos;i++ ){
					Output.push_back( (value_data_array[i-1] & 0xFFFF) | LCD_DATA );
				}
			}

			if( delay_pos > 0 )
			{
				if( value_data_array[delay_pos-1] > 0 )
					Output.push_back( value_data_array[delay_pos-1] | LCD_DELAY );
				else
					Output.push_back( 1 | LCD_DELAY );
			}



		 }
		 break;

   }

   switch( type )           //Delay
   {
	  case 1:
	  case 3:
		 if ( delay_start < str_length && delay_start < slash_pos )//有DELAY
		 {
				l_bracket_pos = str.find( L'(', delay_start );
				r_bracket_pos = str.find( L')', delay_start );
			if ( l_bracket_pos < str_length && r_bracket_pos < str_length )
			{
			   s_value = str.substr( l_bracket_pos + 1, r_bracket_pos - l_bracket_pos - 1 );
			   BlankCropper( s_value );
					swscanf( s_value.c_str(), L"%i", &value_delay );
			   Output.push_back( value_delay | LCD_DELAY );
				}
         }
		 break;

	  case 2:

		 if ( delay_start < str_length && delay_start < slash_pos )//有DELAY
         {
				l_braces_pos = str.find( L'{' );
				r_braces_pos = str.find( L'}', delay_start );
			if ( l_braces_pos < delay_start && r_braces_pos < str_length )
            {
					s_value = str.substr( str.find( L',' ) + 1, str.rfind( L',' ) - str.find( L',' ) - 1 );
			   BlankCropper( s_value );
					swscanf( s_value.c_str(), L"%u", &value_delay );
               Output.push_back( value_delay | LCD_DELAY );
            }
         }
         break;
	  case 7:

		 if ( delay_start < str_length && delay_start < slash_pos )//有DELAY
         {
			r_braces_pos = str.find( L'm', delay_start );

			s_value = str.substr( str.find( L' ' ) + 1, r_braces_pos- str.find( L' ') );
			BlankCropper( s_value );
			swscanf( s_value.c_str(), L"%u", &value_delay );
			Output.push_back( value_delay | LCD_DELAY );
		 }
         break;
   }
}

void LCD_Initial_Parser::BlankCropper(wstring& str)
//去掉所有空格
{
	size_t blank_pos, blank_pos1;
	while( ( blank_pos = str.find(L"  ") ) < str.size() ){
		str.replace( blank_pos, 1, L"" );
	}
}

void LCD_Initial_Parser::BlankCropper(wstring& str, int pos )
//去掉所有空格
{
	size_t blank_pos = str.find(L" ");
	size_t blank_pos_b = str.size();

	while( blank_pos  < str.size() )
	{
		switch( pos )
		{
			case 0:    //head
			if(  blank_pos == 0 )
				str.replace( blank_pos, 1, L"" );
			else
				return;
			break;
			case 1:   //trail
			return;
			break;
			case 2:   //all
			str.replace( blank_pos, 1, L"" );
			return;
		}

		blank_pos = str.find(L" ");
	}
}

void LCD_Initial_Parser::BlankCropper(wstring& str, int pos, wstring ch_op )
//去掉所有空格
{
	size_t blank_pos = str.find(L" ");
	size_t blank_pos_b = str.size();

	while( blank_pos  < str.size() )
	{
		switch( pos )
		{
			case 0:    //head
			if(  blank_pos == 0 )
				str.replace( blank_pos, 1, L"" );
			else
				return;
			break;
			case 1:   //trail
				return;
			break;
			case 2:   //all
			str.replace( blank_pos, 1, L"" );
			return;
		}

		blank_pos = str.find(L" ");
	}

	blank_pos = str.find(ch_op);
	blank_pos_b = str.size();

	while( blank_pos  < str.size() )
	{
		switch( pos )
		{
			case 0:    //head
			if(  blank_pos == 0 )
				str.replace( blank_pos, 1, L"" );
			else
				return;
			break;
			case 1:   //trail
				return;
			break;
			case 2:   //all
			str.replace( blank_pos, 1, L"" );
			return;
		}

		blank_pos = str.find(ch_op);
	}
}
void LCD_Initial_Parser::RenesasFormatCropper(wstring& str)
//去掉所有空格、R、h
{
   size_t pos;
	while( ( pos = str.find(L' ') ) < str.size() )
		str.replace( pos, 1, L"" );
	while( ( pos = str.find(L'R') ) < str.size() )
		str.replace( pos, 1, L"" );
	while( ( pos = str.find(L'H') ) < str.size() )
		str.replace( pos, 1, L"" );
}

TStringList* LCD_Initial_Parser::DebugOut(void)
{
   StringsOut->Clear();
   //AnsiString S;
   for ( size_t i = 0; i < Output.size(); i++ )
   {
		if ( Output[i] & LCD_DELAY )//DELAY
      {
			StringsOut->Add( L"" );
			StringsOut->Add( L"DELAY: " + IntToStr( (int)Output[i]&0xFFFF ) );
		}
		else if ( Output[i] & LCD_CMD )//CMD
      {
			StringsOut->Add( L"" );
			if ( Output[i] & LCD_DCS )
				StringsOut->Add( L"COM:   " + IntToHex( (int)Output[i]&0xFFFF, 4 ) + L"  DCS" );
			else
				StringsOut->Add( L"COM:   " + IntToHex( (int)Output[i]&0xFFFF, 4 ) );
      }
      else// ( ((Output[i] >> 16) & 0x30000) == 0 )//DATA
         StringsOut->Add( L"DATA:  " + IntToHex( (int)Output[i]&0xFFFF, 4 ) );
   }

   return StringsOut;
}
//---------------------------------------------------------------------------
#endif
