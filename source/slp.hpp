#ifndef _slp_hpp_
#define _slp_hpp_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define FAIXA      6.0l
#define DEMIFAIXA  3.0l





//-------------------------------------------------
class tp_slp {

   public:
      char   *_remarks;
      
      double *_w, *_x, *_y, *_y2, *_a, *_b, *_f;

      int
         _qi, _qt, _ql, _qn, _ninputs, _nneurons;


      tp_slp ();
     ~tp_slp ();

     int  length  (void) {return _qn;}
     void destroy (void);
     void init (int , int, double *);

     void    adjust   (double *);
     double *theta    (void) { return _w;};
     void    settheta (double *w) {for (int i=0;i<_qn;i++)_w[i]=w[i];}

     double dotproduct (int );

     double  neuronderiv (int , int );


     double  calc (double *);
     double  deriv (int, int);
     double  deriv1 (int);
     double  deriv2 (int);
     double  deriv2 (int, int);

     inline double neuronderiv1 (int i) {return _y2[i];}
     inline double neuronderiv2 (int i) {return -2.0 * _y[i] * _y2[i];}
     inline double netpow (double x,  int a) {double r=1;while (--a){r *= x;}return r;}

     double  sigma (double );

     double  calc (double *, int);

     int load  (const char *s);
     int save  (const char *s);
};


#endif
