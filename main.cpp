#include <iostream>
#include "tgaimage.h"
#include <algorithm>
#include <time.h>
#include "model.h"
#include <vector>
#include <memory>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green   = TGAColor(0, 255,   0,   255);
const TGAColor blue   = TGAColor(0x66, 0xcc,   0xff,   255);

void line_v1(int x0, int y0, int x1, int y1, TGAImage &image) {
    int yLen = std::abs(y1 - y0);
    int xLen = std::abs(x1 - x0);
    bool steep = false;
    if (xLen < yLen) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    for(int x = x0; x <= x1; x++) {
        float k = (x - x0) / (float)(x1 - x0);
        int y = y0 * (1 - k) + y1 * k + 0.5;
        if (steep) {
            image.set(y, x, white);
        }else {
            image.set(x, y, white);
        }
    }
}

// quick version
void line_v2(int x0, int y0, int x1, int y1, TGAImage &image) {
    int yLen = std::abs(y1 - y0);
    int xLen = std::abs(x1 - x0);
    bool steep = false;
    if (xLen < yLen) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    float derror = 0;
    float dy = (y1 - y0) / (float)(x1 - x0);
    int y = y0;
    for(int x = x0; x <= x1; x++) {
        if (steep) {
            image.set(y, x, white);
        }else {
            image.set(x, y, white);
        }
        derror += dy;
        int dd = std::abs(derror) > 0.5 ? 1 : 0;
        dd = derror > 0 ? dd : -dd;
        y += dd;
        derror -= dd;
    }
}

// quick version
void line_v3(int x0, int y0, int x1, int y1, TGAImage &image) {
    int yLen = std::abs(y1 - y0);
    int xLen = std::abs(x1 - x0);
    bool steep = false;
    if (xLen < yLen) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int derror = 0;
    int dy = std::abs(y1 - y0);
    int dx = (x1 - x0);
    int y = y0;
    for(int x = x0; x <= x1; x++) {
        if (steep) {
            image.set(y, x, white);
        }else {
            image.set(x, y, white);
        }
        derror += dy;
        if (derror > dx) {
            y += y1 - y0 > 0 ? 1 : -1;
            derror -= dx;
        }
    }
}

// quick version
void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color) {
    int x0 = t0.x;
    int y0 = t0.y;
    int x1 = t1.x;
    int y1 = t1.y;
    int yLen = std::abs(y1 - y0);
    int xLen = std::abs(x1 - x0);
    bool steep = false;
    if (xLen < yLen) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int derror = 0;
    int dy = std::abs(y1 - y0)*2;
    int dx = x1 - x0;
    int y = y0;
    for(int x = x0; x <= x1; x++) {
        if (steep) {
            image.set(y, x, color);
        }else {
            image.set(x, y, color);
        }
        derror += dy;
        if (derror > dx) {
            y += y1 - y0 > 0 ? 1 : -1;
            derror -= dx*2;
        }
    }
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    line(t0, t1, image, color);
    line(t1, t2, image, color);
    line(t2, t0, image, color);
}

void drawFrame() {
    int lenght = 600;
	TGAImage image(lenght, lenght, TGAImage::RGB);
	Model *m = new Model("african_head.obj");
    for(int i=0; i < m->nfaces(); i++) {
        std::vector<int> faces = m->face(i);
        for(int j=0; j < faces.size(); j++){
            Vec3f start = m->vert(faces[j]);
            Vec3f end = m->vert(faces[(j+1)%faces.size()]);
//            std::cout<< m->vert(start).x<< std::endl;
            int x0 = (start.x + 1) / 2 * lenght;
            int y0 = (start.y + 1) / 2 * lenght;
            int x1 = (end.x + 1) / 2 * lenght;
            int y1 = (end.y + 1) / 2 * lenght;
//            std::cout<< x0 << " "<< y0 << " " << x1 << " " << y1 <<std::endl;
            Vec2i t0(x0, y0), t1(x1, y1);
            line(t0, t1, image, blue);
        }
    }
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output_frame.tga");
}

// 下平底三角形 std
void fillButtomFlatTriangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    if (t1.x > t2.x) {
        std::swap(t1, t2);
    }
    float invslope1 = (t0.x - t1.x + 0.0) / (t0.y - t1.y);
    float invslope2 = (t0.x - t2.x + 0.0) / (t0.y - t2.y);
    float curx1 = t1.x;
    float curx2 = t2.x;
    int xa = t1.x, xb = t2.x;
    for(int i = t1.y; i <= t0.y; i++){
        for(int j = xa; j <= xb; j++){
            image.set(j, i, color);
        }
        curx1 += invslope1;
        curx2 += invslope2;
        xa = curx1;
        xb = curx2;
    }
}

