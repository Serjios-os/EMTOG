#include "datacalculator.h"

DataCalculator::DataCalculator() {
    this->_current_assigned_fusible = {
        {   0   ,   0       },
        {   10  ,   13.1    },
        {   16  ,   17.6    },
        {   20  ,   22      },
        {   25  ,   27.5    },
        {   32  ,   35.2    },
        {   40  ,   44      },
        {   50  ,   55      },
        {   63  ,   69.3    },
        {   80  ,   88      },
        {   100 ,   110     },
        {   125 ,   137.5   },
        {   160 ,   176     },
        {   200 ,   220     },
        {   250 ,   275     },
        {   315 ,   346.5   },
        {   400 ,   440     },
        {   500 ,   550     },
        {   630 ,   693     },
        {   800 ,   880     },
        {   1000,   1100    },
        {   1250,   1375    }
    };
    this->_current_assigned_disj_dom = {
        {   0   ,   0   },
        {   10  ,   10  },
        {   16  ,   16  },
        {   20  ,   20  },
        {   25  ,   25  },
        {   32  ,   32  },
        {   40  ,   40  },
        {   50  ,   50  },
        {   63  ,   63  },
        {   80  ,   80  },
        {   100 ,   100 },
        {   125 ,   125 },
    };
}

float DataCalculator::current(current_data_t current_data) {
    /* assigned current calculation */
    float p = current_data.puissance_nominale;
    float f = current_data.facteur_puissance;
    float n = current_data.rendement;
    float a = (f && n) ?
        1 / (f * n)
    :
        facteur_puissance_rendement(p, f, n);
    float b = current_data.facteur_utilisation;
    float c = current_data.facteur_simultaneite;
    float d = current_data.facteur_extension;
    float e = current_data.facteur_convertion;
    float Ib = p * a * b * c * d * e;
    return Ib;
}

float DataCalculator::current_assigned(int dispositif, float current) {
    /* assigned current calculation */
    std::vector<std::list<current_assigned_data_t>> current_assigned = {this->_current_assigned_disj_dom, this->_current_assigned_fusible};
    if (dispositif) {
        auto it = current_assigned.at(dispositif - 1).begin();
        auto end = current_assigned.at(dispositif - 1).end();
        while (it != end) {
            if (it->current_assigned > current) {
                return it->circuit_cut_current ;
            }
            it++;
        }
    }
    return 0.00;
}

float DataCalculator::current_admissible(cable_data_t cable_data) {
    /* cable section calculation */
    float __current = 1.0;
    float __correction = 1;
    __correction *= cable_data.cable_coef;
    __correction *= cable_data.cable_meth_ref;
    __correction *= cable_data.cable_temperature;
    __correction *= cable_data.cable_group_cable;
    __correction *= cable_data.cable_resistance_sol;
    __correction *= cable_data.cable_group_conduit;
    __current *= cable_data.courant;
    __current /= __correction;
    return __current;
}

float DataCalculator::facteur_puissance_rendement(float Pn, float & f, float & n) {
    if (! f && ! n){
        if (Pn < 1000) {
            f = DEFAULT_VALUE_FACTEUR_PUISSANCE_0;
            n = DEFAULT_VALUE_RENDEMENT_0;
            return DEFAULT_VALUE_RENDEMENT_FACTEUR_PUISSANCE_0;
        } else if (Pn < 4000) {
            f = DEFAULT_VALUE_FACTEUR_PUISSANCE_1000;
            n = DEFAULT_VALUE_RENDEMENT_1000;
            return DEFAULT_VALUE_RENDEMENT_FACTEUR_PUISSANCE_1000;
        } else if (Pn < 50000) {
            f = DEFAULT_VALUE_FACTEUR_PUISSANCE_4000;
            n = DEFAULT_VALUE_FACTEUR_PUISSANCE_4000;
            return DEFAULT_VALUE_RENDEMENT_FACTEUR_PUISSANCE_4000;
        } else {
            f = DEFAULT_VALUE_FACTEUR_PUISSANCE_50000;
            n = DEFAULT_VALUE_FACTEUR_PUISSANCE_50000;
            return DEFAULT_VALUE_RENDEMENT_FACTEUR_PUISSANCE_50000;
        }
    }
    return 0;
}
