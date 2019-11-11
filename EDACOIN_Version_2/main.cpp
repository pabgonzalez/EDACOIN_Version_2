#include <boost/filesystem.hpp>
#include <iostream>
#include <vector>
#include "BlockchainModel.h"
#include "BlockViewerController.h"
#include "JsonController.h"
#include "NodeController.h"
#include "ImGuiViewer.h"
#include "ImGuiObserverManager.h"
#include "Node.h"
#include <cstdlib>
#include <ctime>
#include <time.h>

using namespace std;
using namespace boost::filesystem;

int main() {
	srand(time(NULL));
	ImGuiObserverManager obsManager;
	if (obsManager.getError() == false)
	{
		BlockchainModel model;
		BlockViewerController blockViewerControl(model);
		JsonController jsonControl(model);
		ImGuiViewer view;
		model.attach(view);
		model.attach(blockViewerControl);
		model.attach(jsonControl);

		NodeController nodeControl;
		FullNode full1({ "127.0.0.1", 10000 }, "Jorge");
		FullNode full2({ "127.0.0.1", 10001 }, "Ricardo");
		full1.appendNeighbourNode(full2);
		full2.appendNeighbourNode(full1);
		nodeControl.addNode(&full1);
		nodeControl.addNode(&full2);

		obsManager.addObserver(&blockViewerControl);
		obsManager.addObserver(&view);
		obsManager.addObserver(&nodeControl);

		while (obsManager.getExit() == false) {
			obsManager.cycle();
			jsonControl.cycle();
		}
	}
	else
		char c = getchar();
}

//int main()
//{
//	SocketType s = {"IP", 22};
//	Node n(s, "myNode");
//	MerkleNode n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,n11,n12,n13;
//	n1.level = n2.level = n3.level = n4.level = n5.level = n6.level = 3;
//	n7.level = n8.level = n9.level = n10.level = 2;
//	n11.level = n12.level = 1;
//	n13.level = 0;
//	n1.newIDstr = "a";
//	n2.newIDstr = "b";
//	n3.newIDstr = "c";
//	n4.newIDstr = "d";
//	n5.newIDstr = "e";
//	n6.newIDstr = "f";
//	n7.newIDstr = "g";
//	n8.newIDstr = "h";
//	n9.newIDstr = "i";
//	n10.newIDstr = "j";
//	n11.newIDstr = "k";
//	n12.newIDstr = "l";
//	n13.newIDstr = "m";
//	vector<MerkleNode> v = { n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,n11,n12,n13 };
//	vector<string> out = n.recursiveMerkleBlock(v, 2);
//	for (int i = 0; i < out.size(); i++)
//	{
//		cout << out[i] << endl;
//	}
//}
