#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include "stdafx.h"
#include "TicTacToe.h"


enum EMarker {markEmpty, markX, markO};

#define MAX_LOADSTRING 100

HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];
EMarker gePlayerTurn = markX;
EMarker board[3][3] =
	{{markEmpty, markEmpty, markEmpty},
	{markEmpty, markEmpty, markEmpty},
	{markEmpty, markEmpty, markEmpty}};
const int squareSize = 100;
UINT scoresBuf[4];
TCHAR scoresBufStr[4][32];


ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Score(HWND, UINT, WPARAM, LPARAM);
typedef void (__cdecl *MYPROC)(LPCSTR);
void FileIO(int setter);


int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
    WNDCLASSEX wincl;

	wincl.cbSize = sizeof(WNDCLASSEX);

	wincl.style			= CS_HREDRAW | CS_VREDRAW;
	wincl.lpfnWndProc	= WndProc;
	wincl.cbClsExtra	= 0;
	wincl.cbWndExtra	= 0;
	wincl.hInstance		= hInstance;
	wincl.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTACTOE));
	wincl.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wincl.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wincl.lpszMenuName	= MAKEINTRESOURCE(IDC_TICTACTOE);
	wincl.lpszClassName	= szWindowClass;
	wincl.hIconSm		= LoadIcon(wincl.hInstance, MAKEINTRESOURCE(IDI_TICTACTOE));


	MSG msg;
	HACCEL hAccelTable;


	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TICTACTOE, szWindowClass, MAX_LOADSTRING);
	if (!RegisterClassEx (&wincl))
        return 0;


	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOE));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
            FileIO(-1);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;
   hInst = hInstance;
   hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        318,
        361,
        HWND_DESKTOP,
        NULL,
        hInstance,
        NULL);
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void FileIO(int setter){
    HANDLE file;
    DWORD b_written, b_read;
    file = CreateFile("score.txt",GENERIC_READ,0,NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
    char bufStr[2048];
    if (file != INVALID_HANDLE_VALUE){
        ReadFile(file,bufStr,2048 ,&b_read, NULL);
        CloseHandle(file);
    }
    char *bufPart =strtok(bufStr, " ");
    int c = 0;
    while(bufPart!=NULL){
    strcpy(scoresBufStr[c],bufPart);
    c++;
    bufPart = strtok(NULL, " ");
    }

    file = CreateFile("score.txt",GENERIC_WRITE,0,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
    LPCSTR spaceChar = " ";
    for(int i=0;i<4;i++){
        scoresBuf[i] = atoi(scoresBufStr[i]);
        if(setter!=i)
            _itot(scoresBuf[i],scoresBufStr[i],10);
        else
            _itot(scoresBuf[i]+1,scoresBufStr[i],10);
    }
    if (file != INVALID_HANDLE_VALUE){
        WriteFile(file, scoresBufStr[0], strlen(scoresBufStr[0]),&b_written, NULL);
        WriteFile(file, " ", strlen(spaceChar),&b_written, NULL);
        WriteFile(file, scoresBufStr[1], strlen(scoresBufStr[1]),&b_written, NULL);
        WriteFile(file, " ", strlen(spaceChar),&b_written, NULL);
        WriteFile(file, scoresBufStr[2], strlen(scoresBufStr[2]),&b_written, NULL);
        WriteFile(file, " ", strlen(spaceChar),&b_written, NULL);
        WriteFile(file, scoresBufStr[3], strlen(scoresBufStr[3]),&b_written, NULL);
        CloseHandle(file);
    }
}

void DrawBoard(HDC hdc)
{
    HPEN hLinePen;
	HPEN hPenOld;
	COLORREF qLineColor;
	qLineColor = RGB(0, 0, 0);
	hLinePen = CreatePen(PS_SOLID, 7, qLineColor);
	hPenOld = (HPEN)SelectObject(hdc, hLinePen);
	for (int iX = squareSize; iX < 3*squareSize; iX += squareSize) {
		MoveToEx(hdc, iX, 0, NULL);
		LineTo(hdc, iX, 3*squareSize);
	}
	for (int iY = squareSize; iY < 3*squareSize; iY += squareSize) {
		MoveToEx(hdc, 0, iY, NULL);
		LineTo(hdc, 3*squareSize, iY);
	}
	SelectObject(hdc, hPenOld);
	DeleteObject(hLinePen);
}

void DrawX(HDC hdc, int xDot, int yDot)
{
	HPEN hPenOld;
	HPEN hLinePen;
	COLORREF qLineColor;
	qLineColor = RGB(255, 0, 0);
	const int penWidth = 10;
	hLinePen = CreatePen(PS_SOLID, penWidth, qLineColor);
	hPenOld = (HPEN)SelectObject(hdc, hLinePen);

	const int lowX	= xDot*squareSize + 2*penWidth;
	const int highX	= (xDot + 1)*squareSize - 2*penWidth;
	const int lowY	= yDot*squareSize + 2*penWidth;
	const int highY	= (yDot + 1)*squareSize - 2*penWidth;

	MoveToEx(hdc, lowX, lowY, NULL);
	LineTo(hdc, highX, highY);
	MoveToEx(hdc, lowX, highY, NULL);
	LineTo(hdc, highX, lowY);

	SelectObject(hdc, hPenOld);
	DeleteObject(hLinePen);
}

void DrawO(HDC hdc, int xDot, int yDot) {
	HPEN hPenOld;
	HPEN hLinePen;
	COLORREF qLineColor;
	qLineColor = RGB(0, 0, 255);
	const int penWidth = 10;
	hLinePen = CreatePen(PS_SOLID, penWidth, qLineColor);
	hPenOld = (HPEN)SelectObject(hdc, hLinePen);

	const int lowX	= xDot*squareSize + 2*penWidth;
	const int highX	= (xDot + 1)*squareSize - 2*penWidth;
	const int lowY	= yDot*squareSize + 2*penWidth;
	const int highY	= (yDot + 1)*squareSize - 2*penWidth;

	Arc(hdc, lowX, lowY, highX, highY, 0, 0, 0, 0);

	SelectObject(hdc, hPenOld);
	DeleteObject(hLinePen);
}

bool PlaceX(LPARAM lParam)
{
	int iPosX = LOWORD(lParam);
	int iPosY = HIWORD(lParam);
	int squareX = (iPosX/squareSize);
	int squareY = (iPosY/squareSize);

	if ((gePlayerTurn == markX) &&
		(squareX < 3) && (squareY < 3) &&
		(board[squareY][squareX]) == markEmpty)
	{
		board[squareY][squareX] = markX;
		return true;
	}
	return false;
}

bool PlaceO(LPARAM lParam)
{
	int iPosX = LOWORD(lParam);
	int iPosY = HIWORD(lParam);
	int squareX = (iPosX/squareSize);
	int squareY = (iPosY/squareSize);

	if ((gePlayerTurn == markO) &&
		(squareX < 3) && (squareY < 3) &&
		(board[squareY][squareX]) == markEmpty)
	{
		board[squareY][squareX] = markO;
		return true;
	}
	return false;
}

bool HasWon() {
	if (board[0][0] != markEmpty) {
		if (board[0][1] == board[0][0] && board[0][2] == board[0][0])
		{
			return true;
		}
		if (board[1][0] == board[0][0] && board[2][0] == board[0][0])
		{
			return true;
		}
	}
	if (board[1][1] != markEmpty) {
		if (board[0][0] == board[1][1] && board[2][2] == board[1][1])
		{
			return true;
		}
		if (board[0][2] == board[1][1] && board[2][0] == board[1][1])
		{
			return true;
		}
		if (board[1][0] == board[1][1] && board[1][2] == board[1][1])
		{
			return true;
		}
		if (board[0][1] == board[1][1] && board[2][1] == board[1][1])
		{
			return true;
		}
	}
	if (board[2][2] != markEmpty) {
		if (board[2][0] == board[2][2] && board[2][1] == board[2][2])
		{
			return true;
		}
		if (board[0][2] == board[2][2] && board[1][2] == board[2][2])
		{
			return true;
		}
	}
	return false;
}

bool IsBoardFull()
{
	for (int iX = 0; iX < 3; ++iX) {
		for (int iY = 0; iY < 3; ++iY) {
			if (board[iY][iX] == markEmpty) {
				return false;
			}
		}
	}
	return true;
}

void ResetGame()
{
	for (int iX = 0; iX < 3; ++iX) {
		for (int iY = 0; iY < 3; ++iY) {
			board[iY][iX] = markEmpty;
		}
	}
	gePlayerTurn = markX;
}

void DLLWindow(char *msg, HWND hWnd)
{
    HINSTANCE lib;
    MYPROC thatFunction;

    lib = LoadLibrary(TEXT("TicTacToe-DLL.dll"));
    if (lib != NULL){
        thatFunction = (MYPROC) GetProcAddress(lib, "SomeFunction");
        if (thatFunction != NULL){
            thatFunction(msg);
        }
        else{
            MessageBox(hWnd, "Problems with getting function", "Error", MB_OK);
        }
        FreeLibrary(lib);
    }
    else {
    MessageBox(hWnd, "Problems with DLL file", "Error", MB_OK);
    FreeLibrary(lib);
    }

}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	PAINTSTRUCT ps;
	HDC hdc;


	switch (message)
	{
	case WM_LBUTTONDOWN:
		{
			bool validMove = PlaceX(lParam);
			if (validMove) {
				gePlayerTurn = markO;
			} else {
			    DLLWindow("Invalid move", hWnd);
			}
			InvalidateRect(hWnd, 0, TRUE);
		    break;
		}
	case WM_RBUTTONDOWN:
		{
			bool validMove = PlaceO(lParam);
			if (validMove) {
				gePlayerTurn = markX;
			} else {
                DLLWindow("Invalid move", hWnd);
			}
			InvalidateRect(hWnd, 0, TRUE);
			break;
		}
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDM_SCORE:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_SCOREBOX), hWnd, Score);
			break;
		case IDM_NEW_GAME:
			ResetGame();
			FileIO(3);
			InvalidateRect(hWnd, 0, TRUE);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		{
		hdc = BeginPaint(hWnd, &ps);
		DrawBoard(hdc);

		for (int iX = 0; iX < 3; ++iX) {
			for (int iY = 0; iY < 3; ++iY) {
				if (board[iY][iX] == markX) {
					DrawX(hdc, iX, iY);
				} else if (board[iY][iX] == markO) {
					DrawO(hdc, iX, iY);
				}
			}
		}

		if (HasWon()) {
			if (gePlayerTurn == markO) {
                FileIO(0);
				::MessageBox(hWnd, _T("X - won"), _T("New Game?"), MB_OK);
			} else {
			    FileIO(1);
				::MessageBox(hWnd, _T("O - won"), _T("New Game?"), MB_OK);
			}
			ResetGame();
			InvalidateRect(hWnd, 0, TRUE);
		} else {
			if (IsBoardFull()) {
                FileIO(2);
				::MessageBox(hWnd, _T("Draw"), _T("New Game?"), MB_OK);
				ResetGame();
				InvalidateRect(hWnd, 0, TRUE);
			}
		}
		EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK Score(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	    {
	        SetDlgItemText(hDlg, IDC_MESSAGE1, scoresBufStr[0]);
	        SetDlgItemText(hDlg, IDC_MESSAGE2, scoresBufStr[1]);
	        SetDlgItemText(hDlg, IDC_MESSAGE3, scoresBufStr[2]);
	        SetDlgItemText(hDlg, IDC_MESSAGE4, scoresBufStr[3]);
	      return (INT_PTR)TRUE;
	    }
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
