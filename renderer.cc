/*
   This file is part of the Mandelbox program developed for the course
    CS/SE  Distributed Computer Systems taught by N. Nedialkov in the
    Winter of 2015-2016 at McMaster University.

    Copyright (C) 2015-2016 T. Gwosdz and N. Nedialkov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>

#include "color.h"
#include "mandelbox.h"
#include "camera.h"
#include "vector3d.h"
#include "3d.h"
#ifdef _OPENMP
#include <omp.h>
#endif
#ifdef _OPENACC
#include <openacc.h>
#include <curand.h>
#endif

// extern double getTime();
// extern void   printProgress( double perc, double time );

extern void rayMarch (const RenderParams &render_params, const vec3 &from, const vec3  &to, double eps, pixelData &pix_data);

extern vec3 getColour(const pixelData &pixData, const RenderParams &render_params,
		      const vec3 &from, const vec3  &direction);

void renderFractal(const CameraParams &camera_params, const RenderParams &renderer_params,unsigned char* image)
{


  const double eps = pow(10.0, renderer_params.detail);
  double farPoint[3];
  vec3 to, from, collision;


  from.SetDoublePoint(camera_params.camPos);

  const int height = renderer_params.height;
  const int width  = renderer_params.width;

  pixelData pix_data;

  #pragma omp parallel\
  default(shared)\
  private(to, pix_data,collision)\
  shared(image,camera_params, renderer_params, from, farPoint)
  {
  // double time = getTime(); // was before pragma loop
  #if defined(_OPENMP)
    int nthreads = omp_get_num_threads();
    int ID = omp_get_thread_num();
    // if (ID==0) printf("Running with %d threads\n",nthreads);
  #else
    int ID = 0;
  #endif

  int i,j,k;
  #pragma omp for schedule (guided)
  for(j = 0; j < height; j++)
  {
		vec3 color;
      //for each column pixel in the row

    for(i = 0; i <width; i++)
  	{
  	  // get point on the 'far' plane
  	  // since we render one frame only, we can use the more specialized method
  	  UnProject(i, j, camera_params, farPoint);

  	  // to = farPoint - camera_params.camPos
  	  to = SubtractDoubleDouble(farPoint,camera_params.camPos);
  	  to.Normalize();

  	  //render the pixel
  	  rayMarch(renderer_params, from, to, eps, pix_data);
      // if (pix_data.escaped == false){
      //   printf("%dx%d - distance = %f\n",i,j,pix_data.distance);
      // }


  	  //get the colour at this pixel
  	  color = getColour(pix_data, renderer_params, from, to);

  	  //save colour into texture
  	  k = (j * width + i)*3;
  	  image[k+2] = (unsigned char)(color.x * 255);
  	  image[k+1] = (unsigned char)(color.y * 255);
  	  image[k]   = (unsigned char)(color.z * 255);
  	}
    //if (ID==0) printProgress((j+1)/(double)height,getTime()-time);
  }
  //if (ID==0) printf("\n rendering done:\n");
}//end parallel

}

