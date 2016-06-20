#include "Document.h"
#include "Memtrack.h"

void Document::constructDom(std::string layoutfile)
{
	std::string path = "layout/";
	path.append(layoutfile);

	std::string csspath = path;
	std::string xmlpath = path;

	csspath.append(".css");
	xmlpath.append(".xml");
	const char * c = csspath.c_str();

	doc.load_file(xmlpath.c_str());

	//doc.traverse(walker);
}

void Document::render()
{
	doc.traverse(walker);
	
}

void Document::renderButtonText(ID3D11DeviceContext *devcon)
{
	button_walker butn;
	butn.devcon = devcon;
	doc.traverse(butn);

}
