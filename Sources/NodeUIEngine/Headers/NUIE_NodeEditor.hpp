#ifndef NODEEDITOR_HPP
#define NODEEDITOR_HPP

#include "NUIE_DrawingContext.hpp"
#include "NUIE_EventHandlers.hpp"
#include "NUIE_NodeUIManager.hpp"
#include "NUIE_NodeUIInteractionHandler.hpp"
#include "NUIE_NodeUIEnvironment.hpp"
#include "NUIE_SkinParams.hpp"

namespace NUIE
{

class NodeEditor
{
public:
	NodeEditor (NodeUIEnvironment& uiEnvironment);
	virtual ~NodeEditor ();

	void						OnMouseDown (const KeySet& pressedKeys, MouseButton button, int x, int y);
	void						OnMouseUp (const KeySet& pressedKeys, MouseButton button, int x, int y);
	void						OnMouseMove (const KeySet& pressedKeys, int x, int y);
	void						OnMouseWheel (const KeySet& pressedKeys, MouseWheelRotation rotation, int x, int y);
	void						OnMouseDoubleClick (const KeySet& pressedKeys, MouseButton button, int x, int y);
	void						OnResize (int newWidth, int newHeight);
	
	void						InvalidateAllNodesDrawing ();
	void						Update ();
	void						Draw ();

	NodeUIManager&				GetNodeUIManager ();
	
	void						Clear ();
	bool						Load (const std::wstring& fileName);
	bool						Save (const std::wstring& fileName) const;

protected:
	NodeUIManager				uiManager;
	NodeUIInteractionHandler	uiInteractionHandler;
	MouseEventTranslator		mouseEventTranslator;
	NodeUIEnvironment&			uiEnvironment;
};

}

#endif