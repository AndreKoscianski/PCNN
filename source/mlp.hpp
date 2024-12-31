#ifndef _mlp_hpp_
#define _mlp_hpp_

#include <stdio.h>
#include <math.h>
//#include <values.h>

#include "vector.hpp"
//#include "funct.hpp"

class tp_mlp {

   private:

      void   allouer    (void);
      void deallouer    (void);

      void prepare_region (void);

      int
//         *_coisa,
         _nlayers,_ninputs, _nneurons, _len_w,
         _layers[10], _nparmscouche[10], _nparmsneuron[10]
         ;

   public:

//      tp_region
//         _region;

      char   *_remarks;

      double ***_w, **_bias, *_x, **_y, ***_ydx, **_ydx2, **_y1, *_a, *_b, *_f,
             *_parms;



      tp_mlp (int, int *, double *);
      tp_mlp ();
     ~tp_mlp ();

//     int *coisa(void);
     int  dimension  (void) {return _len_w;}
     void destroy    (void);
//     void init     (int *, double *);
     void init       (int , int *, double *);

     void adjust (double *);

     double dotproduct (int, int);

     double  neuronderiv (int , int );


     double *theta    (void);
     void    settheta (double *w) ;
     int     length   (void);

     tp_vector vectortheta    (void);
     void      setvectortheta (tp_vector);


     double  calc   (double *);
     double  deriv1 (int);
     double  deriv2 (int, int);

     double neuronderiv1 (int i) ;
     double neuronderiv2 (int i) ;

     double  sigma (double );

     double  calc (double *, int);

     int load  (const char *s);
     int save  (const char *s);
};

#endif
