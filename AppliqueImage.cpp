// AppliqueImage.cpp : Defines the entry point for the application.
//
// CreerImageWim.cpp : Definit le point d'entree de l'application.
//

#include "stdafx.h"

#define MAX_LOADSTRING 100

// Variables globales :
HINSTANCE hInst;                                // instance actuelle
CHAR szTitle[MAX_LOADSTRING];                  // Texte de la barre de titre
CHAR szWindowClass[MAX_LOADSTRING];            // nom de la classe de fenêtre principale
BROWSEINFO bi;
ITEMIDLIST *il;
OPENFILENAME ofn;
INITCOMMONCONTROLSEX iccex; 
WNDCLASS wc;
HWND hWnd,hwndTT,imghWnd;
NOTIFYICONDATAA nf;
RECT rc,rect;
HMENU mnu;
SYSTEMTIME st;
BOOL AffichageMenu=FALSE;
WCHAR url[256];
DWORD version=MAKEWORD(21,1);

char szPath[MAX_PATH];
char buffer[MAX_PATH];
char Result[MAX_PATH];
char szFile[MAX_PATH];
char m_csOutput[0xff];
char tmp[10];

BOOL CALLBACK    ProcedurePrincipale(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ProcedurePropos(HWND, UINT, WPARAM, LPARAM);


char jours[7][10] = {"dimanche", "lundi","mardi","mercredi","jeudi","vendredi","samedi"};
char mois[12][10] = {"janvier", "fevrier","mars", "avril", "mai", "juin","juillet","aout","septembre", "octobre", "novembre", "decembre"};

char* ExtraireNomImage(char* Chemin);
void GetDate();
char* GetTime();
void SauverFichier();
void ExplorerDossiers(LPCTSTR titre);
static BOOL CALLBACK DialogFunc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
LPSTR ExecuteCommandeDos(LPSTR csExeName, LPSTR csArguments);
void GenerationParametre();
int MsgBox(HWND hDlg,char* lpszText,char* lpszCaption, DWORD dwStyle,int lpszIcon);

class ImageWIM{
public:
	char date[0x08];
	bool bootable;
	bool clean;
	char nom[0x30];
	char scratchDir[0x105];
	char fichier[0x104];
	void Reset(){
		GetDate();
		bootable=false;
		clean=false;
		strcpy(nom,Titre);
		EnableWindow(GetDlgItem(hWnd,IDC_BUTTON2),bootable);
		EnableWindow(GetDlgItem(hWnd,IDC_BUTTON4),clean);
		SetDlgItemText(hWnd,IDC_EDIT1,nom);
		SetDlgItemText(hWnd,IDC_EDIT3,0x00);
		SetDlgItemText(hWnd,IDC_CHEMIN,scratchDir);
		EnableWindow(GetDlgItem(hWnd,IDC_BUTTON1),false);
		EnableWindow(GetDlgItem(hWnd,IDC_BUTTON2),false);
		EnableWindow(GetDlgItem(hWnd,IDC_BUTTON4),false);
		EnableWindow(GetDlgItem(hWnd,IDC_BUTTON5),true);
		EnableWindow(GetDlgItem(hWnd,IDC_PROCEDE),false);
	}
	void CreateDiskpartConfig(){
		FILE* f=fopen("disk0","+w");
		fprintf(f,"list disk\n");
		fprintf(f,"select disk 0\n");
		fprintf(f,"clean\n");
		fprintf(f,"convert gpt\n");
		fprintf(f,"create partition primary size=300\n");
		fprintf(f,"format quick fs=ntfs label=\"Reparation Windows\"\n");
		fprintf(f,"assign letter=\"T\"\n");
		fprintf(f,"set id=\"de94bba4-06d1-4d40-a16a-bfd50179d6ac\"\n");
		fprintf(f,"gpt attributes=0x8000000000000001\n");
		fprintf(f,"create partition efi size=260\n");
		fprintf(f,"format quick fs=fat32 label=\"OS10\"\n");
		fprintf(f,"assign letter=\"S\"\n");
		fprintf(f,"create partition msr size=128\n");
		fprintf(f,"create partition primary\n");
		fprintf(f,"format quick fs=ntfs label=\"%s\"\n",nom);
		fprintf(f,"assign letter=\"W\"\n");
		fprintf(f,"set id=de94bba4-06d1-4d40-a16a-bfd50179d6ac\n");
		fprintf(f,"gpt attributes=0x8000000000000001\n");
		fprintf(f,"list volume\n");
		fprintf(f,"exit\n");
	}
	void AppliquerImage(){
		if(clean==true){
			wsprintf(buffer,"dism /Apply-Image /ImageFile:\"%s\" /Index:1 /ApplyDir:W:\\",wim.fichier);
		}else{
			wsprintf(buffer,"dism /Apply-Image /ImageFile:\"%s\" /Index:1 /ApplyDir:C:\\",wim.fichier);
		}
		if(strlen(Result)>3){
			wsprintf(buffer,"dism /Apply-Image /ImageFile:\"%s\" /Index:1 /ApplyDir:\"%s\"",wim.fichier,Result);
		}
		MsgBox(hWnd,buffer,"Merci de soutenir l`auteur",0,IDI_ICON1);
		SetDlgItemText(hWnd,IDC_EDIT2,buffer);
	//	ExecuteCommandeDos(buffer,0);
		system(buffer);
	}
	void SetBootable(){
		if(IsWindowEnabled(GetDlgItem(hWnd,IDC_BUTTON2))==0x01){
			bootable=true;
			ExecuteCommandeDos("Bootrec.exe","/FixMbr");
			ExecuteCommandeDos("Bootrec.exe","/FixBoot");
			ExecuteCommandeDos("Bootrec.exe","/ScanOs");
			ExecuteCommandeDos("Bootrec.exe","/RebuildBcd"); 
		}
	}
	void SetCleanable(){
		if(IsWindowEnabled(GetDlgItem(hWnd,IDC_BUTTON4))==0x01){
			bootable=true;
			clean=true;
			ExecuteCommandeDos("diskpart","/s disk0");
		}
	}

} wim;

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	iccex.dwICC=ICC_WIN95_CLASSES;
	iccex.dwSize=sizeof(iccex);
	InitCommonControlsEx(&iccex);
	GetCurrentDirectory(0xFF,wim.scratchDir);
	memset(&wc,0,sizeof(wc));
	wc.hCursor=LoadCursor(hInstance, (LPCTSTR)IDC_CURSOR1);
	wc.lpfnWndProc = DefDlgProc;
	wc.cbWndExtra = DLGWINDOWEXTRA;
	wc.hInstance = hInstance;
	wc.lpszClassName ="AppliquerImageWim";
	wc.hbrBackground=(HBRUSH)CreateSolidBrush(RGB(0x4C,0x3C,0x5B));
	wc.hIcon=LoadIcon(hInstance,(LPCTSTR)IDI_ICON1);
	wc.style = CS_VREDRAW  | CS_HREDRAW | CS_SAVEBITS | CS_DBLCLKS;
	RegisterClass(&wc);
	return DialogBox(hInstance, (LPCTSTR)IDD_DIALOG1, NULL, (DLGPROC)ProcedurePrincipale);
}
BOOL CALLBACK ProcedurePrincipale(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	hWnd =hDlg;
	switch (message) {
		case WM_INITDIALOG:{
			GetDate();
			SetTimer(hDlg,IDM_TIMER1,1000,(TIMERPROC) NULL);
			RemoveMenu(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_STRING);
			RemoveMenu(GetSystemMenu(hDlg, FALSE), SC_MOVE, MF_STRING);
			AppendMenu(GetSystemMenu(hDlg,FALSE),MF_STRING, IDC_BUTTON3,"A propos de ce programme..");
			AppendMenu(GetSystemMenu(hDlg,FALSE),MF_STRING,0xE140,"Quel Windows s'execute?");
			AppendMenu(GetSystemMenu(hDlg,FALSE),MF_STRING,2,"Fermer ce programme");
			nf.cbSize=sizeof(nf);
			nf.hIcon=wc.hIcon;
			nf.hWnd=hDlg;
			strcpy(nf.szTip,Titre);
			nf.uCallbackMessage=WM_TRAY_ICONE;
			nf.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
			Shell_NotifyIcon(NIM_ADD,&nf);
			imghWnd = CreateWindowEx(0, "STATIC", NULL, WS_VISIBLE|WS_CHILD|SS_ICON,600, 10, 10, 10,hDlg , (HMENU)45000, wc.hInstance, NULL);
			SendMessage(imghWnd, STM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(wc.hInstance,(LPCTSTR)IDI_ICON2));
			SetWindowText(hDlg,Titre);
			GetLocalTime(&st);
			CreateStatusWindow(WS_CHILD|WS_VISIBLE,__argv[0],hDlg,6000);
			sprintf(buffer,"Nous sommes %s, le %2d %s %4d",jours[st.wDayOfWeek],st.wDay,mois[st.wMonth-1],st.wYear); //creation du string de date
			SetDlgItemText(hDlg,IDC_TIME2,buffer);
#ifdef _WIN64
			SetClassLong(hDlg, GCLP_HICON, (long)LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON3)));
#else
			SetClassLong(hDlg, GCL_HICON, (long)LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON3)));
