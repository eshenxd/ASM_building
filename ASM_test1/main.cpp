#include "header.h"
#include "getFiles.h"
#include "vjfacedetect.h"
#include "asmlibrary.h"
#include "asmbuilding.h"

using namespace std;


int main(int argc, char* argv[])
{
	char** imgFiles = NULL;
	char** ptsFiles = NULL;

	int interpolate = 0;
	int width = 8;
	int level = 3;

	double percentage = 0.975;

	ASM_PROFILE_TYPE type = PROFILE_1D;
	
	string img_ext("../images");
	string pts_ext("../points");

	char cascade_name[256] = "face.xml";
	char model[256] = "point.amf";

	/************************************************************************/
	/* Step 0: init face detector                                           */
	/************************************************************************/

	if (init_detect_cascade(cascade_name) == false)
		return -1;

	/************************************************************************/
	/* Step 1: Load image and shape data                                    */
	/************************************************************************/

	vector<string>imagelist;
	vector<string>ptslist;

	getFiles(img_ext, imagelist);
	getFiles(pts_ext, ptslist);

	int n_images = imagelist.size();
	int n_shapes = ptslist.size();

	imgFiles = new char*[n_images];
	ptsFiles = new char*[n_shapes];
	for (int i = 0; i < n_images; i++)
		imgFiles[i] = (char*)imagelist[i].c_str();
	for (int i = 0; i < n_shapes; i++)
		ptsFiles[i] = (char*)ptslist[i].c_str();

	asmbuilding buildASM;

	/************************************************************************/
	/* Step 2: Build an ASMModel                                            */
	/************************************************************************/

	if (buildASM.Train((const char**)imgFiles, n_images, (const char**)ptsFiles, n_shapes,
		interpolate, width, percentage, level, type) == false)
		return -1;

	/************************************************************************/
	/* Step 3: Generate mapping between face-box and shape-groundtruth      */
	/************************************************************************/

	buildASM.BuildDetectMapping((const char**)imgFiles, n_images,
		(const char**)ptsFiles, n_shapes, detect_one_face);

	/************************************************************************/
	/* Step 4: Write model to file                                          */
	/************************************************************************/

	if (buildASM.Write(model) == false)
		return -1;

	destory_detect_cascade();

	delete[] ptsFiles;
	delete[] imgFiles;

	return 0;
}