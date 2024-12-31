#ifndef _genetic_hpp_

#define _genetic_hpp_


#include "vector.hpp"
#include <random>
#include <iostream>



struct s_pop {
   tp_vector *particula;
   double  value;
   char    type;
   int     numb;
};

typedef struct s_pop tp_pop;


class tp_Otimizador {

   private:

      std::uniform_real_distribution<double> _ru;
      std::mt19937 _generator;

      int
         _number_of_parameters
        ,_taille_pop
        ;

      double
         _seil
        ;

      int
         _counter
        ;

      double 
         _temperature
        ,_demitemperature
        ;


   public:

      tp_pop *_pop;

      double
         (*_evaluator) (tp_vector)
         ,_seil0
         ;

      int
         _count
        ,_epoch
        ,(*_stop_me)(void)
        ;

      void
         (*_announcer) (int,double)
        ;




      tp_Otimizador (void) {_seil    = .9;
                            _counter = -1;
                            _pop     = NULL;
                            _ru      = std::uniform_real_distribution<double>(0.0, 1.0);
                           };

   void   hara_kiri (void);



   void   genetic_start_pop (tp_vector );
   void   genetic_gamma     (double *);
   int    genetic_shake     (void);
   void   genetic_mesure    (int);
   void   genetic_order     (void);
   double genetic           (tp_vector );
   tp_vector SouzadeCursi     (int);

   void   project            (void);

   void   temperature        (double a)  { _seil = a;};


   void prepare (int i) {_number_of_parameters = i;}

   void set_stopper   (int (*p)(void)) ;
   void set_announcer (void(*p)(int,double)) ;
   void set_evaluator (double (*p) (tp_vector w));
   void set_loops     (int n)  {_counter = n;};

   void set_pop       (int n)  {_taille_pop = n;};

   tp_vector minimise (tp_vector);


};


#endif