#endif // WIN64
			SendMessage(GetDlgItem(hDlg,IDC_BUTTON3), WM_SETFONT, (WPARAM)GetStockObject(2), 1L);
			SendMessage(GetDlgItem(hDlg,6000), WM_SETFONT, (WPARAM)GetStockObject(0x1E), 1L);
			SendMessage(GetDlgItem(hDlg,IDC_EDIT1), WM_SETFONT, (WPARAM)GetStockObject(7), 1L);
			SendMessage(GetDlgItem(hDlg,IDC_CHEMIN), WM_SETFONT, (WPARAM)GetStockObject(7), 1L);
			SendMessage(GetDlgItem(hDlg,IDC_EDIT3), WM_SETFONT, (WPARAM)GetStockObject(7), 1L);
			SendMessage(GetDlgItem(hDlg,IDC_BUTTON5), WM_SETFONT, (WPARAM)GetStockObject(6), 1L);
			SendMessage(GetDlgItem(hDlg,IDCANCEL), WM_SETFONT, (WPARAM)GetStockObject(0x1E), MAKELPARAM(TRUE, 0));
			SendMessage(GetDlgItem(hDlg,IDOK), WM_SETFONT, (WPARAM)GetStockObject(0x1F), MAKELPARAM(TRUE, 0));
			SendMessage(GetDlgItem(hDlg,0xE140), STM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(wc.hInstance,(LPCTSTR)IDI_ICON1));
			SetTextColor( (HDC)IDC_EDIT1, RGB(128,0xFF,0) );
			EnableWindow(GetDlgItem(hDlg,IDC_PROCEDE),false);
			wim.Reset();


					   }return TRUE;
	case WM_SYSCOMMAND:{
		switch (LOWORD(wParam)) {
			case 0xE140:ShellAbout(hDlg,wc.lpszClassName,"Createur d'images WIM 32Bits\n© Patrice Waechter-Ebling 2022",wc.hIcon);
				break;
			case IDC_BUTTON3:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd,(DLGPROC) ProcedurePropos);
				break;
			case 0xE145:
				break;
			case IDCANCEL:{
					Shell_NotifyIcon(NIM_DELETE,&nf); 
					KillTimer(hDlg,IDM_TIMER1);
					PostQuitMessage(1);
						  }return 0x01;
				}		
					   }break;	
	case WM_COMMAND:{
		switch (LOWORD(wParam)) {
			case IDC_BUTTON1:
				ExplorerDossiers(szTitle);
				break;
			case IDC_BUTTON5:{
				SauverFichier();
							 }break;
			case IDC_PROCEDE:{
				wim.AppliquerImage();
							 }break;
			case IDC_BUTTON3:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd,(DLGPROC) ProcedurePropos);
				break;
			case 0xE141:ShellExecute(hDlg,"open",wim.scratchDir,NULL,NULL,0x01);
				break;
			case IDC_BUTTON2:{
				if(MsgBox(hWnd,"Voulez vous appliquer la pocedure de vidange?\nCette methode supprime toutes les partitions du disque dur principal.\n\nBootfiles restore","Alerte fortement deconseille",MB_YESNO,IDI_ICON2)==0x06){
					EnableWindow(GetDlgItem(hWnd,IDC_BUTTON4),true);
					if(MsgBox(hWnd,"Il n' a pas de retour !\n\nOncontinue sur cette voie ?","Alerte serieuse",MB_YESNO,IDI_ICON2)==0x06){
						EnableWindow(GetDlgItem(hWnd,IDC_BUTTON4),true);
					}else{
						EnableWindow(GetDlgItem(hWnd,IDC_BUTTON2),false);
						EnableWindow(GetDlgItem(hWnd,IDC_BUTTON4),false);
					}
				}else{
					EnableWindow(GetDlgItem(hWnd,IDC_BUTTON2),false);
					EnableWindow(GetDlgItem(hWnd,IDC_BUTTON4),false);
				}
				EnableWindow(GetDlgItem(hWnd,IDC_PROCEDE),true);				
							 }break;
			case 1010:{
					wim.Reset();
							 }break;
			case IDCANCEL:{
					Shell_NotifyIcon(NIM_DELETE,&nf); 
					KillTimer(hDlg,IDM_TIMER1);
					PostQuitMessage(1);
						  }return 0x01;
			}
							 }break;
		case WM_TIMER:{
			GetLocalTime(&st);
			sprintf(buffer,"Nous sommes %s, le %2d %s %4d",jours[st.wDayOfWeek],st.wDay,mois[st.wMonth-1],st.wYear);
			SetDlgItemText(hDlg,IDC_TIME2,buffer);
			sprintf(buffer,"%.2d:%.2d:%.2d",st.wHour,st.wMinute,st.wSecond);
			SetDlgItemText(hDlg,IDC_TIME,buffer);
					  }break;
		case WM_CTLCOLORDLG:		return (long)wc.hbrBackground;
		case WM_CTLCOLORSTATIC:{
			SetTextColor( (HDC)wParam, RGB(0xFF, 0xFF,0xFF) );
			SetBkMode( (HDC)wParam, TRANSPARENT ); 
						   }return (LONG)wc.hbrBackground; //retourne la couleur choisie dans la classe
		case WM_CTLCOLOREDIT:{
			SetBkMode((HDC)wParam, TRANSPARENT); 	
			if (GetDlgItem(hDlg, IDC_EDIT1) == reinterpret_cast<HWND>(lParam)) SetTextColor((HDC)wParam, RGB(0,120,  192)); 
			if (GetDlgItem(hDlg, IDC_CHEMIN) == reinterpret_cast<HWND>(lParam)) SetTextColor((HDC)wParam, RGB(0, 255, 155)); 
			if (GetDlgItem(hDlg, IDC_EDIT3) == reinterpret_cast<HWND>(lParam)) SetTextColor((HDC)wParam, RGB(255,65,  0)); 
			if (GetDlgItem(hDlg, IDC_EDIT2) == reinterpret_cast<HWND>(lParam)) SetTextColor((HDC)wParam, RGB(255,255,  0)); 

						 }return (LONG)(HBRUSH)wc.hbrBackground;
		case WM_CTLCOLORLISTBOX: {
			SetTextColor( (HDC)wParam, RGB(0xFF,0xFF, 0xFF) );
			SetBkMode( (HDC)wParam, TRANSPARENT ); 
							 }return (LONG)(HBRUSH)CreateSolidBrush(RGB(0,0,0));
		case WM_CTLCOLORBTN:{
			SetBkMode( (HDC)wParam, TRANSPARENT );
			switch(LOWORD(wParam)){
			case IDOK:{SetTextColor((HDC)wParam, RGB(0, 0xFF, 0));}break;
			}
				if (GetDlgItem(hWnd, IDCANCEL) == reinterpret_cast<HWND>(lParam)) SetTextColor((HDC)wParam, RGB(0xFF, 0, 0)); 
				if (GetDlgItem(hWnd, 6000) == reinterpret_cast<HWND>(lParam)) SetTextColor((HDC)wParam, RGB( 0, 0,0xFF)); 
						}return (LONG)(HBRUSH)wc.hbrBackground;
		case WM_CTLCOLORMSGBOX:{
			SetTextColor( (HDC)wParam, RGB(0,0xFF, 0xFF) );
						   }return (long)(HBRUSH)CreateSolidBrush(RGB(0xFF,0,128));
		case WM_CLOSE: {
			Shell_NotifyIcon(NIM_DELETE,&nf);
			KillTimer(hDlg,IDM_TIMER1);
			PostQuitMessage(1);
				   }return  0x01;
	}return FALSE;
 
}

// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK ProcedurePropos(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
		case WM_INITDIALOG:{
#ifdef _WIN64
			SetClassLong(hDlg, GCLP_HICON, (long)LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1)));
#else
			SetClassLong(hDlg, GCL_HICON, (long)LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1)));
#endif // WIN64
				   }return TRUE;
	case WM_CTLCOLORDLG:		return (long)CreateSolidBrush(RGB(0xCC, 0xCC, 0x50));
	case WM_CTLCOLORSTATIC: {
		SetTextColor((HDC)wParam, RGB(0, 0, 0xFF));
		SetBkMode((HDC)wParam, TRANSPARENT);
	}return (LONG)CreateSolidBrush(RGB(0xCC, 0xCC, 0x50));
	case WM_CTLCOLORBTN: {
		SetBkMode((HDC)wParam, TRANSPARENT);
		switch (LOWORD(wParam)) {
		case IDOK: {SetTextColor((HDC)wParam, RGB(0, 0xFF, 0)); }break;
		}
		if (GetDlgItem(hWnd, IDCANCEL) == reinterpret_cast<HWND>(lParam)) SetTextColor((HDC)wParam, RGB(0xFF, 0, 0));
	}return (LONG)(HBRUSH)wc.hbrBackground;
	case WM_CTLCOLORMSGBOX: {
		SetTextColor((HDC)wParam, RGB(0, 0xFF, 0xFF));
	}return (long)(HBRUSH)CreateSolidBrush(RGB(0xFF, 0, 128));

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
LPSTR ExecuteCommandeDos(LPSTR csExeName, LPSTR csArguments){
	char tempEx[0xFE];
	wsprintf(tempEx,"%s %s",csExeName,csArguments);
	SECURITY_ATTRIBUTES secattr; 
	ZeroMemory(&secattr,sizeof(secattr));
	secattr.nLength = sizeof(secattr);
	secattr.bInheritHandle = TRUE;
	HANDLE rPipe, wPipe;
	CreatePipe(&rPipe,&wPipe,&secattr,0);
	STARTUPINFO sInfo; 
	ZeroMemory(&sInfo,sizeof(sInfo));
	PROCESS_INFORMATION pInfo; 
	ZeroMemory(&pInfo,sizeof(pInfo));
	sInfo.cb=sizeof(sInfo);
	sInfo.dwFlags=STARTF_USESTDHANDLES;
	sInfo.hStdInput=NULL; 
	sInfo.hStdOutput=wPipe; 
	sInfo.hStdError=wPipe;
	CreateProcess(0,(LPSTR)tempEx,0,0,TRUE,NORMAL_PRIORITY_CLASS|CREATE_FORCEDOS/*CREATE_NO_WINDOW*/,0,0,&sInfo,&pInfo);
	CloseHandle(wPipe);
	char buf[100];
	DWORD reDword; 
	BOOL res;
	do{
		res=::ReadFile(rPipe,buf,100,&reDword,0);
		strcat(m_csOutput,buf);
	}while(res);
	SetDlgItemText(hWnd,6000,m_csOutput);
	return m_csOutput;
}
char* ExtraireNomImage(char* Chemin){
	wsprintf(wim.nom,"%s_%s",PathFindFileNameA(Chemin), wim.date);
	return wim.nom;
}
void GetDate(){
	GetLocalTime(&st);
	wsprintf(wim.date,"%4d%.2d%.2d",st.wYear,st.wMonth,st.wDay);
}
char* GetTime(){
	GetLocalTime(&st);
	wsprintf(tmp,"%.2d%:2d%:2d",st.wHour,st.wMinute,st.wSecond);
	return tmp;
}
void GenerationParametre(){
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(wc.hInstance, 1, szHello, MAX_LOADSTRING);
	char scratch[0xFE];
	GetCurrentDirectory(0xFE,scratch);
	GetDlgItemText(hWnd,IDC_EDIT1,wim.nom,sizeof(wim.nom));
	/*
char edition[0x16];
int compression=0;
	*/
}
int MsgBox(HWND hDlg,char* lpszText,char* lpszCaption, DWORD dwStyle,int lpszIcon){
	MSGBOXPARAMS lpmbp;
	lpmbp.hInstance=wc.hInstance;
	lpmbp.cbSize=sizeof(MSGBOXPARAMS);
	lpmbp.hwndOwner=hDlg;
	lpmbp.dwLanguageId=MAKELANGID(0x0800,0x0800); //par defaut celui du systeme
	lpmbp.lpszText=lpszText;
	if(lpszCaption!=NULL){
		lpmbp.lpszCaption=lpszCaption;
	}else{
		lpmbp.lpszCaption=Titre;
	}
	lpmbp.dwStyle=dwStyle|0x00000080L;
	lpmbp.lpszIcon=(LPCTSTR)lpszIcon;
	lpmbp.lpfnMsgBoxCallback=0;
	return  MessageBoxIndirect(&lpmbp);
}
void SauverFichier(){
	SHGetSpecialFolderPath(hWnd,szPath,CSIDL_PERSONAL,FALSE);
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrInitialDir = szPath;
	ofn.lpstrFilter ="Fichiers Source\0*.wim\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrTitle="Enregister l'image sous";
	ofn.lpstrFileTitle =NULL;
	ofn.nMaxFileTitle = 0;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn)==TRUE) { 
		HANDLE hf = CreateFile(ofn.lpstrFile, GENERIC_WRITE,0, (LPSECURITY_ATTRIBUTES) NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,(HANDLE) NULL); //acces au fichier
		SetDlgItemText(hWnd,IDC_EDIT3,ofn.lpstrFile);
		SetDlgItemText(hWnd,IDC_EDIT1,ExtraireNomImage(ofn.lpstrFile));
		strcpy(wim.fichier,ofn.lpstrFile);
		EnableWindow(GetDlgItem(hWnd,IDC_PROCEDE),false);
		EnableWindow(GetDlgItem(hWnd,IDC_BUTTON1),true);
		EnableWindow(GetDlgItem(hWnd,IDC_BUTTON5),false);

	}
}
void ExplorerDossiers(LPCTSTR titre){
	bi.hwndOwner=hWnd;
	bi.pidlRoot=NULL;
	bi.pszDisplayName=&buffer[0];
	bi.lpszTitle=titre;
	bi.ulFlags=BIF_RETURNONLYFSDIRS |BIF_DONTGOBELOWDOMAIN|BIF_STATUSTEXT |BIF_EDITBOX ;
	bi.lpfn=NULL; 
	il=SHBrowseForFolder(&bi);
	if (il==NULL) return ;
	if(SHGetPathFromIDList(il,&Result[0])){
		SetDlgItemText(hWnd,IDC_CHEMIN,Result);
		if(strlen(Result)<=3){
			CHAR szVolumeName[MAX_PATH];
			BOOL bSucceeded = GetVolumeInformation(Result,szVolumeName,MAX_PATH,NULL,NULL,NULL,NULL,0);
			SetDlgItemText(hWnd,IDC_EDIT1,ExtraireNomImage(szVolumeName));
		}else{
			SetDlgItemText(hWnd,IDC_EDIT1,ExtraireNomImage(Result));
		}
		if(strcmp(Result,"C:\\")==0x00){
			EnableWindow(GetDlgItem(hWnd,IDC_BUTTON2),true);
			if(MsgBox(hWnd,"Voulez vous appliquer une image systeme bootable?","Alerte",MB_YESNO,IDI_ICON2)==0x06){
				EnableWindow(GetDlgItem(hWnd,IDC_BUTTON4),true);
			}else{
				EnableWindow(GetDlgItem(hWnd,IDC_BUTTON2),false);
				EnableWindow(GetDlgItem(hWnd,IDC_PROCEDE),true);
			}
		}else{
			EnableWindow(GetDlgItem(hWnd,IDC_BUTTON2),false);
			EnableWindow(GetDlgItem(hWnd,IDC_PROCEDE),true);
		}

		EnableWindow(GetDlgItem(hWnd,IDC_BUTTON1),false);
	}
}
