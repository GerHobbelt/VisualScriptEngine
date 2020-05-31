#include "SimpleTest.hpp"
#include "NUIE_NodeEditor.hpp"
#include "BI_InputUINodes.hpp"
#include "BI_ViewerUINodes.hpp"
#include "VisualTestFramework.hpp"

using namespace NE;
using namespace NUIE;
using namespace BI;

namespace NodeEditorVisualTest
{

const BasicSkinParams& GetSkinParamsWithSlotCircles ()
{
	static const BasicSkinParams slotCirclesSkinParams (
		/*backgroundColor*/ NUIE::Color (250, 250, 250),
		/*connectionLinePen*/ NUIE::Pen (NUIE::Color (38, 50, 56), 1.0),
		/*nodePadding*/ 5.0,
		/*nodeBorderPen*/ NUIE::Pen (NUIE::Color (38, 50, 56), 1.0),
		/*nodeHeaderTextFont*/ NUIE::Font (L"Arial", 16.0),
		/*nodeHeaderTextColor*/ NUIE::Color (250, 250, 250),
		/*nodeHeaderErrorTextColor*/ NUIE::Color (250, 250, 250),
		/*nodeHeaderBackgroundColor*/ NUIE::Color (41, 127, 255),
		/*nodeHeaderErrorBackgroundColor*/ NUIE::Color (199, 80, 80),
		/*nodeContentTextFont*/ NUIE::Font (L"Arial", 14.0),
		/*nodeContentTextColor*/ NUIE::Color (0, 0, 0),
		/*nodeContentBackgroundColor*/ NUIE::Color (236, 236, 236),
		/*slotTextColor*/ NUIE::Color (0, 0, 0),
		/*slotTextBackgroundColor*/ NUIE::Color (246, 246, 246),
		/*needToDrawSlotCircles*/ true,
		/*slotCircleSize*/ NUIE::Size (8.0, 8.0),
		/*selectionBlendColor*/ NUIE::BlendColor (NUIE::Color (41, 127, 255), 0.25),
		/*disabledBlendColor*/ NUIE::BlendColor (NUIE::Color (0, 138, 184), 0.2),
		/*selectionRectPen*/ NUIE::Pen (NUIE::Color (41, 127, 255), 1.0),
		/*nodeSelectionRectPen*/ NUIE::Pen (NUIE::Color (41, 127, 255), 3.0),
		/*buttonBorderPen*/ NUIE::Pen (NUIE::Color (146, 152, 155), 1.0),
		/*buttonBackgroundColor*/ NUIE::Color (217, 217, 217),
		/*textPanelTextColor*/ NUIE::Color (0, 0, 0),
		/*textPanelBackgroundColor*/ NUIE::Color (236, 236, 236),
		/*groupNameFont*/ NUIE::Font (L"Arial", 16.0),
		/*groupNameColor*/ NUIE::Color (0, 0, 0),
		/*groupBackgroundColors*/ NUIE::NamedColorSet ({
			{ NE::LocalizeString (L"Blue"), NUIE::Color (160, 200, 240) },
			{ NE::LocalizeString (L"Green"), NUIE::Color (160, 239, 160) },
			{ NE::LocalizeString (L"Red"), NUIE::Color (239, 189, 160) }
		}),
		/*groupPadding*/ 10.0,
		/*mouseMoveMinOffset*/ 2.0
	);
	return slotCirclesSkinParams;
}

class SimpleNodeEditorTestEnv : public NodeEditorTestEnv
{
public:
	SimpleNodeEditorTestEnv (const BasicSkinParams& skinParams) :
		NodeEditorTestEnv (skinParams)
	{
		doubleUpDownNode.reset (new DoubleUpDownNode (String (L"Number"), NUIE::Point (100, 200), 20, 10));
		rangeInputNode.reset (new DoubleIncrementedNode (String (L"Range"), NUIE::Point (300, 400)));
		viewerUINode1.reset (new MultiLineViewerNode (String (L"Viewer"), NUIE::Point (600, 100), 5));
		viewerUINode2.reset (new MultiLineViewerNode (String (L"Viewer 2"), NUIE::Point (600, 400), 5));
		
		nodeEditor.AddNode (doubleUpDownNode);
		nodeEditor.AddNode (rangeInputNode);
		nodeEditor.AddNode (viewerUINode1);
		nodeEditor.AddNode (viewerUINode2); 

		nodeEditor.Update ();
		RecalcPositions ();
	}

