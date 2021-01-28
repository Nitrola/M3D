#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <time.h>
#include <cstdlib>
#include <limits>
#include <sys/stat.h>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,0, 255);
const TGAColor green   = TGAColor(0, 255,0,255);
const TGAColor blue   = TGAColor(0, 0,255,255);
const TGAColor sky_blue   = TGAColor(0, 255,255,255);

int width = 800;
int height = 800;
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


Vec3f interpol(Point verts[],int x,int y){
    float v1,v2,v3;
    
    // math stuff
    v1 = 
    1.0f * ((verts[1].y - verts[2].y) * (x - verts[2].x) + (verts[2].x - verts[1].x) * (y - verts[2].y)) 
    / ((verts[1].y - verts[2].y) * (verts[0].x - verts[2].x)+(verts[2].x - verts[1].x) * (verts[0].y - verts[2].y));
    
    v2 = 
    1.0f * ((verts[2].y - verts[0].y) * (x - verts[2].x) + (verts[0].x - verts[2].x) * (y - verts[2].y))
    /((verts[1].y - verts[2].y) * (verts[0].x - verts[2].x )+(verts[2].x - verts[1].x) * (verts[0].y - verts[2].y));
    
    v3 = 1.0f - (v1 + v2);
    
    return Vec3f(v1,v2,v3);
    
}

 
Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
    Vec3f s[2];
    for (int i=2; i--; ) {
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return Vec3f(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}
void fill_triangle_interpole(Point verts[],int zbuffer[],TGAImage& image, TGAColor color){
    // square formed by the triangle
    int lx=verts[0].x,rx=verts[0].x,ly=verts[0].y,ry=verts[0].y;
    for(int i=0;i < 3;i++){
        lx = std::min(lx,verts[i].x);
        rx = std::max(rx,verts[i].x);
        ly = std::min(ly,verts[i].y);
        ry = std::max(ry,verts[i].y);
    }
    //we check with interpolation if any pixel of the box is inside
    //and draw them if they are
    int z = 0;
    for(int x=lx; x <=rx; x++){
        for(int y=ly; y<=ry; y++){
            Vec3f v = interpol(verts,x,y);z = 0;
            for(int i=0;i<3;i++){
                z += verts[i].y * v.z;
            }
            if( v.x <= 1.0 && v.x >= 0.0 &&
                v.y <= 1.0 && v.y >= 0.0 && 
                v.z <= 1.0 && v.z >= 0.0){
                if(zbuffer[x + y * width] < z){
                    zbuffer[x + y * width] = z;
                    draw_pixel(x,y,image,color);
                }
            }
        }
    }
}

void fill_triangle_interpole(Point verts[],Vec3f vec_vert[], int zbuffer[],TGAImage& image, TGAImage texture,Vec3f vec_texture[],float light_intensity){
    // square formed by a triangle 
    int lx=verts[0].x,rx=verts[0].x,ly=verts[0].y,ry=verts[0].y;
    for(int i=0;i < 3;i++){
        lx = std::min(lx,verts[i].x);
        rx = std::max(rx,verts[i].x);
        ly = std::min(ly,verts[i].y);
        ry = std::max(ry,verts[i].y);
    }
    //we check with interpolation if any pixel of the box is inside
    //and draw them if they are
    int z = 0;
    Vec3f P;
    for(int x=lx; x <=rx; x++){
        for(int y=ly; y<=ry; y++){
            P.x = (float)x;
            P.y = (float)y;
            Vec3f v = interpol(verts,x,y);
            z = 0;
            for(int i=0;i<3;i++){
                z += vec_vert[i][2] * v[i];
            }
            if( v.x >= 0.0 &&
                 v.y >= 0.0 && 
                 v.z >= 0.0){
                if(zbuffer[x + y * width] < z){
                    zbuffer[x + y * width] = z;
                    int texture_x =(int) ( texture.get_width()*(
                        vec_texture[0].x*v.x+
                        vec_texture[1].x*v.y+
                        vec_texture[2].x*v.z));
                    int texture_y =(int) (texture.get_height()*(
                        vec_texture[0].y*v.x+
                        vec_texture[1].y*v.y+
                        vec_texture[2].y*v.z));
                    TGAColor texture_pixel = texture.get(texture_x,texture_y);                    
                    texture_pixel.r *=light_intensity;
                    texture_pixel.g *=light_intensity;
                    texture_pixel.b *=light_intensity;
                    draw_pixel(x,y,image,texture_pixel);
                }
            }
        }
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

void fill_model_light(Model* model,int zbuffer[],float scale,TGAImage& image){
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
        Vec3f normal = cross(vec_verts[2] - vec_verts[0],vec_verts[1] - vec_verts[0]);
        normal.normalize();
        // Projection of the normal on the light
        float light_intensity = normal*light;
        if(light_intensity > 0){
            fill_triangle_interpole(verts,zbuffer,image,
                        TGAColor(255*light_intensity,
                                255*light_intensity,
                                255*light_intensity,
                                255)
                        );
        }
    }
}

void fill_model_texture(Model* model,TGAImage texture,int zbuffer[],float scale,TGAImage& image){
    Point verts[3];
    Vec3f light = Vec3f(0.0,.0,-1.0);
    Vec3f vec_verts[3];
    Vec3f vec_texture[3];
    for(int i=0; i < model->nb_faces();i++){
        std::vector<int> face = model->face(i);
        for(int j=0;j<3;j++){
            verts[j].x = (model->vert(face[j]).x + 1.) * scale;
            verts[j].y = (model->vert(face[j]).y + 1.) * scale;
            vec_verts[j] = model->vert(face[j]);
            //vec_texture[j] = model->vert_texture(face[j]);
        } 
        std::vector<int> face_texture = model->face_texture(i);
        for(int j=0;j<3;j++){
            vec_texture[j] = model->vert_texture(face_texture[j]);
        }
        // normal vector of the face orthogonal to the Point 0
        Vec3f normal = cross(vec_verts[2] - vec_verts[0],vec_verts[1] - vec_verts[0]);
        normal.normalize();
        // Projection of the normal on the light
        float light_intensity = normal*light;
        
        if(light_intensity > 0){
            fill_triangle_interpole(verts,vec_verts,zbuffer,image,texture,vec_texture,light_intensity);
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
    TGAImage filled_model_image(width,height,TGAImage::RGB);
    TGAImage filled_lighted_model_image(width,height,TGAImage::RGB);
    TGAImage filled_textured_model_image(width,height,TGAImage::RGB);
    
    mkdir("output",0777);
    
    //lines
    draw_line(p1,p2,line_image,red);
    draw_line(p2,p1,line_image,sky_blue);
    draw_line(p3,p4,line_image,green);
    line_image.flip_vertically();
    line_image.write_tga_file("output/lines.tga");
    
    //triangles
    Point triangle_verts[3] = {p1,p2,p3};
    Point filled_triangle_verts[3] = {p2,p3,p4};
    
    fill_triangle(filled_triangle_verts,triangle_image,green);
    draw_triangle(filled_triangle_verts,triangle_image,red);
    // draw the border over the triangle to check
    triangle_image.write_tga_file("output/triangle.tga");
    
    //model to display
    Model *african_head = new Model("models/african_head.obj");
    TGAImage difuse_african_head(1024,1024,TGAImage::RGB);
    difuse_african_head.read_tga_file("models/african_head_diffuse.tga");
    difuse_african_head.flip_vertically();
    //wireframe model
    wireframe(african_head,400/2.,wired_model_image,green);
    wired_model_image.flip_vertically();
    wired_model_image.write_tga_file("output/model.tga");
    
    // filled with random colors
    filled_model(african_head,width/2.,filled_model_image,white);
    filled_model_image.flip_vertically();
    filled_model_image.write_tga_file("output/filled_model.tga");
    
    // filled with light
    int zbuffer[width*height];
    for(int i = 0 ; i < width*height;i++){
        zbuffer[i] = std::numeric_limits<int>::min();
    }

    fill_model_light(african_head, zbuffer,height/2,filled_lighted_model_image);
    filled_lighted_model_image.flip_vertically();
    filled_lighted_model_image.write_tga_file("output/filled_lighted_model.tga");
    
    for(int i = 0 ; i < width*height;i++){
        zbuffer[i] = std::numeric_limits<int>::min();
    }
    fill_model_texture(african_head,difuse_african_head,zbuffer, height/2,filled_textured_model_image);    
    filled_textured_model_image.flip_vertically();
    filled_textured_model_image.write_tga_file("output/filled_textured_model.tga");
    //freeing memory
    delete african_head;
    
    return 0;
}
