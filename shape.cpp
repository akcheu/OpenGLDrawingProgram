// Allen Cheung | OpenGL Project

#include <typeinfo>
#include <unordered_map>
#include <cmath>
using namespace std;

#include "shape.h"
#include "util.h"

#define EQUIL_WIDTH 0.86602540378443864676372317075294

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font_, const string& textdata_):
      glut_bitmap_font(glut_bitmap_font_), textdata(textdata_) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}

polygon::polygon (const vertex_list& vertices): vertices(vertices) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
            polygon({{0, 0}, {width, 0},
   {width, height}, {0, height}}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}
   
diamond::diamond (const GLfloat width, const GLfloat height):polygon
   ({{0, height/2}, {0 - width/2, 0}, {0, 0 - height/2},
      {width/2, 0}}) {
   DEBUGF ('c', this);
}
   
triangle::triangle (const vertex_list& vertices): polygon(vertices) {
   DEBUGF ('c', this);
}
   
equilateral::equilateral (GLfloat width):polygon
   ({{0, 0}, {width, 0}, {width/2, width * GLfloat(EQUIL_WIDTH)}}) {
   DEBUGF ('c', this);
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   auto display = reinterpret_cast<const GLubyte*> (textdata.c_str());
   glRasterPos2f(center.xpos, center.ypos);
   glColor3d(color.ubvec[0], color.ubvec[1], color.ubvec[2]);
   glutBitmapString(glut_bitmap_font, display);
}
   
void text::draw_border (const vertex& center, const rgbcolor& color,
                        const GLfloat& width) const {
   auto display = reinterpret_cast<const GLubyte*> (textdata.c_str());
   size_t length = glutBitmapLength (glut_bitmap_font, display);
   size_t height = glutBitmapHeight (glut_bitmap_font);
   glLineWidth(width);
   glBegin (GL_LINE_LOOP);
   glColor3d(color.ubvec[0], color.ubvec[1], color.ubvec[2]);
   glVertex2f (center.xpos + length + width, center.ypos - width);
   glVertex2f (center.xpos - width, center.ypos - width);
   glVertex2f (center.xpos - width, center.ypos + height);
   glVertex2f (center.xpos + length + width, center.ypos + height);
   glEnd();
}
   
void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   glEnable(GL_LINE_SMOOTH);
   glColor3d(color.ubvec[0], color.ubvec[1], color.ubvec[2]);
   const float delta = 2 * M_PI / 32;
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      float x = dimension.xpos/2 * cos(theta) + center.xpos;
      float y = dimension.ypos/2 * sin(theta) + center.ypos;
      glVertex2f(x, y);
   }
   glEnd();
}
   
void ellipse::draw_border (const vertex& center, const rgbcolor&
                           color, const GLfloat& width) const {
   glLineWidth(width);
   glEnable (GL_LINE_SMOOTH);
   glBegin (GL_LINE_LOOP);
   glColor3d(color.ubvec[0], color.ubvec[1], color.ubvec[2]);
   const float delta = 2 * M_PI / 32;
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      float x = dimension.xpos/2 * cos(theta) + center.xpos;
      float y = dimension.ypos/2 * sin(theta) + center.ypos;
      glVertex2f(x, y);
   }
   glEnd();
}
   
void polygon::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   glEnable(GL_LINE_SMOOTH);
   glColor3d(color.ubvec[0], color.ubvec[1], color.ubvec[2]);
   float x_avg {0}, y_avg {0}, x_coord{0}, y_coord{0};
   for (const auto& vertex: vertices) {
      x_avg += vertex.xpos;
      y_avg += vertex.ypos;
   }
   x_avg /= vertices.size();
   y_avg /= vertices.size();
   auto itor = vertices.cbegin();
   while (itor != vertices.cend()) {
      x_coord = center.xpos + itor->xpos - x_avg;
      y_coord = center.ypos + itor->ypos - y_avg;
      glVertex2f(x_coord, y_coord);
      itor++;
   }
   glEnd();
}
   
void polygon::draw_border (const vertex& center, const rgbcolor&
                           color, const GLfloat& width) const {
   glLineWidth(width);
   glEnable(GL_LINE_SMOOTH);
   glBegin (GL_LINE_LOOP);
   glColor3d(color.ubvec[0], color.ubvec[1], color.ubvec[2]);
   float xavg = 0;
   float yavg = 0;
   for (const auto& vertex: vertices){
      xavg += vertex.xpos;
      yavg += vertex.ypos;
   }
   xavg /= vertices.size();
   yavg /= vertices.size();
   for(auto itor = vertices.cbegin(); itor != vertices.cend(); ++itor){
      float xpos = center.xpos + itor->xpos - xavg;
      float ypos = center.ypos + itor->ypos - yavg;
      glVertex2f(xpos, ypos);
   }
   glEnd();
}
   
void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}