	void RecalcPositions ()
	{
		pointInBackground = Point (5.0, 5.0);
		doubleInputRect = doubleUpDownNode->GetNodeRect (uiEnvironment);
		rangeInputRect = rangeInputNode->GetNodeRect (uiEnvironment);
		viewer1InputRect = viewerUINode1->GetNodeRect (uiEnvironment);
		viewer2InputRect = viewerUINode2->GetNodeRect (uiEnvironment);
		viewer1InputSlotRect = viewerUINode1->GetInputSlotRect (uiEnvironment, SlotId ("in"));
		viewer2InputSlotRect = viewerUINode2->GetInputSlotRect (uiEnvironment, SlotId ("in"));
		doubleUpDownOutputSlotRect = doubleUpDownNode->GetOutputSlotRect (uiEnvironment, SlotId ("out"));
		rangeOutputSlotSRect = rangeInputNode->GetOutputSlotRect (uiEnvironment, SlotId ("out"));
		doubleInputHeaderPoint = doubleInputRect.GetTopCenter () + Point (5.0, 5.0);
		rangeInputHeaderPoint = rangeInputRect.GetTopCenter () + Point (5.0, 5.0);
	}

	UINodePtr	doubleUpDownNode;
	UINodePtr	rangeInputNode;
	UINodePtr	viewerUINode1;
	UINodePtr	viewerUINode2;

	Point		pointInBackground;
	Rect		doubleInputRect;
	Rect		rangeInputRect;
	Rect		viewer1InputRect;
	Rect		viewer2InputRect;
	Rect		viewer1InputSlotRect;
	Rect		viewer2InputSlotRect;
	Rect		doubleUpDownOutputSlotRect;
	Rect		rangeOutputSlotSRect;

