//
// Created by fatih on 09.03.2017.
//

#include <shapes/triangle.hpp>
#include <physics/aabb.hpp>

/********************************************************/

/* AABB-triangle overlap test code                      */

/* by Tomas Akenine-Möller                              */

/* Function: int triBoxOverlap(float boxcenter[3],      */

/*          float boxhalfsize[3],float triverts[3][3]); */

/* History:                                             */

/*   2001-03-05: released the code in its first version */

/*   2001-06-18: changed the order of the tests, faster */

/*                                                      */

/* Acknowledgement: Many thanks to Pierre Terdiman for  */

/* suggestions and discussions on how to optimize code. */

/* Thanks to David Hunt for finding a ">="-bug!         */

/********************************************************/

#include <math.h>

#include <stdio.h>



#define X 0

#define Y 1

#define Z 2



#define CROSS(dest,v1,v2) \
dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
dest[2]=v1[0]*v2[1]-v1[1]*v2[0];



#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])



#define SUB(dest,v1,v2) \
dest[0]=v1[0]-v2[0]; \
dest[1]=v1[1]-v2[1]; \
dest[2]=v1[2]-v2[2];



#define FINDMINMAX(x0,x1,x2,min,max) \
min = max = x0;   \
if(x1<min) min=x1;\
if(x1>max) max=x1;\
if(x2<min) min=x2;\
if(x2>max) max=x2;



int planeBoxOverlap(float normal[3], float vert[3], float maxbox[3])	// -NJMP-

{

    int q;

    float vmin[3],vmax[3],v;

    for(q=X;q<=Z;q++)

    {

        v=vert[q];					// -NJMP-

        if(normal[q]>0.0f)

        {

            vmin[q]=-maxbox[q] - v;	// -NJMP-

            vmax[q]= maxbox[q] - v;	// -NJMP-

        }

        else

        {

            vmin[q]= maxbox[q] - v;	// -NJMP-

            vmax[q]=-maxbox[q] - v;	// -NJMP-

        }

    }

    if(DOT(normal,vmin)>0.0f) return 0;	// -NJMP-

    if(DOT(normal,vmax)>=0.0f) return 1;	// -NJMP-



    return 0;

}





/*======================== X-tests ========================*/

#define AXISTEST_X01(a, b, fa, fb)			   \
p0 = a*v0[Y] - b*v0[Z];			       	   \
p2 = a*v2[Y] - b*v2[Z];			       	   \
if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
if(min>rad || max<-rad) return 0;



#define AXISTEST_X2(a, b, fa, fb)			   \
p0 = a*v0[Y] - b*v0[Z];			           \
p1 = a*v1[Y] - b*v1[Z];			       	   \
if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
if(min>rad || max<-rad) return 0;



/*======================== Y-tests ========================*/

#define AXISTEST_Y02(a, b, fa, fb)			   \
p0 = -a*v0[X] + b*v0[Z];		      	   \
p2 = -a*v2[X] + b*v2[Z];	       	       	   \
if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
if(min>rad || max<-rad) return 0;



#define AXISTEST_Y1(a, b, fa, fb)			   \
p0 = -a*v0[X] + b*v0[Z];		      	   \
p1 = -a*v1[X] + b*v1[Z];	     	       	   \
if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
if(min>rad || max<-rad) return 0;



/*======================== Z-tests ========================*/



#define AXISTEST_Z12(a, b, fa, fb)			   \
p1 = a*v1[X] - b*v1[Y];			           \
p2 = a*v2[X] - b*v2[Y];			       	   \
if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
if(min>rad || max<-rad) return 0;



#define AXISTEST_Z0(a, b, fa, fb)			   \
p0 = a*v0[X] - b*v0[Y];				   \
p1 = a*v1[X] - b*v1[Y];			           \
if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
if(min>rad || max<-rad) return 0;



int triBoxOverlap(float boxcenter[3],float boxhalfsize[3],float triverts[3][3])
{
    float v0[3],v1[3],v2[3];
    float min,max,p0,p1,p2,rad,fex,fey,fez;		// -NJMP- "d" local variable removed
    float normal[3],e0[3],e1[3],e2[3];

    SUB(v0,triverts[0],boxcenter);
    SUB(v1,triverts[1],boxcenter);
    SUB(v2,triverts[2],boxcenter);

    SUB(e0,v1,v0);      /* tri edge 0 */
    SUB(e1,v2,v1);      /* tri edge 1 */
    SUB(e2,v0,v2);      /* tri edge 2 */

    fex = fabsf(e0[X]);
    fey = fabsf(e0[Y]);
    fez = fabsf(e0[Z]);

    AXISTEST_X01(e0[Z], e0[Y], fez, fey);
    AXISTEST_Y02(e0[Z], e0[X], fez, fex);
    AXISTEST_Z12(e0[Y], e0[X], fey, fex);

    fex = fabsf(e1[X]);
    fey = fabsf(e1[Y]);
    fez = fabsf(e1[Z]);

    AXISTEST_X01(e1[Z], e1[Y], fez, fey);
    AXISTEST_Y02(e1[Z], e1[X], fez, fex);
    AXISTEST_Z0(e1[Y], e1[X], fey, fex);

    fex = fabsf(e2[X]);
    fey = fabsf(e2[Y]);
    fez = fabsf(e2[Z]);

    AXISTEST_X2(e2[Z], e2[Y], fez, fey);
    AXISTEST_Y1(e2[Z], e2[X], fez, fex);
    AXISTEST_Z12(e2[Y], e2[X], fey, fex);

    FINDMINMAX(v0[X],v1[X],v2[X],min,max);

    if(min>boxhalfsize[X] || max<-boxhalfsize[X]) return 0;

    FINDMINMAX(v0[Y],v1[Y],v2[Y],min,max);

    if(min>boxhalfsize[Y] || max<-boxhalfsize[Y]) return 0;

    FINDMINMAX(v0[Z],v1[Z],v2[Z],min,max);

    if(min>boxhalfsize[Z] || max<-boxhalfsize[Z]) return 0;

    CROSS(normal,e0,e1);

    if(!planeBoxOverlap(normal,v0,boxhalfsize)) return 0;	// -NJMP-

    return 1;   /* box and triangle overlaps */

}




void tri_aabb_collision()
{
    rtr::shapes::triangle tri (std::array<glm::vec3, 3>{ glm::vec3{-0.0888, 0.0877, 0.0142} , glm::vec3{ -0.0896, 0.0915, 0.0178 } , glm::vec3{ -0.0893, 0.0895, 0.0116}  });

    rtr::physics::aabb aabb{ glm::vec3{-0.016800, 0.110152, -0.00148300} , glm::vec3{0.155159, 0.153686, 0.120393}  };

    float center[] = {-0.016800, 0.110152, -0.00148300};
    float ext[] = {0.155159 / 2, 0.153686 / 2, 0.120393 / 2};

    float verts[][3] = {
            {-0.0888, 0.0877, 0.0142},
            {-0.0896, 0.0915, 0.0178},
            {-0.0893, 0.0895, 0.0116}
    };

    auto r = triBoxOverlap(center, ext, verts);

    using rtr::physics::intersect;

    auto res = intersect(aabb, tri);
    assert(res);
}
