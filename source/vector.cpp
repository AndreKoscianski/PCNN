#include <stdio.h>
#include <string.h>
#include <math.h>


#define __vector_cpp
#include "vector.hpp"
#undef __vector_cpp


//------------------------------------------------------
tp_vector::tp_vector (void) {

   _vectorNsize  =
   _size_sur_4   =
   _originalsize = 0;

   x            = NULL;
}
//------------------------------------------------------
tp_vector::tp_vector (const tp_vector &v) {

   x = new double[_vectorNsize=v._vectorNsize];

   _originalsize = v._originalsize;
   _size_sur_4   = v._size_sur_4;

//   _size_sur_4 = _vectorNsize / 4;

   double *p = x;
   double *pv = &v.x[0];

   for (int i = _size_sur_4; i; i--) {
      *p = *pv;  p++;  pv++;
      *p = *pv;  p++;  pv++;
      *p = *pv;  p++;  pv++;
      *p = *pv;  p++;  pv++;
   }

}
//------------------------------------------------------
void tp_vector::dimension_it (int i) {

   if (0 > i)
      return;

   if (0 < _vectorNsize)
      delete [] x;

   _originalsize = i;

   i = (i + 3) / 4 ;

   _size_sur_4 = i;

   _vectorNsize = 4 * i;

   x = new double[_vectorNsize];
   double *p;

   p = x;

   for (i = _size_sur_4; i; i--) {
      *p = 0.0; ++p;
      *p = 0.0; ++p;
      *p = 0.0; ++p;
      *p = 0.0; ++p;
   }

}
//------------------------------------------------------
int tp_vector::size (void) {
   return _originalsize;
}
//------------------------------------------------------
tp_vector::tp_vector (int i) {

   _vectorNsize =
   _originalsize = 0;

   dimension_it (i);
}
//------------------------------------------------------
void tp_vector::mul (const double n) {

   if (0 >= _vectorNsize)
      return;

   double *p = x;

   for (int i = _size_sur_4; i; i--) {
      (*p) *= n; ++p;
      (*p) *= n; ++p;
      (*p) *= n; ++p;
      (*p) *= n; ++p;
    }
};
//------------------------------------------------------
tp_vector operator * (const tp_vector &v1, const double k) {

   int n = v1._originalsize;

   if (0 >= n)
      return tp_vector (4);

   tp_vector r(n);

   double *pr = &r.x[0];
   double *pv = &v1.x[0];

   for (int i = v1._size_sur_4; i; i--) {
      *pr = (*pv) * k;    ++pr;   ++pv;
      *pr = (*pv) * k;    ++pr;   ++pv;
      *pr = (*pv) * k;    ++pr;   ++pv;
      *pr = (*pv) * k;    ++pr;   ++pv;
   }
   return r;
};
//------------------------------------------------------
tp_vector tp_vector::operator * (const double k) {

   int n = _originalsize;

   if (0 >= n)
      return tp_vector (4);

   tp_vector r(n);

   double *pr = &r.x[0];
   double *pv = x;

   for (int i = _size_sur_4; i; i--) {
      *pr = (*pv) * k;    ++pr;   ++pv;
      *pr = (*pv) * k;    ++pr;   ++pv;
      *pr = (*pv) * k;    ++pr;   ++pv;
      *pr = (*pv) * k;    ++pr;   ++pv;
   }
   return r;
};
//------------------------------------------------------
tp_vector operator + (const tp_vector &v1, const tp_vector &v2) {

   int n = v1._originalsize;

   if (0 >= n)
      return tp_vector(1);

   tp_vector r(n);

   double *pr=&r.x[0];
   double *p1=&v1.x[0];
   double *p2=&v2.x[0];

   for (int i = v1._size_sur_4; i; i--) {
      *pr = *p1 + *p2;       pr++;       p1++;       p2++;
      *pr = *p1 + *p2;       pr++;       p1++;       p2++;
      *pr = *p1 + *p2;       pr++;       p1++;       p2++;
      *pr = *p1 + *p2;       pr++;       p1++;       p2++;
   }
   return r;
};
//------------------------------------------------------
tp_vector tp_vector::operator + (const tp_vector &v2) {

   int n = _originalsize;

   if (0 >= n)
      return tp_vector(1);

   tp_vector r(n);

   double *pr = & r.x[0];
   double *p1 =     x   ;
   double *p2 = &v2.x[0];

   for (int i = _size_sur_4; i; i--) {
      *pr = *p1 + *p2;       pr++;       p1++;       p2++;
      *pr = *p1 + *p2;       pr++;       p1++;       p2++;
      *pr = *p1 + *p2;       pr++;       p1++;       p2++;
      *pr = *p1 + *p2;       pr++;       p1++;       p2++;
   }
   return r;
};
//------------------------------------------------------
tp_vector& tp_vector::operator = (const tp_vector &C) {

      if (this == &C) return *this;

      if (_vectorNsize != C._vectorNsize)
         dimension_it (C._originalsize);

      memcpy (x, C.x, (sizeof (double)) * _vectorNsize);

      return *this;
};
//------------------------------------------------------
char * tp_vector::find_next_n (char *p) {

   while (*p && ((*p == '-') || (*p == '.') ||((*p >= '0') && (*p <= '9'))))
      p++;

   while (*p && !((*p == '-') || (*p == '.') ||((*p >= '0') && (*p <= '9'))))
      p++;

   return *p ? p : NULL;
}

