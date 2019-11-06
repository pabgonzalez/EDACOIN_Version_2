#include <boost/filesystem.hpp>
#include <iostream>
#include <vector>
#include "BlockchainModel.h"
#include "BlockViewerController.h"
#include "JsonController.h"
#include "ImGuiViewer.h"
#include "ImGuiObserverManager.h"
#include "Node.h"
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace boost::filesystem;

//int main() {
//	ImGuiObserverManager obsManager;
//	if (obsManager.getError() == false)
//	{
//		BlockchainModel model;
//		BlockViewerController blockViewerControl(model);
//		JsonController jsonControl(model);
//		ImGuiViewer view;
//		model.attach(view);
//		model.attach(blockViewerControl);
//		model.attach(jsonControl);
//		obsManager.addObserver(&blockViewerControl);
//		obsManager.addObserver(&view);
//
//		while (obsManager.getExit() == false) {
//			obsManager.cycle();
//			jsonControl.cycle();
//		}
//	}
//	else
//		char c = getchar();
//}

int main()
{
	SocketType s = {"IP", 22};
	Node n(s, "myNode");
	n.generateBlockHeader("bID");
}
