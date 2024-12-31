#ifndef _slp_hpp_
#define _slp_hpp_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define FAIXA      5.0l
#define DEMIFAIXA  2.5l





//-------------------------------------------------
class tp_gnn {

   public:
      char   *_remarks;
      
      double *_w, *_x, *_y, *_neuron, *_a, *_b, *_f;

      int
         _qi, _qt, _ql, _qn
        ,_Nn, _Ne, _Npi, _Npp, _Npt;


      tp_gnn ();
     ~tp_gnn ();

     int  length  (void) {return _qn;}
     void destroy (void);
     void init (int , int, double *);

     void    adjust   (double *);
     double *theta    (void) { return _w;};
     void    settheta (double *w) {for (int i=0;i<_qn;i++)_w[i]=w[i];}

     double dotproduct (int );

     double  neuronderiv (int, int , int , int);

     double  calc (double *);
     double  deriv (int, int, int);
     inline double deriv1 (int i) { return deriv (1, i, 0);}
     inline double deriv2 (int i, int j) { return deriv (2, i, j);}


     double  sigma (double );

     double  calc (double *, int);

     int load  (const char *s);
     int save  (const char *s);
};


#endif
