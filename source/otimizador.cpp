
#include "otimizador.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>


#define M_Random ((_ru(_generator) *_temperature) - _demitemperature)

void tp_Otimizador::set_stopper (int (*p)(void)) {
   _stop_me = p;
}
void tp_Otimizador::set_announcer (void (*p) (int, double)) {
   _announcer = p;
}
void tp_Otimizador::set_evaluator (double (*p)(tp_vector)) {
   _evaluator = p;
}


//--------------------------------------------------------

void tp_Otimizador::hara_kiri (void) {

   if (NULL != _pop) {
      for (int i = 0; i < _taille_pop; i++)
         delete _pop[i].particula;

      delete[] _pop;
   }

   _pop = NULL;
}



void tp_Otimizador::genetic_start_pop (tp_vector w0) {

   int     i,j,n;

   if (_taille_pop < 10)
      _taille_pop = 10;

   n = w0.size ();

   tp_vector X(n), Y(n);

   X = w0;

   if (NULL != _pop)
      hara_kiri ();

   _pop = new tp_pop [_taille_pop];

   if (NULL == _pop) {
      throw "Not enough memory for genetic!";
      exit (0);
   }


   for (i = 0; i < _taille_pop; i++)
      _pop[i].particula = new (tp_vector)(n);


   *_pop[0].particula = X;

   for (i = 1; i < _taille_pop; i++) {

      for (j = 0; j < _number_of_parameters; j++)
         Y.x[j] = M_Random; 

      *_pop[i].particula = X + Y;

   }

}


//--------------------------------------------------------


int tp_Otimizador::genetic_shake (void) {

   int   i, j, k, l, nfois;
//   char *p, t;
//   double a;

   int n;
   n = _number_of_parameters;

   tp_vector X(n), v(n);

   for (i = 0, k = _taille_pop - 1; k > i; i++) {


      nfois = (int) (5. * pow ((double).95, (double) i));

      X.attrib (*_pop[i].particula);

      for (l = 0; (k > i) && (l < 1+nfois); l++, k--) {

         for (j = 0; j < _number_of_parameters; j++)
            v.x[j] = X.x[j] + M_Random;

         (*_pop[k].particula).attrib (v);
      }
   }

   return k;
}

//--------------------------------------------------------


void tp_Otimizador::genetic_mesure (int i) {

   if (_epoch < 1) i = 0;

   for (; i < _taille_pop; i++)
      _pop[i].value = _evaluator (*_pop[i].particula);

}

//--------------------------------------------------------


int sort_function (const void *p1, const void *p2) {
   double dif = ((struct s_pop *) p1) -> value
                -
                ((struct s_pop *) p2) -> value;

   if (dif < 0.0) return -1;
   if (dif > 0.0) return 1;

   return 0;
}


//--------------------------------------------------------



void tp_Otimizador::genetic_order (void) {

   qsort ((void *)_pop, _taille_pop, sizeof(_pop[0]), sort_function);

}


//--------------------------------------------------------



double tp_Otimizador::genetic (tp_vector w0) {

   ++_epoch;

   _number_of_parameters = w0.size ();

   tp_vector X(_number_of_parameters);
   int i;
   static double perf;
   double d;

   d = _evaluator (w0);

   if (_epoch < 1) {

      _temperature     = _seil;
      _demitemperature = .5 * _temperature;

      perf = 1e10;

      genetic_start_pop (w0);

   }

   _temperature     = _seil * exp ((float)(-_epoch) / 500.);
   _demitemperature = .5 * _temperature;

   i = genetic_shake ();

   genetic_mesure (i);

   //genetic_order ();
   qsort ((void *)_pop, _taille_pop, sizeof(_pop[0]), sort_function);

   d = _pop[0].value;

   if (perf == d)
      _seil *= .97;
   else if (perf > d) {
      perf = d;
   }

   X.attrib (*_pop[0].particula);

   _evaluator (X);

   return _pop[0].value;
}




tp_vector tp_Otimizador::minimise (tp_vector w0) {

   _epoch = -1;

   _seil0 = _seil;

   while (1) {
      if (NULL != _stop_me) {
         if (_stop_me ())
            break;
      }

       if (_counter > 0) {
          if (_epoch > _counter)
             break;
       }

      genetic (w0);

      if (NULL != _announcer)
         _announcer (_epoch, _pop[0].value);
   }

   _evaluator (*_pop[0].particula);

   tp_vector response;

   response = *(_pop[0].particula);

   hara_kiri ();

   return response;
}



tp_vector tp_Otimizador::SouzadeCursi (int k) {

   int i;
   double s2, n, g;

   tp_vector v(_number_of_parameters);
   v .reset ();

   n = 1 + (_epoch % 100);

   s2 = 1./ log(n * _pop[0].value);
   v = (*_pop[0].particula);

   for (i = 1; i < _taille_pop; i++) {

      if (i == k)
         continue;

      g  = 1./ log(n * _pop[0].value);
      v  = v + ((*_pop[i].particula) * g);
      s2 = s2 + g;
   }

   v = v * (1./s2);

   return v;
}





