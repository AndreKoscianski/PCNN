#ifndef __vectorN_hpp
#define __vectorN_hpp

#include <math.h>


class tp_vector {

   public:

   int _vectorNsize  ;
   int _originalsize ;
   int _size_sur_4   ;
   double *x;

     tp_vector (void);
     tp_vector (const tp_vector &v);
     tp_vector (int i) ;

     void dimension_it (int i);
     int  size  (void);

     tp_vector operator *  (const double k);
     double    operator *  (const tp_vector &v2);
     tp_vector operator +  (const tp_vector &v2);
     tp_vector& operator = (const tp_vector &C) ;
     tp_vector& operator = (const char *s) ;
     tp_vector (const char *s);
     tp_vector operator -  (const tp_vector &v2);
     tp_vector operator -  ();
     tp_vector subvector (int io, int in) ;

     double    operator [] (int idx) ;
//     double &  operator  [] (int idx) ; //const


     char   *find_next_n (char *p) ;
     void    reset     (void);
     double  norm      (void);
     void    normalise (void);
//     void    sort      (void);
     double  max       (void);
     double  maxabs    (void);
     void    setvector (double *w) ;
     void    todouble  (double *w) ;
     double *todouble  (void);
     void    mul       (const double n);
     void    multiply  (double n) ;
     void    attrib    (const tp_vector &C) ;
     void    concatenate (tp_vector v2) ;
     void    add       (const tp_vector &C) ;
     void    sub       (const tp_vector &C) ;
     char   *c_str     (char *_2char) ;
     double  length    (void);
     double  dotproduct (tp_vector v) ;
     double  ManhattanNorm (tp_vector v);
};


tp_vector operator +  (const tp_vector &v1, const tp_vector &v2);

#endif
