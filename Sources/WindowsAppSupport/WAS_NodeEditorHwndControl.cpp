#include "WAS_NodeEditorHwndControl.hpp"
#include "WAS_WindowsAppUtils.hpp"
#include "NE_Debug.hpp"

namespace WAS
{

class SetCaptureHandler
{
public:
	SetCaptureHandler () :
		counter (0)
	{
	
	}

	void HandleMouseDown (HWND hwnd)
	{
		if (counter == 0) {
			SetCapture (hwnd);
		}
		counter += 1;
	}

	void HandleMouseUp ()
	{
		counter -= 1;
		if (counter <= 0) { // sometimes down and up are not in pair
			ReleaseCapture ();
			counter = 0;
		}
	}

private:
	int counter;
};

static LRESULT CALLBACK StaticWindowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static SetCaptureHandler setCaptureHandler;

	if (msg == WM_CREATE) {
		LPCREATESTRUCT createStruct = LPCREATESTRUCT (lParam);
		SetWindowLongPtr (hwnd, GWLP_USERDATA, (LONG_PTR) createStruct->lpCreateParams);
	}

	NodeEditorHwndControl* control = (NodeEditorHwndControl*) GetWindowLongPtr (hwnd, GWLP_USERDATA);
	if (control == nullptr) {
		return DefWindowProc (hwnd, msg, wParam, lParam);
	}

	NUIE::NodeEditor* nodeEditor = control->GetNodeEditor ();
	if (DBGERROR (nodeEditor == nullptr)) {
		return DefWindowProc (hwnd, msg, wParam, lParam);
	}

	switch (msg) {
		case WM_PAINT:
			control->Draw ();
			break;
		case WM_ERASEBKGND:
			return 0;
		case WM_LBUTTONDOWN:
			{
				setCaptureHandler.HandleMouseDown (hwnd);
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseDown (WAS::GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Left, x, y);
			}
			break;
		case WM_MBUTTONDOWN:
			{
				setCaptureHandler.HandleMouseDown (hwnd);
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseDown (WAS::GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Middle, x, y);
			}
			break;
		case WM_RBUTTONDOWN:
			{
				setCaptureHandler.HandleMouseDown (hwnd);
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseDown (WAS::GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Right, x, y);
			}
			break;
		case WM_LBUTTONUP:
			{
				setCaptureHandler.HandleMouseUp ();
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseUp (WAS::GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Left, x, y);
			}
			break;
		case WM_MBUTTONUP:
			{
				setCaptureHandler.HandleMouseUp ();
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseUp (WAS::GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Middle, x, y);
			}
			break;
		case WM_RBUTTONUP:
			{
				setCaptureHandler.HandleMouseUp ();
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseUp (WAS::GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Right, x, y);
			}
			break;
		case WM_MOUSEMOVE:
			{
				SetFocus (hwnd); // before Windows 10 only the focused window catches the mouse wheel message
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseMove (WAS::GetModiferKeysFromEvent (wParam), x, y);
			}
			break;
		case WM_MOUSEWHEEL:
			{
				POINT mousePos;
				mousePos.x = GET_X_LPARAM (lParam);
				mousePos.y = GET_Y_LPARAM (lParam);
				ScreenToClient (hwnd, &mousePos);
				int delta = GET_WHEEL_DELTA_WPARAM (wParam);
				NUIE::MouseWheelRotation rotation = delta > 0 ? NUIE::MouseWheelRotation::Forward : NUIE::MouseWheelRotation::Backward;
				nodeEditor->OnMouseWheel (WAS::GetModiferKeysFromEvent (wParam), rotation, mousePos.x, mousePos.y);
			}
			break;
		case WM_LBUTTONDBLCLK:
			{
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseDoubleClick (WAS::GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Left, x, y);
			}
			break;
		case WM_MBUTTONDBLCLK:
			{
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseDoubleClick (WAS::GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Middle, x, y);
			}
			break;
		case WM_RBUTTONDBLCLK:
			{
				int x = GET_X_LPARAM (lParam);
				int y = GET_Y_LPARAM (lParam);
				nodeEditor->OnMouseDoubleClick (WAS::GetModiferKeysFromEvent (wParam), NUIE::MouseButton::Right, x, y);
			}
			break;
		case WM_SIZE:
			{
				int newWidth = LOWORD (lParam);
				int newHeight = HIWORD (lParam);
				nodeEditor->OnResize (newWidth, newHeight);
			}
			break;
		case WM_KEYDOWN:
			{
				NUIE::Key pressedKey (NUIE::PressedKeyCode::Undefined);
				bool isControlPressed = (GetKeyState (VK_CONTROL) < 0);
				if (isControlPressed) {
					switch (wParam) {
						case 'A':
							pressedKey = NUIE::PressedKeyCode::SelectAll;
							break;
						case 'C':
							pressedKey = NUIE::PressedKeyCode::Copy;
							break;
						case 'V':
							pressedKey = NUIE::PressedKeyCode::Paste;
							break;
						case 'Z':
							pressedKey = NUIE::PressedKeyCode::Undo;
							break;
						case 'Y':
							pressedKey = NUIE::PressedKeyCode::Redo;
							break;
					}
				} else {
					switch (wParam) {
						case VK_ESCAPE:
							pressedKey = NUIE::PressedKeyCode::Escape;
							break;
						case VK_DELETE:
							pressedKey = NUIE::PressedKeyCode::Delete;
							break;
					}
				}
				if (pressedKey.IsValid ()) {
					nodeEditor->OnKeyPress (pressedKey);
				}
			}
			break;
	}

	return DefWindowProc (hwnd, msg, wParam, lParam);
}

NodeEditorHwndControl::NodeEditorHwndControl () :
	nodeEditor (nullptr),
	hwnd (NULL)
{

}

NodeEditorHwndControl::~NodeEditorHwndControl ()
{

}

bool NodeEditorHwndControl::Init (NUIE::NodeEditor* nodeEditorPtr, HWND parentHandle, int x, int y, int width, int height)
{
	nodeEditor = nodeEditorPtr;
	DBGASSERT (nodeEditor != nullptr);

	hwnd = CreateCustomControl (parentHandle, StaticWindowProc, L"NodeEditorHwndControl", this);
	if (DBGERROR (hwnd == NULL)) {
		return false;
	}

	bitmapContext.Init (hwnd);

	ShowWindow (hwnd, SW_SHOW);
	UpdateWindow (hwnd);
	MoveWindow (hwnd, x, y, width, height, TRUE);

	return true;
}

NUIE::NodeEditor* NodeEditorHwndControl::GetNodeEditor ()
{
	return nodeEditor;
}

NUIE::DrawingContext& NodeEditorHwndControl::GetDrawingContext ()
{
	return bitmapContext;
}

HWND NodeEditorHwndControl::GetWindowHandle ()
{
	return hwnd;
}

void NodeEditorHwndControl::Resize (int x, int y, int width, int height)
{
	MoveWindow (hwnd, x, y, width, height, TRUE);
	if (nodeEditor != nullptr) {
		nodeEditor->OnResize (width, height);
	}
}

void NodeEditorHwndControl::Invalidate ()
{
	InvalidateRect (hwnd, NULL, FALSE);
}

void NodeEditorHwndControl::Draw ()
{
	if (nodeEditor != nullptr) {
		nodeEditor->Draw ();
	}
	bitmapContext.BlitToWindow (hwnd);
}

}