	Point		doubleInputHeaderPoint;
	Point		rangeInputHeaderPoint;
};

class SimpleNodeEditorTestEnvWithConnections : public SimpleNodeEditorTestEnv
{
public:
	SimpleNodeEditorTestEnvWithConnections (const BasicSkinParams& skinParams) :
		SimpleNodeEditorTestEnv (skinParams)
	{
		nodeEditor.ConnectOutputSlotToInputSlot (doubleUpDownNode->GetUIOutputSlot (SlotId ("out")), rangeInputNode->GetUIInputSlot (SlotId ("start")));
		nodeEditor.ConnectOutputSlotToInputSlot (doubleUpDownNode->GetUIOutputSlot (SlotId ("out")), rangeInputNode->GetUIInputSlot (SlotId ("step")));
		nodeEditor.ConnectOutputSlotToInputSlot (doubleUpDownNode->GetUIOutputSlot (SlotId ("out")), viewerUINode1->GetUIInputSlot (SlotId ("in")));
		nodeEditor.ConnectOutputSlotToInputSlot (rangeInputNode->GetUIOutputSlot (SlotId ("out")), viewerUINode2->GetUIInputSlot (SlotId ("in")));

		nodeEditor.Update ();
		RecalcPositions ();
	}
};

TEST (SelectionTest)
{
	SimpleNodeEditorTestEnv env (GetDefaultSkinParams ());
	
	{ // deselect all
		env.Click (env.pointInBackground);
		ASSERT (env.CheckReference ("Selection_NoSelection.svg"));
	}

	{ // select one node by clicking on the header
		Point doubleInputHeaderPoint = env.doubleInputRect.GetTopCenter () + Point (5.0, 5.0);
		env.Click (doubleInputHeaderPoint);
		ASSERT (env.CheckReference ("Selection_DoubleNodeSelected.svg"));
	}

	{ // deselect all nodes by clicking on the empty are
		env.Click (Point (5.0, 5.0));
		ASSERT (env.CheckReference ("Selection_NoSelection.svg"));
	}
	
	{ // select node with selection rect
		Point rectSelectStart = env.rangeInputRect.GetTopLeft () - Point (10.0, 10.0);
		Point rectSelectEnd (env.viewer2InputRect.GetCenter ().GetX (), env.rangeInputRect.GetBottom () + 10.0);
		env.DragDrop (rectSelectStart, rectSelectEnd, [&] () {
			ASSERT (env.CheckReference ("Selection_SelectionRect.svg"));
		});
		ASSERT (env.CheckReference ("Selection_RangeNodeSelected.svg"));
	}

	{ // deselect all
		env.Click (env.pointInBackground);
		ASSERT (env.CheckReference ("Selection_NoSelection.svg"));
	}

	{ // select two nodes with selection rect
		Point rectSelectStart = env.rangeInputRect.GetTopLeft () - Point (10.0, 10.0);
		Point rectSelectEnd (env.viewer2InputRect.GetRight () + 10.0, env.rangeInputRect.GetBottom () + 10.0);
		env.DragDrop (rectSelectStart, rectSelectEnd, [&] () {
			ASSERT (env.CheckReference ("Selection_SelectionRect2.svg"));
		});
		ASSERT (env.CheckReference ("Selection_RangeAndViewer2Selected.svg"));
	}

	{ // select another node by clicking on the header
		Point doubleInputHeaderPoint = env.doubleInputRect.GetTopCenter () + Point (5.0, 5.0);
		env.Click (doubleInputHeaderPoint);
		ASSERT (env.CheckReference ("Selection_DoubleNodeSelected.svg"));
	}

	{ // append another node to selection
		Point viewer1InputHeaderPoint = env.viewer1InputRect.GetTopCenter () + Point (5.0, 5.0);
		env.CtrlClick (viewer1InputHeaderPoint);
		ASSERT (env.CheckReference ("Selection_DoubleAndViewer1Selected.svg"));
	}

	{ // remove the other node from selection
		Point viewer1InputHeaderPoint = env.viewer1InputRect.GetTopCenter () + Point (5.0, 5.0);
		env.CtrlClick (viewer1InputHeaderPoint);
		ASSERT (env.CheckReference ("Selection_DoubleNodeSelected.svg"));
	}

	{ // deselect all
		env.Click (env.pointInBackground);
		ASSERT (env.CheckReference ("Selection_NoSelection.svg"));
	}

	{ // select two nodes with selection rect
		Point rectSelectStart = env.rangeInputRect.GetTopLeft () - Point (10.0, 10.0);
		Point rectSelectEnd (env.viewer2InputRect.GetRight () + 10.0, env.rangeInputRect.GetBottom () + 10.0);
		env.DragDrop (rectSelectStart, rectSelectEnd);
		ASSERT (env.CheckReference ("Selection_RangeAndViewer2Selected.svg"));
	}

	{ // select nodes with selection rect
		Point rectSelectStart = env.doubleInputRect.GetTopLeft () - Point (10.0, 10.0);
		Point rectSelectEnd = env.doubleInputRect.GetBottomRight () + Point (10.0, 10.0);
		env.nodeEditor.OnMouseDown (ModifierKeys ({ ModifierKeyCode::Control }), MouseButton::Left, (int) rectSelectStart.GetX (), (int) rectSelectStart.GetY ());
		env.nodeEditor.OnMouseMove (ModifierKeys ({ ModifierKeyCode::Control }), (int) rectSelectEnd.GetX (), (int) rectSelectEnd.GetY ());
		env.nodeEditor.OnMouseUp (ModifierKeys ({ ModifierKeyCode::Control }), MouseButton::Left, (int) rectSelectEnd.GetX (), (int) rectSelectEnd.GetY ());
		ASSERT (env.CheckReference ("Selection_DoubleRangeAndViewer2Selected.svg"));
	}
}

TEST (CreateGroupTest)
{
	SimpleNodeEditorTestEnv env (GetDefaultSkinParams ());

	{ // select two nodes with selection rect
		Point rectSelectStart = env.rangeInputRect.GetTopLeft () - Point (10.0, 10.0);
		Point rectSelectEnd (env.viewer2InputRect.GetRight () + 10.0, env.rangeInputRect.GetBottom () + 10.0);
		env.DragDrop (rectSelectStart, rectSelectEnd);
		ASSERT (env.CheckReference ("CreateGroup_NodesSelected.svg"));
	}

	{ // create group
		env.KeyPress (Key (KeyCode::Group));
		ASSERT (env.CheckReference ("CreateGroup_GroupCreated.svg"));
	}
}

TEST (MoveNodesTest)
{
	SimpleNodeEditorTestEnv env (GetDefaultSkinParams ());
	env.nodeEditor.ConnectOutputSlotToInputSlot (env.doubleUpDownNode->GetUIOutputSlot (SlotId ("out")), env.rangeInputNode->GetUIInputSlot (SlotId ("start")));

	{ // select nodes with selection rect
		Point rectSelectStart = env.doubleInputRect.GetTopLeft () - Point (10.0, 10.0);
		Point rectSelectEnd = env.rangeInputRect.GetBottomRight () + Point (10.0, 10.0);
		env.DragDrop (rectSelectStart, rectSelectEnd);
		ASSERT (env.CheckReference ("Move_DoubleAndRangeSelected.svg"));
	}

	{ // move the three nodes together
		Point targetPoint = env.rangeInputHeaderPoint + Point (50.0, 40.0);
		env.DragDrop (env.rangeInputHeaderPoint, targetPoint);
		ASSERT (env.CheckReference ("Move_DoubleAndRangeMoved.svg"));
	}
}

TEST (MoveCopyNodesTest)
{
	SimpleNodeEditorTestEnv env (GetDefaultSkinParams ());
	env.nodeEditor.ConnectOutputSlotToInputSlot (env.doubleUpDownNode->GetUIOutputSlot (SlotId ("out")), env.rangeInputNode->GetUIInputSlot (SlotId ("start")));

	{ // select nodes with selection rect
		Point rectSelectStart = env.doubleInputRect.GetTopLeft () - Point (10.0, 10.0);
		Point rectSelectEnd = env.rangeInputRect.GetBottomRight () + Point (10.0, 10.0);
		env.DragDrop (rectSelectStart, rectSelectEnd);
		ASSERT (env.CheckReference ("MoveCopy_DoubleAndRangeSelected.svg"));
	}

	{ // duplicate the three nodes together
		Point targetPoint = env.rangeInputHeaderPoint + Point (50.0, -60.0);
		env.DragDrop (ModifierKeys ({ ModifierKeyCode::Control }), env.rangeInputHeaderPoint, targetPoint, [&] () {
			ASSERT (env.CheckReference ("MoveCopy_DoubleAndRangeDuringDuplicate.svg"));
		});
		ASSERT (env.CheckReference ("MoveCopy_DoubleAndRangeDuplicated.svg"));
	}
}

TEST (SlotConnectionTest)
{
	SimpleNodeEditorTestEnv env (GetDefaultSkinParams ());

	Point doubleOutputSlotPosition = env.doubleUpDownNode->GetOutputSlotRect (env.uiEnvironment, SlotId ("out")).GetCenter ();
	Point rangeOutputSlotPosition = env.rangeInputNode->GetOutputSlotRect (env.uiEnvironment, SlotId ("out")).GetCenter ();
	Rect viewer1InputSlotRect = env.viewerUINode1->GetInputSlotRect (env.uiEnvironment, SlotId ("in"));
	Point viewer1InputSlotPosition = viewer1InputSlotRect.GetCenter ();
	Point viewer2InputSlotPosition = env.viewerUINode2->GetInputSlotRect (env.uiEnvironment, SlotId ("in")).GetCenter ();
	Point rangeStartInputSlotPosition = env.rangeInputNode->GetInputSlotRect (env.uiEnvironment, SlotId ("start")).GetCenter ();

	ASSERT (env.CheckReference ("SlotConnection_Basic.svg"));

	{ // start connecting double output slot without target
		Point targetPosition = doubleOutputSlotPosition + Point (200.0, -100.0);
		env.DragDrop (doubleOutputSlotPosition, targetPosition, [&] () {
			ASSERT (env.CheckReference ("SlotConnection_DraggingDoubleOutput.svg"));
		});
		ASSERT (env.CheckReference ("SlotConnection_Basic.svg"));
	}

	{ // start connecting viewer1 input slot without target
		Point targetPosition = viewer1InputSlotPosition - Point (200.0, -100.0);
		env.DragDrop (viewer1InputSlotPosition, targetPosition, [&] () {
			ASSERT (env.CheckReference ("SlotConnection_DraggingViewer1Input.svg"));
		});
		ASSERT (env.CheckReference ("SlotConnection_Basic.svg"));
	}

	{ // connect double output slot to viewer1 input slot
		Point targetPos = viewer1InputSlotRect.GetLeftCenter () - Point (5.0, 0.0);
		env.DragDrop (doubleOutputSlotPosition, targetPos, [&] () {
			ASSERT (env.CheckReference ("SlotConnection_ConnectingDoubleToViewer1.svg"));
		});
		ASSERT (env.CheckReference ("SlotConnection_DoubleConnectedToViewer1.svg"));
	}

	{ // start connecting double output slot without target again
		Point targetPosition = doubleOutputSlotPosition + Point (200.0, -100.0);
		env.DragDrop (doubleOutputSlotPosition, targetPosition, [&] () {
			ASSERT (env.CheckReference ("SlotConnection_DraggingConnectedViewer1Input.svg"));
		});
		ASSERT (env.CheckReference ("SlotConnection_DoubleConnectedToViewer1.svg"));
	}

	{ // connect viewer2 input slot to range output slot
		env.DragDrop (viewer2InputSlotPosition, rangeOutputSlotPosition, [&] () {
			ASSERT (env.CheckReference ("SlotConnection_ConnectingViewer2ToRange.svg"));
		});
		ASSERT (env.CheckReference ("SlotConnection_AllViewersConnected.svg"));
	}

	{ // connect double output slot to range start slot
		env.DragDrop (doubleOutputSlotPosition, rangeStartInputSlotPosition);
		ASSERT (env.CheckReference ("SlotConnection_AllConnected.svg"));
	}

	viewer2InputSlotPosition = env.viewerUINode2->GetInputSlotRect (env.uiEnvironment, SlotId ("in")).GetCenter ();
	{ // connect double output slot to viewer2 input slot
		env.DragDrop (doubleOutputSlotPosition, viewer2InputSlotPosition, [&] () {
			ASSERT (env.CheckReference ("SlotConnection_ConnectingDoubleToViewer2.svg"));
		});
		ASSERT (env.CheckReference ("SlotConnection_DoubleToViewer2Connected.svg"));
	}
}

TEST (PanAndZoomTest)
{
	SimpleNodeEditorTestEnv env (GetDefaultSkinParams ());
	ASSERT (env.CheckReference ("PanAndZoom_Basic.svg"));

	Point doubleOutputSlotPosition = env.doubleUpDownNode->GetOutputSlotRect (env.uiEnvironment, SlotId ("out")).GetCenter ();
	Point rangeStartInputSlotPosition = env.rangeInputNode->GetInputSlotRect (env.uiEnvironment, SlotId ("start")).GetCenter ();
	Point rangeOutputSlotPosition = env.rangeInputNode->GetOutputSlotRect (env.uiEnvironment, SlotId ("out")).GetCenter ();
	Point viewer2InputSlotPosition = env.viewerUINode2->GetInputSlotRect (env.uiEnvironment, SlotId ("in")).GetCenter ();
	{
		env.DragDrop (doubleOutputSlotPosition, rangeStartInputSlotPosition);	
		env.DragDrop (rangeOutputSlotPosition, viewer2InputSlotPosition);
		ASSERT (env.CheckReference ("PanAndZoom_Connections.svg"));
	}

	Point panStartPoint = env.rangeInputNode->GetNodeRect (env.uiEnvironment).GetTopLeft ();
	Point panEndPoint = panStartPoint + Point (50.0, -50.0);

	{
		env.nodeEditor.OnMouseDown (EmptyModifierKeys, MouseButton::Right, (int) panStartPoint.GetX (), (int) panStartPoint.GetY ());
		env.nodeEditor.OnMouseMove (EmptyModifierKeys, (int) panEndPoint.GetX (), (int) panEndPoint.GetY ());
		env.nodeEditor.OnMouseUp (EmptyModifierKeys, MouseButton::Right, (int) panEndPoint.GetX (), (int) panEndPoint.GetY ());
		ASSERT (env.CheckReference ("PanAndZoom_Panned.svg"));
	}

	for (size_t i = 0; i < 5; i++) {
		env.nodeEditor.OnMouseWheel (EmptyModifierKeys, MouseWheelRotation::Forward, (int) panEndPoint.GetX (), (int) panEndPoint.GetY ());
	}
	ASSERT (env.CheckReference ("PanAndZoom_ZoomedIn.svg"));

	for (size_t i = 0; i < 10; i++) {
		env.nodeEditor.OnMouseWheel (EmptyModifierKeys, MouseWheelRotation::Forward, (int) panEndPoint.GetX (), (int) panEndPoint.GetY ());
	}
	ASSERT (env.CheckReference ("PanAndZoom_ZoomedIn2.svg"));

	for (size_t i = 0; i < 20; i++) {
		env.nodeEditor.OnMouseWheel (EmptyModifierKeys, MouseWheelRotation::Backward, (int) panEndPoint.GetX (), (int) panEndPoint.GetY ());
	}
	ASSERT (env.CheckReference ("PanAndZoom_ZoomedOut.svg"));

	for (size_t i = 0; i < 10; i++) {
		env.nodeEditor.OnMouseWheel (EmptyModifierKeys, MouseWheelRotation::Backward, (int) panEndPoint.GetX (), (int) panEndPoint.GetY ());
	}
	ASSERT (env.CheckReference ("PanAndZoom_ZoomedOut2.svg"));
}

TEST (DeleteNodeTest)
{
	SimpleNodeEditorTestEnvWithConnections env (GetDefaultSkinParams ());
	ASSERT (env.CheckReference ("Delete_Basic.svg"));
	env.SetNextCommandName (L"Delete Nodes");
	env.RightClick (env.rangeInputHeaderPoint);
	ASSERT (env.CheckReference ("Delete_RangeDeleted.svg"));
}

TEST (CopyPasteTest)
{
	SimpleNodeEditorTestEnvWithConnections env (GetDefaultSkinParams ());
	ASSERT (env.CheckReference ("CopyPaste_Basic.svg"));
	env.Click (env.rangeInputHeaderPoint);
	env.CtrlClick (env.doubleInputHeaderPoint);
	ASSERT (env.CheckReference ("CopyPaste_TwoNodesSelected.svg"));
	env.SetNextCommandName (L"Copy Nodes");
	env.RightClick (env.doubleInputHeaderPoint);
	Point targetPoint = env.doubleInputHeaderPoint + Point (120, 20);
	env.SetNextCommandName (L"Paste Nodes");
	env.RightClick (targetPoint);
	ASSERT (env.CheckReference ("CopyPaste_TwoNodesPasted.svg"));
}

TEST (NodeGroupingTest)
{
	SimpleNodeEditorTestEnvWithConnections env (GetDefaultSkinParams ());
	ASSERT (env.CheckReference ("NodeGrouping_Basic.svg"));

	{ // select nodes with selection rect
		Point rectSelectStart = env.doubleInputRect.GetTopLeft () - Point (10.0, 10.0);
		Point rectSelectEnd = env.rangeInputRect.GetBottomRight () + Point (10.0, 10.0);
		env.nodeEditor.OnMouseDown (NUIE::EmptyModifierKeys, MouseButton::Left, (int) rectSelectStart.GetX (), (int) rectSelectStart.GetY ());
		env.nodeEditor.OnMouseMove (NUIE::EmptyModifierKeys, (int) rectSelectEnd.GetX (), (int) rectSelectEnd.GetY ());
		env.nodeEditor.OnMouseUp (NUIE::EmptyModifierKeys, MouseButton::Left, (int) rectSelectEnd.GetX (), (int) rectSelectEnd.GetY ());
		ASSERT (env.CheckReference ("NodeGrouping_DoubleAndRangeSelected.svg"));
	}

	{ // group the selected nodes, and deselect all
		env.SetNextCommandName (L"Create New Group");
		env.RightClick (env.rangeInputHeaderPoint);
		env.Click (env.pointInBackground);
		ASSERT (env.CheckReference ("NodeGrouping_DoubleAndRangeGrouped.svg"));
	}

	{ // move the group
		env.DragDrop (env.doubleInputRect.GetTopRight () + Point (10.0, 0.0), env.doubleInputRect.GetTopRight () + Point (20.0, 10.0));
		ASSERT (env.CheckReference ("NodeGrouping_GroupMoved.svg"));
	}

	{ // move one node
		Point currentRangeInputHeaderPoint = env.rangeInputNode->GetNodeRect (env.uiEnvironment).GetTopCenter () + Point (5.0, 5.0);
		env.DragDrop (currentRangeInputHeaderPoint, currentRangeInputHeaderPoint + Point (10.0, 10.0), [&] () {
			ASSERT (env.CheckReference ("NodeGrouping_RangeInputMoving.svg"));
		});
		ASSERT (env.CheckReference ("NodeGrouping_RangeInputMoved.svg"));
	}
}

TEST (InputSlotReconnectionTest)
{
	SimpleNodeEditorTestEnvWithConnections env (GetDefaultSkinParams ());
	ASSERT (env.CheckReference ("InputSlotReconnection_Basic.svg"));

	env.DragDrop (ModifierKeys ({ ModifierKeyCode::Control }), env.viewer1InputSlotRect.GetCenter (), env.viewer2InputSlotRect.GetCenter (), [&] () {
		ASSERT (env.CheckReference ("InputSlotReconnection_DuringConnection.svg"));
	});

	ASSERT (env.CheckReference ("InputSlotReconnection_Reconnected.svg"));
	
	env.nodeEditor.Undo ();
	ASSERT (env.CheckReference ("InputSlotReconnection_AfterUndo.svg"));

	env.DragDrop (ModifierKeys ({ ModifierKeyCode::Control }), env.viewer1InputSlotRect.GetCenter (), Point (0.0, 0.0));
	ASSERT (env.CheckReference ("InputSlotReconnection_AfterDisconnect.svg"));
}

TEST (OutputSlotReconnectionTest)
{
	SimpleNodeEditorTestEnvWithConnections env (GetDefaultSkinParams ());
	ASSERT (env.CheckReference ("OutputSlotReconnection_Basic.svg"));

	env.DragDrop (ModifierKeys ({ ModifierKeyCode::Control }), env.rangeOutputSlotSRect.GetCenter (), env.doubleUpDownOutputSlotRect.GetCenter (), [&] () {
		ASSERT (env.CheckReference ("OutputSlotReconnection_DuringConnection.svg"));
	});

	ASSERT (env.CheckReference ("OutputSlotReconnection_Reconnected.svg"));

	env.nodeEditor.Undo ();
	ASSERT (env.CheckReference ("OutputSlotReconnection_AfterUndo.svg"));

	env.DragDrop (ModifierKeys ({ ModifierKeyCode::Control }), env.rangeOutputSlotSRect.GetCenter (), Point (0.0, 0.0));
	ASSERT (env.CheckReference ("OutputSlotReconnection_AfterDisconnect.svg"));
}

TEST (UndoTest)
{
	NodeEditorTestEnv env (GetDefaultSkinParams ());
	ASSERT (env.CheckReference ("UndoTest_Empty.svg"));

	env.SetNextCommandName (L"Create Number Node");
	env.RightClick (Point (100, 100));
	env.SetNextCommandName (L"Create Number Node");
	env.RightClick (Point (100, 300));
	env.SetNextCommandName (L"Create Addition Node");
	env.RightClick (Point (300, 200));
	env.SetNextCommandName (L"Create Viewer Node");
	env.RightClick (Point (600, 200));
	env.DragDrop (Point (120, 100), Point (240, 200));
	env.DragDrop (Point (120, 300), Point (240, 230));
	env.DragDrop (Point (300, 200), Point (540, 200));
	env.Click (Point (145, 135));
	env.DragDrop (Point (200, 100), Point (800, 400));
	env.DragDrop (Point (300, 160), Point (320, 180));
	env.SetNextCommandName (L"Create New Group");
	env.RightClick (Point (320, 180));
	env.Click (Point (320, 180));
	env.SetNextCommandName (L"Delete Nodes");
	env.RightClick (Point (320, 180));

	ASSERT (env.CheckReference ("UndoTest_Initial.svg"));

	for (int i = 1; i <= 11; i++) {
		env.nodeEditor.OnKeyPress (Key (KeyCode::Undo));
		std::string indexString = std::to_string (i);
		while (indexString.length () < 2) {
			indexString = "0" + indexString;
		}
		ASSERT (env.CheckReference ("UndoTest_Undo_" + indexString + ".svg"));
	}
	for (int i = 1; i <= 11; i++) {
		env.nodeEditor.OnKeyPress (Key (KeyCode::Redo));
		std::string indexString = std::to_string (i);
		while (indexString.length () < 2) {
			indexString = "0" + indexString;
		}
		ASSERT (env.CheckReference ("UndoTest_Redo_" + indexString + ".svg"));
	}
}

TEST (ManualUpdateTest)
{
	NodeEditorTestEnv env (GetDefaultSkinParams ());
	env.SetNextCommandName (L"Create Integer Node");
	env.RightClick (Point (100, 100));
	env.SetNextCommandName (L"Create Increase Node");
	env.RightClick (Point (300, 200));
	env.SetNextCommandName (L"Create Viewer Node");
	env.RightClick (Point (600, 300));
	env.DragDrop (Point (125, 100), Point (240, 220));
	env.DragDrop (Point (360, 220), Point (530, 300));
	ASSERT (env.CheckReference ("ManualUpdateTest_Init.svg"));

	env.Click (Point (145, 140));
	ASSERT (env.CheckReference ("ManualUpdateTest_AutoUpdateMode.svg"));
	env.nodeEditor.SetUpdateMode (NodeEditor::UpdateMode::Manual);

	env.Click (Point (145, 140));
	ASSERT (env.CheckReference ("ManualUpdateTest_ManualUpdateMode.svg"));
	
	env.nodeEditor.ManualUpdate ();
	ASSERT (env.CheckReference ("ManualUpdateTest_ManualUpdateMode_ForceUpdate.svg"));
}

TEST (ManualUpdateTestUndo)
{
	NodeEditorTestEnv env (GetDefaultSkinParams ());
	env.nodeEditor.SetUpdateMode (NodeEditor::UpdateMode::Manual);

	env.SetNextCommandName (L"Create Integer Node");
	env.RightClick (Point (100, 100));
	env.SetNextCommandName (L"Create Increase Node");
	env.RightClick (Point (300, 200));
	env.SetNextCommandName (L"Create Viewer Node");
	env.RightClick (Point (600, 300));
	env.DragDrop (Point (360, 220), Point (530, 300));
	env.DragDrop (Point (125, 100), Point (240, 220));
	ASSERT (env.CheckReference ("ManualUpdateTestUndo_Init.svg"));

	env.nodeEditor.ManualUpdate ();
	ASSERT (env.CheckReference ("ManualUpdateTestUndo_ManualUpdate.svg"));

	env.nodeEditor.Undo ();
	ASSERT (env.CheckReference ("ManualUpdateTestUndo_AfterUndo.svg"));

	env.nodeEditor.ManualUpdate ();
	ASSERT (env.CheckReference ("ManualUpdateTestUndo_ManualUpdateAfterUndo.svg"));
}

TEST (FitToWindowTest)
{
	SimpleNodeEditorTestEnvWithConnections env (GetDefaultSkinParams ());
	ASSERT (env.CheckReference ("FitToWindow_Basic.svg"));
	env.nodeEditor.FitToWindow ();
	ASSERT (env.CheckReference ("FitToWindow_Fit.svg"));
}

TEST (AlignToWindowTest)
{
	SimpleNodeEditorTestEnvWithConnections env (GetDefaultSkinParams ());
	ASSERT (env.CheckReference ("AlignToWindow_Basic.svg"));
	env.nodeEditor.AlignToWindow ();
	ASSERT (env.CheckReference ("AlignToWindow_Align.svg"));
}

TEST (AlignToWindowTestWithScale)
{
	SimpleNodeEditorTestEnvWithConnections env (GetDefaultSkinParams ());
	env.uiEnvironment.SetWindowScale (1.5);
	ASSERT (env.CheckReference ("AlignToWindowWithScale_Basic.svg"));
	env.nodeEditor.AlignToWindow ();
	ASSERT (env.CheckReference ("AlignToWindowWithScale_Align.svg"));
}

TEST (CenterToWindowTest)
{
	SimpleNodeEditorTestEnvWithConnections env (GetDefaultSkinParams ());
	ASSERT (env.CheckReference ("CenterToWindow_Basic.svg"));
	env.nodeEditor.CenterToWindow ();
	ASSERT (env.CheckReference ("CenterToWindow_Align.svg"));
}

TEST (CenterToWindowTestWithScale)
{
	SimpleNodeEditorTestEnvWithConnections env (GetDefaultSkinParams ());
	env.uiEnvironment.SetWindowScale (1.5);
	ASSERT (env.CheckReference ("CenterToWindowWithScale_Basic.svg"));
	env.nodeEditor.CenterToWindow ();
	ASSERT (env.CheckReference ("CenterToWindowWithScale_Align.svg"));
}

TEST (SlotCirclesTest)
{
	SimpleNodeEditorTestEnvWithConnections env (GetSkinParamsWithSlotCircles ());
	ASSERT (env.CheckReference ("SlotCircles.svg"));
}

TEST (SlotCirclesFitToWindowTest)
{
	SimpleNodeEditorTestEnvWithConnections env (GetSkinParamsWithSlotCircles ());
	env.nodeEditor.FitToWindow ();
	ASSERT (env.CheckReference ("SlotCircles_FitToWindow.svg"));
}

}
