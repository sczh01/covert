//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;

#define RENESAS_FORMAT_DEFAULT

//---------------------------------------------------------------------------
void TMainForm::AddCMDPacket(TStringList* psl, DWORD cmd, vector<DWORD>* param, CONVERT_SETTING* setting)
{
	static old_cnt;
	size_t count = param->size();

	if ( setting->bRenesasFormat )//setc
	{
		if ( cmd & LCD_DCS )//DCS
		{
			//0xBC
			S.sprintf(L"{setc, 0x000BC}\r\n{setp, 0x%05X}\r\n{setp, 0x%05X}\r\n",
				(count & 0xFF), ((count & 0xFF00)>>8));
			psl->Add(S);
			//0xBD
			psl->Add(L"{setc, 0x000BD}");
			psl->Add(L"{setp, 0x00000}");
			psl->Add(L"{setp, 0x00000}\r\n");
			//0xBE
			S.sprintf(L"{setc, 0x000BE}\r\n{setp, 0x%05X}\r\n{setp, 0x%05X}\r\n",
				(count & 0xFF), ((count & 0xFF00)>>8));
			psl->Add(S);

			S.sprintf(L"{setc, 0x%05X}", cmd & 0xFF);
			psl->Add(S);
			for ( size_t i = 0; i < count; i++ )
			{
				S.sprintf(L"{setp, 0x%05X}", (*param)[i] & 0xFF);
				psl->Add(S);
			}
			psl->Add(L"");
		}
		else
		{
			++count;
			//0xBC
			S.sprintf(L"{setc, 0x000BC}\r\n{setp, 0x%05X}\r\n{setp, 0x%05X}\r\n",
				(count & 0xFF), ((count & 0xFF00)>>8));
			psl->Add(S);
			//0xBD
			psl->Add(L"{setc, 0x000BD}");
			psl->Add(L"{setp, 0x00000}");
			psl->Add(L"{setp, 0x00000}\r\n");
			//0xBE
			S.sprintf(L"{setc, 0x000BE}\r\n{setp, 0x%05X}\r\n{setp, 0x%05X}\r\n",
				(count & 0xFF), ((count & 0xFF00)>>8));
			psl->Add(S);
			//0xBF
			psl->Add(L"{setc, 0x000BF}");
			S.sprintf(L"{setp, 0x%05X}", cmd & 0xFF);
			psl->Add(S);

			--count;
			for ( size_t i = 0; i < count; i++ )
			{
				S.sprintf(L"{setp, 0x%05X}", (*param)[i] & 0xFF);
				psl->Add(S);
			}
			psl->Add(L"");
		}
	}
	else//C Format
	{
		if ( (cmd & LCD_DCS) && (setting->mipi_always_gcs==0) )//DCS
		{
			//0xBC
			if ( setting->outIF_16bit == 0 )//8bit
			{
				if( setting->is_gcs == 1 )
				{
					S.sprintf(L"%s(0xB7);\r\n%s(0x60);\r\n%s(0x03);\r\n",
						setting->strCMD.c_str(), setting->strDATA.c_str(), setting->strDATA.c_str());
					psl->Add(S);
					setting->is_gcs = 0;
				}
				if( old_cnt != count ){
					S.sprintf(L"%s(0xBC);\r\n%s(0x%02X);\r\n%s(0x%02X);\r\n",
						setting->strCMD.c_str(), setting->strDATA.c_str(),
						(count & 0xFF), setting->strDATA.c_str(), ((count & 0xFF00)>>8));

					psl->Add(S);
				}
				old_cnt = count;
				S.sprintf(L"%s(0x%02X);", setting->strCMD.c_str(), cmd & 0xFF);
				psl->Add(S);
				for ( size_t i = 0; i < count; i++ )
				{
					S.sprintf(L"%s(0x%02X);", setting->strDATA.c_str(), (*param)[i] & 0xFF);
					psl->Add(S);
				}
				psl->Add(L"");
			}
			else//16bit
			{
				if( setting->is_gcs == 1 )
				{
					S.sprintf(L"%s(0xB7);\r\n%s(0x360);\r\n",
						setting->strCMD.c_str(), setting->strDATA.c_str());
					psl->Add(S);
					setting->is_gcs = 0;
				}
				if( old_cnt != count ){
					S.sprintf(L"%s(0xBC);\r\n%s(0x%02X);\r\n",
						setting->strCMD.c_str(), setting->strDATA.c_str(), count & 0xFFFF);

					psl->Add(S);
				}
				old_cnt = count;
				S.sprintf(L"%s(0x%02X);", setting->strCMD.c_str(), cmd & 0xFF);
				psl->Add(S);

				for ( size_t i = 0; i < count/2; i++ ){
					S.sprintf(L"%s(0x%02X%02X);", setting->strDATA.c_str(),(*param)[i*2+1] & 0xFF, (*param)[i*2] & 0xFF);
					psl->Add(S);
				}

				if( (count)%2 ){
					S.sprintf(L"%s(0x00%02X);", setting->strDATA.c_str(), (*param)[count-1] & 0xFF);
					psl->Add(S);
				}
				psl->Add(L"");
			}

		}//handle GCS
		else
		{
			++count;
			//0xBC
			if ( setting->outIF_16bit == 0 ){//8bit
				if( setting->is_gcs == 0 )
				{
					S.sprintf(L"%s(0xB7);\r\n%s(0x20);\r\n%s(0x03);\r\n",
						setting->strCMD.c_str(), setting->strDATA.c_str(), setting->strDATA.c_str());
					psl->Add(S);
					setting->is_gcs = 1;
				}

				if( old_cnt != count ){
					S.sprintf(L"%s(0xBC);\r\n%s(0x%02X);\r\n%s(0x%02X);\r\n",
						setting->strCMD.c_str(), setting->strDATA.c_str(),
						(count & 0xFF), setting->strDATA.c_str(), ((count & 0xFF00)>>8));
					psl->Add(S);
				}
			}
			else{//16bit
				if( setting->is_gcs == 0 ){
					S.sprintf(L"%s(0xB7);\r\n%s(0x320);\r\n",
						setting->strCMD.c_str(), setting->strDATA.c_str());
					psl->Add(S);
					setting->is_gcs = 1;
				}
				if( old_cnt != count ){
					S.sprintf(L"%s(0xBC);\r\n%s(0x%02X);\r\n",
						setting->strCMD.c_str(), setting->strDATA.c_str(), count & 0xFFFF);
					psl->Add(S);
				}
			}
			old_cnt = count;
			//0xBF
			psl->Add(setting->strCMD + L"(0xBF);");

			if ( setting->outIF_16bit == 0 ){//8bit
				S.sprintf(L"%s(0x%02X);", setting->strDATA.c_str(), cmd & 0xFF);
				psl->Add(S);
				--count;
				for ( size_t i = 0; i < count; i++ ){
					S.sprintf(L"%s(0x%02X);", setting->strDATA.c_str(), (*param)[i] & 0xFF);
					psl->Add(S);
				}
			}
			else{
				S.sprintf(L"%s(0x%02X%02X);", setting->strDATA.c_str(),(*param)[0] & 0xFF, cmd & 0xFF);
				psl->Add(S);
				count-=2;
				for ( size_t i = 0; i < count/2; i++ ){
					S.sprintf(L"%s(0x%02X%02X);", setting->strDATA.c_str(),(*param)[i*2+2] & 0xFF, (*param)[i*2+1] & 0xFF);
					psl->Add(S);
				}

				if( (count)%2 ){
					S.sprintf(L"%s(0x00%02X);", setting->strDATA.c_str(), (*param)[count] & 0xFF);
					psl->Add(S);
				}
			}
			psl->Add(L"");
		}
   }
}
//8bit 16bit convert
void TMainForm::AddCMDPacket(TStringList* psl, DWORD cmd, vector<DWORD>* param, CONVERT_SETTING* setting, DWORD type)
{
	size_t count = param->size();


	switch ( setting->bRenesasFormat ){ //output format by rdoOutputType
	case 0:  //setc: RSP tools format
		if ( type == 0 )//8bit
		{
			size_t i = 0;
			S.sprintf(L"{setc,0x00%02X};", cmd & 0xFF);
			psl->Add(S);
			for ( i = 0; i < count; i++ )
			{
				S.sprintf(L"{setp,0x00%02X};",(*param)[i] & 0xFF);
				psl->Add(S);
			}
		}
		else if( type == 1 )//16bit
		{
			size_t i = 0;
			S.sprintf(L"{setc,0x00%02X};", cmd & 0xFF);
			psl->Add(S);
			for ( i = 0; i < count; i++ )
			{
				S.sprintf(L"{setp,0x00%02X};",(*param)[i] & 0xFF);
				psl->Add(S);
				S.sprintf(L"{setp,0x00%02X};",((*param)[i]>>8) & 0xFF);
				psl->Add(S);
			}

			psl->Add(L"");
		}

		break;
	case 1: //C Format : DVI 2828 format
		if(type == 0) //in_if 8bit
			if ( setting->outIF_16bit == 1 )//16bit
			{
				size_t i = 0;
				S.sprintf(L"%s(0x%02X);", setting->strCMD.c_str(), cmd & 0xFF);
				psl->Add(S);
				for ( i = 0; i < count/2; i++ )
				{
					S.sprintf(L"%s(0x%02X%02X);", setting->strDATA.c_str(),(*param)[i*2+1] & 0xFF, (*param)[i*2] & 0xFF);
					psl->Add(S);
				}

				if( count%2 )
				{
					S.sprintf(L"%s(0x00%02X);", setting->strDATA.c_str(), (*param)[i*2] & 0xFF);
					psl->Add(S);
				}
				psl->Add(L"");
			}
			else       //out 8bit
			{
				size_t i = 0;
				S.sprintf(L"%s(0x%02X);", setting->strCMD.c_str(), cmd & 0xFF);
				psl->Add(S);
				for ( i = 0; i < count; i++ )
				{
					S.sprintf(L"%s(0x%02X);", setting->strDATA.c_str(),(*param)[i] & 0xFF);
					psl->Add(S);
				}

				psl->Add(L"");

			}
		else if(type == 1)  //16bit
			if ( setting->outIF_16bit == 1 )//16bit
			{
				size_t i = 0;
				S.sprintf(L"%s(0x%02X);", setting->strCMD.c_str(), cmd & 0xFF);
				psl->Add(S);
				for ( i = 0; i < count; i++ )
				{
					S.sprintf(L"%s(0x%04X);", setting->strDATA.c_str(),(*param)[i]&0xffff);
					psl->Add(S);
				}
			}
			else  //out 8bit
			{
				size_t i = 0;
				S.sprintf(L"%s(0x%02X);", setting->strCMD.c_str(), cmd & 0xFF);
				psl->Add(S);
				for ( i = 0; i < count; i++ )
				{
					S.sprintf(L"%s(0x%02X);", setting->strDATA.c_str(),(*param)[i] & 0xFF);
					psl->Add(S);
					S.sprintf(L"%s(0x%02X);", setting->strDATA.c_str(),((*param)[i]>>8) & 0xFF);
					psl->Add(S);
				}

				psl->Add(L"");

			}
		break;
	case 2:   //cmd array
		if ( type == 0 || (LeExFunc->Text.ToInt()& 0x40) ){//8bit
			if( 0==(LeExFunc->Text.ToInt()&0x3f) ){       //this is for SSD2828
				size_t i = 0, j=0;
				S = L"";
				S1= L"";
				if( (cmd & 0xFF) >= 0xB0 && (cmd & 0xFF) == 0xBF ){
					S.sprintf(L"%s%s0x%02X", setting->strCMD.c_str(), setting->strAHead.c_str(), (*param)[0] & 0xFF);
					S1 = S;
					j = 1;
				}
				else if( (cmd & 0xFF) < 0xB0 ){
					S.sprintf(L"%s%s0x%02X", setting->strCMD.c_str(), setting->strAHead.c_str(), cmd & 0xFF);
					S1 = S;
					j =0;
				}
				else
					break;

				for ( i = j; i < count; i++ ){
					S.sprintf(L"%s0x%02X", setting->strAGap.c_str(),(*param)[i] & 0xFF);
					S1+=S;
				}
				S.sprintf(L"%s;", setting->strATrail.c_str());
				S1+=S;
				psl->Add(S1);
				psl->Add(L"");

			//W_COM[0]=0x00032902; W_COM[1]=0x000CB4; W_DATA(W_COM,2,1);
			} else if( 1 ==(LeExFunc->Text.ToInt()&0x3f) ){       //this is not SSD2828, normall
				size_t i = 0, j=0;
				S = L"";
				S1= L"";
				if( (cmd & 0xFF) >= 0xB0 ){
					S.sprintf(L"%s%s0x%02X", setting->strCMD.c_str(), setting->strAHead.c_str(), cmd & 0xFF );
					S1 = S;
					j = 0;
				}
				else if( (cmd & 0xFF) < 0xB0 ){
					S.sprintf(L"%s%s0x%02X", setting->strCMD.c_str(), setting->strAHead.c_str(), cmd & 0xFF);
					S1 = S;
					j =0;
				}
				else
					break;

				for ( i = j; i < count; i++ ){
					S.sprintf(L"%s0x%02X", setting->strAGap.c_str(),(*param)[i] & 0xFF);
					S1+=S;
				}
				S.sprintf(L"%s", setting->strATrail.c_str());
				S1+=S;
				psl->Add(S1);
				//psl->Add(L"");
			}
			else if( 2 == (LeExFunc->Text.ToInt()&0x3f)){   //MTK format   WCOM[0]=xxx; WCOM[1]=yyy; WCOM[n]=nnn;  WDATA(WCOM,n+1,delay_time);
				size_t i = 0, j=0, k =0;

				if( (cmd & 0xFF) >= 0xB0 && (cmd & 0xFF) == 0xBF ){
					S.sprintf(L"%s[0]=0x00%02X2902;", setting->strCMD.c_str(), count);//(*param)[0] & 0xFF);
					psl->Add(S);

					j =0;
					k = count/4;
					S1 = L"";

					for ( i = j; i < k*4; i+=4 ){
						S.sprintf(L"%s[%d]=0x%02X%02X%02X%02X;", setting->strCMD.c_str(),i/4+1,(*param)[i+3] & 0xFF,(*param)[i+2] & 0xFF,(*param)[i+1] & 0xFF,(*param)[i] & 0xFF);

						psl->Add(S);
					}
					j = count%4 ;
					if( j ){
						S2.sprintf(L"%s[%d]=0x",setting->strCMD.c_str(),i/4+1);
						S=S2;
						for( k=1; k<=j; k++ ){
							S2.sprintf(L"%02X", (*param)[count-k] & 0xFF);
							S+=S2;
						}
						S2.sprintf(L";");
						S+=S2;
						psl->Add(S);
					}
					if( j )
						S.sprintf(L"%s(%s,%d,1);", setting->strDATA.c_str(),setting->strCMD.c_str(),i/4+2);
					else
						S.sprintf(L"%s(%s,%d,1);", setting->strDATA.c_str(),setting->strCMD.c_str(),i/4+1);

					S1+=S;
					psl->Add(S1);
					psl->Add(L"");
				}
				else if( (cmd & 0xFF) < 0xB0 ){
					if( ((cmd & 0xFF) == 0x11) || ((cmd & 0xFF) == 0x10)||((cmd & 0xFF) == 0x29)||((cmd & 0xFF) == 0x28) ){
						S.sprintf(L"%s[0]=0x00%02X0500;", setting->strCMD.c_str(), cmd & 0xFF);//(*param)[0] & 0xFF);

						psl->Add(S);
						psl->Add(L"");
						return;
					}
					else {
						S.sprintf(L"%s[0]=0x00%02X3902;", setting->strCMD.c_str(), count+1);//(*param)[0] & 0xFF);
						psl->Add(S);
						//psl->Add(L"");
					}

					if( count>3 ){
						S.sprintf(L"%s[1]=0x%02X%02X%02X%02X;", setting->strCMD.c_str(),(*param)[2] & 0xFF,(*param)[1] & 0xFF,(*param)[0] & 0xFF,cmd & 0xFF);
						psl->Add(S);
						j =3;
						k = (count-3)/4;
						S1 = L"";
						S = L"";
						for ( i = j; i < k*4; i+=4 ){
							S.sprintf(L"%s[%d]=0x%02X%02X%02X%02X);", setting->strCMD.c_str(),(i+1)/4+1,(*param)[i+3] & 0xFF,(*param)[i+2] & 0xFF,(*param)[i+1] & 0xFF,(*param)[i] & 0xFF);

							psl->Add(S);
							//psl->Add(L"");
						}
						j = (count+1)%4 ;

						if( j ){
							S2.sprintf(L"%s[%d]=0x",setting->strCMD.c_str(),(i+1)/4+1);
							S=S2;
							for( k=0; k<j; k++ ) {
								S2.sprintf(L"%02X", (*param)[count-k] & 0xFF);
								S+=S2;
							}
							S2.sprintf(L";");
							S+=S2;
							psl->Add(S);
						}
					}
					else{
						S2.sprintf(L"%s[1]=0x", setting->strCMD.c_str());
						S1 = L"";
						S = S2;
						for( i=count; i>0; i-- ){
							S2.sprintf(L"%02X", (*param)[count-1] & 0xFF);
							S+=S2;
						}
						S2.sprintf(L"%02X", cmd & 0xFF );
						S+=S2;
						S2.sprintf(L";");
						S+=S2;

						psl->Add(S);
						j = 0 ;
					}

					if( (count%4)!=3 )
						S.sprintf(L"%s(%s,%d,1);", setting->strDATA.c_str(),setting->strCMD.c_str(),(count+1)/4+2);
					else
						S.sprintf(L"%s(%s,%d,1);", setting->strDATA.c_str(),setting->strCMD.c_str(),(count+1)/4+1);

					S1+=S;
					psl->Add(S1);
					psl->Add(L"");
					S1 = L"";
				}
			}  //{0x00062902,0x001004B3,0x00000000,0x01}; for 2828 format
			else if( 3 == (LeExFunc->Text.ToInt()&0x3f)){   //MTK format 2:  {0x00062902,0x001004B3,0x00000000,0x01};
				size_t i = 0, j=0, k =0;
				setting->isShowDelay = 0;
				if( count > param_cnt_16bit )
					count = param_cnt_16bit;

				if( (cmd & 0xFF) >= 0xB0 && (cmd & 0xFF) == 0xBF ){
					S1.sprintf(L"{0x00%02X2902,", count);//(*param)[0] & 0xFF);
					j =0;
					k = count/4;

					for ( i = j; i < k*4; i+=4 ){
						S.sprintf(L"0x%02X%02X%02X%02X,",(*param)[i+3] & 0xFF,(*param)[i+2] & 0xFF,(*param)[i+1] & 0xFF,(*param)[i] & 0xFF);
						S1+=S;
					}
					j = count%4 ;
					S=L"";
					if( j ){
						S=L"0x";
						for( k=0;k<4-j;k++)
							S+=L"00";

						for( k=1; k<=j; k++ ) {
							S2.sprintf(L"%02X", (*param)[count-k] & 0xFF);
							S+=S2;
						}
						S+=L",";
					}
					S2.sprintf(L"0x%02X};", setting->iDelaytime);
					S+=S2;
					S1+=S;
					psl->Add(S1);
				}
				else if( (cmd & 0xFF) < 0xB0 ){
					S1 = L"";
					S = L"";
					if( ((cmd & 0xFF) == 0x11) || ((cmd & 0xFF) == 0x10)||((cmd & 0xFF) == 0x29)||((cmd & 0xFF) == 0x28) ){
						S.sprintf(L"{0x00%02X0500,", cmd & 0xFF);//(*param)[0] & 0xFF);
						S2.sprintf(L"0x%02X};", setting->iDelaytime);
						S+=S2;
						psl->Add(S);
						psl->Add(L"");
						return;
					}
					else {
						S1.sprintf(L"{0x00%02X3902,", count+1);//(*param)[0] & 0xFF);
					}

					if( count>3 ){
						S.sprintf(L"0x%02X%02X%02X%02X,", (*param)[2] & 0xFF,(*param)[1] & 0xFF,(*param)[0] & 0xFF,cmd & 0xFF);
						S1+=S;

						j =3;
						k = (count-3)/4;

						for ( i = j; i < k*4; i+=4 ){
							S.sprintf(L"0x%02X%02X%02X%02X,",(i+1)/4+1,(*param)[i+3] & 0xFF,(*param)[i+2] & 0xFF,(*param)[i+1] & 0xFF,(*param)[i] & 0xFF);
							S1+=S;
						}
						j = (count+1)%4 ;
						S=L"";
						if( j ){
							S=L"0x";
							for( k=0;k<4-j;k++)
								S+=L"00";
							for( k=0; k<j; k++ ) {
								S2.sprintf(L"%02X", (*param)[count-k] & 0xFF);
								S+=S2;
							}
							S+=L",";
						}

						S2.sprintf(L"0x%02X};", setting->iDelaytime);
						S+=S2;
						S1+=S;
						psl->Add(S1);
					}
					else{
						S=L"0x";

						for( k=0;k<3-count;k++)
							S+=L"00";

						for( i=count; i>0; i-- ){
							S2.sprintf(L"%02X", (*param)[count-1] & 0xFF);
							S+=S2;
						}

						S2.sprintf(L"%02X", cmd & 0xFF);
						S+=S2;

						S2.sprintf(L",0x%02X};", setting->iDelaytime);
						S+=S2;
						S1+=S;
						psl->Add(S1);
					}
				}else if( (cmd & 0xFF) == 0xBC )
							param_cnt_16bit = (*param)[0]& 0xFF;
			} //{0x00062902,0x001004B3,0x00000000,0x01}; for non-2828 format
			else if( 5 == (LeExFunc->Text.ToInt()&0x3f)){   //MTK format 2:  {0x00062902,0x001004B3,0x00000000,0x01};
				size_t i = 0, j=0, k =0;
				setting->isShowDelay = 0;
				if( (cmd & 0xFF) >= 0xB0 ){
					S1.sprintf(L"{0x00%02X2902,", count+1);//(*param)[0] & 0xFF);
					j =0;
					k = count/4;

					if( count>3 ){
						S.sprintf(L"0x%02X%02X%02X%02X,", (*param)[2] & 0xFF,(*param)[1] & 0xFF,(*param)[0] & 0xFF,cmd & 0xFF);
						S1+=S;

						j =3;
						k = (count-3)/4;

						for ( i = j; i < k*4; i+=4 ){
							S.sprintf(L"0x%02X%02X%02X%02X,",(*param)[i+3] & 0xFF,(*param)[i+2] & 0xFF,(*param)[i+1] & 0xFF,(*param)[i] & 0xFF);
							S1+=S;
						}
						j = (count+1)%4 ;
						S=L"";
						if( j ){
							S=L"0x";
							for( k=0;k<4-j;k++)
								S+=L"00";
							for( k=0; k<j; k++ ) {
								S2.sprintf(L"%02X", (*param)[count-k-1] & 0xFF);
								S+=S2;
							}
							S+=L",";
						}

						S2.sprintf(L"0x%02X};", setting->iDelaytime);
						S+=S2;
						S1+=S;
						psl->Add(S1);
					}
					else{
						S=L"0x";

						for( k=0;k<3-count;k++)
							S+=L"00";

						for( i=count; i>0; i-- ){
							S2.sprintf(L"%02X", (*param)[i-1] & 0xFF);
							S+=S2;
						}

						S2.sprintf(L"%02X", cmd & 0xFF);
						S+=S2;

						S2.sprintf(L",0x%02X};", setting->iDelaytime&0xFF);
						S+=S2;
						S1+=S;
						psl->Add(S1);
					}
				}
				else if( (cmd & 0xFF) < 0xB0 )
				{
					S1 = L"";
					S = L"";
					if( ((cmd & 0xFF) == 0x11) || ((cmd & 0xFF) == 0x10)||((cmd & 0xFF) == 0x29)||((cmd & 0xFF) == 0x28) ){
						S.sprintf(L"{0x00%02X0500,", cmd & 0xFF);//(*param)[0] & 0xFF);
						S2.sprintf(L"0x%02X};", setting->iDelaytime&0xFF);
						S+=S2;
						psl->Add(S);
						psl->Add(L"");
						return;
					}
					else {
						S1.sprintf(L"{0x00%02X3902,", count+1);//(*param)[0] & 0xFF);
					}

					if( count>3 ){
						S.sprintf(L"0x%02X%02X%02X%02X,", (*param)[2] & 0xFF,(*param)[1] & 0xFF,(*param)[0] & 0xFF,cmd & 0xFF);
						S1+=S;

						j =3;
						k = (count-3)/4;

						for ( i = j; i < k*4; i+=4 ){
							S.sprintf(L"0x%02X%02X%02X%02X,",(i+1)/4+1,(*param)[i+3] & 0xFF,(*param)[i+2] & 0xFF,(*param)[i+1] & 0xFF,(*param)[i] & 0xFF);
							S1+=S;
						}
						j = (count+1)%4 ;
						S=L"";
						if( j ){
							S=L"0x";
							for( k=0;k<4-j;k++)
								S+=L"00";
							for( k=0; k<j; k++ ) {
								S2.sprintf(L"%02X", (*param)[count-k] & 0xFF);
								S+=S2;
							}
							S+=L",";
						}

						S2.sprintf(L"0x%02X};", setting->iDelaytime&0xFF);
						S+=S2;
						S1+=S;
						psl->Add(S1);
					}
					else{
						S=L"0x";

						for( k=0;k<3-count;k++)
							S+=L"00";

						for( i=count; i>0; i-- ){
							S2.sprintf(L"%02X", (*param)[count-1] & 0xFF);
							S+=S2;
						}

						S2.sprintf(L"%02X", cmd & 0xFF);
						S+=S2;

						S2.sprintf(L",0x%02X};", setting->iDelaytime&0xFF);
						S+=S2;
						S1+=S;
						psl->Add(S1);
					}
				}else if( (cmd & 0xFF) == 0xBC )
							param_cnt_16bit = (*param)[0]& 0xFF;
			}
			else if( 6 == (LeExFunc->Text.ToInt()&0x3f)){   //MTK format 2:  {0x00062902,0x001004B3,0x00000000,0x01};
				size_t i = 0, j=0, k =0;
				setting->isShowDelay = 0;
				if( (cmd & 0xFF) >= 0xB0 ){
					if( count>1 )
					S1.sprintf(L"{0x00%02X2902,", count+1);//(cmd & 0xFF0000)>>16);//(*param)[0] & 0xFF);
					else
					S1.sprintf(L"{0x00%02X2302,", count+1);//(cmd & 0xFF0000)>>16);//(*param)[0] & 0xFF);
					j =0;
					k = count/4;

					if( count>3 ){
						S.sprintf(L"0x%02X%02X%02X%02X,", (*param)[2] & 0xFF,(*param)[1] & 0xFF,(*param)[0] & 0xFF,cmd & 0xFF);
						S1+=S;

						j =3;
						k = (count-3)/4;

						for ( i = j; i < k*4; i+=4 ){
							S.sprintf(L"0x%02X%02X%02X%02X,",(*param)[i+3] & 0xFF,(*param)[i+2] & 0xFF,(*param)[i+1] & 0xFF,(*param)[i] & 0xFF);
							S1+=S;
						}
						j = (count+1)%4 ;
						S=L"";
						if( j ){
							S=L"0x";
							for( k=0;k<4-j;k++)
								S+=L"00";
							for( k=0; k<j; k++ ) {
								S2.sprintf(L"%02X", (*param)[count-k-1] & 0xFF);
								S+=S2;
							}
							S+=L",";
						}

						S2.sprintf(L"0x%02X};", setting->iDelaytime);
						S+=S2;
						S1+=S;
						psl->Add(S1);
					}
					else{
						S=L"0x";

						for( k=0;k<3-count;k++)
							S+=L"00";

						for( i=count; i>0; i-- ){
							S2.sprintf(L"%02X", (*param)[i-1] & 0xFF);
							S+=S2;
						}

						S2.sprintf(L"%02X", cmd & 0xFF);
						S+=S2;

						S2.sprintf(L",0x%02X};", setting->iDelaytime&0xFF);
						S+=S2;
						S1+=S;
						psl->Add(S1);
					}
				}
				else if( (cmd & 0xFF) < 0xB0 )
				{
					S1 = L"";
					S = L"";

					if( count>3 ){
						S1.sprintf(L"{0x00%02X3902,", count+1);//(cmd & 0xFF0000)>>16);//(*param)[0] & 0xFF);
						S.sprintf(L"0x%02X%02X%02X%02X,", (*param)[2] & 0xFF,(*param)[1] & 0xFF,(*param)[0] & 0xFF,cmd & 0xFF);
						S1+=S;

						j =3;
						k = (count-3)/4;

						for ( i = j; i < k*4; i+=4 ){
							S.sprintf(L"0x%02X%02X%02X%02X,",(i+1)/4+1,(*param)[i+3] & 0xFF,(*param)[i+2] & 0xFF,(*param)[i+1] & 0xFF,(*param)[i] & 0xFF);
							S1+=S;
						}
						j = (count+1)%4 ;
						S=L"";
						if( j ){
							S=L"0x";
							for( k=0;k<4-j;k++)
								S+=L"00";
							for( k=0; k<j; k++ ) {
								S2.sprintf(L"%02X", (*param)[count-k] & 0xFF);
								S+=S2;
							}
							S+=L",";
						}

						S2.sprintf(L"0x%02X};", setting->iDelaytime&0xFF);
						S+=S2;
						S1+=S;
						psl->Add(S1);
					}
					else if( count>1 && count<4){
						S1.sprintf(L"{0x00%02X%02X02,", count+1,(cmd & 0xFF0000)>>16);//(*param)[0] & 0xFF);
						S=L"0x";

						for( k=0;k<3-count;k++)
							S+=L"00";

						for( i=count; i>0; i-- ){
							S2.sprintf(L"%02X", (*param)[count-1] & 0xFF);
							S+=S2;
						}

						S2.sprintf(L"%02X", cmd & 0xFF);
						S+=S2;

						S2.sprintf(L",0x%02X};", setting->iDelaytime&0xFF);
						S+=S2;
						S1+=S;
						psl->Add(S1);
					}
					else if(count==1) {
						S.sprintf(L"{0x%02X%02X1500,", (*param)[0] & 0xFF, cmd & 0xFF);//(cmd & 0xFF0000)>>16);//(*param)[0] & 0xFF);
						S2.sprintf(L"0x%02X};", setting->iDelaytime&0xFF);
						S+=S2;
						psl->Add(S);
						psl->Add(L"");
						return;
					}
					else if(count==0) {
						S.sprintf(L"{0x00%02X0500,", cmd & 0xFF);//(cmd & 0xFF0000)>>16);//(*param)[0] & 0xFF);
						S2.sprintf(L"0x%02X};", setting->iDelaytime&0xFF);
						S+=S2;
						psl->Add(S);
						psl->Add(L"");
						return;
					}
				}else if( (cmd & 0xFF) == 0xBC )
							param_cnt_16bit = (*param)[0]& 0xFF;
			}
			else if( 7 == (LeExFunc->Text.ToInt()&0x3f)){   //MTK format 2:  {0x00062902,0x001004B3,0x00000000,0x01};
				size_t i = 0, j=0, k =0;
				setting->isShowDelay = 0;
				if( ((cmd & 0xFF) != 0x11) && ((cmd & 0xFF) != 0x29 ) && ((cmd & 0xFF) != 0x28 ) &&((cmd & 0xFF) != 0x10 )\
				&&((cmd & 0xFF) != 0x35)&&((cmd & 0xFF) != 0x36)&&((cmd & 0xFF) != 0x51) &&((cmd & 0xFF) != 0x53)&&((cmd & 0xFF) != 0x55)){
					S1.sprintf(L"{0x00%02X2902,", count+1);
					j =0;
					k = count/4;

					if( count>3 ){
						S.sprintf(L"0x%02X%02X%02X%02X,", (*param)[2] & 0xFF,(*param)[1] & 0xFF,(*param)[0] & 0xFF,cmd & 0xFF);
						S1+=S;

						j =3;
						k = (count-3)/4;

						for ( i = j; i < k*4; i+=4 ){
							S.sprintf(L"0x%02X%02X%02X%02X,",(*param)[i+3] & 0xFF,(*param)[i+2] & 0xFF,(*param)[i+1] & 0xFF,(*param)[i] & 0xFF);
							S1+=S;
						}
						j = (count+1)%4 ;
						S=L"";
						if( j ){
							S=L"0x";
							for( k=0;k<4-j;k++)
								S+=L"00";
							for( k=0; k<j; k++ ) {
								S2.sprintf(L"%02X", (*param)[count-k-1] & 0xFF);
								S+=S2;
							}
							S+=L",";
						}

						S2.sprintf(L"0x%02X};", setting->iDelaytime);
						S+=S2;
						S1+=S;
						psl->Add(S1);
					}
					else{
						S=L"0x";

						for( k=0;k<3-count;k++)
							S+=L"00";

						for( i=count; i>0; i-- ){
							S2.sprintf(L"%02X", (*param)[i-1] & 0xFF);
							S+=S2;
						}

						S2.sprintf(L"%02X", cmd & 0xFF);
						S+=S2;

						S2.sprintf(L",0x%02X};", setting->iDelaytime&0xFF);
						S+=S2;
						S1+=S;
						psl->Add(S1);
					}
				}
				else if( ((cmd & 0xFF) == 0x11) || ((cmd & 0xFF) == 0x29 ) || ((cmd & 0xFF) == 0x28 ) ||((cmd & 0xFF) == 0x10 ) )
				{
					S1 = L"";
					S = L"";
					S.sprintf(L"{0x00%02X0500,", cmd & 0xFF);//(cmd & 0xFF0000)>>16);//(*param)[0] & 0xFF);
					S2.sprintf(L"0x%02X};", setting->iDelaytime&0xFF);
					S+=S2;
					psl->Add(S);
					psl->Add(L"");
					return;
				}
				else if( ((cmd & 0xFF) == 0x36) || ((cmd & 0xFF) == 0x35 ) || ((cmd & 0xFF) == 0x51 ) ||((cmd & 0xFF) == 0x53 ) ||((cmd & 0xFF) == 0x55 ) )
				{
					S1 = L"";
					S = L"";
					S.sprintf(L"{0x%02X%02X1500,", (*param)[0] & 0xFF, cmd & 0xFF);//(cmd & 0xFF0000)>>16);//(*param)[0] & 0xFF);
					S2.sprintf(L"0x%02X};", setting->iDelaytime&0xFF);
					S+=S2;
					psl->Add(S);
					psl->Add(L"");
					return;
				}else if( (cmd & 0xFF) == 0xBC )
					param_cnt_16bit = (*param)[0]& 0xFF;
			}
		} //8bit end
		else //16bit
		{
			if( 3 == (LeExFunc->Text.ToInt()&0x3f)){   //MTK format 2:  {0x00062902,0x001004B3,0x00000000,0x01};
				size_t i = 0, j=0, k =0;

				if( (cmd & 0xFF) >= 0xB0 && (cmd & 0xFF) == 0xBF ){
					S1.sprintf(L"{0x00%02X2902,", count);//(*param)[0] & 0xFF);
					j =0;
					k = count/4;

					for ( i = j; i < k*4; i+=4 ){
						S.sprintf(L"0x%02X%02X%02X%02X,",i/4+1,(*param)[i+3] & 0xFF,(*param)[i+2] & 0xFF,(*param)[i+1] & 0xFF,(*param)[i] & 0xFF);
						S1+=S;
					}
					j = count%4 ;
					S=L"";
					if( j ){
						S=L"0x";
						for( k=0;k<4-j;k++)
							S+=L"00";

						for( k=1; k<=j; k++ ) {
							S2.sprintf(L"%02X", (*param)[count-k] & 0xFF);
							S+=S2;
						}
						S+=L",";
					}
					S+=L"0x01};";
					S1+=S;
					psl->Add(S1);
				}
				else if( (cmd & 0xFF) < 0xB0 ){
					S1 = L"";
					S = L"";
					if( ((cmd & 0xFF) == 0x11) || ((cmd & 0xFF) == 0x10)||((cmd & 0xFF) == 0x29)||((cmd & 0xFF) == 0x28) ){
						S.sprintf(L"{0x00%02X0500,0x01}", cmd & 0xFF);//(*param)[0] & 0xFF);
						psl->Add(S);
						psl->Add(L"");
						return;
					}
					else {
						S1.sprintf(L"{0x00%02X3902,", count+1);//(*param)[0] & 0xFF);
					}

					if( count>3 ){
						S.sprintf(L"0x%02X%02X%02X%02X,", (*param)[2] & 0xFF,(*param)[1] & 0xFF,(*param)[0] & 0xFF,cmd & 0xFF);
						S1+=S;

						j =3;
						k = (count-3)/4;

						for ( i = j; i < k*4; i+=4 ){
							S.sprintf(L"0x%02X%02X%02X%02X,",(i+1)/4+1,(*param)[i+3] & 0xFF,(*param)[i+2] & 0xFF,(*param)[i+1] & 0xFF,(*param)[i] & 0xFF);
							S1+=S;
						}
						j = (count+1)%4 ;
						S=L"";
						if( j ){
							S=L"0x";
							for( k=0;k<4-j;k++)
								S+=L"00";
							for( k=0; k<j; k++ ) {
								S2.sprintf(L"%02X", (*param)[count-k] & 0xFF);
								S+=S2;
							}
							S+=L",";
						}
						S+=L"0x1};";
						S1+=S;
						psl->Add(S1);
					}
					else{
						S=L"0x";

						for( k=0;k<3-count;k++)
							S+=L"00";

						for( i=count; i>0; i-- ){
							S2.sprintf(L"%02X", (*param)[count-1] & 0xFF);
							S+=S2;
						}

						S2.sprintf(L"%02X", cmd & 0xFF);
						S+=S2;

						S+=L",0x01};";
						S1+=S;
						psl->Add(S1);
					}
				}
			}
			else{

				size_t i = 0, j=0;
				if( (cmd & 0xFF) >= 0xB0 && (cmd & 0xFF) == 0xBF ){
					S.sprintf(L"%s%s0x%02X", setting->strCMD.c_str(), setting->strAHead.c_str(), (*param)[0] & 0xFF);
					S1 = S;

					S.sprintf(L"%s0x%02X", setting->strAGap.c_str(), ((*param)[0]>>8) & 0xFF);
					S1 += S;
					j = 1;
				}
				else if( (cmd & 0xFF) < 0xB0 ){
					S.sprintf(L"%s%s0x%02X", setting->strCMD.c_str(), setting->strAHead.c_str(), cmd & 0xFF);
					S1 = S;
					j =0;
				}
				else if( (cmd & 0xFF) == 0xBC ){
					param_cnt = (*param)[0];
					break;
				}

				for ( i = j; i < count; i++ ){
					S.sprintf(L"%s0x%02X", setting->strAGap.c_str(),(*param)[i] & 0xFF);
					S1+=S;

					if(  i < (count -1)  )
					{
						S.sprintf(L"%s0x%02X", setting->strAGap.c_str(),((*param)[i]>>8) & 0xFF);
						S1+=S;
					}
					else if( (param_cnt&0x1) == 0 )
					{
						S.sprintf(L"%s0x%02X", setting->strAGap.c_str(),((*param)[i]>>8) & 0xFF);
						S1+=S;
					}
				}
				S.sprintf(L"%s;", setting->strATrail.c_str());
				S1+=S;
				psl->Add(S1);
				psl->Add(L"");
			}
		}
		break;
	case 3://customer requst format: Many type.
	// Ex 1: {0x23, 0, 0, 0, 0, 2, {0xB0, 0x00}}. W/O Bridge IC, Ex:input data is SSD2828.
	// Ex 2: {0xB0,2 {0x00, 0x01}}
	// Ex 3: {0x23, 2, {0xB0, 0x00}} W/O Bridge IC, Ex:input data is SSD2828.

		if ( type == 0 )//8bit
		{
			size_t i = 0, j=0;
			if( 0x04==(LeExFunc->Text.ToInt()&0x0f) ) {  //for SSD2828 command
				if( (cmd & 0xFF) >= 0xB0 && (cmd & 0xFF) == 0xBF ){
					S.sprintf(L"{0x29,0,0,0,%d,%d,{0x%02X",setting->iDelaytime&0xFF, (count)& 0xFF, (*param)[0] & 0xFF);
					S1 = S;
					j = 1;
				}
				else if( (cmd & 0xFF) == 0x11 || (cmd & 0xFF) == 0x10 || (cmd & 0xFF) == 0x28|| (cmd & 0xFF) == 0x29 ){
					S.sprintf(L"{0x05,0,0,0,%d,%d,{0x%02X",setting->iDelaytime&0xFF, (count+1)& 0xFF, cmd & 0xFF);
					S1 = S;
					j =0;
				}
				else if( (cmd & 0xFF) < 0xB0 ){
					S.sprintf(L"{0x39,0,0,0,%d,%d,{0x%02X",setting->iDelaytime&0xFF, (count+1)& 0xFF, cmd & 0xFF);
					S1 = S;
					j =0;
				}else  break;
				for ( i = j; i < count; i++ ){
					S.sprintf(L"%s0x%02X", setting->strAGapOut.c_str(),(*param)[i] & 0xFF);
					S1+=S;
				}
				S.sprintf(L"}},");
				S1+=S;
				psl->Add(S1);
				setting->isShowDelay = 1;
			}if( 0x05==(LeExFunc->Text.ToInt()&0x0f) ) {  //linux DTSI format: 29 00 00 00 delay 00 cnt cmd param1 param2 ...
				if( (cmd & 0xFF) >= 0xB0 ){
					S.sprintf(L"29 01 00 00 %02X 00 %02X %02X",setting->iDelaytime&0xFF, (count+1)& 0xFF, cmd & 0xFF);
					S1 = S;
					j = 0;
				}
				else if( (cmd & 0xFF) == 0x11 || (cmd & 0xFF) == 0x10 || (cmd & 0xFF) == 0x28|| (cmd & 0xFF) == 0x29 ){
					S.sprintf(L"05 01 00 00 %02X 00 %02X %02X",setting->iDelaytime&0xFF, (count+1)& 0xFF, cmd & 0xFF);
					S1 = S;
					j =0;
					count = 0;
				}
				else if( (cmd & 0xFF) < 0xB0 ){
					S.sprintf(L"39 01 00 00 %02X 00 %02X %02X",setting->iDelaytime&0xFF, (count+1)& 0xFF, cmd & 0xFF);
					S1 = S;
					j =0;
				}else  break;

				for ( i = j; i < count; i++ ){
					S.sprintf(L"%s%02X", setting->strAGapOut.c_str(), (*param)[i] & 0xFF);
					S1+=S;
				}
				psl->Add(S1);
				setting->isShowDelay = 0;
			}else if(0x40==(LeExFunc->Text.ToInt()&0xc0)){ //for {0x23, 2, {0xB0, 0x00}}
				if( (cmd & 0xFF) >= 0xB0 /*&& (cmd & 0xFF) == 0xBF*/ ){
					S.sprintf(L"{0x29,%02d,{0x%02X", (count+1)& 0xFF, cmd & 0xFF);//(*param)[0] & 0xFF);
					S1 = S;
					j = 0;
				}
				else if( (cmd & 0xFF) == 0x11 || (cmd & 0xFF) == 0x10 || (cmd & 0xFF) == 0x28|| (cmd & 0xFF) == 0x29 ){
					S.sprintf(L"{0x05,%02d,{0x%02X", (count+1)& 0xFF, cmd & 0xFF);
					S1 = S;
					j =0;
				}
				else if( (cmd & 0xFF) < 0xB0 ){
					S.sprintf(L"{0x39,%02d,{0x%02X", (count+1)& 0xFF, cmd & 0xFF);
					S1 = S;
					j =0;
				}
				else   break;

				for ( i = j; i < count; i++ ){
					S.sprintf(L"%s0x%02X", setting->strAGapOut.c_str(),(*param)[i] & 0xFF);
					S1+=S;
				}
				S.sprintf(L"}},");
				S1+=S;
				psl->Add(S1);
				setting->isShowDelay = 1;
			}else if(0x32==(LeExFunc->Text.ToInt()&0xff)){ //Note: Other = 0x42(66) //for {0xB0,2 {0x00, 0x01}}
				S1 = S.sprintf(L"{0x%02X,%d,{", cmd & 0xFF, (count)& 0xFF);
				S1 += S.sprintf(L"0x%02X", (*param)[i] & 0xFF);

				for ( i = 1; i < count; i++ ){
					S.sprintf(L"%s0x%02X", setting->strAGapOut.c_str(),(*param)[i] & 0xFF);
					S1+=S;
				}
				S.sprintf(L"}},");
				S1+=S;
				psl->Add(S1);
				setting->isShowDelay = 1;
			}
			else if(0x02==(LeExFunc->Text.ToInt()&0x0f)) {
				if( (cmd & 0xFF) >= 0xB0 /*&& (cmd & 0xFF) == 0xBF*/ ){
					S.sprintf(L"{0x29,0,0,0,%d,%d,{0x%02X",setting->iDelaytime&0xFF, (count+1)& 0xFF, cmd & 0xFF);//(*param)[0] & 0xFF);
					S1 = S;
					j = 0;
				}
				else if( (cmd & 0xFF) == 0x11 || (cmd & 0xFF) == 0x10 || (cmd & 0xFF) == 0x28|| (cmd & 0xFF) == 0x29 ){
					S.sprintf(L"{0x05,0,0,0,%d,%d,{0x%02X",setting->iDelaytime&0xFF, (count+1)& 0xFF, cmd & 0xFF);
					S1 = S;
					j =0;
				}
				else if( (cmd & 0xFF) < 0xB0 ){
					S.sprintf(L"{0x39,0,0,0,%d,%d,{0x%02X",setting->iDelaytime&0xFF, (count+1)& 0xFF, cmd & 0xFF);
					S1 = S;
					j =0;
				}
				else   break;

				for ( i = j; i < count; i++ )
				{
					S.sprintf(L"%s0x%02X", setting->strAGapOut.c_str(),(*param)[i] & 0xFF);
					S1+=S;
				}
				S.sprintf(L"}},");
				S1+=S;
				psl->Add(S1);
				setting->isShowDelay = 0;
			}
			else if(0x09==(LeExFunc->Text.ToInt()&0x0f)) {    //E7422 format
				if( (cmd & 0xFF) >= 0xB0 /*&& (cmd & 0xFF) == 0xBF*/ ){
					S.sprintf(L"%s 0x29 0x%02X",setting->strCMD.c_str(), cmd & 0xFF);
					S1 = S;
					j = 0;
				}
				else if( (cmd & 0xFF) == 0x11 || (cmd & 0xFF) == 0x10 || (cmd & 0xFF) == 0x28|| (cmd & 0xFF) == 0x29 ){
					S.sprintf(L"%s 0x05 0x%02X",setting->strCMD.c_str(), cmd & 0xFF);
					S1 = S;
					j =0;
				}
				else if( (cmd & 0xFF) < 0xB0 ){
					S.sprintf(L"%s 0x39 0x%02X",setting->strCMD.c_str(), cmd & 0xFF);
					S1 = S;
					j =0;
				}
				else   break;

				for ( i = j; i < count; i++ )
				{
					S.sprintf(L"%s0x%02X", setting->strAGapOut.c_str(),(*param)[i] & 0xFF);
					S1+=S;
				}

				psl->Add(S1);
				setting->isShowDelay = 1;
			}
		}
		else //16bit
		{
			size_t i = 0, j=0;
			if( 0x04==(LeExFunc->Text.ToInt()&0x0f) ) {  //for SSD2828 command
				if( ((cmd & 0xFF) >= 0xB0) && ((cmd & 0xFF) == 0xBF) ){
					S.sprintf(L"{0x29,0,0,0,%04d,%02d,{0x%02X",setting->iDelaytime&0xFF, param_cnt& 0xFF, (*param)[0] & 0xFF);
					S1 = S;

					S.sprintf(L"%s0x%02X", setting->strAGap.c_str(), ((*param)[0]>>8) & 0xFF);
					S1 += S;
					j = 1;
				}
				else if( (cmd & 0xFF) == 0x11 || (cmd & 0xFF) == 0x10 || (cmd & 0xFF) == 0x28|| (cmd & 0xFF) == 0x29 ){
					S.sprintf(L"{0x05,0,0,0,%04d,%02d,{0x%02X", setting->iDelaytime&0xFF,param_cnt& 0xFF, cmd & 0xFF);
					S1 = S;
					j =0;
				}
				else if( (cmd & 0xFF) < 0xB0 ){
					S.sprintf(L"{0x39,0,0,0,%04d,%02d,{0x%02X", setting->iDelaytime&0xFF,(param_cnt+1)& 0xFF, cmd & 0xFF);
					S1 = S;
					j =0;
				}
				else if( (cmd & 0xFF) == 0xBC ){
					param_cnt = (*param)[0];
					break;
				}
				else  break;
			}else
			if( ((cmd & 0xFF) >= 0xB0) /*&& ((cmd & 0xFF) == 0xBF)*/ ){
				S.sprintf(L"{0x29,0,0,0,%04d,%02d,{0x%02X", setting->iDelaytime&0xFF,param_cnt& 0xFF, (*param)[0] & 0xFF);
				S1 = S;

				S.sprintf(L"%s0x%02X", setting->strAGap.c_str(), ((*param)[0]>>8) & 0xFF);
				S1 += S;
				j = 1;
			}
			else if( (cmd & 0xFF) < 0xB0 ){
				S.sprintf(L"{0x39,0,0,0,%04d,%02d,{0x%02X", setting->iDelaytime&0xFF,param_cnt& 0xFF, cmd & 0xFF);
				S1 = S;
				j =0;
			}
			else if( (cmd & 0xFF) == 0xBC ){
				param_cnt = (*param)[0];
				break;
			}
			else 	break;

			//psl->Add(S);
			for ( i = j; i < count; i++ )
			{
				S.sprintf(L"%s0x%02X", setting->strAGap.c_str(),(*param)[i] & 0xFF);
				S1+=S;

				if(  i < (count -1)  )
				{
					S.sprintf(L"%s0x%02X", setting->strAGap.c_str(),((*param)[i]>>8) & 0xFF);
					S1+=S;
				}
				else if( (param_cnt&0x1) == 0 )
				{
 					S.sprintf(L"%s0x%02X", setting->strAGap.c_str(),((*param)[i]>>8) & 0xFF);
					S1+=S;
				}
			}
			S.sprintf(L"}};");
			S1+=S;
			psl->Add(S1);
//			psl->Add(L"");
		}

		break;
	}
}
void __fastcall TMainForm::LoadParser()
{
	parser.S_COM = LeCMD1->Text.UpperCase().c_str();
	parser.S_DATA = LeDATA1->Text.UpperCase().c_str();
	parser.S_DELAY = LeDELAY1->Text.UpperCase().c_str();
	parser.S_AHead = LeArrayHead->Text.UpperCase().c_str();
	parser.S_ATrail = LeArrayTrail->Text.UpperCase().c_str();
	parser.S_AGap = LeArrayGap->Text.UpperCase().c_str();
	parser.S_ACmdPos = LeCmdPos->Text.UpperCase().c_str();
	parser.S_ADataPos = LeDataPos->Text.UpperCase().c_str();
	parser.S_ADelayPos = LeDelayPos->Text.UpperCase().c_str();
	parser.S_Annotate = LeAnnotate->Text.UpperCase().c_str();
	parser.S_ADataCntPos = LeDataCntPos->Text.UpperCase().c_str();
	parser.S_ADTPos = LeDataTypePos->Text.UpperCase().c_str();
	parser.S_ExFunc = LeExFunc->Text.UpperCase().c_str();

	TStringList *pList = new TStringList;
	parser.in_data_type = if_in->ItemIndex;
	parser.out_data_type = if_out->ItemIndex;
	parser.ex_func = LeExFunc->Text.ToInt();

#if 1
	TIniFile* iniFile = new TIniFile( ChangeFileExt(Application->ExeName, L".ini") );
	S = iniFile->ReadString( L"CTRL", L"Param",L"");

	pList->Delimiter = ',';
	pList->DelimitedText = S;
	pList->Count;
	if( pList->Count )
		for( int i=0; i<64; i++){
			ex_param[i]= StrToInt(pList->Strings[i]);
	}
#endif
	memcpy( parser.ex_param,ex_param,64);

	parser.LoadInitial( Memo1->Lines, rdoInputType->ItemIndex + 1 );
	Memo2->Clear();
	setting.bRenesasFormat = rdoOutputType->ItemIndex;
	setting.def_is_gcs = 1;//gcs
	setting.is_gcs = 1;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DoCIE(TStringList* psl)
{
	unsigned int j,l,m,n;
	m = 255/41;
	n = 0;
	for( j=0; j<41; j++ )      //blue
	{
		l = j*m;
		n = ((255-l)<<16)|(l << 8);
		S.sprintf( L"0x%06X", n );
		psl->Add(S);
	}

	for( j=0; j<40; j++ )     //green
	{
		l = j*m;
		n = ((255-l)<<8)|l;
		S.sprintf( L"0x%06X", n );
		psl->Add(S);
	}


	for( j=0; j<40; j++ )
	{
		l = j*m;
		n = (l<<16)|(255-l);
		S.sprintf( L"0x%06X", n );
		psl->Add(S);
	}
	S.sprintf( L"0x%06X", 0xff0000 ); //red
	psl->Add(S);
}

void __fastcall TMainForm::DoLineGenerate(TStringList* psl)
{
		float x1,y1,x2,y2,xn,yn,stepx,stepy;
		int step,j;

		x1 = StrToFloatDef(Edit1->Text.UpperCase().c_str(),6);
		y1 = StrToFloatDef(Edit2->Text.UpperCase().c_str(),6);
		x2 = StrToFloatDef(Edit3->Text.UpperCase().c_str(),6);
		y2 = StrToFloatDef(Edit4->Text.UpperCase().c_str(),6);
		 // xn = x1+ n*(x2-x1) /step
		 // yn = y1+ n*(y2-y1) /step

		step  = StrToIntDef(Edit5->Text.UpperCase().c_str(),0);
		stepx = (x2-x1)/step;
		stepy = (y2-y1)/step;
		for( j = 1; j<step; j ++ )
		{
			xn = x1+ j*stepx;
			yn = y1+ j*stepy;

			S.sprintf( L"%-06.4f,%-06.4f", xn,yn );
			psl->Add(S);
		}
}
void __fastcall TMainForm::BtnConvert1Click(TObject *Sender)
{
	unsigned int i,j,l,m,n,k,cmd;

	LoadParser();
	TStringList* psl = new TStringList();
	vector<DWORD> param;
	param.reserve(32);

	if(   if_type->ItemIndex == 0     )   //convert Type: group select
	{
		switch( rdoOutputType->ItemIndex )     //指令类型(output): group select
		{
			case 0: //C type for output
			for ( i = 0; i < parser.Output.size(); i++ )
			{
				if ( parser.Output[i] & LCD_DELAY )//DELAY
				{
					S.sprintf( L"%s(%d);\r\n", LeDELAY2->Text.c_str(), parser.Output[i]&0xFFFF );//Delayms(20)
					psl->Add(S);
				}
				else if ( parser.Output[i] & LCD_CMD )//CMD
				{
					cmd = parser.Output[i];
					param.clear();

					if( 0x43== (LeExFunc->Text.ToInt()&0xff) )
						i++;

					for ( k = i+1; k < parser.Output.size(); k++ )
					{
						if ( (parser.Output[k] & LCD_DATA) )//DATA
							param.push_back(parser.Output[k]);
						else
							break;
					}

					setting.strCMD = LeCMD2->Text;
					setting.strDATA = LeDATA2->Text;
					setting.outIF_16bit = if_out->ItemIndex;
					setting.mipi_gcs_dcs_change = MIPI_Data_Type->Checked;
					setting.mipi_always_gcs = AllGCS->Checked;

					AddCMDPacket(psl, cmd, &param, &setting);
				}
			}
			break;
			case 1: //RSP format output
			for ( i = 0; i < parser.Output.size(); i++ ){
				if ( parser.Output[i] & LCD_DELAY ){//DELAY
					S.sprintf( L"{time, %04u, ms}\r\n", parser.Output[i]&0xFFFF );//{time, 0025,ms}
					psl->Add(S);
				}
				else if ( parser.Output[i] & LCD_CMD ){//CMD
					cmd = parser.Output[i];
					param.clear();
					for ( k = i+1; k < parser.Output.size(); k++ ){
						if ( (parser.Output[k] & LCD_DATA) )//DATA
							param.push_back(parser.Output[k]);
						else
							break;
					}
					setting.outIF_16bit = if_out->ItemIndex;    //8 / 16bit
					AddCMDPacket(psl, cmd, &param, &setting);
				}
			}
			break;

			case 2:    //reg_set(cmd,p0,p1,...,pn)
			for ( i = 0; i < parser.Output.size(); i++ )
			{
				if ( parser.Output[i] & LCD_DELAY )//DELAY
				{
					if( setting.isShowDelay ){
						S.sprintf( L"%s(%d);\r\n", LeDELAY2->Text.c_str(), parser.Output[i]&0xFFFF );//Delayms(20)
						psl->Add(S);
					}
				}
				else if ( parser.Output[i] & LCD_CMD )//CMD
				{
					cmd = parser.Output[i];
					param.clear();
					if( 0x43== (LeExFunc->Text.ToInt()&0xff) )
						i++;
					for ( k = i+1; k < parser.Output.size(); k++ )
					{
						if ( (parser.Output[k] & LCD_DATA) )//DATA
							param.push_back(parser.Output[k]);
 						else if ((parser.Output[k] & LCD_DELAY)) {
							setting.iDelaytime = parser.Output[k]&0xffff;
							i=k;
							break;
						}else break;
					}

					setting.strCMD = LeCMD2->Text;
					setting.strDATA = LeDATA2->Text;
					setting.strAHead = LeArrayHead->Text;
					setting.strATrail = LeArrayTrail->Text;
					setting.strAGap = LeArrayGap->Text;
					setting.strAGapOut = LeArrayGapOut->Text;

					setting.outIF_16bit = if_out->ItemIndex;
					AddCMDPacket(psl, cmd, &param, &setting,if_in->ItemIndex);
				}
			}
			break;
			case 3:    //reg_set(cmd,p0,p1,...,pn)
			for ( i = 0; i < parser.Output.size(); i++ )
			{
				setting.iDelaytime = 0;
				if ( parser.Output[i] & LCD_DELAY )//DELAY   {REGFLAG_DELAY, 120, {0}},
				{
					if( 0x42== (LeExFunc->Text.ToInt()&0xff) || 0x32== (LeExFunc->Text.ToInt()&0xff) ){
						if( setting.isShowDelay ){
							if( (parser.Output[i]&0xFFFF) >1 ){
								S.sprintf( L"{REGFLAG_DELAY, %d, {0}},\r\n", parser.Output[i]&0xFFFF );//Delayms(20)
								psl->Add(S);
							}
						}
					}else{
						if( setting.isShowDelay ){
							if( (parser.Output[i]&0xFFFF) >1 ){
								S.sprintf( L"%s(%d);\r\n", LeDELAY2->Text.c_str(), parser.Output[i]&0xFFFF );//Delayms(20)
								psl->Add(S);
							}
						}
					}
				}
				else if ( parser.Output[i] & LCD_CMD )//CMD
				{
					cmd = parser.Output[i];
					param.clear();
					for ( k = i+1; k < parser.Output.size(); k++ )
					{
						if ( (parser.Output[k] & LCD_DATA) )//DATA
							param.push_back(parser.Output[k]);
						else if ((parser.Output[k] & LCD_DELAY)) {
							setting.iDelaytime = parser.Output[k]&0xffff;
							i=k-1;
							break;
						}else break;

					}

					setting.strCMD = LeCMD2->Text;
					setting.strDATA = LeDATA2->Text;
					setting.strAHead = LeArrayHead->Text;
					setting.strATrail = LeArrayTrail->Text;
					setting.strAGap = LeArrayGap->Text;
					setting.strAGapOut = LeArrayGapOut->Text;

					setting.outIF_16bit = if_out->ItemIndex;
					AddCMDPacket(psl, cmd, &param, &setting,if_in->ItemIndex);
				}
			}
			if( 0x42== (LeExFunc->Text.ToInt()&0xff) || 0x42== (LeExFunc->Text.ToInt()&0xff)){
				if( setting.isShowDelay ){
					S.sprintf( L"{REGFLAG_END_OF_TABLE, 0x00, {0}} ,\r\n" );//Delayms(20)
					psl->Add(S);
				}
			}
			break;
		}
	}
	else if( if_type->ItemIndex == 1 )
	{
		switch( rdoOutputType->ItemIndex )   //output memo format
		{
			case 0:       //c format
			for ( i = 0; i < parser.Output.size(); i++ )
			{
				if ( parser.Output[i] & LCD_DELAY )//DELAY
				{
					S.sprintf( L"%s(%d);\r\n", LeDELAY2->Text.c_str(), parser.Output[i]&0xFFFF );//Delayms(20)
					psl->Add(S);
				}
				else if ( parser.Output[i] & LCD_CMD )//CMD
				{
					cmd = parser.Output[i];
					param.clear();
					for ( k = i+1; k < parser.Output.size(); k++ )
					{
						if ( (parser.Output[k] & LCD_DATA) )//DATA
							param.push_back(parser.Output[k]);
						else
							break;
					}

					setting.strCMD = LeCMD2->Text;
					setting.strDATA = LeDATA2->Text;
					setting.bRenesasFormat = 1;
					setting.outIF_16bit = if_out->ItemIndex;
					AddCMDPacket(psl, cmd, &param, &setting,if_in->ItemIndex);
				}
			}
			break;
			case 1:
			for ( i = 0; i < parser.Output.size(); i++ ) {
				if ( parser.Output[i] & LCD_DELAY ){//DELAY
					S.sprintf( L"{time, %04u, ms}\r\n", parser.Output[i]&0xFFFF );//{time, 0025,ms}
					psl->Add(S);
				}
				else if ( parser.Output[i] & LCD_CMD ){//CMD
					cmd = parser.Output[i];
					param.clear();
					for ( k = i+1; k < parser.Output.size(); k++ ){
						if ( (parser.Output[k] & LCD_DATA) )//DATA
							param.push_back(parser.Output[k]);
						else
							break;
					}
					setting.bRenesasFormat = 0;
					setting.outIF_16bit = if_out->ItemIndex;   // 8/16 bit
					AddCMDPacket(psl, cmd, &param, &setting,if_in->ItemIndex);
				}
			}
			break;
		}
	}
	else if( if_type->ItemIndex == 4 ){  //0xmm...
		switch( rdoOutputType->ItemIndex ){   //output memo format
			case 0:       //c format
			if( if_out->ItemIndex  == 0 )// 8bit
				for ( k = 0; k < parser.Output.size(); k++ ) {
					S.sprintf(L"%s(0x%02X);", LeDATA2->Text.c_str(),parser.Output[k] & 0xFF);
					psl->Add(S);
				}
			 else{
				for ( k = 0; k < parser.Output.size(); k+=2 ){
					S.sprintf(L"%s(0x%02X%02X);", LeDATA2->Text.c_str(),parser.Output[k+1] & 0xFF, parser.Output[k] & 0xFF);
					psl->Add(S);
				}
			 }
			break;
		}
    }
	else if( if_type->ItemIndex == 2 ){  //GSWAP ==>
		DoGSWAPForward( psl );
	}
	else if( if_type->ItemIndex == 3 ){  //GSWAP <==
		DoGSWAPBack( psl );
	}
	if( rdoInputType->ItemIndex == 11){  //Do CIE generated
		DoCIE( psl );
		Memo2->Lines->Assign(psl);
		delete psl;
		return;
	}
	else if( rdoInputType->ItemIndex == 12 ){  //Line generate
		DoLineGenerate( psl );
		Memo2->Lines->Assign(psl);
		delete psl;
		return;
	}
	else if( rdoInputType->ItemIndex == 16 )
	{
		Memo2->ScrollBars=ssVertical;
		Memo2->Lines->Assign(parser.log.outstr);
		return;
    }

	Memo2->Lines->Assign(psl);
	delete psl;

	/*  //debug
	Memo2->Clear();
	Memo2->Lines->Add( L"数量: " + IntToStr((int)parser.Output.size() ) );
	Memo2->Lines->AddStrings( parser.DebugOut() );
	*/
}
void __fastcall TMainForm::DoGSWAPForward( TStringList* psl )
{
	unsigned long long GSWAP;
	switch( rdoOutputType->ItemIndex )   //output memo format
	{
	case 6:
	for ( int i = 0; i < parser.Output.size(); i+=5 )
	{
		GSWAP = 0;
		GSWAP += parser.Output[i+4];
		GSWAP <<= 8;
		GSWAP += parser.Output[i+3];
		GSWAP <<= 8;
		GSWAP += parser.Output[i+2];
		GSWAP <<= 8;
		GSWAP += parser.Output[i+1];
		GSWAP <<= 8;
		GSWAP += parser.Output[i+0];

		S.sprintf( L"%02u", GSWAP&0x1f );
		psl->Add(S);
		GSWAP>>=5;
		S.sprintf( L"%02u", GSWAP&0x1f );
		psl->Add(S);
		GSWAP>>=5;
		S.sprintf( L"%02u", GSWAP&0x1f );
		psl->Add(S);
		GSWAP>>=5;
		S.sprintf( L"%02u", GSWAP&0x1f );
		psl->Add(S);
		GSWAP>>=5;
		S.sprintf( L"%02u", GSWAP&0x1f );
		psl->Add(S);
		GSWAP>>=5;
		S.sprintf( L"%02u", GSWAP&0x1f );
		psl->Add(S);
		GSWAP>>=5;
		S.sprintf( L"%02u", GSWAP&0x1f );
		psl->Add(S);
		GSWAP>>=5;
		S.sprintf( L"%02u", GSWAP&0x1f );
		psl->Add(S);
	}
	break;

	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 7:
	S1 = L"";
	for ( int i = 0; i < parser.Output.size(); i+=5 )
	{
		GSWAP = 0;
		GSWAP += parser.Output[i+4];
		GSWAP <<= 8;
		GSWAP += parser.Output[i+3];
		GSWAP <<= 8;
		GSWAP += parser.Output[i+2];
		GSWAP <<= 8;
		GSWAP += parser.Output[i+1];
		GSWAP <<= 8;
		GSWAP += parser.Output[i+0];

		S1+=S.sprintf( L"%02u ", GSWAP&0x1f );
		GSWAP>>=5;
		S1+=S.sprintf( L"%02u ", GSWAP&0x1f );
		GSWAP>>=5;
		S1+=S.sprintf( L"%02u ", GSWAP&0x1f );
		GSWAP>>=5;
		S1+=S.sprintf( L"%02u ", GSWAP&0x1f );
		GSWAP>>=5;
		S1+=S.sprintf( L"%02u ", GSWAP&0x1f );
		GSWAP>>=5;
		S1+=S.sprintf( L"%02u ", GSWAP&0x1f );
		GSWAP>>=5;
		S1+=S.sprintf( L"%02u ", GSWAP&0x1f );
		GSWAP>>=5;
		S1+=S.sprintf( L"%02u ", GSWAP&0x1f );
	}
	psl->Add(S1);

	break;
	}
}
void __fastcall TMainForm::DoGSWAPBack( TStringList* psl )
{
	unsigned long long GSWAP;
	switch( rdoOutputType->ItemIndex )   //output memo format
	{
	case 6:
	for ( int i = 0; i < parser.Output.size(); i+=8 )
	{
		GSWAP = 0;
		GSWAP += parser.Output[i+7];
		GSWAP <<= 5;
		GSWAP += parser.Output[i+6];
		GSWAP <<= 5;
		GSWAP += parser.Output[i+5];
		GSWAP <<= 5;
		GSWAP += parser.Output[i+4];
		GSWAP <<= 5;
		GSWAP += parser.Output[i+3];
		GSWAP <<= 5;
		GSWAP += parser.Output[i+2];
		GSWAP <<= 5;
		GSWAP += parser.Output[i+1];
		GSWAP <<= 5;
		GSWAP += parser.Output[i+0];

		S.sprintf( L"0x%02x", GSWAP&0xff );
		psl->Add(S);
		GSWAP>>=8;
		S.sprintf( L"0x%02x", GSWAP&0xff );
		psl->Add(S);
		GSWAP>>=8;
		S.sprintf( L"0x%02x", GSWAP&0xff );
		psl->Add(S);
		GSWAP>>=8;
		S.sprintf( L"0x%02x", GSWAP&0xff );
		psl->Add(S);
		GSWAP>>=8;
		S.sprintf( L"0x%02x", GSWAP&0xff );
		psl->Add(S);
		//GSWAP>>=8;
	}
	break;
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 7:
	S1=L"";
	for ( int i = 0; i < parser.Output.size(); i+=8 )
	{
		GSWAP = 0;
		GSWAP += parser.Output[i+7];
		GSWAP <<= 5;
		GSWAP += parser.Output[i+6];
		GSWAP <<= 5;
		GSWAP += parser.Output[i+5];
		GSWAP <<= 5;
		GSWAP += parser.Output[i+4];
		GSWAP <<= 5;
		GSWAP += parser.Output[i+3];
		GSWAP <<= 5;
		GSWAP += parser.Output[i+2];
		GSWAP <<= 5;
		GSWAP += parser.Output[i+1];
		GSWAP <<= 5;
		GSWAP += parser.Output[i+0];

		S1+=S.sprintf( L"%02x ", GSWAP&0xff );
		GSWAP>>=8;
		S1+=S.sprintf( L"%02x ", GSWAP&0xff );
		GSWAP>>=8;
		S1+=S.sprintf( L"%02x ", GSWAP&0xff );
		GSWAP>>=8;
		S1+=S.sprintf( L"%02x ", GSWAP&0xff );
		GSWAP>>=8;
		S1+=S.sprintf( L"%02x ", GSWAP&0xff );
	}
	psl->Add(S1);

	break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::BtnCopy2Click(TObject *Sender)
{
	Clipboard()->AsText = Memo2->Text;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LinkLabel1Click(TObject *Sender)
{
	S = L"0x00, 0x01, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,\r\n\
0x10, 0x11, 0x12, 0x13,\r\n\
0x20, 0x21, 0x26, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E,\r\n\
0x30, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3C, 0x3E,\r\n\
0x44, 0x45,\r\n\
0xA1";
	S1 = L"\
inout	other(dec)	output \r\n \
any	00 	Default for normal operation.\r\n\ \
any	01 	Lilac2 format.\r\n\ \
any	02 	-> MTK WCOM[0]=0xmm,WCOM[n]=0xnn,WDATA(WCOM,n);\r\n\ \
1	03	input is 8bit, {0x00062902,0x001004B3,0x01};\r\n\ \
non-1	05	input is 8bit, {0x00062902,0x001004B3,0x01};\r\n\ \
any	67	input is 16bit, {0x00062902,0x001004B3,0x01};\r\n\ \
10	02	input is {0x00062902,0x001004B3,0x01},转为其他格式\r\n\ \
10	50	input is {0x00062902,0x001004B3,0x01},转为其他格式\r\n\ \
10	00	input is linux kernel dtsi，转为其他格式 0为基\r\n\ \
6	00	input is E7xxx系列格式 转为其他格式 1为基\r\n\ \
1	04	input is WCOM()->{29,0,0,0,2{0xmm,0xnn)} 1为基\r\n\ \
19-2	06	{0x00062902,0x001004B3,0x01};以1为基\r\n\ \
6-4	50	{0xB0,1,{0x04}};以1为基\r\n\ \
6-4	05	linux DTSI format: 29 00 00 00 delay 00 cnt cmd param1 param2 ;以1为基\r\n\ \
6-4	09	E7422 format ;以1为基\r\n\ \
20-4	50	input is Excel format: B0 04 ==> {0xB0,1,{0x04}};以1为基\r\n\ \
				";
	if( Sender == LinkLabel_Other )
	{
		//MessageBox( Handle, S1.c_str(), L"Other 的种类及使用案例", MB_OK);
		Form1->Memo1->SetTextBuf(S1.c_str());
		Form1->ShowModal();
	}
	else
		MessageBox( Handle, S.c_str(), L"本程序支持的DCS命令", MB_OK);
}
//---------------------------------------------------------------------------
#include <FileCtrl.hpp>
void __fastcall TMainForm::btnSaveClick(TObject *Sender)
{
	if( Sender == btnSave ) {
		if ( saveDlgInitial->Execute() )
			Memo2->Lines->SaveToFile( saveDlgInitial->FileName );
	}
	else if(Sender == btnSavePrj ){
		if ( saveDlgInitial->Execute() )
			SavePrj( saveDlgInitial->FileName );
	}
	else if(Sender == btnOpenPrj ){
		if ( openDlgLogDir->Execute() )
			OpenPrj(openDlgLogDir->FileName);
	}
	else if(Sender == btnOpenLogfile){
		if (!SelectDirectory(L"请选择目录","",S))
		{
			//MessageBox(NULL,L"error",L"warning",MB_OK);
		}else{

			TSearchRec sr;
			TStringList* sli = new TStringList();
			if (FindFirst(S + "\\*.txt", faAnyFile, sr) == 0)
			{
				do{
					sli->Add(S +"\\"+ sr.Name);
				} while (FindNext(sr) == 0);
				FindClose(sr);
			}
			Memo1->Lines = sli;
			ShowMessage(AnsiString().sprintf( "找到%d个文件:\n%s", sli->Count, sli->Text)); // 显示一下搜索结果
			sli->SaveToFile(L"c:\\1.txt"); // 顺便把结果保存到一个文本文件中

			delete sli;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OpenPrj(UnicodeString usPrjPath)
{
	DragAcceptFiles( Memo1->Handle, true );
	TStringList *pList = new TStringList;
	//DragAcceptFiles( Memo2->Handle, true );
	TIniFile* iniFile = new TIniFile( ChangeFileExt(usPrjPath, L".ini") );
	LeCMD1->Text=iniFile->ReadString( L"CTRL", L"CMD1", L"W_COM" );
	LeCMD2->Text=iniFile->ReadString( L"CTRL", L"CMD2", L"W_COM" );
	LeDATA1->Text=iniFile->ReadString( L"CTRL", L"DATA1", L"W_DATA" );
	LeDATA2->Text=iniFile->ReadString( L"CTRL", L"DATA2", L"W_DATA" );
	LeDELAY1->Text =iniFile->ReadString( L"CTRL", L"DELAY1",L"Delay" );
	LeDELAY2->Text=iniFile->ReadString( L"CTRL", L"DELAY2", L"Delay" );
	LeCmdPos->Text=iniFile->ReadString( L"CTRL", L"CMD_POS",L"1"  );
	LeDataPos->Text=iniFile->ReadString( L"CTRL", L"DATA_POS", L"2" );
	LeDelayPos->Text=iniFile->ReadString( L"CTRL", L"DELAY_POS",L"0" );
	LeAnnotate->Text=iniFile->ReadString( L"CTRL", L"ANNOTATE", L"//" );
	LeDataTypePos->Text=iniFile->ReadString( L"CTRL", L"D_TYPE_POS", L"0" );
	LeDataCntPos->Text=iniFile->ReadString( L"CTRL", L"D_CNT_POS", L"0" );
	LeArrayHead->Text=iniFile->ReadString( L"CTRL", L"A_HEAD", L"{" );
	LeArrayTrail->Text =iniFile->ReadString( L"CTRL", L"A_TRAIL",L"}");
	LeArrayHeadOut->Text=iniFile->ReadString( L"CTRL", L"A_HEAD1", L"{" );
	LeArrayTrailOut->Text =iniFile->ReadString( L"CTRL", L"A_TRAIL1",L"}");
	LeArrayGap->Text =iniFile->ReadString( L"CTRL", L"A_GAP1", L",");
	LeArrayGapOut->Text=iniFile->ReadString( L"CTRL", L"A_GAP2", L",");
	LeExFunc->Text=iniFile->ReadString( L"CTRL", L"OTHER",L"0"  );
	rdoInputType->ItemIndex=iniFile->ReadInteger( L"CTRL", L"in_format", 1 );
	rdoOutputType->ItemIndex=iniFile->ReadInteger( L"CTRL", L"out_format", 1 );
	if_in->ItemIndex=iniFile->ReadInteger( L"CTRL", L"if_in", 1 );
	if_out->ItemIndex=iniFile->ReadInteger( L"CTRL", L"if_out",  1);
	if_type->ItemIndex=iniFile->ReadInteger( L"CTRL", L"if_type", 1 );
	MIPI_Data_Type->Checked=iniFile->ReadBool( L"CTRL", L"DCS_GCS_SW", false  );
	AllGCS->Checked=iniFile->ReadBool( L"CTRL", L"ALLGCS", false );
	S = iniFile->ReadString( L"CTRL", L"Param",L"");

	pList->Delimiter = ',';
	pList->DelimitedText = S;
	pList->Count;
	if( pList->Count )
	for( int i=0; i<64; i++){
		ex_param[i]= StrToInt(pList->Strings[i]);
	}
	S = ChangeFileExt(usPrjPath, L".init.txt");
	if ( FileExists( S ) )
		Memo1->Lines->LoadFromFile(S);

	S = ChangeFileExt(usPrjPath, L".save.txt");
	if ( FileExists( S ) )
		Memo2->Lines->LoadFromFile(S);
}

void __fastcall TMainForm::SavePrj(UnicodeString usPrjPath)
{
	if ( FileExists( usPrjPath ) )
	{
		//Memo1->Lines->SaveToFile(usPrjPath);
		//Memo2->Lines->SaveToFile(usPrjPath);
	}
	else{
		Memo1->Lines->SaveToFile(usPrjPath);
		Memo2->Lines->SaveToFile(usPrjPath);
	}
	S = ChangeFileExt(usPrjPath, L".init.txt");
	Memo1->Lines->SaveToFile(S);

	S = ChangeFileExt(usPrjPath, L".save.txt");
	Memo2->Lines->SaveToFile(S);

	TIniFile* iniFile = new TIniFile( ChangeFileExt(usPrjPath, L".ini") );
	iniFile->WriteString( L"CTRL", L"CMD1", LeCMD1->Text );
	iniFile->WriteString( L"CTRL", L"CMD2", LeCMD2->Text );
	iniFile->WriteString( L"CTRL", L"DATA1", LeDATA1->Text );
	iniFile->WriteString( L"CTRL", L"DATA2", LeDATA2->Text );
	iniFile->WriteString( L"CTRL", L"DELAY1", LeDELAY1->Text );
	iniFile->WriteString( L"CTRL", L"DELAY2", LeDELAY2->Text );
	iniFile->WriteString( L"CTRL", L"CMD_POS", LeCmdPos->Text );
	iniFile->WriteString( L"CTRL", L"DATA_POS", LeDataPos->Text );
	iniFile->WriteString( L"CTRL", L"DELAY_POS", LeDelayPos->Text );
	iniFile->WriteString( L"CTRL", L"ANNOTATE", LeAnnotate->Text );
	iniFile->WriteString( L"CTRL", L"D_TYPE_POS", LeDataTypePos->Text );
	iniFile->WriteString( L"CTRL", L"D_CNT_POS", LeDataCntPos->Text );
	iniFile->WriteString( L"CTRL", L"A_HEAD", LeArrayHead->Text );
	iniFile->WriteString( L"CTRL", L"A_TRAIL", LeArrayTrail->Text );
	iniFile->WriteString( L"CTRL", L"A_HEAD1", LeArrayHeadOut->Text );
	iniFile->WriteString( L"CTRL", L"A_TRAIL1", LeArrayTrailOut->Text );
	iniFile->WriteString( L"CTRL", L"A_GAP1", LeArrayGap->Text );
	iniFile->WriteString( L"CTRL", L"A_GAP2", LeArrayGapOut->Text );
	iniFile->WriteString( L"CTRL", L"OTHER", LeExFunc->Text );
	iniFile->WriteInteger( L"CTRL", L"in_format", rdoInputType->ItemIndex );
	iniFile->WriteInteger( L"CTRL", L"out_format", rdoOutputType->ItemIndex );
	iniFile->WriteInteger( L"CTRL", L"if_in", if_in->ItemIndex );
	iniFile->WriteInteger( L"CTRL", L"if_out", if_out->ItemIndex );
	iniFile->WriteInteger( L"CTRL", L"if_type", if_type->ItemIndex );
	iniFile->WriteBool( L"CTRL", L"DCS_GCS_SW", MIPI_Data_Type->Checked );
	iniFile->WriteBool( L"CTRL", L"ALLGCS", AllGCS->Checked );

	S =L"";
	for( int i=0; i<64; i++){
		S += IntToStr(ex_param[i]);
		S += L",";
	}
	iniFile->WriteString( L"CTRL", L"Param", S );
}

void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	SavePrj(  Application->ExeName);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Memo1Click(TObject *Sender)
{
	//MessageBox(NULL, L"Select Test Log file",L"warning",MB_OK);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FindDir(UnicodeString Dir, TStringList* FileList)
{
	TSearchRec sr;

	if( FindFirst(Dir+"\\*",faAnyFile,sr) == 0){
		while(1){
			if(sr.Attr==faDirectory&&sr.Name!="."&&sr.Name!=".."){
				FindDir(Dir+"\\"+sr.Name,FileList);
			}
			else if( sr.Attr==faArchive ){
				FileList->Add(Dir +"\\"+ sr.Name);
			}
			if(FindNext(sr)!=0) break;
		}
	}
	FindClose(sr);
	return;
}

void __fastcall TMainForm::rdoInputTypeClick(TObject *Sender)
{
	static invoke_num=0;

	if( rdoInputType->ItemIndex == 16 ){
		//if ( openDlgLogDir->Execute() )
		//	S=openDlgLogDir->FileName;
		if (invoke_num==0) {
		   invoke_num++;
		   return;
		}else  invoke_num++;

		if (!SelectDirectory(L"请选择目录","",S))
		{
			//MessageBox(NULL,L"error",L"warning",MB_OK);
		}else{

			TSearchRec sr;
			TStringList* sli = new TStringList();
			/*if (FindFirst(S + "\\*.txt", faAnyFile, sr) == 0)
			{
				do{
					sli->Add(S +"\\"+ sr.Name);
				} while (FindNext(sr) == 0);
				FindClose(sr);
			}  */
			FindDir(S,sli);
			Memo1->Lines = sli;
			ShowMessage(AnsiString().sprintf( "找到%d个文件:\n%s", sli->Count, sli->Text)); // 显示一下搜索结果
			sli->SaveToFile(L"c:\\1.txt"); // 顺便把结果保存到一个文本文件中

			delete sli;

		}
	}
}
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner) : TForm(Owner)
{
	OpenPrj( Application->ExeName) ;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MainAppEventsMessage(tagMSG &Msg, bool &Handled)
{
	if ( Msg.message == WM_DROPFILES )
	//处理文件拖放
   {
	  HDROP hDrop = (HDROP)Msg.wParam;
	  int iLength = DragQueryFile( hDrop, 0, 0, 0 ) + 1;
	  TCHAR* FileFullPath = new TCHAR[iLength];
		DragQueryFile( hDrop, 0, FileFullPath, iLength );
		FileFullPath[iLength-1] = 0;
		DragFinish( hDrop );

		UnicodeString FileExt = ExtractFileExt( FileFullPath ).UpperCase();

		TMemo* pMemo;
		if ( Msg.hwnd == Memo1->Handle )
			pMemo = Memo1;
/*		else if (Msg.hwnd == Memo2->Handle)
			pMemo = Memo2;   */
		else
			goto FUNC_RETURN;

		if ( FileExt == L".TXT" || FileExt == L".C" || FileExt == L".CPP" || FileExt == L".H" )
		//载入初始化
		{
			pMemo->Clear();
			pMemo->Lines->LoadFromFile( UnicodeString(FileFullPath) );
		}

FUNC_RETURN:
		delete[] FileFullPath;
		DragFinish( hDrop );
	}
}
//---------------------------------------------------------------------------