//------------------------------------------------------

tp_vector& tp_vector::operator = (const char *s) {

      if (NULL == this) return *this;


      double auqs[50];
      int i = 0;
      char *p = (char *) s;

      while (p && *p) {

         auqs[i] = atof (p);

         p = find_next_n (p);

         i++;
      }

      this->dimension_it (i);

      i--;

      while (i > -1) {
         x[i] = auqs[i];
         i--;
      }

      return *this;
};


tp_vector::tp_vector (const char *s) {

      double auqs[1000];
      int i = 0;
      char *p = (char*) s;

      while (p && *p) {

         auqs[i] = atof (p);

         p = find_next_n (p);

         i++;
      }

   _vectorNsize = 0;

   dimension_it (i);

   --i;
      while (i > -1) {
         x[i] = auqs[i];
         i--;
      }
}


//------------------------------------------------------
tp_vector operator - (const tp_vector &v1, const tp_vector &v2) {

   int n = v1._originalsize;

   if (0 >= n)
      return tp_vector(4);

   tp_vector r(n);

   double *pr = &r.x[0];
   double *p1 = &v1.x[0];
   double *p2 = &v2.x[0];

   for (int i = v1._size_sur_4; i; i--) {
      *pr = *p1 - *p2;      pr++;      p1++;      p2++;
      *pr = *p1 - *p2;      pr++;      p1++;      p2++;
      *pr = *p1 - *p2;      pr++;      p1++;      p2++;
      *pr = *p1 - *p2;      pr++;      p1++;      p2++;
   }

   return r;
};
//------------------------------------------------------
tp_vector operator - (const tp_vector &v1) {

   int n = v1._originalsize;

   if (0 >= n)
      return tp_vector(1);

   tp_vector r(n);

   double *pv = &v1.x[0];
   double *pr = &r.x[0];

   for (int i = v1._size_sur_4; i; i--) {
      *pr = -(*pv);      ++pr;      ++pv;
      *pr = -(*pv);      ++pr;      ++pv;
      *pr = -(*pv);      ++pr;      ++pv;
      *pr = -(*pv);      ++pr;      ++pv;
   }
   return r;
};
//------------------------------------------------------
double operator * (const tp_vector &v1, const tp_vector &v2) {

   double r = 0.0;

   int n = v1._originalsize;

   if (0 >= n)
      return 0.;

   double *p1 = &v1.x[0];
   double *p2 = &v2.x[0];

   for (int i = v1._size_sur_4; i; i--) {
      r += (*p1 * *p2);     ++p1;     ++p2;
      r += (*p1 * *p2);     ++p1;     ++p2;
      r += (*p1 * *p2);     ++p1;     ++p2;
      r += (*p1 * *p2);     ++p1;     ++p2;
   }

   return r;
};
//------------------------------------------------------
void tp_vector::reset (void) {

   if (0 >= _vectorNsize)
      return;

   double *p=x;

   for (int i = _size_sur_4; i; i--) {
      *p = 0;  ++p;
      *p = 0;  ++p;
      *p = 0;  ++p;
      *p = 0;  ++p;
   }
}
//------------------------------------------------------
double tp_vector::norm (void) {
   double *p,aux=0.;

   if (0 >= _vectorNsize)
      return -1;

   p = &x[0];
   for (int i = _size_sur_4; i; i--) {
      aux = aux + ((*p)*(*p));   ++p;
      aux = aux + ((*p)*(*p));   ++p;
      aux = aux + ((*p)*(*p));   ++p;
      aux = aux + ((*p)*(*p));   ++p;
   }
   return sqrt(aux);
}
//------------------------------------------------------
void tp_vector::normalise (void) {

   double *p;
   double          aux = norm ();

   if (0 >= _vectorNsize)
      return;

   if (aux > 0.) {

      p = &x[0];
      for (int i= _size_sur_4; i; i--) {
         *p /= aux;    p++;
         *p /= aux;    p++;
         *p /= aux;    p++;
         *p /= aux;    p++;
      }
   }
}

//------------------------------------------------------
//void tp_vector::sort (void) {
//
//   std::sort (&x[0], &x[_originalsize]);
//
//}

//------------------------------------------------------
double tp_vector::max (void) {
   double d = x[0];

   double *p = x;

   for (int i = _size_sur_4; i; i--) {
      if (d < *p) d = *p;   p++;
      if (d < *p) d = *p;   p++;
      if (d < *p) d = *p;   p++;
      if (d < *p) d = *p;   p++;
   }

   return d;
}
//------------------------------------------------------
double tp_vector::maxabs (void) {
   double d,e;
   double *p=x;

   d = fabs(*p);

   for (int i= _size_sur_4; i; i--) {
      e = fabs(*p);  if (d < e) d = e;  ++p;
      e = fabs(*p);  if (d < e) d = e;  ++p;
      e = fabs(*p);  if (d < e) d = e;  ++p;
      e = fabs(*p);  if (d < e) d = e;  ++p;
   }

   return d;
}

