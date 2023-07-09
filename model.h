#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <vector>
#include "geometry.h"
#include <string>

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<Vec3f> uvs_;
	std::vector<std::vector<int> > faces_;
	std::vector<std::vector<int> > faceuv_;
public:
	Model(const std::string &);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	Vec3f uvs(int i);
	std::vector<int> faceuv(int idx);
	std::vector<int> face(int idx);
};


#endif // MODEL_H_INCLUDED
