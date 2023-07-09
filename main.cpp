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
void line(Vec2i& t0, Vec2i& t1, TGAImage &image, TGAColor color) {
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

void triangle(Vec2i& t0, Vec2i& t1, Vec2i& t2, TGAImage &image, TGAColor color) {
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
	image.write_tga_file("drawFrame.tga");
}

// 下平底三角形 std
void fillButtomFlatTriangle(Vec3i& t0, Vec3i& t1, Vec3i& t2, TGAImage &image, TGAColor color, int* zbuffer=nullptr) {
    if (t1.x > t2.x) {
        std::swap(t1, t2);
    }
    int dy = std::abs(t2.y - t0.y);
    float invslope1 = (t0.x - t1.x + 0.0) / (t0.y - t1.y);
    float invslope2 = (t0.x - t2.x + 0.0) / (t0.y - t2.y);
    float curx1 = t1.x;
    float curx2 = t2.x;
    int xa = t1.x, xb = t2.x;
    for(int y = t1.y; y <= t0.y; y++){
        for(int j = xa; j <= xb; j++){
            int dxx = std::abs(xb - xa);
            int z1 = (t1.z * (float)(t0.y - y) / dy + t0.z * (float)(y - t1.y) / dy);
            int z2 = (t2.z * (float)(t0.y - y) / dy + t0.z * (float)(y - t2.y) / dy);
            int z3 = (z2 * (float)(j - xa) / dy + z1 * (float)(xb - j) / dy);
            //if (zbuffer)
            //std::cout<< "2zbuffer=" << zbuffer[j*600+y] << " z3=" << z3 << std::endl;
            if(!zbuffer){
                image.set(j, y, color);
            }else if(zbuffer[j*800+y] < z3) {
                zbuffer[j*800+y] = z3;
                image.set(j, y, color);
            }
        }
        curx1 += invslope1;
        curx2 += invslope2;
        xa = curx1;
        xb = curx2;
    }
}

// 上平底三角形 std
void fillTopFlatTriangle(Vec3i& t0, Vec3i& t1, Vec3i& t2, TGAImage &image, TGAColor color, int* zbuffer) {
    if (t1.x > t2.x) {
        std::swap(t1, t2);
    }
    int dy = std::abs(t2.y - t0.y);
    float invslope1 = (t1.x - t0.x + 0.0) / (t1.y - t0.y);
    float invslope2 = (t2.x - t0.x + 0.0) / (t2.y - t0.y);
    float curx1 = t1.x;
    float curx2 = t2.x;
    int xa = t1.x, xb = t2.x;
    for(int y = t1.y; y >= t0.y; y--){
        for(int j = xa; j <= xb; j++){
            int dxx = std::abs(xb - xa);
            int z1 = (t1.z * (float)(y - t0.y) / dy + t0.z * (float)(t1.y - y) / dy);
            int z2 = (t2.z * (float)(y - t0.y) / dy + t0.z * (float)(t2.y - y) / dy);
            int z3 = (z2 * (float)(j - xa) / dy + z1 * (float)(xb - j) / dy);
            //if (zbuffer)
            //std::cout<< "2zbuffer=" << zbuffer[j*600+y] << " z3=" << z3 << std::endl;
            if(!zbuffer){
                image.set(j, y, color);
            }else if(zbuffer[j*800+y] < z3) {
                zbuffer[j*800+y] = z3;
                image.set(j, y, color);
            }
            image.set(j, y, color);
        }
        curx1 -= invslope1;
        curx2 -= invslope2;
        xa = curx1;
        xb = curx2;
    }
}

// 平底三角形 v1 std
void fillFlatTriangle_v1(Vec2i& t0, Vec2i& t1, Vec2i& t2, TGAImage &image, TGAColor color){
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
void bresenhamButtomFlatTriangle(Vec3i& t0, Vec3i& t1, Vec3i& t2, TGAImage &image, TGAColor color, int* zbuffer){
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
    int dza = std::abs(t1.z - t0.z);
    int dzb = std::abs(t2.z - t0.z);
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
            int dxx = std::abs(xb - xa);
            int z1 = (t1.z * (float)(t0.y - y) / dy + t0.z* (float)(y - t1.y) / dy);
            int z2 = (t2.z * (float)(t0.y - y) / dy + t0.z  * (float)(y - t2.y) / dy);
            int z3 = (z2 * (float)(j - xa) / dy + z1 * (float)(xb - j) / dy);
            //if (zbuffer)
            //std::cout<< "2zbuffer=" << zbuffer[j*600+y] << " z3=" << z3 << std::endl;
            if(!zbuffer){
                image.set(j, y, color);
            }else if(zbuffer[j*800+y] < z3) {
                zbuffer[j*800+y] = z3;
                image.set(j, y, color);
            }
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
void bresenhamTopFlatTriangle(Vec3i& t0, Vec3i& t1, Vec3i& t2, TGAImage &image, TGAColor color, int* zbuffer){
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

    int dza = std::abs(t1.z - t0.z);
    int dzb = std::abs(t2.z - t0.z);
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
            int dxx = std::abs(xb - xa);
            int z1 = (t1.z * (float)(y - t0.y) / dy + t0.z * (float)(t1.y - y) / dy);
            int z2 = (t2.z * (float)(y - t0.y) / dy + t0.z * (float)(t2.y - y) / dy);
            int z3 = (z2 * (float)(j - xa) / dy + z1 * (float)(xb - j) / dy);
            //if (zbuffer)
            //std::cout<< "2zbuffer=" << zbuffer[j*600+y] << " z3=" << z3 << std::endl;
            if(!zbuffer){
                image.set(j, y, color);
            }else if(zbuffer[j*800+y] < z3) {
                zbuffer[j*800+y] = z3;
                image.set(j, y, color);
            }
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
void fillFlatTriangle(Vec3i& t0, Vec3i& t1, Vec3i& t2, TGAImage &image, TGAColor color, int* zbuffer=nullptr){
    if(t0.y > t1.y){
//        fillButtomFlatTriangle(t0, t1, t2, image, color, zbuffer);
        bresenhamButtomFlatTriangle(t0, t1, t2, image, color, zbuffer);
//        fillFlatTriangle_v1(t0, t1, t2, image, color);
    }
    else {
//        fillTopFlatTriangle(t0, t1, t2, image, color, zbuffer);
        bresenhamTopFlatTriangle(t0, t1, t2, image, color, zbuffer);
//        fillFlatTriangle_v1(t0, t1, t2, image, color);
    }
}

// Bresenham
void fillTriangle_v2(Vec3i& t0, Vec3i& t1, Vec3i& t2, TGAImage &image, TGAColor color) {
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
        int z3 = t2.z * (t1.y - t0.y + 0.0) / dy + t0.z * (t2.y - t1.y+0.0) / dy + 0.5;
        Vec3i t3(x3, t1.y, t1.z);
        fillFlatTriangle(t0, t1, t3, image, color);
        fillFlatTriangle(t2, t1, t3, image, color);
    }
}

//三角形
void fillTriangle_v1(Vec3i& t0, Vec3i& t1, Vec3i& t2, TGAImage &image, TGAColor color, int* zbuffer = nullptr) {
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
        fillFlatTriangle(t2, t0, t1, image, color, zbuffer);
    }else if(t1.y == t2.y) {
        fillFlatTriangle(t0, t1, t2, image, color, zbuffer);
    }else if (t0.y == t2.y) {
         fillFlatTriangle(t1, t0, t2, image, color);
    }else{ // 不是平底三角形，分割成两个平底三角形
        int dx = t2.x - t0.x;
        int dy = t2.y - t0.y;
        int x3 = t2.x * (t1.y - t0.y + 0.0) / dy + t0.x * (t2.y - t1.y+0.0) / dy + 0.5;
        int z3 = t2.z * (t1.y - t0.y + 0.0) / dy + t0.z * (t2.y - t1.y+0.0) / dy + 0.5;
        Vec3i t3(x3, t1.y, z3);
        fillFlatTriangle(t0, t1, t3, image, color, zbuffer);
        fillFlatTriangle(t2, t1, t3, image, color, zbuffer);
    }
}

Vec3f barycentri1c(Vec3f* pts, Vec3f P) {
    Vec3f u = Vec3f(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x) ^ Vec3f(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y);
    /* `pts` and `P` has integer value as coordinates
       so `abs(u[2])` < 1 means `u[2]` is 0, that means
       triangle is degenerate, in this case return something with negative coordinates */
    if (std::abs(u.z) < 1) return Vec3f(-1, 1, 1);
    return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void drawTriangles_frame() {
    int lenght = 600;
    TGAImage image(lenght, lenght, TGAImage::RGB);
    Vec3i t0[3] = {Vec3i(10, 70, 1),   Vec3i(50, 160, 1),  Vec3i(70, 80, 1)};
    Vec3i t1[3] = {Vec3i(180, 50, 1),  Vec3i(150, 1, 1),   Vec3i(70, 180, 1)};
    Vec3i t2[3] = {Vec3i(180, 150, 1), Vec3i(120, 160, 1), Vec3i(130, 180, 1)};
    fillTriangle_v1(t2[0], t2[1], t2[2], image, blue);
    fillTriangle_v1(t0[0], t0[1], t0[2], image, blue);
    fillTriangle_v1(t1[0], t1[1], t1[2], image, blue);
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("drawTriangles_frame.tga");
}

void drawFlat() {
    int lenght = 600;
	TGAImage image(lenght, lenght, TGAImage::RGB);
	Model *m = new Model("african_head.obj");
    for(int i=0; i < m->nfaces(); i++) {
        std::vector<int> faces = m->face(i);
        Vec3i t0[3];
        Vec3f world[3];
        for(int j=0; j < faces.size(); j++){
//            std::cout<< "faces.size==" << faces.size() <<  "  j==" << j << " m->nfaces()" <<  m->nfaces() << " i =" << i << std::endl;
            Vec3f start = m->vert(faces[j]);
            int x0 = (start.x + 1) / 2 * lenght;
            int y0 = (start.y + 1) / 2 * lenght;
            int z0 = (start.z + 1) / 2 * lenght;
            t0[j] = Vec3i(x0, y0, z0); //, t1(x1, y1);
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
	image.write_tga_file("drawFlat.tga");
}

Vec3f barycentric(Vec3f* pts, Vec3f P) {
    Vec3f u = Vec3f(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x) ^ Vec3f(pts[2].y-pts[0].y, pts[1].y-pts[0].y, pts[0].y-P.y);
    if (std::abs(u.z) < 1) return Vec3f(-1,1,1);
    return Vec3f(1.f - (u.x + u.y) / u.z, u.x / u.z, u.y / u.z);
    float r = ((pts[0].y - pts[1].y) * P.x + (pts[1].x - pts[0].x) * P.y + pts[0].x * pts[1].y - pts[1].x * pts[0].y) /
        ((pts[0].y - pts[1].y) * pts[2].x + (pts[1].x - pts[0].x) * pts[2].y + pts[0].x * pts[1].y - pts[1].x * pts[0].y);
    float b = ((pts[0].y - pts[2].y) * P.x + (pts[2].x - pts[0].x) * P.y + pts[0].x * pts[2].y - pts[2].x * pts[0].y) /
        ((pts[0].y - pts[2].y) * pts[1].x + (pts[2].x - pts[0].x) * pts[1].y + pts[0].x * pts[2].y - pts[2].x * pts[0].y);
    return Vec3f(1.f - r - b, b, r);
}

Vec3f barycentric_ccj(Vec3f* pts, Vec3f P) {
    float r = ((pts[0].y - pts[1].y) * P.x + (pts[1].x - pts[0].x) * P.y + pts[0].x * pts[1].y - pts[1].x * pts[0].y) /
        ((pts[0].y - pts[1].y) * pts[2].x + (pts[1].x - pts[0].x) * pts[2].y + pts[0].x * pts[1].y - pts[1].x * pts[0].y);
    float b = ((pts[0].y - pts[2].y) * P.x + (pts[2].x - pts[0].x) * P.y + pts[0].x * pts[2].y - pts[2].x * pts[0].y) /
        ((pts[0].y - pts[2].y) * pts[1].x + (pts[2].x - pts[0].x) * pts[1].y + pts[0].x * pts[2].y - pts[2].x * pts[0].y);
    return Vec3f(1.f - r - b, b, r);
}
static float* record = new float[600 * 600];
void triangle(Vec3f* pts, Vec3f* uv, float* zbuffer, TGAImage& image, TGAImage& texture, TGAColor color) {
    Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width(), image.get_height());
    for (int i = 0; i < 3; i++) {
        bboxmin.x = std::max(0.f, std::min(bboxmin.x, pts[i].x));
        bboxmin.y = std::max(0.f, std::min(bboxmin.y, pts[i].y));

        bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
        bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
    }
    Vec3i P;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
            Vec3f bc_screen = barycentric_ccj(pts, Vec3f(P.x, P.y, P.z));
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
            P.z = pts[0].z * bc_screen.x + pts[1].z * bc_screen.y + pts[2].z * bc_screen.z;
            if (zbuffer[int(int(P.y) * 600 + P.x)] < P.z) {
                //record[int(P.y * image.get_width() + P.x]
                //std::cout << int(color.r) << int(color.g) << int(color.b) << std::endl;
                int w, h;
                float pu, pv;
                pu = uv[0].x * bc_screen.x + uv[1].x * bc_screen.y + uv[2].x * bc_screen.z;
                pv = uv[0].y * bc_screen.x + uv[1].y * bc_screen.y + uv[2].y * bc_screen.z;
                //std::cout << int(Tt.r) << "_" << int(Tt.g) << "_" << int(Tt.a) << std::endl;
                w = pu * texture.get_width();
                h = (1.f - pv) * texture.get_height();
                //std::cout << pu << "_" << pv << "_" << w << "_" << h << "_" << uv[0].x << "_" << uv[0].y<< std::endl;
                //TGAColor Tt(texture.get(w, h))
                TGAColor Tt(texture.get(w, h));
                //std::cout << int(Tt.r) << "_" << int(Tt.g) << "_" << int(Tt.a) << std::endl;
                image.set((int)P.x, (int)P.y, texture.get(w, h));
                //std::cout << P.x << " _ " << P.y << "\n" << std::endl;
                //std::cout << int(P.y * image.get_width() + P.x) << "_" << zbuffer[int(P.y * image.get_width() + P.x)] << "_" << P.z << std::endl;
                
                zbuffer[int(int(P.y) * image.get_width() + P.x)] = P.z;
       
            }
            else {
                auto color1 = image.get((int)P.x, (int)P.y);
                //std::cout << "__" << int(color1.r) << int(color1.g) << int(color1.b) << std::endl;
            }
            //image.set(P.x, P.y, color);
        }
    }
}

void drawFlat_zbuffer() {
    int lenght = 600;
	TGAImage image(lenght, lenght, TGAImage::RGB);
    TGAImage texture;
    texture.read_tga_file("african_head_diffuse.tga");
	Model *m = new Model("african_head.obj");
    float* zbuffer = new float[lenght * lenght]{0};
    for (int i = 0; i < lenght * lenght; i++) {
        zbuffer[i] = -10086;
    }
    for(int i=0; i < m->nfaces(); i++) {
        std::vector<int> faces = m->face(i);
        std::vector<int> faceuvs = m->faceuv(i);
        Vec3f t0[3];
        Vec3f world[3];
        Vec3f uvs[3];
        for(int j=0; j < faces.size(); j++){
//            std::cout<< "faces.size==" << faces.size() <<  "  j==" << j << " m->nfaces()" <<  m->nfaces() << " i =" << i << std::endl;
            Vec3f start = m->vert(faces[j]);
            //Vec3f uv = m->uvs(faceuvs[j]);
            float x0 = (start.x + 1) / 2 * lenght;
            float y0 = (start.y + 1) / 2 * lenght;
            float z0 = (start.z + 1) / 2 * lenght;
            t0[j] = Vec3f(x0, y0, z0); //, t1(x1, y1);
            uvs[j] = m->uvs(faceuvs[j]);
            world[j] = start;
        }
//        std::cout << t0[0].x << "-" << t0[0].y << "-"  << t0[1].x << "-"  << t0[1].y << "-"  << t0[2].x << "-"  << t0[2].y << std::endl;
        Vec3f n = (world[0]-world[1]) ^ (world[2] - world[0]);
        n.normalize();
        //std::cout<< n.x << "n.y" << n.y << "n.z" << n.z << std::endl;
        float inten = n * Vec3f(0.0, 0.0, -1.0);
        if (inten > 0) {
            triangle(t0, uvs, zbuffer, image, texture, TGAColor(inten * 255, inten * 255, inten * 255, 255));
           // fillTriangle_v1(t0[0], t0[1], t0[2], image, TGAColor(inten*255, inten*255, inten*255, 255), zbuffer);
        }
    }
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("drawFlatZbuffer.tga");
}

void drawWithTexture() {
    int length = 800;
    TGAImage image(length, length, TGAImage::RGB);
    Model * m = new Model("african_head.obj");
    //for(int i=0; i < )

//     int lenght = 800;
// 	TGAImage image(lenght, lenght, TGAImage::RGB);
// 	Model *m = new Model("african_head.obj");
// 	int* zbuffer = new int[lenght * lenght]{0};

//     for(int i=0; i < m->nfaces(); i++) {
//         std::vector<int> faces = m->face(i);
//         std::vector<int> faceuvs = m->faceuv(i);
//         Vec3i t0[3];
//         Vec3f world[3];
//         for(int j=0; j < faces.size(); j++){
// //            std::cout<< "faces.size==" << faces.size() <<  "  j==" << j << " m->nfaces()" <<  m->nfaces() << " i =" << i << std::endl;
//             Vec3f start = m->vert(faces[j]);
//             Vec3f uv = m->uvs(faceuvs[j]);
//             int u = uv.u * lenght;
//             int v = uv.v * lenght;
//             int x0 = (start.x + 1) / 2 * lenght;
//             int y0 = (start.y + 1) / 2 * lenght;
//             int z0 = (start.z + 1) / 2 * lenght;
//             t0[j] = Vec3i(x0, y0, z0); //, t1(x1, y1);
//             world[j] = start;
//         }
// //        std::cout << t0[0].x << "-" << t0[0].y << "-"  << t0[1].x << "-"  << t0[1].y << "-"  << t0[2].x << "-"  << t0[2].y << std::endl;
//         Vec3f n = (world[0]-world[1]) ^ (world[2] - world[0]);
//         n.normalize();
//         //std::cout<< n.x << "n.y" << n.y << "n.z" << n.z << std::endl;
//         float inten = n * Vec3f(0.0, 0.0, -1.0);
//         if (inten > 0) {
//             fillTriangle_v1(t0[0], t0[1], t0[2], image, TGAColor(inten*255, inten*255, inten*255, 255), zbuffer );
//         }
//     }
//     image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
// 	image.write_tga_file("drawFlatZbuffer.tga");
}
std::unique_ptr<int> myfunction (std::unique_ptr<int> i) {  // function:
    std::cout << ' ' << *i << std::endl;
    return i;
}
int test(){
    std::vector<std::unique_ptr<int>> veci;
    veci.push_back(std::unique_ptr<int>(new int(1)));
    std::cout <<"size1:"<< veci.size() << std::endl;
    //for_each(veci.begin(), veci.end(), myfunction);
    for(auto i = veci.begin(); i !=veci.end(); i++ ){
            if(*i && (*i).get()) {
                std::cout << ' ' << (**i);
            }else {
                std::cout << " - " << std::endl;
            }
    }
    std::cout<<"size2:" << veci.size() << std::endl;
    return 0;
}


int main(int argc, char** argv) {
    //drawFrame();
    //drawTriangles_frame();
    //drawFlat();
    drawFlat_zbuffer();
    //drawWithTexture();
	return 0;
}