//------------------------------------------------------

void tp_vector::setvector (double *w) {

   memcpy (x, w, _originalsize * sizeof (double));

}

//------------------------------------------------------
void tp_vector::todouble (double *w) {

   if (NULL == w) {
      if (_originalsize > 0)
         throw ("tp_vector::todouble (NULL)");

      return;
   }

   memcpy (w, x, _originalsize * sizeof (double));
}


//------------------------------------------------------
double *tp_vector::todouble (void) {
   return x;
}

//------------------------------------------------------
void tp_vector::multiply (double n) {

   double *p = x;

   for (int i = _size_sur_4; i; i--) {
      (*p) *= n;  p++;
      (*p) *= n;  p++;
      (*p) *= n;  p++;
      (*p) *= n;  p++;
   }
}
//------------------------------------------------------
void tp_vector::attrib (const tp_vector &C) {

   if (this == &C) return;

   if (!(_vectorNsize + C._vectorNsize))
      return;

   if (_vectorNsize != C._vectorNsize) {
      dimension_it (C._originalsize);
   }

   memcpy (x, C.x, (sizeof (double)) * _originalsize);
};

//------------------------------------------------------

void tp_vector::concatenate (tp_vector v2) {

   tp_vector aux (_originalsize + v2._originalsize);

   memcpy (aux.x, x, (sizeof (double)) * _originalsize);

   memcpy (&aux.x[_originalsize],
            v2.x,
            (sizeof (double)) * v2._originalsize);

   attrib (aux);
}

//------------------------------------------------------
void tp_vector::add (const tp_vector &C) {

   if (!(_vectorNsize + C._vectorNsize))
      return;

   if (_vectorNsize != C._vectorNsize) {
      dimension_it (C._originalsize);
   }

   double *p1 = &x[0];
   double *p2 = &C.x[0];

   for (int i = _size_sur_4; i; i--) {
      *p1 = *p1 + *p2;          p1++;      p2++;
      *p1 = *p1 + *p2;          p1++;      p2++;
      *p1 = *p1 + *p2;          p1++;      p2++;
      *p1 = *p1 + *p2;          p1++;      p2++;
   }
};
//------------------------------------------------------
void tp_vector::sub (const tp_vector &C) {

   if (!(_vectorNsize + C._vectorNsize))
      return;

   if (_vectorNsize != C._vectorNsize) {
      dimension_it (C._originalsize);
   }

   double *p1 = &x[0];
   double *p2 = &C.x[0];

   for (int i = _size_sur_4; i; i--) {
      *p1 = *p1 - *p2;          p1++;      p2++;
      *p1 = *p1 - *p2;          p1++;      p2++;
      *p1 = *p1 - *p2;          p1++;      p2++;
      *p1 = *p1 - *p2;          p1++;      p2++;
   }
};
//------------------------------------------------------
char *tp_vector::c_str (char *_2char) {


   if (NULL == _2char) return NULL;

   char aux[25];

   _2char[0] = 0;

   sprintf (aux, "%19.18lf", x[0]);
   strcat  (_2char, aux);

   for (int k = 1; k < _originalsize; k++) {

      sprintf (aux, ";%19.18lf", x[k]);
      strcat  (_2char, aux);
   }

   return _2char;
}
//------------------------------------------------------
double tp_vector::length (void) {

   double n, tot;

   tot = 0.0;
   for (int k = 0; k < _originalsize; k++) {

      n = x[k];

      tot += (n*n);
   }

   return sqrt (tot);
}




//------------------------------------------------------
tp_vector tp_vector::subvector (int io, int in) {

   int n = in - io + 1;

   tp_vector v(n);

   memcpy (v.x, &x[io], n * sizeof (double));

   return v;
};

//------------------------------------------------------
double tp_vector::dotproduct (tp_vector v) {

   double tot;

   tot = 0.0;
   for (int k = 0; k < _originalsize; k++)
      tot += x[k] * v.x[k];

   return tot;
}

//------------------------------------------------------

double   tp_vector:: operator [] (int idx) {

   if ((idx < 0) || (idx > _originalsize)) throw "Error!";
   
   return x[idx];   
}

//------------------------------------------------------
//double & tp_vector:: operator  [] (int idx) const {
//
//   if ((idx < 0) || (idx > _originalsize)) throw "Error!";
//   
//   return x[idx];  
//}
//------------------------------------------------------
double tp_vector::ManhattanNorm (tp_vector v) {

   double n, max = 0.0;

   for (int i = 0; i < _originalsize; ++i) {
   
      n = fabs (x[i] - v.x[i]);
   
      if (n > max)
         max = n;
   
   }
   
   return max;
}
