#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,0, 255);
const TGAColor green   = TGAColor(0, 255,0,255);
const TGAColor blue   = TGAColor(0, 0,255,255);
const TGAColor sky_blue   = TGAColor(0, 255,255,255);

struct Point{
 int x , y;
};


void draw_pixel(int x, int y,TGAImage &image,TGAColor color){
    image.set(x,y,color);
}

void draw_line(Point p1, Point p2,TGAImage &image,TGAColor color){
    
    // distance between the two point
    int dx = abs(p2.x - p1.x);
    int dy = -abs(p2.y - p1.y);
    
    // directions of the coordinates
    int dirx = p1.x < p2.x ? 1 : -1;
    int diry = p1.y < p2.y ? 1 : -1;
        
    
    // slope of the line
    // 1.f of it's a straight line
    float derr = dy + dx;
    float err;
    
    
    while(1){
            draw_pixel(p1.x,p1.y,image,color);
            // we've reach the end point, we step out of the loop
            if(p1.x == p2.x && p1.y == p2.y) break;
            err = 2 * derr;
            //next colonum
            if(err >= dy){
                derr += dy;
                p1.x += dirx;
            }
            //next line
            if(err <= dx){ 
                derr += dx;
                p1.y += diry;
            }
    }
}


void wireframe(Model* model,float scale,TGAImage& image, TGAColor color){
    // each triangle of the file 
    for(int i = 0; i< model->nb_faces();i++){
        std::vector<int> face = model->face(i);
        // three point of a triangle
        for(int j=0; j < 3;j++){
            Vec3f v1 = model->vert(face[j]);
            Vec3f v2 = model->vert(face[(j+1)%3]);
            Point p1{
                (int)((v1.x+1.)*scale),
                (int)((v1.y+1.)*scale)
                
            };
            Point p2{
                (int)((v2.x+1.)*scale),
                (int)((v2.y+1.)*scale)
            };
            draw_line(p1,p2,image,color);
        }
    }
}

void draw_triangle(Vec3f vert,TGAImage image, TGAColor color){
    
    
}

int main(int argc, char*argv[]){
    Point p1{26,40};
    Point p2{160,80};
    Point p3{40,26};
    Point p4{80,160};
    TGAImage line_image(200, 200, TGAImage::RGB);
    TGAImage model_image(400,400,TGAImage::RGB);
    draw_line(p1,p2,line_image,red);
    draw_line(p2,p1,line_image,sky_blue);
    draw_line(p3,p4,line_image,green);
    Model *african_head = new Model("models/african_head.obj");
    wireframe(african_head,400/2.,model_image,green);
    line_image.flip_vertically();
    line_image.write_tga_file("lines.tga");
    model_image.flip_vertically();
    model_image.write_tga_file("model.tga");
    return 0;
}


void draw_triangle(){

    
}