// 上平底三角形 std
void fillTopFlatTriangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    if (t1.x > t2.x) {
        std::swap(t1, t2);
    }
    float invslope1 = (t1.x - t0.x + 0.0) / (t1.y - t0.y);
    float invslope2 = (t2.x - t0.x + 0.0) / (t2.y - t0.y);
    float curx1 = t1.x;
    float curx2 = t2.x;
    int xa = t1.x, xb = t2.x;
    for(int i = t1.y; i >= t0.y; i--){
        for(int j = xa; j <= xb; j++){
            image.set(j, i, color);
        }
        curx1 -= invslope1;
        curx2 -= invslope2;
        xa = curx1;
        xb = curx2;
    }
}

// 平底三角形 v1 std
void fillFlatTriangle_v1(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color){
    int dy = t1.y - t0.y;
    int miny = t1.y;
    int maxy = t0.y;
    if(miny > maxy){
        std::swap(miny, maxy);
    }
    for(int i = miny; i <= maxy; i++){
        int xa = ((t1.x + 0.5) * (i - t0.y) / dy + (t0.x + 0.5) * (t1.y - i) / dy);
        int xb = ((t2.x + 0.5) * (i - t0.y) / dy + (t0.x + 0.5) * (t2.y - i) / dy);
        if(xa > xb){
            std::swap(xa, xb);
        }
        for(int j = xa; j <= xb; j++){
            image.set(j, i, color);
        }
    }
}


// 下平底三角形 bresenham
void bresenhamButtomFlatTriangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color){
    if(t1.x > t2.x) {
        std::swap(t1, t2);
    }
    int x0 = t0.x;
    int y0 = t0.y;
    int x1 = t1.x;
    int y1 = t1.y;
    int x2 = t2.x;
    int dya = std::abs(y1 - y0);
    int dxa = std::abs(x1 - x0);
    int dxb = std::abs(x2 - x0);
    int derror = 0;
    int dy = dya;
    int dx = dxa*2;
    int xa = x1;
    int derror1 = 0;
    int dy1 = dya;
    int dx1 = dxb*2;
    int xb = x2;
    for(int y = y1; y <= y0; y++) {
        for(int j = xa; j <= xb; j++){
            image.set(j, y, color);
        }
        derror += dx;
        while (derror > dy && dy != 0) {
            xa += x0 - x1 > 0 ? 1 : -1;
            derror -= dy*2;
        }
        derror1 += dx1;
        while (derror1 > dy1 && dy1 != 0) {
            xb += x0 - x2 > 0 ? 1: -1;
            derror1 -= dy1*2;
        }

    }
}

// 上平底三角形 bresenham
void bresenhamTopFlatTriangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color){
    if(t1.x > t2.x) {
        std::swap(t1, t2);
    }
    int x0 = t0.x;
    int y0 = t0.y;
    int x1 = t1.x;
    int y1 = t1.y;
    int x2 = t2.x;
    int dya = std::abs(y1 - y0);
    int dxa = std::abs(x1 - x0);
    int dxb = std::abs(x2 - x0);
    int derror = 0;
    int dy = dya;
    int dx = dxa*2;
    int xa = x1;
    int derror1 = 0;
    int dy1 = dya;
    int dx1 = dxb*2;
    int xb = x2;
    for(int y = y1; y >= y0; y--) {
        for(int j = xa; j <= xb; j++){
            image.set(j, y, color);
        }
        derror += dx;
        while (derror > dy && dy != 0) {
            xa += x0 - x1 > 0 ? 1 : -1;
            derror -= dy*2;
        }
        derror1 += dx1;
        while (derror1 > dy1 && dy1 != 0) {
            xb += x0 - x2 > 0 ? 1: -1;
            derror1 -= dy1*2;
        }
    }
}

// 平底三角形 分上下平底三角形处理
void fillFlatTriangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color){
    if(t0.y > t1.y){
//        fillButtomFlatTriangle(t0, t1, t2, image, color);
        bresenhamButtomFlatTriangle(t0, t1, t2, image, color);
//        fillFlatTriangle_v1(t0, t1, t2, image, color);
    }
    else {
//        fillTopFlatTriangle(t0, t1, t2, image, color);
        bresenhamTopFlatTriangle(t0, t1, t2, image, color);
//        fillFlatTriangle_v1(t0, t1, t2, image, color);
    }
}

