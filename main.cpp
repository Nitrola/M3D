#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <sys/stat.h>
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

void display_point(Point p){
    std::cout << "x: " << p.x << " y: " << p.y << std::endl;
}

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


void draw_triangle(Point verts[],TGAImage& image, TGAColor color){
    draw_line(verts[0],verts[1],image,color);
    draw_line(verts[1],verts[2],image,color);
    draw_line(verts[2],verts[0],image,color);
}

void fill_triangle(Point verts[],TGAImage& image, TGAColor color){
    
    //we start by sorting the points by theirs y coordinate
    if (verts[0].y > verts[1].y) 
        std::swap(verts[0],verts[1]);
    if (verts[0].y > verts[2].y) 
        std::swap(verts[2],verts[0]);
    if (verts[1].y > verts[2].y) 
        std::swap(verts[1],verts[2]);
    
    // slopes of the lines
    float slope1 = (float)(verts[1].x - verts[0].x) / (verts[1].y - verts[0].y);
    float slope2 = (float)(verts[2].x - verts[0].x) / (verts[2].y - verts[0].y);
    
    Point start = verts[0];
    Point end = verts[0];
    
    float posX = (float)verts[0].x;
    float posY = (float)verts[0].x;

    //top half
    for(int i = verts[0].y;i < verts[1].y ; i++){
        draw_line(start,end,image,color);
        posX += slope1;
        posY += slope2;
        start.y++;
        end.y++;
        start.x = (int)posX;
        end.x = (int)posY;
    }
    
    
    slope1 = (float)(verts[2].x - verts[1].x) / (verts[2].y - verts[1].y);
    slope2 = (float)(verts[2].x - verts[0].x) / (verts[2].y - verts[0].y);
    start = verts[2];
    end = verts[2];
    
    posX = (float)verts[2].x;
    posY = (float)verts[2].x;

    //bottom half
    for(int i = verts[2].y;i >= verts[1].y ; i--){
        draw_line(start,end,image,color);
        posX -= slope1;
        posY -= slope2;
        start.y--;
        end.y--;
        start.x = (int)posX;
        end.x = (int)posY;
    }    
}


void filled_model(Model* model,float scale,TGAImage& image, TGAColor color){
    Point verts[3];
    std::srand(time(0));
    for(int i=0; i < model->nb_faces();i++){
        std::vector<int> face = model->face(i);
        for(int j=0;j<3;j++){
            verts[j].x = (model->vert(face[j]).x + 1.) * scale;
            verts[j].y = (model->vert(face[j]).y + 1.) * scale;
        }
        TGAColor face_color = TGAColor(std::rand() * 255,std::rand() * 255,std::rand() * 255,255);
        fill_triangle(verts,image,face_color);
    }
}

void fill_model_light(Model* model,float scale,TGAImage& image){
    Point verts[3];
    Vec3f light = Vec3f(0.0,.0,-1.0);
    Vec3f vec_verts[3];
    for(int i=0; i < model->nb_faces();i++){
        std::vector<int> face = model->face(i);
        for(int j=0;j<3;j++){
            verts[j].x = (model->vert(face[j]).x + 1.) * scale;
            verts[j].y = (model->vert(face[j]).y + 1.) * scale;
            
            vec_verts[j] = model->vert(face[j]);
            
        }
        // normal vector of the face orthogonal to the Point 0
        Vec3f normal = (vec_verts[2] - vec_verts[0])^(vec_verts[1] - vec_verts[0]);
        normal.normalize();
        // Projection of the normal on the light
        float light_intensity = normal*light;
        if(light_intensity > 0){
            fill_triangle(verts,image,
                        TGAColor(255*light_intensity,
                                255*light_intensity,
                                255*light_intensity,
                                255)
                        );
        }
    }
}

int main(int argc, char*argv[]){
    
    //some points
    Point p1{26,40};
    Point p2{160,80};
    Point p3{40,26};
    Point p4{80,160};
    
    // TGA Image to save
    TGAImage line_image(200, 200, TGAImage::RGB);
    TGAImage wired_model_image(400,400,TGAImage::RGB);
    TGAImage triangle_image(200,200,TGAImage::RGB);
    TGAImage filled_model_image(400,400,TGAImage::RGB);
    TGAImage filled_lighted_model_image(400,400,TGAImage::RGB);
    mkdir("output",0777);
    
    //lines
    draw_line(p1,p2,line_image,red);
    draw_line(p2,p1,line_image,sky_blue);
    draw_line(p3,p4,line_image,green);
    line_image.flip_vertically();
    line_image.write_tga_file("output/lines.tga");
    
    //triangles
    Point triangle_verts[3] = {p1,p2,p3};
    //draw_triangle(triangle_verts,triangle_image,red);
    Point filled_triangle_verts[3] = {p2,p3,p4};
    
    fill_triangle(filled_triangle_verts,triangle_image,green);
    draw_triangle(filled_triangle_verts,triangle_image,red);
    // draw the border over the triangle to check
    //triangle_image.flip_vertically();   
    triangle_image.write_tga_file("output/triangle.tga");
    
    //model to display
    Model *african_head = new Model("models/african_head.obj");
    
    //wireframe model
    wireframe(african_head,400/2.,wired_model_image,green);
    wired_model_image.flip_vertically();
    wired_model_image.write_tga_file("output/model.tga");
    
    // filled with random colors
    filled_model(african_head,400/2.,filled_model_image,white);
    filled_model_image.flip_vertically();
    filled_model_image.write_tga_file("output/filled_model.tga");
    
    // filled with light
    fill_model_light(african_head,400/2,filled_lighted_model_image);
    filled_lighted_model_image.flip_vertically();
    filled_lighted_model_image.write_tga_file("output/filled_lighted_model.tga");
    
    
    //freeing memory
    delete african_head;
    
    return 0;
}



