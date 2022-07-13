#ifndef DATACALCULATOR_H
#define DATACALCULATOR_H

#include <configuration.h>

#define DISPOSITIF_DISJONCTEUR_INDUSTRIEL  (0)
#define DISPOSITIF_DISJONCTEUR_DOMESTIQUE  (1)
#define DISPOSITIF_FUSIBLE                 (2)


typedef struct current_data_s {
    float puissance_nominale;
    float rendement;
    float facteur_puissance;
    float facteur_utilisation;
    float facteur_simultaneite;
    float facteur_extension;
    float facteur_convertion;
} current_data_t;

typedef struct cable_data_s {
    float courant;
    float longeur;
    float cable_coef;
    float cable_meth_ref;
    float cable_temperature;
    float cable_group_cable;
    float cable_resistance_sol;
    float cable_group_conduit;
} cable_data_t;

typedef struct current_assigned_data_s {
    float current_assigned;
    float circuit_cut_current;
} current_assigned_data_t;

class DataCalculator
{
private:
    float facteur_puissance_rendement(float, float &, float &);
    std::list<current_assigned_data_t> _current_assigned_fusible;
    std::list<current_assigned_data_t> _current_assigned_disj_dom;
public:
    DataCalculator();
    float current_assigned(int, float);
    float current(current_data_t);
    float current_admissible(cable_data_t);
};

#endif // DATACALCULATOR_H