// Bresenham
void fillTriangle_v2(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    if(t0.y > t1.y) {
        std::swap(t0, t1);
    }
    if(t1.y > t2.y) {
        std::swap(t1, t2);
    }
    if(t0.y > t1.y) {
        std::swap(t0, t1);
    }
    // 是平底三角形
    if (t0.y == t1.y || t1.y == t2.y){
        fillFlatTriangle(t0, t1, t2, image, color);
    }else{ // 不是平底三角形，分割成两个平底三角形
        int dx = t2.x - t0.x;
        int dy = t2.y - t0.y;
        int x3 = t2.x * (t1.y - t0.y + 0.0) / dy + t0.x * (t2.y - t1.y+0.0) / dy + 0.5;
        Vec2i t3(x3, t1.y);
        fillFlatTriangle(t0, t1, t3, image, color);
        fillFlatTriangle(t2, t1, t3, image, color);
    }
}

//三角形
void fillTriangle_v1(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    // 三个点按照y坐标从小到大排序
    if(t0.y > t1.y) {
        std::swap(t0, t1);
    }
    if(t1.y > t2.y) {
        std::swap(t1, t2);
    }
    if(t0.y > t1.y) {
        std::swap(t0, t1);
    }
    // 是平底三角形
    if (t0.y == t1.y){
        fillFlatTriangle(t2, t0, t1, image, color);
    }else if(t1.y == t2.y) {
        fillFlatTriangle(t0, t1, t2, image, color);
    }else if (t0.y == t2.y) {
         fillFlatTriangle(t1, t0, t2, image, color);
    }else{ // 不是平底三角形，分割成两个平底三角形
        int dx = t2.x - t0.x;
        int dy = t2.y - t0.y;
        int x3 = t2.x * (t1.y - t0.y + 0.0) / dy + t0.x * (t2.y - t1.y+0.0) / dy + 0.5;
        Vec2i t3(x3, t1.y);
        fillFlatTriangle(t0, t1, t3, image, color);
        fillFlatTriangle(t2, t1, t3, image, color);
    }
}

void drawTriangles_frame() {
    int lenght = 600;
    TGAImage image(lenght, lenght, TGAImage::RGB);
    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};
    fillTriangle_v1(t2[0], t2[1], t2[2], image, blue);
    fillTriangle_v1(t0[0], t0[1], t0[2], image, blue);
    fillTriangle_v1(t1[0], t1[1], t1[2], image, blue);
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
}

void drawFlat() {
    int lenght = 600;
	TGAImage image(lenght, lenght, TGAImage::RGB);
	Model *m = new Model("african_head.obj");
    for(int i=0; i < m->nfaces(); i++) {
        std::vector<int> faces = m->face(i);
        Vec2i t0[3];
        Vec3f world[3];
        for(int j=0; j < faces.size(); j++){
//            std::cout<< "faces.size==" << faces.size() <<  "  j==" << j << " m->nfaces()" <<  m->nfaces() << " i =" << i << std::endl;
            Vec3f start = m->vert(faces[j]);
            int x0 = (start.x + 1) / 2 * lenght;
            int y0 = (start.y + 1) / 2 * lenght;
            t0[j] = Vec2i(x0, y0); //, t1(x1, y1);
            world[j] = start;
        }
//        std::cout << t0[0].x << "-" << t0[0].y << "-"  << t0[1].x << "-"  << t0[1].y << "-"  << t0[2].x << "-"  << t0[2].y << std::endl;
        Vec3f n = (world[0]-world[1]) ^ (world[2] - world[0]);
        n.normalize();
        float inten = n * Vec3f(0.0, 0.0, -1.0);
        if (inten > 0) {
            fillTriangle_v1(t0[0], t0[1], t0[2], image, TGAColor(inten*255, inten*255, inten*255, 255));
        }
    }
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output_flat.tga");
}
std::unique_ptr<int> myfunction (std::unique_ptr<int> i) {  // function:
    std::cout << ' ' << *i << std::endl;
    return i;
}
int test(){
    std::vector<std::unique_ptr<int>> veci;
    veci.push_back(std::unique_ptr<int>(new int(1)));
    std::cout <<"size1:"<< veci.size() << std::endl;
    for_each(veci.begin(), veci.end(), myfunction);
    for(auto i = veci.begin(); i !=veci.end(); i++ ){
            if(*i && (*i).get()) {
                std::cout << ' ' << (**i);
            }else {
                std::cout << " - " << std::endl;
            }
    }
    std::cout<<"size2:" << veci.size() << std::endl;
}


int main(int argc, char** argv) {
    //drawFrame();
    //drawTriangles_frame();
    drawFlat();
    //drawFrame();
    test();
	return 0;
}
